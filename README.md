# URay Engine

URay Engine is a work-in-progress game engine and scene editor built with C++20 and Vulkan 1.3. It includes an editor workflow, an asset pipeline, and a rendering pipeline.

> This project is under active development. Features and APIs may change.

## Features

<img width="1919" height="1030" alt="image" src="https://github.com/user-attachments/assets/98a7085d-53fc-4c6e-a596-624ffead6ac1" />

- Core Layer
  - Virtual file system
  - Input action system
  - Log system
  - Performance analytics (both CPU and GPU)
  - Event Ray (event based architecture)
  - Runtime type identification
- Render Layer
  - Vulkan 1.3
  - Multiple attachment rendering (scene color attachment, object id attachment, ...)
  - Shader relfection (SPIR-V)
  - Clustered Light Culling
- Engine Layer
  - Asset pipeline (texture, font, material, shader, mesh)
  - Component architecture
- Editor Layer
  - Retained widget system
  - Pick system based object id
  - Play in editor

## Tech Stack

| Area | Technology |
| --- | --- |
| Languages | C++20 |
| Graphics API | Vulkan 1.3 |
| Build System | CMake, Ninja |
| Windowing / Input | GLFW |
| Shaders | HLSL, DXC, SPIR-V Reflect |
| Fonts | FreeType |
| Serialization | yaml-cpp |
| Scripting | .NET 10 |

## Requirements

- Windows
- MSVC / Visual Studio C++ Build Tools
- CMake 3.20 or later
- Ninja
- Vulkan SDK and a Vulkan 1.3-capable GPU driver
- .NET SDK 10.0.12
- Internet access during the first CMake configuration, for `yaml-cpp`

## Build

```powershell
cmake --preset Sandbox-Debug
cmake --build --preset Sandbox-Debug
```

---
***This engine is under development, so it may contain bugs and may not run correctly in certain environments.***
