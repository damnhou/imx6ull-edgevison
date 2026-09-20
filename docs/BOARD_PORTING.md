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

