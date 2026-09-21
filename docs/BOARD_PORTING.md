# 正点原子 ALPHA 板端适配

## 硬件基线

资料中的 ALPHA V2.8 使用 MCIMX6Y2CVM08AB，单核 Cortex-A7 800 MHz。核心板存在两种：

| 版本 | DDR | 非易失存储 | 推理建议 |
|---|---:|---:|---|
| eMMC | 512 MB | 8 GB eMMC | 推荐，可运行 CLI 并保留后续 Qt 空间 |
| NAND | 256 MB | 512 MB NAND | 先运行精简 CLI，严格观察内存和存储 |

先用板端命令确认实际配置：

```bash
cat /proc/cpuinfo
free -h
lsblk
df -h
uname -m
ldd --version
```

## 工具链选择

第二篇参考教程使用百问网 Buildroot 工具链。正点原子出厂根文件系统不能仅因为 CPU 相同就
直接使用它。C++ 程序还涉及 glibc、libstdc++、动态加载器和 hard-float ABI。

推荐顺序：

1. 使用与当前根文件系统配套的 FSL/Qt SDK；
2. 若板端系统明确使用正点原子教程中的 Linaro ABI，再使用资料提供的 GCC 4.9.4；
3. 使用 `readelf -A` 检查 hard-float、NEON 等属性；
4. 使用 `readelf -d` 和板端 `ldd` 检查依赖。

## 本项目已验证环境

以下组合已在 ALPHA i.MX6ULL eMMC/512 MB 开发板上完成实际编译、部署和推理：

| 项目 | 已验证配置 |
|---|---|
| 虚拟机 | Ubuntu 16.04.7 x86_64 |
| SDK | FSL Yocto SDK `4.1.15-2.1.0` |
| 交叉编译器 | GCC 5.3.0，`arm-poky-linux-gnueabi-` |
| SDK 环境脚本 | `/opt/fsl-imx-x11/4.1.15-2.1.0/environment-setup-cortexa7hf-neon-poky-linux-gnueabi` |
| CPU/ABI | ARMv7、Cortex-A7、NEON、hard-float |
| 板端加载器 | `/lib/ld-linux-armhf.so.3` |
| 板端部署目录 | `/opt/imx6ull-edgevision` |

交叉编译命令：

```bash
source /opt/fsl-imx-x11/4.1.15-2.1.0/\
environment-setup-cortexa7hf-neon-poky-linux-gnueabi

export CROSS_COMPILE=arm-poky-linux-gnueabi-
export IMX6ULL_SYSROOT="$SDKTARGETSYSROOT"

cd ~/imx6ull-edgevision
BUILD_JOBS=2 ./scripts/build_arm.sh
```

构建脚本兼容 BSP 虚拟机自带的 CMake 3.5。`THREADS_PTHREAD_ARG=2` 是老版
`FindThreads.cmake` 交叉编译时需要的预置结果：测试程序能够链接，但 ARM 程序不能在
x86 虚拟机上直接执行。

产物检查结果应包含：

```text
ELF 32-bit LSB executable, ARM, EABI5
interpreter /lib/ld-linux-armhf.so.3
Flags: Version5 EABI, hard-float ABI
```

## 已验证板端运行

```bash
cd /opt/imx6ull-edgevision
./edgevision_cli \
  --param models/squeezenet_v1.1.param \
  --bin models/squeezenet_v1.1.bin \
  --labels models/synset_words.txt \
  --image assets/cat.jpg \
  --width 227 --height 227 --pixel bgr \
  --mean 104,117,123 --norm 1,1,1 \
  --input data --output prob \
  --topk 5 --warmup 1 --repeat 1 --threads 1
```

2026-09-21 实测结果：

| 图片 | Top-1 | 置信度 | 单线程推理时间 | 峰值 RSS |
|---|---|---:|---:|---:|
| `cat.jpg` | tabby cat | 0.273 | 654 ms | 36.5 MB |
| `cat1.jpg` | guinea pig | 0.182 | 654 ms | 36.6 MB |
| `cat2.jpg` | tabby cat | 0.199 | 648 ms | 30.4 MB |

板端结果与 Windows 主机结果一致。`cat1.jpg` 的 Top-1 是错误分类，这是模型精度问题，
不是 ARM 移植或数值计算错误。

## 常见错误

### 找不到动态加载器

现象：文件存在且有执行权限，但运行提示 `No such file or directory`。通常是 ELF 指定的动态
加载器与板端不一致。执行：

```bash
readelf -l edgevision_cli | grep interpreter
ls -l /lib/ld-linux*
```

### GLIBC 或 GLIBCXX 版本过高

说明编译器/sysroot 比板端系统新。应使用与根文件系统匹配的 SDK重新构建，不要从电脑复制
`libc.so` 覆盖板端系统。

### illegal instruction

检查编译参数是否超出了 Cortex-A7 能力。目标参数应保持在：

```text
-mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard
```

### 模型运行但分类错误

优先检查 RGB/BGR、输入尺寸、缩放裁剪方法、均值、归一化系数及 blob 名称。不要先怀疑
NEON 或 NCNN 推理核心。
