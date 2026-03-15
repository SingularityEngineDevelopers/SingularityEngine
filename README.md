# Singularity Engine
## A lightweight, code driven 3D game engine built for performance

Singularity Engine is a minimalist game engine designed for people who prefer writing code 
over clicking over menus. It prioritizes low-overhead, platform agnosticism, fast compilation and straightforward API.
The engine embraces the verbosity of the newest graphics APIs (Vulkan, D3D12) to deliver performance tuned display system.

> [!IMPORTANT]
> This project is in very early development state (Alpha). It's not functional yet.
> The API may go through fundamental changes and core features are still being implemented.

# 🚀 Key Features
- Editor-less Workflow: Build your entire game in your IDE of choice. You don't need to worry about installing ton of heavy SDKs.
- High performance: Minimal abstraction for maximum frame rates.
- Platform agnostic: Support for Windows, Linux and MacOS

> [!WARNING]
> Abstractions for Linux and MacOS aren't implemented yet, but they will be soon.

# 🛠 Gettning Started
## Prerequisities
List of what you need to compile and run the engine
- A compiler capable of compiling C++20
- CMake
- VulkanSDK (only if taking part in engine development around Vulkan API)

> [!NOTE]
> Please note, that the list of prerequisities may change in the future due to the engine's early state of development.

## Quick start
```
# Clone the repository
git clone https://github.com/SingularityEngineDevelopers/SingularityEngine.git

# Build the engine
cd SingularityEngine
cmake --preset vulkan-release .
cmake --build --preset build-vulkan-release .
```

# 🗺 Current Status & Roadmap
- [ ] Custom memory allocators (in progress)
- [ ] 3D Rendering
- [ ] Editor interface (long term goal)

> [!IMPORTANT]
> Current roadmap contains only several entries now. More of them are going to be added
> in the future as more ideas would come.

# 🤝 Contributions
I'm aiming to make this engine one of the fastest indie game engines available. If you'd like to help 
in development or bug fixes, feel free to open a pull request!
1. Fork the project
2. Create a branch
3. Commit
4. Open a Pull Request

# ⚖️ License
This project is licensed under Apache License, version 2.0. See the [LICENSE.md](/LICENSE.md) file for details.

# 📜 Credits
## Open-Source projects
- [SDL3](https://github.com/libsdl-org/SDL): Used for window management and input handling
- [volk](https://github.com/zeux/volk): meta loader for Vulkan to ease development