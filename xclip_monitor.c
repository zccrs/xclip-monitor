#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

// XRes扩展必需支持
#include <X11/extensions/XRes.h>

typedef struct {
    Display *display;
    Window root;
    Atom clipboard_atom;
    Atom clipboard_manager_atom;
    Window clipboard_owner;
} ClipboardMonitor;

// 读取进程的命令行信息
char* get_process_cmdline(pid_t pid) {
    char proc_path[256];
    FILE *file;
    static char cmdline[1024];
    size_t len;

    snprintf(proc_path, sizeof(proc_path), "/proc/%d/cmdline", pid);

    file = fopen(proc_path, "r");
    if (!file) {
        return NULL;
    }

    len = fread(cmdline, 1, sizeof(cmdline) - 1, file);
    fclose(file);

    if (len <= 0) {
        return NULL;
    }

    cmdline[len] = '\0';

    // 将NULL分隔符替换为空格，使命令行更易读
    for (size_t i = 0; i < len - 1; i++) {
        if (cmdline[i] == '\0') {
            cmdline[i] = ' ';
        }
    }

    return cmdline;
}

// 获取窗口所属的进程ID (仅使用XRes扩展)
pid_t get_window_pid(Display *display, Window window) {
    pid_t result_pid = -1;
    int event_base, error_base;

    // 检查XRes扩展是否可用
    if (!XResQueryExtension(display, &event_base, &error_base)) {
        fprintf(stderr, "XRes扩展不可用\n");
        return -1;
    }

    // 直接使用XResQueryClientIds获取窗口所属客户端的PID
    XResClientIdSpec client_spec;
    client_spec.client = window;  // 直接使用窗口ID
    client_spec.mask = XRES_CLIENT_ID_PID_MASK;

    XResClientIdValue *client_ids;
    long num_ids;

    if (XResQueryClientIds(display, 1, &client_spec, &num_ids, &client_ids) == Success) {
        // 遍历返回的ID值，查找PID
        for (long i = 0; i < num_ids; i++) {
            if (client_ids[i].spec.mask & XRES_CLIENT_ID_PID_MASK) {
                // 使用专门的函数获取PID
                pid_t pid = XResGetClientPid(&client_ids[i]);
                if (pid > 0) {
                    result_pid = pid;
                    break;
                }
            }
        }

        // 使用正确的清理函数
        XResClientIdsDestroy(num_ids, client_ids);
    } else {
        fprintf(stderr, "无法通过XRes获取进程ID\n");
    }

    return result_pid;
}

// 打印窗口和进程信息
void print_window_info(Display *display, Window window) {
    XTextProperty window_name;
    XClassHint class_hint;
    pid_t pid;
    char *cmdline;

    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("剪切板所有者信息:\n");
    printf("窗口ID: 0x%lx\n", window);

    // 获取窗口名称
    if (XGetWMName(display, window, &window_name) && window_name.value) {
        printf("窗口名称: %s\n", (char*)window_name.value);
        XFree(window_name.value);
    }

    // 获取窗口类信息
    if (XGetClassHint(display, window, &class_hint)) {
        printf("应用程序类: %s\n", class_hint.res_class ? class_hint.res_class : "未知");
        printf("应用程序名: %s\n", class_hint.res_name ? class_hint.res_name : "未知");
        if (class_hint.res_class) XFree(class_hint.res_class);
        if (class_hint.res_name) XFree(class_hint.res_name);
    }

    // 获取进程ID
    pid = get_window_pid(display, window);
    if (pid > 0) {
        printf("进程ID: %d\n", pid);

        // 获取命令行信息
        cmdline = get_process_cmdline(pid);
        if (cmdline) {
            printf("命令行: %s\n", cmdline);
        } else {
            printf("命令行: 无法获取\n");
        }
    } else {
        printf("进程ID: 无法获取\n");
    }

    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
}

// 初始化剪切板监控器
int init_clipboard_monitor(ClipboardMonitor *monitor) {
    monitor->display = XOpenDisplay(NULL);
    if (!monitor->display) {
        fprintf(stderr, "无法打开X显示\n");
        return -1;
    }

    monitor->root = DefaultRootWindow(monitor->display);
    monitor->clipboard_atom = XInternAtom(monitor->display, "CLIPBOARD", False);
    monitor->clipboard_manager_atom = XInternAtom(monitor->display, "CLIPBOARD_MANAGER", False);

    // 监听剪切板所有权变化事件
    XSelectInput(monitor->display, monitor->root, PropertyChangeMask);

    // 获取当前剪切板所有者
    monitor->clipboard_owner = XGetSelectionOwner(monitor->display, monitor->clipboard_atom);

    printf("X11剪切板监控器已启动\n");
    printf("当前剪切板所有者: 0x%lx\n\n", monitor->clipboard_owner);

    if (monitor->clipboard_owner != None) {
        print_window_info(monitor->display, monitor->clipboard_owner);
    }

    return 0;
}

// 清理资源
void cleanup_clipboard_monitor(ClipboardMonitor *monitor) {
    if (monitor->display) {
        XCloseDisplay(monitor->display);
    }
}

// 主监控循环
void monitor_clipboard_changes(ClipboardMonitor *monitor) {
    XEvent event;
    Window new_owner;

    printf("开始监控剪切板变化... (按Ctrl+C退出)\n\n");

    while (1) {
        // 检查剪切板所有者是否发生变化
        new_owner = XGetSelectionOwner(monitor->display, monitor->clipboard_atom);

        if (new_owner != monitor->clipboard_owner) {
            monitor->clipboard_owner = new_owner;

            if (new_owner != None) {
                printf("检测到剪切板变化! 时间: ");
                fflush(stdout);
                system("date '+%Y-%m-%d %H:%M:%S'");

                print_window_info(monitor->display, new_owner);
            } else {
                printf("剪切板被清空\n\n");
            }
        }

        // 处理X事件 (虽然我们主要通过轮询检测变化)
        while (XPending(monitor->display)) {
            XNextEvent(monitor->display, &event);
        }

        // 短暂休眠避免过度消耗CPU
        usleep(100000); // 100ms
    }
}

int main() {
    ClipboardMonitor monitor = {0};

    printf("=== X11剪切板监控工具 ===\n");
    printf("功能: 监控剪切板变化并显示所有者进程信息\n\n");

    if (init_clipboard_monitor(&monitor) != 0) {
        return 1;
    }

    monitor_clipboard_changes(&monitor);

    cleanup_clipboard_monitor(&monitor);
    return 0;
}
