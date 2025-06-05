# X11剪切板监控工具

这是一个用C语言编写的X11剪切板监控工具，能够实时监控剪切板的变化，并显示剪切板所有者的详细信息。

## 功能特性

- 🔍 实时监控X11剪切板变化
- 📋 获取剪切板所有者窗口信息
- 🏷️ 显示应用程序名称和类别
- 🆔 使用XRes扩展获取进程ID
- 📝 显示进程的完整命令行信息
- ⏰ 实时显示变化时间戳
- 🎯 低CPU占用的轮询机制

## 系统要求

- Linux系统（支持X11）
- X11开发库
- XRes扩展支持
- GCC编译器

## 依赖安装

### Ubuntu/Debian系统
```bash
sudo apt update
sudo apt install build-essential libx11-dev libxres-dev
```

### CentOS/RHEL/Fedora系统
```bash
# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install libX11-devel libXres-devel

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install libX11-devel libXres-devel
```

### Arch Linux
```bash
sudo pacman -S base-devel libx11 libxres
```

## 编译和安装

### 使用CMake构建

#### 快速编译
```bash
mkdir build
cd build
cmake ..
make
```

#### 指定构建类型
```bash
# Debug构建 (包含调试信息)
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Release构建 (优化版本)
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

#### 安装到系统路径
```bash
sudo make install
```

#### 卸载
```bash
make uninstall
```

#### 清理构建文件
```bash
# 清理构建目录
rm -rf build
```

## 使用方法

### 基本运行
```bash
./xclip-monitor
```

### 后台运行并记录日志
```bash
./xclip-monitor > clipboard.log 2>&1 &
```

### 运行时输出示例
```
=== X11剪切板监控工具 ===
功能: 监控剪切板变化并显示所有者进程信息

X11剪切板监控器已启动
当前剪切板所有者: 0x2200003

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
剪切板所有者信息:
窗口ID: 0x2200003
窗口名称: Firefox
应用程序类: firefox
应用程序名: Navigator
进程ID: 12345
命令行: /usr/lib/firefox/firefox --new-instance
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

开始监控剪切板变化... (按Ctrl+C退出)

检测到剪切板变化! 时间: 2024-01-15 14:30:25
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
剪切板所有者信息:
窗口ID: 0x3400004
窗口名称: Visual Studio Code
应用程序类: code
应用程序名: code
进程ID: 23456
命令行: /usr/share/code/code --unity-launch
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## 技术实现

### 核心技术
- **X11 Selection API**: 监控剪切板所有权变化
- **XRes扩展**: 获取X资源和进程ID的映射关系
- **_NET_WM_PID属性**: 备用的进程ID获取方法
- **/proc文件系统**: 读取进程命令行信息

### 工作原理
1. 初始化X11连接和剪切板原子
2. 定期轮询剪切板所有者状态
3. 检测到变化时获取新所有者窗口信息
4. 使用多种方法尝试获取进程ID
5. 从/proc/[pid]/cmdline读取命令行

### 进程ID获取策略
1. **首选方式**: _NET_WM_PID窗口属性
2. **备用方式**: XRes扩展资源匹配
3. **容错处理**: 无法获取时显示相应提示

## 文件结构

```
xclip-monitor/
├── xclip_monitor.c           # 主源代码文件
├── CMakeLists.txt           # CMake构建配置
├── cmake_uninstall.cmake.in # CMake卸载脚本模板
├── test_demo.sh            # 演示脚本
├── README.md               # 项目说明文档
└── build/                  # CMake构建目录
    └── xclip-monitor       # 编译生成的可执行文件
```

## 故障排除

### 常见问题

#### 1. 编译错误：找不到X11头文件
```
fatal error: X11/Xlib.h: No such file or directory
```
**解决方案**: 安装X11开发包
```bash
sudo apt install libx11-dev libxres-dev  # Ubuntu/Debian
```

#### 2. XRes扩展不可用
程序运行时显示"XRes扩展不可用"
**解决方案**: 这是正常情况，程序会自动使用备用方法

#### 3. 无法获取进程ID
某些应用程序可能不设置_NET_WM_PID属性
**解决方案**: 这是应用程序的限制，工具会显示"无法获取"

#### 4. 权限问题
无法读取某些进程的/proc/[pid]/cmdline
**解决方案**: 这是系统安全机制，属于正常现象

### 调试技巧

#### 检查X11环境
```bash
echo $DISPLAY
xdpyinfo | grep -i clipboard
```

#### 查看剪切板状态
```bash
xclip -selection clipboard -o  # 查看当前剪切板内容
xprop -root | grep CLIPBOARD   # 查看剪切板属性
```

## 开发说明

### 代码结构
- `ClipboardMonitor`: 主要数据结构
- `init_clipboard_monitor()`: 初始化函数
- `monitor_clipboard_changes()`: 主监控循环
- `get_window_pid_by_xres()`: 进程ID获取
- `print_window_info()`: 信息显示

### 扩展功能建议
- 添加剪切板内容记录功能
- 支持PRIMARY选择区监控
- 添加过滤和规则配置
- 支持网络远程监控
- 集成系统通知

## 许可证

本项目采用MIT许可证。详见LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request来改进这个工具。

## 作者

由AI助手创建，用于X11剪切板监控需求。
