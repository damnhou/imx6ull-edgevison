# 工程架构

## 第一阶段边界

第一阶段从本地图片文件开始，到分类 Top-K 结果结束。摄像头驱动、V4L2、视频队列和 Qt
界面不属于当前运行链路。这样可以先独立验证模型、预处理、交叉编译器和板端运行库。

## 模块职责

| 模块 | 文件 | 职责 |
|---|---|---|
| 命令行配置 | `cli_options.*` | 解析模型、图片、blob、尺寸、颜色和归一化参数 |
| 推理模块 | `ncnn_classifier.*` | 图片解码、预处理、NCNN 推理、Top-K 排序和标签映射 |
| 性能统计 | `system_metrics.*` | 获取进程峰值 RSS |
| 入口 | `main.cpp` | 错误码、文本／JSON 输出和模块编排 |

## 缓冲区和数据所有权

```text
图片文件
  │ cv::imread 解码
  ▼
cv::Mat image              原始 BGR 像素
  │ from_pixels_resize
  ▼
ncnn::Mat input            227×227×3 float，完成颜色变换和归一化
  │ Extractor::input
  ▼
NCNN 中间 blob             推理过程中由 NCNN allocator 管理
  │ Extractor::extract
  ▼
ncnn::Mat output           1000 类概率
  │ partial_sort
  ▼
vector<Prediction>         仅保留 Top-K 结果
```

输入图像只解码一次，预处理也只执行一次。预热和重复计时只覆盖推理过程，避免把磁盘读取和
图片解码混入神经网络延迟。报告分别输出 `preprocess_ms` 和 `average_inference_ms`。

## 平台决策

- `C++11`：兼容资料中的 GCC 4.9.4。
- `NCNN_VULKAN=OFF`：i.MX6ULL 没有可供 NCNN 使用的 Vulkan 计算设备。
- `NCNN_OPENMP=OFF`：目标 CPU 为单核 Cortex-A7。
- `NCNN_SIMPLEOCV=ON`：只需要图片解码，不引入完整 OpenCV 运行库。
- 默认 1 个推理线程：避免制造无效的线程调度开销。

