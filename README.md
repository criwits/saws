# SAWS: Simple Aircraft War Server

SAWS（Simple Aircraft War Server，简单飞机大战服务器）是一个使用纯 C 语言写成的，可以在 Linux 系统上运行的，~~（也许）高性能的~~ 适用于简单「飞机大战」游戏的后端服务器。

SAWS 是哈尔滨工业大学（深圳）的《面向对象的软件构造实践》课程项目的一部分。~~但 SAWS 是面向过程（事件循环）的。~~

## 源码结构

SAWS 是一个使用 CMake 构建的纯 C 语言项目。

- `src`：`.c` 代码所在的文件夹。
除了程序入口文件 `main.c` 外，其他各模块的代码都应放在 `src` 之下的子文件夹中。
- `include`：`.h` 头文件所在的文件夹。
其之下的 `common` 文件夹内的头文件可以直接用 `#include <foo.h>` 来包含。
其他各子文件夹内的头文件，则要指明子路径，比如 `#include <foo/bar.h>`
- `CMakeLists.txt`：CMake 的配置文件。

## 通信约定

SAWS 是 [SAWA](https://git.hit.edu.cn/criwits/sawa) 的后端，与 SAWA 以一致的通信方式通信。

协议栈：WebSocket，信息载体：JSON

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

返回双方协商后的尺寸信息和难度，双方接到这条消息时开始游戏：

```json
{
  "type": "game_start",
  "ratio": 1.6
}
```
---

### 上传 NPC 信息

只能由房主上传。

```json
{
  "type": "npc_upload",
  "mob": 0,
  "id": 14,
  "location_x": 134,
  "location_y": 476,
  "speed_x": 0,
  "speed_y": 3,
  "hp": 10
}
```

### 下发 NPC 信息

只会发给房客。

```json
{
  "type": "npc_spawn",
  "mob": 0,
  "id": 14,
  "location_x": 134,
  "location_y": 476,
  "speed_x": 0,
  "speed_y": 3,
  "hp": 10
}
```

### 上传移动信息

注意坐标是缩放前的：

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
  "hp_decrease": 10,
  "location_x": 514,
  "location_y": 114
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
如果 remove 为 -1，则无条件得分（炸弹道具）。


### 无条件消除飞机

对应「飞机飞出边界」事件，由房主负责。

```json
{
  "type": "remove_aircraft",
  "remove": 14
}
```

### 道具生成

0 = Blood, 1 = Bomb, 2 = Bullet

```json
    {
      "type": "prop_spawn",
      "props": [
        {
          "id": 1,
          "kind": 0,
          "location_x": 514,
          "location_y": 114
        },
        {
          "id": 2,
          "kind": 0,
          "location_x": 143,
          "location_y": 241
        }
      ]
    }
```

### 道具无条件移除

```json
{
  "type": "remove_prop",
  "remove": 14
}
```

### 道具碰撞

```json
{
  "type": "prop_action",
  "id": 14
}
```

### 道具生效

炸弹生效：接到此消息则清屏，然后给自己加上 `add_score` 的分数

```json
{
  "type": "bomb_action",
  "add_score": 1540
}
```

加血生效：接到此消息，给自己无条件加血 `add_hp`（注意封顶）

```json
{
  "type": "blood_action"
}
```

子弹道具生效：接到此消息，若 `target` 为 `true`，则给自己启用效果；否则，给队友使用效果。

```json
{
  "type": "bullet_action",
  "target": true
}
```

### 本方死亡、按下返回键，通知服务器结束本局游戏

任一一方发送此消息，服务器将结束本局游戏。

`reason` 字段：0 = 因发信方死亡结束；1 = 因发信方按返回键结束。

```json
{
  "type": "game_end_request",
  "reason": 1
}
```

### 服务器通知双方结束游戏

`this_score` 为本方分数，`teammate_score` 为对方分数。`reason` 是结束游戏的原因。

```json
{
  "type": "game_end",
  "reason": 1,
  "this_score": 1700,
  "teammate_score": 1870
}
```


## 编译运行指南

本项目目前只能运行在 Linux 平台上。使用 CMake 构建运行。

需要下面这些库：

- libwebsockets (4.3-stable)
- libmariadb

## 许可

MIT