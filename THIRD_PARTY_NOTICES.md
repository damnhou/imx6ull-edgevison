# 第三方组件说明

- Tencent NCNN：BSD 3-Clause License，工程固定使用 `20230816` 标签作为首个兼容基线。
- `nihui/ncnn-assets`：仅通过下载脚本取得 SqueezeNet NCNN 模型，使用时应同时检查上游许可。
- ImageNet `synset_words.txt`：从 NCNN `20230816` 示例目录下载。
- 参考教程：Angus《在嵌入式开发板上部署深度神经网络》第一、二篇，采用
  CC BY-NC-SA 4.0。工程重新实现了命令行、预处理参数化、性能统计和正点原子工具链适配，
  不把教程或 NCNN 的工作声明为原创。

