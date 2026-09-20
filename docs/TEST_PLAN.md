# 测试计划

## PC 端正确性

对至少 10 张固定图片保存以下信息：

- 图片 SHA-256；
- PyTorch、ONNX Runtime、NCNN 的 Top-5；
- 最大输出误差；
- 使用的预处理参数；
- 模型文件 SHA-256。

模型转换完成的判据不是“文件生成成功”，而是三种运行时输出在预期误差范围内且 Top-K
一致。允许的数值误差需要结合 FP32/FP16/INT8 类型单独定义。

## 交叉编译产物

检查：

```bash
file edgevision_cli
arm-linux-gnueabihf-readelf -h edgevision_cli
arm-linux-gnueabihf-readelf -A edgevision_cli
arm-linux-gnueabihf-readelf -d edgevision_cli
```

记录编译器完整版本、NCNN 提交、CMake 选项、根文件系统版本和 sysroot 路径。

## 板端功能

1. 无参数执行，确认帮助和错误码；
2. 模型路径错误，确认清楚报错；
3. 图片损坏，确认解码失败；
4. 正常图片输出 Top-5；
5. 连续执行 100 次，确认结果稳定且无异常增长；
6. `--json` 输出可被脚本解析。

## 板端性能

固定 CPU 调频策略和系统负载，先预热，再至少运行 20 次。记录：

| 项目 | 值 |
|---|---|
| 板卡和核心板版本 | 待实测 |
| DDR 可用容量 | 待实测 |
| 内核和根文件系统 | 待实测 |
| 模型及 SHA-256 | 待实测 |
| 模型文件大小 | 待实测 |
| 预处理耗时 | 待实测 |
| 首次推理耗时 | 待实测 |
| 平均／最小／最大推理耗时 | 待实测 |
| 峰值 RSS | 待实测 |
| Top-1／Top-5 正确性 | 待实测 |

不以单张图片的一次耗时作为最终性能结论，也不在测试前写“实时推理”。

