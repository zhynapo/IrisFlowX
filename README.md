# IrisFlowX

IrisFlowX is a visual image processing workflow design tool built on Qt6 and a graphical node editor framework (QtNodes). It provides users with an intuitive drag-and-drop environment for image algorithm modeling, simplifying the creation and debugging of computer vision pipelines.

## Features

- Visual dataflow programming with node-based interface
- Support for loading/saving images and image preview
- Integration with OpenCV for image processing operations
- Support for advanced analysis functions like ONNX model inference
- Configurable parameters for processing nodes (defined in JSON)
- Plugin-based architecture supporting dynamic loading of node modules
- Real-time image processing with low-latency rendering

## Prerequisites

- CMake 3.21+
- Qt6.4+ (preferably MSVC2022 64-bit)
- Visual Studio 2022 (or compatible MSVC compiler)
- OpenCV 4.x (world build with all modules)
- IrisX SDK (commercial image processing library)
- Python (for running `gen.py` to auto-generate code)

## Building

### Setup Instructions

1. Clone the repository and initialize submodules:
   ```bash
   git clone <repo-url>
   git submodule update --init --recursive
   ```

2. Create a build directory and configure:
   ```bash
   mkdir build && cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

3. Build the project:
   ```bash
   cmake --build . --config Release
   ```

### Notes on Dependencies

The project requires specific paths for Qt, OpenCV, and IrisX. Modify the [CMakeLists.txt](CMakeLists.txt) file according to your local installation paths:

- Qt6 path: By default set to `"C:/Qt/6.9.3/msvc2022_64"`
- OpenCV path: By default set to `"D:/000_Workspace/300_lib/opencv/build_world/install"`
- IrisX path: By default set to `"C:/Program Files (x86)/IrisX"`

## Architecture

The project follows several design patterns:

- **Model-View-Controller (MVC)**: Standard Qt architecture for UI and logic separation
- **Plugin Pattern**: Dynamic loading of nodes via PluginLoader
- **Factory Pattern**: BaseNodeRegistry for registering and creating node instances
- **Data Flow Pattern**: Graphical data flow model based on QtNodes

### Components

- `MainWindow`: Manages the main interface and scene view
- `FlowCore`: Provides base models, node factories, and parameter editors
- `nodes`: Contains specific image processing node implementations
- `QtNodes`: Provides graphics scene, connections, and node rendering support
- External libraries: OpenCV and IrisX for image processing capabilities

## Structure

```
IrisFlowX/
├── 3rdparty/                 # Third-party dependencies
│   └── nodeeditor/           # QtNodes graphical node editor
├── cmake/                    # CMake helper scripts
├── src/
│   ├── FlowCore/             # Core flow functionality
│   ├── app/                  # Main application files
│   ├── include/              # Global includes
│   └── nodes/                # Node implementations
│       ├── Analytics/        # Analysis nodes
│       ├── Processing/       # Processing nodes
│       ├── params/           # Parameter configuration files
│       └── utils/            # Utility nodes
├── CMakeLists.txt            # Main build configuration
└── README.md
```

## Usage

IrisFlowX allows users to create image processing workflows by dragging and connecting nodes. Each node represents an image processing operation such as filtering, transformation, or feature detection. Parameters for each operation can be adjusted in real-time.

The application supports various image processing nodes:
- Image loading and saving
- Filtering operations (blur, sharpen, etc.)
- Feature detection algorithms (ORB, SIFT, SURF, etc.)
- Geometric transformations
- Advanced analytics

## Node Development

New nodes can be created using the JSON-based code generation system:
1. Define the node properties in a JSON configuration file
2. Run the `gen.py` script to generate C++ code
3. Register the node in the plugin system

Nodes are organized by functionality:
- Processing: Basic image processing operations
- Analytics: Advanced analysis operations
- Utils: Utility operations

## Contributing

We welcome contributions to the IrisFlowX project. Please follow these steps:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Qt6 for the GUI framework
- QtNodes for the node editor framework
- OpenCV for computer vision algorithms
- Special thanks to the contributors of the node editor library