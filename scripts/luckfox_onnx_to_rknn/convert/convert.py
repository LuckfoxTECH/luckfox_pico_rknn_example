import sys

from rknn.api import RKNN

def parse_arg():
    if len(sys.argv) < 5:
        print("Usage: python3 {} [onnx_model_path] [dataset_path] [output_rknn_path] [model_type]".format(sys.argv[0]));
        exit(1)

    model_path = sys.argv[1]
    dataset_path= sys.argv[2]
    output_path = sys.argv[3]
    model_type = sys.argv[4]

    return model_path, dataset_path, output_path,model_type

if __name__ == '__main__':
    model_path, dataset_path, output_path, model_type= parse_arg()

    # Create RKNN object
    rknn = RKNN(verbose=False)

    # Pre-process config
    print('--> Config model')
    if model_type == 'Retinaface':
        rknn.config(mean_values=[[104, 117, 123]], std_values=[[1, 1, 1]], target_platform='rv1103',
                                quantized_algorithm="normal", quant_img_RGB2BGR=True,optimization_level=0)
        print("Use retinaface mode")
    else:
        rknn.config(mean_values=[[0, 0, 0]], std_values=[[255, 255, 255]], target_platform='rv1103')
    
    print('done')

    # Load model
    print('--> Loading model')
    ret = rknn.load_onnx(model=model_path)
    if ret != 0:
        print('Load model failed!')
        exit(ret)
    print('done')

    # Build model
    print('--> Building model')
    ret = rknn.build(do_quantization=True, dataset=dataset_path)
    if ret != 0:
        print('Build model failed!')
        exit(ret)
    print('done')

    # Export rknn model
    print('--> Export rknn model')
    ret = rknn.export_rknn(output_path)
    if ret != 0:
        print('Export rknn model failed!')
        exit(ret)
    print('done')

    # Release
    rknn.release()
