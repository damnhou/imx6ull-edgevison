# 模型目录

模型权重不提交到仓库。运行 `scripts/download_squeezenet.ps1` 或
`scripts/download_squeezenet.sh` 下载 NCNN 示例模型和 ImageNet 标签，并生成 SHA-256 清单。

默认参数对应 NCNN SqueezeNet 1.1：

- 输入：`data`，`227 x 227`
- 输出：`prob`
- 图像通道：BGR
- 均值：`104,117,123`
- 归一化系数：`1,1,1`

更换模型时必须同步修改尺寸、通道顺序、均值、归一化系数和 blob 名称。不能仅替换
`.param/.bin` 文件而继续沿用 SqueezeNet 的预处理参数。

