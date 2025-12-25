// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 zzzz0317

#ifndef SCREEN_CONFIG_H
#define SCREEN_CONFIG_H

#include <stdbool.h>

#define MAX_PAGES 10
#define CONFIG_FILE "/etc/config/xgp_screen"
#define BRIGHTNESS_PATH "/sys/class/backlight/backlight/brightness"
#define MAX_BRIGHTNESS_PATH "/sys/class/backlight/backlight/max_brightness"
#define BRIGHTNESS_PATH_ALT "/sys/class/leds/lcd-backlight/brightness"

// 滚动类型
typedef enum {
    SCROLL_TYPE_NONE,           // 无动画
    SCROLL_TYPE_FADE,           // 淡入淡出
    SCROLL_TYPE_SLIDE_UP,       // 向上滑动
    SCROLL_TYPE_SLIDE_DOWN,     // 向下滑动
    SCROLL_TYPE_SLIDE_LEFT,     // 向左滑动
    SCROLL_TYPE_SLIDE_RIGHT,    // 向右滑动
    SCROLL_TYPE_OVER_UP,        // 从下往上覆盖
    SCROLL_TYPE_OVER_DOWN,      // 从上往下覆盖
    SCROLL_TYPE_OVER_LEFT,      // 从右往左覆盖
    SCROLL_TYPE_OVER_RIGHT      // 从左往右覆盖
} scroll_type_t;

// 页面类型
typedef enum {
    PAGE_SYSTEM_INFO,
    PAGE_SYSTEM_STATUS,
    PAGE_NETWORK_INFO,
    PAGE_MODEM_INFO,
    PAGE_MODEM_SIGNAL,
    PAGE_COUNT
} page_type_t;

// 页面配置
typedef struct {
    page_type_t type;
    bool enabled;
    int delay;      // 停留时间 (毫秒)
    int order;      // 显示顺序
} page_config_t;

// 屏幕配置
typedef struct {
    bool enabled;           // 是否启用屏幕
    scroll_type_t scroll_type;  // 滚动类型
    int default_delay;      // 默认停留时间
    bool skip_boot;         // 跳过启动画面
    bool skip_splash;       // 跳过闪屏画面
    page_config_t pages[PAGE_COUNT];
    int enabled_page_count;
    page_type_t page_order[PAGE_COUNT];  // 按顺序排列的启用页面
} screen_config_t;

// 全局配置
extern screen_config_t g_screen_config;

// 函数声明
int load_screen_config(void);
const char* page_type_to_name(page_type_t type);
page_type_t page_name_to_type(const char* name);
scroll_type_t scroll_name_to_type(const char* name);
void init_default_config(void);

#endif // SCREEN_CONFIG_H
