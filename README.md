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

### 请求登入服务器

```json
{
  "type": "user_query",
  "username": "<用户名>",
  "password": "<密码>"
}
```

#### 返回

如果 uid 为 -1 表示失败。

```json
{
  "type": "user_query_response",
  "uid": -1
}
````

### 请求下载房间列表

```json
{
  "type": "room_info"
}
```

返回：（如果没有可用房间，`rooms` 数组就是空的）

```json
{
  "type": "room_info_response",
  "rooms": [
    {
      "room_id": 1,
      "difficulty": 0
    },
    {
      "room_id": 3,
      "difficulty": 2
    }
  ]
}
```

### 请求新建房间

```json
{
  "type": "create_room",
  "difficulty": 2
}
```

返回创建成功的房间号：

```json
{
  "type": "create_room_response",
  "room_id": 4
}
```

### 加入房间

```json
{
  "type": "join_room",
  "room_id": 4
}
```

#### 成功

```json
{
  "type": "join_room_response",
  "success": true
}
```

#### 失败

```json
{
  "type": "join_room_response",
  "success": false
}
```

#### 告知房主

```json
{
  "type": "room_ready"
}
```

### 上传屏幕信息

```json
{
  "type": "resolution",
  "width": 1080,
  "height": 1920
}
```

返回双方协商后的尺寸信息和难度 => 双方接到这条消息时开始游戏

- `valid_height`：你这个客户端实际能用来渲染的画面高度（这个值用来替代你的 Graphics.screenHeight）
- `scale`：你得到的坐标都要乘上这个数才能用。你上报坐标之前都要除以这个数。

```json
{
  "type": "resolution_response",
  "ratio": 1.6,
  "enemy_bullet_power": 30,
  "boss_bullet_power": 45
}
```
---

### 下发 NPC 信息

```json
{
  "type": "npc_spawn",
  "mob": "mob",
  "id": 14,
  "location_x": 134,
  "location_y": 476,
  "speed_x": 0,
  "speed_y": 3,
  "hp": 10
}
```

### 上传移动信息
（坐标是缩放前的）
```json
{
  "type": "movement",
  "new_x": 753,
  "new_y": 255
}
```

队友将得到：

```json
{
  "type": "teammate_movement",
  "new_x": 753,
  "new_y": 255
}
```

### 上传击伤信息
```json
{
  "type": "damage",
  "id": 14,
  "hp_decrease": 10
}
```

### 下发得分和删除飞机信息
```json
{
  "type": "score",
  "remove": 14,
  "score": 20
}
```

### 上传受伤信息
```json
{
  "type": "hurt",
  "damage": 20
}
```

## 编译运行指南

本项目使用 JetBrains CLion 开发。
因此，如果使用 CLion 直接打开整个项目，那么应该不需要进行额外的配置。

本项目使用了下面这些库：

- libwebsockets

## 许可

MIT