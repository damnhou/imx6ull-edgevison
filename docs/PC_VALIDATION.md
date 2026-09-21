# PC 端验证记录

## 环境

- Windows 10/11 x64
- CMake 4.4.3
- Qt 5.14.2 自带 MinGW-w64 GCC 7.3.0
- NCNN `20230816`
- SqueezeNet v1.1，ImageNet 1000 类
- Vulkan、OpenMP、AVX512 关闭

## 验证结果

2026-09-21 使用三张本地猫图片进行验证，程序均成功完成图片解码、预处理、推理和 Top-5
输出。`cat.jpg` 和 `cat2.jpg` 的 Top-1 为 `tabby cat`；俯拍白猫 `cat1.jpg` 的 Top-1 为
`guinea pig`。

这说明程序链路已经跑通，但也体现了 ImageNet 预训练 SqueezeNet 对拍摄角度、主体纹理和
直接拉伸缩放较敏感。不能用三张图片推导模型准确率，更不能把一次分类错误解释为部署失败。

PC 端单线程五次推理平均耗时约为 32.6 至 44.0 ms。该数值只代表当前 x86 电脑环境，不能
作为 i.MX6ULL 的性能结论。

## 复现

```powershell
.\scripts\build_host.ps1
.\scripts\run_squeezenet.ps1 -Image .\assets\cat.jpg
```

测试图片默认被 `.gitignore` 排除，避免将私人图片误提交到公开仓库。
