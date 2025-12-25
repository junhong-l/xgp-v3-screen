// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 zzzz0317

#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "screen_config.h"
#include "ui/ui.h"

// 屏幕管理器状态
typedef struct {
    int current_page_index;     // 当前页面在page_order中的索引
    bool initialized;
    lv_timer_t *switch_timer;   // 自动切换定时器
} screen_manager_t;

extern screen_manager_t g_screen_manager;

// 初始化屏幕管理器
void screen_manager_init(void);

// 启动自动屏幕切换
void screen_manager_start(void);

// 停止自动屏幕切换
void screen_manager_stop(void);

// 切换到下一个页面
void screen_manager_next(void);

// 切换到指定页面
void screen_manager_goto(page_type_t page);

// 获取当前页面
page_type_t screen_manager_current(void);

// 获取LVGL动画类型
lv_screen_load_anim_t screen_manager_get_anim_type(void);

// 获取页面对应的LVGL屏幕对象
lv_obj_t** screen_manager_get_screen(page_type_t page);

// 获取页面对应的初始化函数
void (*screen_manager_get_init_func(page_type_t page))(void);

// 更新当前页面的页码显示
void screen_manager_update_page_number(page_type_t page, int current, int total);

#endif // SCREEN_MANAGER_H
