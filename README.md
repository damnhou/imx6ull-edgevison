# EdgeVision i.MX6ULL 本地图像推理

面向正点原子 ALPHA（NXP i.MX6ULL / Cortex-A7）的 NCNN 图像分类工程。第一阶段只处理
本地 JPEG、PNG、BMP 文件，完成图像解码、预处理、CPU 推理、Top-K 输出与耗时统计；
不包含摄像头、V4L2 或 Qt 界面。

## 当前状态

- 已完成可参数化的 C++11 命令行推理程序。
- 已关闭 Vulkan，默认单线程运行，符合 i.MX6ULL 单核 Cortex-A7 的硬件条件。
- 已下载并固定 NCNN `20230816`，SqueezeNet 模型文件已通过 SHA-256 校验。
- 已在 Windows 10/11、Qt MinGW-w64 环境完成 PC 端构建和三张本地图片推理。
- 已提供 ARMv7-A hard-float、NEON/VFPv4 工具链文件。
- 已通过命令行参数单元测试；尚未进行 i.MX6ULL 板端实测。

因此当前可以表述为“完成 NCNN 本地图像推理程序及 PC 端验证，并建立 i.MX6ULL 交叉编译
配置”。在板端实际运行并记录结果前，不应写“已完成板端部署”或具体板端推理帧率。

## 数据流

```text
本地图片路径
    ↓ simpleocv 解码为 BGR
通道转换、缩放、减均值、归一化
    ↓ ncnn::Mat
NCNN Cortex-A7 CPU 推理（Vulkan OFF，默认 1 线程）
    ↓ 分类概率
Top-K 排序、标签映射、延迟和峰值 RSS 输出
```

## 目录

```text
imx6ull-edgevision/
├─ CMakeLists.txt
├─ cmake/                         # ARM 交叉编译配置
├─ src/                           # CLI、NCNN 推理、性能统计
├─ tests/                         # 无 NCNN 依赖的参数测试
├─ scripts/                       # 依赖下载、构建、运行与部署
├─ models/                        # 模型说明，权重不入库
├─ assets/                        # 用户提供的测试图片
└─ docs/                          # 架构、板端适配和测试计划
```

## 第一次运行

### 1. 获取固定版本 NCNN

Windows PowerShell：

```powershell
.\scripts\fetch_ncnn.ps1
```

Linux：

```bash
./scripts/fetch_ncnn.sh
```

脚本默认拉取 NCNN `20230816` 标签。固定版本可以减少旧版交叉编译器与最新主分支之间的
不确定性。

### 2. 下载 SqueezeNet 示例模型

```powershell
.\scripts\download_squeezenet.ps1
```

或：

```bash
./scripts/download_squeezenet.sh
```

### 3. PC 端构建和验证

需要 CMake、支持 C++11 的编译器和 Git：

```powershell
.\scripts\build_host.ps1
.\scripts\run_squeezenet.ps1 -Image .\assets\test.jpg
```

程序默认进行 1 次预热和 5 次计时推理。示例输出：

```text
image=1280x720
preprocess_ms=<实测值>
first_inference_ms=<实测值>
average_inference_ms=<实测值>
min_inference_ms=<实测值>
max_inference_ms=<实测值>
peak_rss_kb=<实测值>
top1=<class_id>,<score>,<label>
```

## 自定义模型参数

```bash
edgevision_cli \
  --param model.param \
  --bin model.bin \
  --labels labels.txt \
  --image input.jpg \
  --input data --output prob \
  --width 227 --height 227 \
  --pixel bgr \
  --mean 104,117,123 \
  --norm 1,1,1 \
  --warmup 1 --repeat 20 --threads 1 --topk 5
```

`--pixel rgb` 表示模型需要 RGB，程序会把图片解码得到的 BGR 转为 RGB。均值和归一化
系数按变换后的通道顺序填写。PyTorch 常见的 ImageNet 预处理可换算为：

```text
mean = 0.485*255, 0.456*255, 0.406*255
norm = 1/(0.229*255), 1/(0.224*255), 1/(0.225*255)
```

更换模型时必须同时确认输入尺寸、颜色通道、mean/std、输入输出 blob 名称。预处理不一致
会造成“模型可以运行但分类完全错误”。

## i.MX6ULL 构建

资料内提供两套环境：Linaro `arm-linux-gnueabihf` 和面向 Cortex-A7hf NEON 的 FSL/Qt SDK。
实际构建必须与板端根文件系统的 glibc 和 C++ 运行库匹配。

使用 Linaro 前缀的基本方式：

```bash
export PATH=/path/to/gcc-linaro-4.9.4/bin:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
./scripts/build_arm.sh
```

如使用 FSL/Qt SDK，应先执行 SDK 安装后生成的 `environment-setup-*`，再检查它提供的
`CC`、`CXX`、`SDKTARGETSYSROOT` 和 CMake toolchain。不能把百问网教程里的
`arm-buildroot-linux-gnueabihf-*` 前缀直接用于正点原子根文件系统。

构建后检查：

```bash
file build-imx6ull/edgevision_cli
arm-linux-gnueabihf-readelf -h build-imx6ull/edgevision_cli
arm-linux-gnueabihf-readelf -d build-imx6ull/edgevision_cli
```

目标应为 ARM 32 位 hard-float ELF，且不应依赖板端不存在的动态库。

## 板端运行

部署脚本：

```bash
./scripts/deploy.sh root@BOARD_IP
scp assets/test.jpg root@BOARD_IP:/opt/edgevision/assets/
```

板端执行：

```bash
cd /opt/edgevision
./edgevision_cli \
  --param models/squeezenet_v1.1.param \
  --bin models/squeezenet_v1.1.bin \
  --labels models/synset_words.txt \
  --image assets/test.jpg \
  --width 227 --height 227 --pixel bgr \
  --mean 104,117,123 --norm 1,1,1 \
  --input data --output prob --warmup 1 --repeat 20 --threads 1
```

详细检查项见 [板端适配](docs/BOARD_PORTING.md)和[测试计划](docs/TEST_PLAN.md)。

## 后续范围

第一版完成并记录板端数据后，再依次考虑：

1. PyTorch/ONNX/NCNN 数值一致性验证；
2. MobileNetV2 与 SqueezeNet 的延迟、内存和模型大小比较；
3. INT8 量化对速度和准确率的影响；
4. V4L2 摄像头输入；
5. Qt 展示界面和工作线程。

## 来源与许可

用户代码采用 MIT License。NCNN、模型资产和参考教程的来源见
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
