
**其他语言版本: [English](README.md), [中文](README_CN.md).**
# 模型转换

## 1 文件结构

```bash
luckfox_onnx_to_rknn
├── convert -------------------------------------模型转换 python 脚本
├── dataset -------------------------------------模型转换参考数据集
│   └── pic
│       ├── facenet
│       │   └── face.jpg
│       ├── retinaface
│       │   └── face.jpg
│       └── yolov5
│           └── bus.jpg
├── model ---------------------------------------onnx 模型和 rknn 模型
└── sim -----------------------------------------软件模拟器模型测试脚本
```

## 2 环境搭建

**实验平台：ubuntu22.04（WSL）**

使用 Conda 创建 python 虚拟环境，可以在多个应用场景下灵活切换，避免出现版本不匹配而无法运行等问题。实例在 AI 模型的训练和模型的转换过程中需要使用不同的 python 虚拟环境。

#### 2.1 安装 miniconda 工具
+ 检查是否安装了miniconda 或其他 conda 工具，如果有输出版本号说明已经安装。
    ```
    conda --version
    ```
+ 下载安装包
    ```
     wget https://mirrors.tuna.tsinghua.edu.cn/anaconda/miniconda/Miniconda3-4.6.14-Linux-x86_64.sh
    ```
+ 安装miniconda
    ```
    chmod 777 Miniconda3-py38_4.8.3-Linux-x86_64.sh
    bash Miniconda3-py38_4.8.3-Linux-x86_64.sh
    ```
    **注意**：miniconda 的安装包必须使用 `chmod 777` 来设置权限。

    执行安装后输入回车阅读许可条款，输入 `yes` 接受许可继续安装，再次输入 `回车` 会在家目录下创建 `miniconda` 文件夹，后续创建的虚拟环境会放置在此处。最后再输入一次 `yes` 进行 Conda 初始化 。
+ 在计算机的终端窗口，进入 Conda base环境
    ```
    # miniconda3 安装的目录(自定义安装根据实际情况进行修改)
    source ~/miniconda3/bin/activate 
    # 成功后，命令行提示符会变成以下形式：
    # (base) xxx@xxx:~$
    ```

#### 2.2 创建 RKNN-Toolkit2 Conda 环境

+ 创建 RKNN-Toolkit2 开发 Conda 环境， `-n` 参数表示环境名称，指定 python 版本为**3.8**（建议版本）
    ```
    conda create -n RKNN-Toolkit2 python=3.8
    ```
    输入 `y` 确认安装默认的安装包。
+ 进入 RKNN-Toolkit2 Conda 环境
    ```
    conda activate RKNN-Toolkit2
    ```
+ 验证 python 是否使用正确
    ```
    python --version
    ```
    **注意**：部分开发环境在创建 Conda 环境后 Python 版本没有切换正常，重新启动终端可以解决。
+ 获取 RKNN-Toolkit2 安装包
    ```
    git clone https://github.com/rockchip-linux/rknn-toolkit2.git
    ```
+ 进入文件夹
    ```bash
    cd rknn-toolkit2
    ```
+ 设置 `pip` 镜像源（根据网络条件选择是否跳过）
    ```
    pip config set global.index-url http://pypi.douban.com/simple/
    pip config set install.trusted-host pypi.douban.com
    ```
+ 安装 RKNN-Toolkit2 相关的依赖库，`cp38` 为对应的 Conda 环境 python 版本，实验使用的版本为 3.8 所以使用后缀为 `cp38` 的依赖项
    ```
    pip install -r rknn-toolkit2/packages/requirements_cp38-1.6.0.txt
    ```
+ 安装 RKNN-Toolkit2
    ```
    pip install rknn-toolkit2/packages/rknn_toolkit2-1.6.0+81f21f4d-cp38-cp38-linux_x86_64.whl
    ```
    根据 python 版本，选择 `packages` 文件夹下的安装包文件，其中的 `81f21f4d` 为 commit 号，根据实际情况进行选择。使用 python3.8 对应带有 `cp38` 后缀的安装包。

#### 2.3 验证
+ 验证是否安装成功，如果没有报错说明安装成功。
    ```
    python
    >>> from rknn.api import RKNN
    ```

## 3 模型转换
+ 进入RKNN-Toolkit2 Conda 开发环境
    ```
    conda activate RKNN-Toolkit2
    ```
+ 模型转换
    ```
    cd convert
    pyhthon convert.py <onnx模型地址> <训练集地址> <导出模型地址> <模型类型>
    ```
    + **onnx 模型地址**：训练导出的 onnx ，例程提供的模型位置在`luckfox_onnx_to_rknn/model`中
    + **训练集地址**：只需要提供少量的图片作为模型转换的参考，将图片的储存地址写在 `txt` 文件中作为参数导入到转换文件中。
    + **导出模型地址**：导出的 rknn 模型名称与地址，注意要以 `.rknn` 为后缀
    + **模型类型**：根据转换模型的种类提供不同的 rknn 预处理设置，在实例中只有 retinaface 模型转换需要特殊配置。

+ Retinface 模型转换
    ```
    python convert.py ../model/retinaface.onnx ../dataset/retinaface_dataset.txt ../model/retinaface.rknn Retinaface
    ```
+ Facenet 模型转换
    ```
    python convert.py ../model/facenet.onnx ../dataset/facenet_dataset.txt ../model/facenet.rknn Facenet
    ```
+ Yolov5 模型转换
    ```
    python convert.py ../model/yolov5.onnx ../dataset/yolov5_dataset.txt ../model/yolov5.rknn Yolov5
    ```

## 4 模型测试
+ 进入 RKNN-Toolkit2 Conda 开发环境
    ```
    conda activate RKNN-Toolkit2
    ```

### 4.1 retinaface
+ 进入文件夹
    ```
    cd sim/retinaface
    ```
+ 执行
    ```
    python retinaface.py
    ```
    标注 `test.jpg` 图像中的人脸和五个特征点的坐标值，输出图像到 `result.jpg` 。

### 4.2 facenet
+ 进入文件夹
    ```
    cd sim/facenet
    ```
+ 执行
    ```
    python facenet.py
    ```
    输出 `001.jpg` 和 `002.jpg` 两张图片的欧氏距离