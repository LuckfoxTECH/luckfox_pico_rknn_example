// Copyright (c) 2023 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*-------------------------------------------
                Includes
-------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>   
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "retinaface_facenet.h"
#include <time.h>
#include <sys/time.h>

#include "dma_alloc.cpp"

#define USE_DMA    0

/*-------------------------------------------
                  Main Function
-------------------------------------------*/
int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("%s <retinaface model_path> <facenet model_path> <reference pic_path>\n", argv[0]);
        return -1;
    }

    system("RkLunch-stop.sh");
    const char *model_path  = argv[1];
    const char *model_path2 = argv[2];
    const char *image_path  = argv[3]; 

    clock_t start_time;
    clock_t end_time;
 
    //Model Input
    //Retinaface
    int retina_width    = 640;
    int retina_height   = 640;
    //Facenet
    int facenet_width   = 160;
    int facenet_height  = 160;
    int channels = 3;

    int ret;
    rknn_app_context_t app_retinaface_ctx;
    rknn_app_context_t app_facenet_ctx; 
    object_detect_result_list od_results;

    memset(&app_retinaface_ctx, 0, sizeof(rknn_app_context_t));
    memset(&app_facenet_ctx, 0, sizeof(rknn_app_context_t));

    //Init Model
    init_retinaface_facenet_model(model_path, model_path2, &app_retinaface_ctx, &app_facenet_ctx);

    //Init fb
    int disp_flag = 0;
    int pixel_size = 0;
    size_t screensize = 0;
    int disp_width  = 0;
    int disp_height = 0;
    void* framebuffer = NULL; 
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;
    
    int framebuffer_fd = 0; //for DMA
    cv::Mat disp;
  
    int fb = open("/dev/fb0", O_RDWR);
    if(fb == -1)
        printf("Screen OFF!\n");
    else
        disp_flag = 1;


    if(disp_flag){
        ioctl(fb, FBIOGET_VSCREENINFO, &fb_var);
        ioctl(fb, FBIOGET_FSCREENINFO, &fb_fix);

        disp_width = fb_var.xres;
        disp_height = fb_var.yres;  
        pixel_size = fb_var.bits_per_pixel / 8;
        printf("Screen width = %d, Screen height = %d, Pixel_size = %d\n",disp_width, disp_height, pixel_size);
        
        screensize = disp_width * disp_height * pixel_size;
        framebuffer = (uint8_t*)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
        
        if( pixel_size == 4 )//ARGB8888
            disp = cv::Mat(disp_height, disp_width, CV_8UC3);
        else if ( pixel_size == 2 ) //RGB565
            disp = cv::Mat(disp_height, disp_width, CV_16UC1); 

#if USE_DMA
        dma_buf_alloc(RV1106_CMA_HEAP_PATH,
                      disp_width * disp_height * pixel_size,  
                      &framebuffer_fd, 
                      (void **) & (disp.data)); 
#endif
    }
    else{
        disp_height = 240;
        disp_width = 240;
    }

    //Init Opencv-mobile
    cv::VideoCapture cap;
    cv::Mat bgr(disp_height, disp_width, CV_8UC3);
    cv::Mat retina_input(retina_height, retina_width, CV_8UC3, app_retinaface_ctx.input_mems[0]->virt_addr);
    cap.set(cv::CAP_PROP_FRAME_WIDTH,  disp_width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, disp_height);
    cap.open(0); 

    //Get referencve img feature
    cv::Mat image = cv::imread(image_path);
    cv::Mat facenet_input(facenet_height, facenet_width, CV_8UC3, app_facenet_ctx.input_mems[0]->virt_addr);
    letterbox(image,facenet_input); 
    ret = rknn_run(app_facenet_ctx.rknn_ctx, nullptr);
    if (ret < 0) {
        printf("rknn_run fail! ret=%d\n", ret);
        return -1;
    }
    uint8_t  *output = (uint8_t *)(app_facenet_ctx.output_mems[0]->virt_addr);
    float* reference_out_fp32 = (float*)malloc(sizeof(float) * 128); 
    output_normalization(&app_facenet_ctx,output,reference_out_fp32);
    //memset(facenet_input.data, 0, facenet_width * facenet_height * channels);

    float* out_fp32 = (float*)malloc(sizeof(float) * 128); 
    char show_text[12]; 
    char fps_text[32]; 
    float fps = 0;
    
    while(1)
    {
        start_time = clock();
        //opencv get photo
        cap >> bgr;
        
        cv::resize(bgr, retina_input, cv::Size(retina_width,retina_height), 0, 0, cv::INTER_LINEAR);
        ret = inference_retinaface_model(&app_retinaface_ctx, &od_results);
        if (ret != 0)
        {
            printf("init_retinaface_model fail! ret=%d\n", ret);
            return -1;
        }

        for (int i = 0; i < od_results.count; i++)
        {
            //Get det 
            object_detect_result *det_result = &(od_results.results[i]);
            mapCoordinates(bgr, retina_input, &det_result->box.left , &det_result->box.top);
            mapCoordinates(bgr, retina_input, &det_result->box.right, &det_result->box.bottom);

            cv::rectangle(bgr,cv::Point(det_result->box.left ,det_result->box.top),
                          cv::Point(det_result->box.right,det_result->box.bottom),cv::Scalar(0,255,0),3);
            
            //Face capture
            cv::Rect roi(det_result->box.left,det_result->box.top, 
                         (det_result->box.right - det_result->box.left),
                         (det_result->box.bottom - det_result->box.top));
            cv::Mat face_img = bgr(roi);

            //Give five key points
            // for(int j = 0; j < 5;j ++)
            // {
            //     //printf("point_x = %d point_y = %d\n",det_result->point[j].x,
            //     //                                     det_result->point[j].y);
            //     cv::circle(bgr,cv::Point(det_result->point[j].x,det_result->point[j].y),10,cv::Scalar(0,255,0),3);
            // }
            
            letterbox(face_img,facenet_input); 
            ret = rknn_run(app_facenet_ctx.rknn_ctx, nullptr);
            if (ret < 0) {
                printf("rknn_run fail! ret=%d\n", ret);
                return -1;
            }
            output = (uint8_t *)(app_facenet_ctx.output_mems[0]->virt_addr);

            output_normalization(&app_facenet_ctx, output, out_fp32);

            float norm = get_duclidean_distance(reference_out_fp32,out_fp32); 
            printf("@ (%d %d %d %d) %.3f\n",
                    det_result->box.left ,det_result->box.top,
                    det_result->box.right,det_result->box.bottom,
                    norm);      
            sprintf(show_text,"norm=%f",norm);
            cv::putText(bgr, show_text, cv::Point(det_result->box.left, det_result->box.top - 8),
                                           cv::FONT_HERSHEY_SIMPLEX,0.5,
                                           cv::Scalar(0,255,0),
                                           1);

        } 
        
        if(disp_flag){
            //Fps Show
            sprintf(fps_text,"fps=%.1f",fps); 
            cv::putText(bgr,fps_text,cv::Point(0, 20),
                        cv::FONT_HERSHEY_SIMPLEX,0.5,
                        cv::Scalar(0,255,0),1);

            //LCD Show 
            if( pixel_size == 4 ) 
                cv::cvtColor(bgr, disp, cv::COLOR_BGR2BGRA);
            else if( pixel_size == 2 )
                cv::cvtColor(bgr, disp, cv::COLOR_BGR2BGR565);
            memcpy(framebuffer, disp.data, disp_width * disp_height * pixel_size);
#if USE_DMA
            dma_sync_cpu_to_device(framebuffer_fd);
#endif  
        }
        //Update Fps
        end_time = clock();
        fps = ((float)CLOCKS_PER_SEC / (end_time - start_time)) ;
    } 
 
    free(reference_out_fp32);
    free(out_fp32);

    if(disp_flag){
        close(fb); 
        munmap(framebuffer, screensize);
#if USE_DMA
        dma_buf_free(disp_width*disp_height*2,
                     &framebuffer_fd, 
                     bgr.data);
#endif
    }

    release_facenet_model(&app_facenet_ctx);
    release_retinaface_model(&app_retinaface_ctx);

    return 0;
}
