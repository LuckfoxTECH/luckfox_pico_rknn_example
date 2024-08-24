
**Read this in other languages: [English](README.md), [中文](README_CN.md).**
# Model Conversion
## 1 File Structure

```bash
luckfox_onnx_to_rknn
├── convert -------------------------------------Python scripts for model conversion
├── dataset -------------------------------------Reference dataset for model conversion
│   └── pic
│       ├── facenet
│       │   └── face.jpg
│       ├── retinaface
│       │   └── face.jpg
│       └── yolov5
│           └── bus.jpg
├── model ---------------------------------------ONNX models and RKNN models
└── sim -----------------------------------------Script for testing models on software simulator
```

## 2 Environment Setup

**Experiment Platform: Ubuntu 22.04 (WSL)**

Using Conda to create a Python virtual environment allows flexible switching between different application scenarios, avoiding issues such as version mismatches that can prevent programs from running. Different Python virtual environments are required during the training and conversion of AI models.

#### 2.1 Install Miniconda Tool
+ Check if Miniconda or another Conda tool is installed. If a version number is displayed, it is already installed.
    ```
    conda --version
    ```
+ Download the installer
    ```
    wget https://mirrors.tuna.tsinghua.edu.cn/anaconda/miniconda/Miniconda3-4.6.14-Linux-x86_64.sh
    ```
+ Install Miniconda
    ```
    chmod 777 Miniconda3-py38_4.8.3-Linux-x86_64.sh
    bash Miniconda3-py38_4.8.3-Linux-x86_64.sh
    ```
    **Note:** The Miniconda installer must have its permissions set using `chmod 777`.

    After starting the installation, press Enter to read the license terms, type `yes` to accept the license and continue with the installation. Press Enter again to create the `miniconda` folder in your home directory, where the virtual environments will be stored. Finally, type `yes` once more to initialize Conda.
+ In the terminal window, enter the Conda base environment:
    ```
    # The directory where Miniconda3 is installed (customize based on your installation)
    source ~/miniconda3/bin/activate 
    # After successful activation, the command line prompt will appear as follows:
    # (base) xxx@xxx:~$
    ```

#### 2.2 Create the RKNN-Toolkit2 Conda Environment

+ Create the RKNN-Toolkit2 development Conda environment. The `-n` parameter specifies the environment name, and Python version **3.8** is recommended.
    ```
    conda create -n RKNN-Toolkit2 python=3.8
    ```
    Enter `y` to confirm the installation of default packages.
+ Activate the RKNN-Toolkit2 Conda environment:
    ```
    conda activate RKNN-Toolkit2
    ```
+ Verify that Python is correctly installed:
    ```
    python --version
    ```
    **Note:** In some development environments, the Python version may not switch correctly after creating the Conda environment. Restarting the terminal can resolve this issue.
+ Obtain the RKNN-Toolkit2 installation package:
    ```
    git clone https://github.com/rockchip-linux/rknn-toolkit2.git
    ```
+ Enter the directory:
    ```bash
    cd rknn-toolkit2
    ```
+ Set the `pip` mirror source (optional based on network conditions):
    ```
    pip config set global.index-url http://pypi.douban.com/simple/
    pip config set install.trusted-host pypi.douban.com
    ```
+ Install the dependencies for RKNN-Toolkit2. `cp38` corresponds to the Python version in the Conda environment. The experiment uses version 3.8, so the dependencies with the `cp38` suffix are used:
    ```
    pip install -r rknn-toolkit2/packages/requirements_cp38-1.6.0.txt
    ```
+ Install RKNN-Toolkit2:
    ```
    pip install rknn-toolkit2/packages/rknn_toolkit2-1.6.0+81f21f4d-cp38-cp38-linux_x86_64.whl
    ```
    Choose the installation package file from the `packages` folder based on the Python version. The `81f21f4d` represents the commit number, so choose according to your specific situation. Use the package with the `cp38` suffix for Python 3.8.

#### 2.3 Verification
+ Verify that the installation was successful. If there are no errors, the installation is successful:
    ```
    python
    >>> from rknn.api import RKNN
    ```

## 3 Model Conversion
+ Enter the RKNN-Toolkit2 Conda development environment:
    ```
    conda activate RKNN-Toolkit2
    ```
+ Model conversion:
    ```
    cd convert
    python convert.py <onnx_model_path> <dataset_path> <output_model_path> <model_type>
    ```
    + **onnx_model_path**: The path to the ONNX model exported during training. The example model is located in `luckfox_onnx_to_rknn/model`.
    + **dataset_path**: Provide a small number of images as a reference for model conversion. The storage path of the images should be written in a `txt` file and passed as a parameter to the conversion script.
    + **output_model_path**: The name and path of the exported RKNN model. Ensure it has a `.rknn` suffix.
    + **model_type**: Provide different RKNN preprocessing settings based on the model type being converted. In this example, only the RetinaFace model requires special configuration during conversion.

+ RetinaFace Model Conversion:
    ```
    python convert.py ../model/retinaface.onnx ../dataset/retinaface_dataset.txt ../model/retinaface.rknn Retinaface
    ```
+ FaceNet Model Conversion:
    ```
    python convert.py ../model/facenet.onnx ../dataset/facenet_dataset.txt ../model/facenet.rknn Facenet
    ```
+ Yolov5 Model Conversion:
    ```
    python convert.py ../model/yolov5.onnx ../dataset/yolov5_dataset.txt ../model/yolov5.rknn Yolov5
    ```

## 4 Model Testing
+ Enter the RKNN-Toolkit2 Conda development environment:
    ```
    conda activate RKNN-Toolkit2
    ```

### 4.1 RetinaFace
+ Enter the directory:
    ```
    cd sim/retinaface
    ```
+ Execute:
    ```
    python retinaface.py
    ```
    Annotates the coordinates of the faces and five feature points in the `test.jpg` image and outputs the image to `result.jpg`.

### 4.2 FaceNet
+ Enter the directory:
    ```
    cd sim/facenet
    ```
+ Execute:
    ```
    python facenet.py
    ```
    Outputs the Euclidean distance between the two images `001.jpg` and `002.jpg`.