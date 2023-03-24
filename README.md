## 乱七八糟的图形与物理知识学习

#### 〇、前提：

##### 1. 根目录名格式要求：

该项目使用 vcpkg 管理大部分第三方库，vcpkg 的 manifest 模式要求项目名必须为全小写、横杠模式(例如: proj-vk-test)。由于该项目在 CMake 中定义根目录的目录名为项目名，故目录名也必须用全小写、横杠模式。

##### 2. 编译器：

该项目基于 Vulkan - SDL3 - PhysX，PhysX 不支持 Mingw，若想要在 Windows 平台上编译，请使用 MSVC 或 Clang。

#### 一、编译环境：

##### 1. 配置 vulkan sdk

windows:

1. 前往 [LunarXchange (lunarg.com)](https://vulkan.lunarg.com/sdk/home)
2. 下载最新 vulkan SDK Installer 并安装
3. 安装完成后可运行安装目录下的 `Bin\vkcube.exe` 或在命令行运行 `vulkaninfo` 测试环境

linux:

- 可以类似 windows 在 [LunarXchange (lunarg.com)](https://vulkan.lunarg.com/sdk/home) 上获取 SDK 并安装
- 或者执行如下指令

```sh
#ubuntu16  vulkan版本1.126
wget -qO - http://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add -
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.1.126-xenial.list http://packages.lunarg.com/vulkan/1.1.126/lunarg-vulkan-1.1.126-xenial.list
sudo apt update
sudo apt install vulkan-sdk
```

##### 2. 配置 vcpkg

详见：https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md

#### 二、CMake 设置

设 %ProjDir% 为当前项目的根目录，做如下操作：

- 在 ``%ProjDir%/..`` 目录新建 `Config.cmake` 文件，该文件可填入 CMake 的设置项，其内容会在根项目 `Project(${ProjDirName})` 创建前被执行。

需要设置的 CMake 变量：

```cmake
# --------------------------------- 必要的 --
set(CMAKE_TOOLCHAIN_FILE "{}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file") # vcpkg 目录

# --------------------------------- 可选的 --
```
