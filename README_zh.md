# IrisFlowX

IrisFlowX 是一个基于 Qt6 和图形化节点编辑框架（QtNodes）构建的可视化图像处理流程设计工具，旨在为用户提供直观的拖拽式图像算法建模环境。

## 功能特点

- 节点式图像处理流程编辑（数据流编程）
- 支持加载/保存图像、图像预览
- 集成 OpenCV 进行图像处理运算
- 支持 ONNX 模型推理等高级分析功能
- 参数可配置的处理节点（JSON 定义参数结构）
- 插件化架构支持动态加载节点模块

## 环境要求

- CMake 3.21+
- Qt6.4+ （推荐 MSVC2022 64位）
- Visual Studio 2022 （或兼容 MSVC 编译器）
- OpenCV 4.x （完整构建版，包含所有模块）
- IrisX SDK （商业图像处理库）
- Python （用于运行 `gen.py` 自动生成代码）

## 构建说明

### 安装步骤

1. 克隆仓库并初始化子模块：
   ```bash
   git clone <repo-url>
   git submodule update --init --recursive
   ```

2. 创建构建目录并配置：
   ```bash
   mkdir build && cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

3. 构建项目：
   ```bash
   cmake --build . --config Release
   ```

### 关于依赖项

项目需要特定路径下的 Qt、OpenCV 和 IrisX。请根据您的本地安装路径修改 [CMakeLists.txt](CMakeLists.txt)：

- Qt6 路径：默认设置为 `"C:/Qt/6.9.3/msvc2022_64"`
- OpenCV 路径：默认设置为 `"D:/000_Workspace/300_lib/opencv/build_world/install"`
- IrisX 路径：默认设置为 `"C:/Program Files (x86)/IrisX"`

## 架构设计

项目采用了多种设计模式：

- **模型-视图-控制器 (MVC)**：Qt 标准架构，用于 UI 与逻辑分离
- **插件模式**：通过 PluginLoader 加载动态节点模块
- **工厂模式**：BaseNodeRegistry 用于注册和创建节点实例
- **数据流模式**：基于 QtNodes 的图形化数据流模型

### 组件交互

- `MainWindow`：管理主界面和场景视图
- `FlowCore`：提供基础模型、节点工厂和参数编辑器
- `nodes` 模块：包含具体图像处理节点实现
- `QtNodes`：提供图形场景、连接、节点渲染等底层支持
- `OpenCV` 和 `IrisX`：作为外部库提供图像处理能力

## 项目结构

```
IrisFlowX/
├── 3rdparty/                 # 第三方依赖
│   └── nodeeditor/           # QtNodes 图形节点编辑器
├── cmake/                    # CMake 辅助脚本
├── src/
│   ├── FlowCore/             # 核心流程功能
│   ├── app/                  # 主应用程序文件
│   ├── include/              # 全局包含文件
│   └── nodes/                # 节点实现
│       ├── Analytics/        # 分析节点
│       ├── Processing/       # 处理节点
│       ├── params/           # 参数配置文件
│       └── utils/            # 工具节点
├── CMakeLists.txt            # 主构建配置
└── README.md
```

## 使用方法

IrisFlowX 允许用户通过拖拽和连接节点创建图像处理工作流。每个节点代表一个图像处理操作，例如过滤、变换或特征检测。可以实时调整每个操作的参数。

应用程序支持各种图像处理节点：
- 图像加载和保存
- 过滤操作（模糊、锐化等）
- 特征检测算法（ORB、SIFT、SURF 等）
- 几何变换
- 高级分析

## 节点开发

新节点可以通过基于 JSON 的代码生成系统创建：
1. 在 JSON 配置文件中定义节点属性
2. 运行 `gen.py` 脚本生成 C++ 代码
3. 在插件系统中注册节点

节点按功能组织：
- Processing：基本图像处理操作
- Analytics：高级分析操作
- Utils：实用程序操作

## 贡献

我们欢迎对 IrisFlowX 项目的贡献。请遵循以下步骤：

1. Fork 仓库
2. 创建功能分支
3. 进行修改
4. 提交 Pull Request

## 许可证

该项目根据 MIT 许可证授权 - 详见 [LICENSE](LICENSE) 文件。

## 致谢

- Qt6 提供了 GUI 框架
- QtNodes 提供了节点编辑器框架
- OpenCV 提供了计算机视觉算法
- 特别感谢节点编辑器库的贡献者们