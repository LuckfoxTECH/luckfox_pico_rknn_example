import numpy as np
import cv2
import os
from rknn.api import RKNN
from PIL import Image
from sklearn import preprocessing
from scipy.spatial.distance import pdist
os.environ['RKNN_DRAW_DATA_DISTRIBUTE']="1"

if __name__ == '__main__':
    BUILD_QUANT = True

    # Create RKNN object
    rknn = RKNN()
    print('--> config model')
    rknn.config(mean_values=[[0, 0, 0]], std_values=[[255, 255, 255]], target_platform='rv1103')
    print('done')

    # Load model
    print('--> Lccoading model')
    ret = rknn.load_onnx(model='./mobilefacenet.onnx')
    if ret != 0:
        print('Load retinaface failed!')
        exit(ret)
    print('done')

    # Build model
    print('--> Building model')
    ret = rknn.build(do_quantization=True, dataset='./facenet_dataset.txt')
    if ret != 0:
        print('Build model failed!')
        exit(ret)
    print('done')

    # Init runtime
    print('--> Init runtime environment')
    ret = rknn.init_runtime()
    if ret != 0:
        print('Init runtime environment failed')
        exit(ret)
    print('done')

    # Set inputs and inference
    image_1 = Image.open("001.jpg")
    image_1 = image_1.resize((160,160), Image.BICUBIC)
    img1 = np.asarray(image_1, np.uint8)
    outputs1 = np.array(rknn.inference(data_format='nhwc', inputs=[img1])[0])
    outputs1 = preprocessing.normalize(outputs1, norm='l2') 
    
    image_2 = Image.open("002.jpg")
    image_2 = image_2.resize((160,160), Image.BICUBIC)
    img2 = np.asarray(image_2, np.uint8)
    outputs2 = np.array(rknn.inference(data_format='nhwc', inputs=[img2])[0])
    outputs2 = preprocessing.normalize(outputs2, norm='l2') 

    # Get distance 
    distance = np.linalg.norm(outputs2 - outputs1, axis=1)
    print("distance:", distance)
    

    rknn.release()
