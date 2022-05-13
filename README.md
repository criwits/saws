# SAWS: Simple Aircraft War Server

SAWS（Simple Aircraft War Server，简单飞机大战服务器）是一个使用纯 C 语言写成的，可以在 *nix 系统上运行的，高性能的适用于「飞机大战」游戏的后端服务器。

SAWS 是哈尔滨工业大学（深圳）的《面向对象的软件构造实践》课程项目的一部分。

## 源码结构

SAWS 是一个使用 CMake 构建的纯 C 语言项目。
其源码结构与诸多经典的 C 语言项目类似。

- `src`：`.c` 代码所在的文件夹。
除了程序入口文件 `main.c` 外，其他各模块的代码都应放在 `src` 之下的子文件夹中。
- `include`：`.h` 头文件所在的文件夹。
其之下的 `common` 文件夹内的头文件可以直接用 `#include <foo.h>` 来包含。
其他各子文件夹内的头文件，则要指明子路径，比如 `#include <foo/bar.h>`
- `CMakeLists.txt`：CMake 的配置文件。

## 通信约定

SAWS 是 [SAWA](https://git.hit.edu.cn/criwits/sawa) 的后端。

## 编译运行指南

本项目使用 JetBrains CLion 开发。
因此，如果使用 CLion 直接打开整个项目，那么应该不需要进行额外的配置。

本项目使用了下面这些动态链接库：

- libwebsockets

## 许可

MIT