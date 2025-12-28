// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 zzzz0317

#include "screen_manager.h"
#include <stdio.h>

screen_manager_t g_screen_manager = {0};

// 延迟更新页码的定时器回调
static void update_page_number_timer_cb(lv_timer_t *timer)
{
    page_type_t page = (page_type_t)(intptr_t)lv_timer_get_user_data(timer);
    screen_manager_update_page_number(page, 
        g_screen_manager.current_page_index + 1, 
        g_screen_config.enabled_page_count);
    lv_timer_delete(timer);
}

// 获取页面对应的LVGL屏幕对象指针
lv_obj_t** screen_manager_get_screen(page_type_t page)
{
    switch (page) {
        case PAGE_SYSTEM_INFO:
            printf("screen_manager_get_screen: SYSTEM_INFO -> &ui_SystemInfo\n");
            return &ui_SystemInfo;
        case PAGE_SYSTEM_STATUS:
            printf("screen_manager_get_screen: SYSTEM_STATUS -> &ui_SystemStatus\n");
            return &ui_SystemStatus;
        case PAGE_NETWORK_INFO:
            printf("screen_manager_get_screen: NETWORK_INFO -> &ui_NetworkInfo\n");
            return &ui_NetworkInfo;
        case PAGE_MODEM_INFO:
            printf("screen_manager_get_screen: MODEM_INFO -> &ui_ModemInfo\n");
            return &ui_ModemInfo;
        case PAGE_MODEM_SIGNAL:
            printf("screen_manager_get_screen: MODEM_SIGNAL -> &ui_ModemSignal\n");
            return &ui_ModemSignal;
        default:
            printf("screen_manager_get_screen: UNKNOWN page type %d\n", page);
            return NULL;
    }
}

// 获取页面对应的初始化函数
void (*screen_manager_get_init_func(page_type_t page))(void)
{
    switch (page) {
        case PAGE_SYSTEM_INFO:
            printf("screen_manager_get_init_func: SYSTEM_INFO -> ui_SystemInfo_screen_init\n");
            return ui_SystemInfo_screen_init;
        case PAGE_SYSTEM_STATUS:
            printf("screen_manager_get_init_func: SYSTEM_STATUS -> ui_SystemStatus_screen_init\n");
            return ui_SystemStatus_screen_init;
        case PAGE_NETWORK_INFO:
            printf("screen_manager_get_init_func: NETWORK_INFO -> ui_NetworkInfo_screen_init\n");
            return ui_NetworkInfo_screen_init;
        case PAGE_MODEM_INFO:
            printf("screen_manager_get_init_func: MODEM_INFO -> ui_ModemInfo_screen_init\n");
            return ui_ModemInfo_screen_init;
        case PAGE_MODEM_SIGNAL:
            printf("screen_manager_get_init_func: MODEM_SIGNAL -> ui_ModemSignal_screen_init\n");
            return ui_ModemSignal_screen_init;
        default:
            printf("screen_manager_get_init_func: UNKNOWN page type %d\n", page);
            return NULL;
    }
}

// 获取LVGL动画类型
lv_screen_load_anim_t screen_manager_get_anim_type(void)
{
    switch (g_screen_config.scroll_type) {
        case SCROLL_TYPE_NONE:        return LV_SCR_LOAD_ANIM_NONE;
        case SCROLL_TYPE_FADE:        return LV_SCR_LOAD_ANIM_FADE_IN;
        case SCROLL_TYPE_SLIDE_UP:    return LV_SCR_LOAD_ANIM_MOVE_TOP;
        case SCROLL_TYPE_SLIDE_DOWN:  return LV_SCR_LOAD_ANIM_MOVE_BOTTOM;
        case SCROLL_TYPE_SLIDE_LEFT:  return LV_SCR_LOAD_ANIM_MOVE_LEFT;
        case SCROLL_TYPE_SLIDE_RIGHT: return LV_SCR_LOAD_ANIM_MOVE_RIGHT;
        case SCROLL_TYPE_OVER_UP:     return LV_SCR_LOAD_ANIM_OVER_TOP;
        case SCROLL_TYPE_OVER_DOWN:   return LV_SCR_LOAD_ANIM_OVER_BOTTOM;
        case SCROLL_TYPE_OVER_LEFT:   return LV_SCR_LOAD_ANIM_OVER_LEFT;
        case SCROLL_TYPE_OVER_RIGHT:  return LV_SCR_LOAD_ANIM_OVER_RIGHT;
        default:                      return LV_SCR_LOAD_ANIM_MOVE_TOP;
    }
}

// 定时器回调函数
static void switch_timer_cb(lv_timer_t *timer)
{
    screen_manager_next();
}

// 切换到下一个页面
void screen_manager_next(void)
{
    if (g_screen_config.enabled_page_count == 0) {
        printf("screen_manager_next: No pages enabled!\n");
        return;
    }

    // 计算下一个页面索引
    g_screen_manager.current_page_index =
        (g_screen_manager.current_page_index + 1) % g_screen_config.enabled_page_count;

    page_type_t next_page = g_screen_config.page_order[g_screen_manager.current_page_index];

    printf("screen_manager_next: index=%d, page_type=%d (%s)\n",
           g_screen_manager.current_page_index, next_page, page_type_to_name(next_page));

    lv_obj_t **screen = screen_manager_get_screen(next_page);
    void (*init_func)(void) = screen_manager_get_init_func(next_page);

    printf("screen_manager_next: screen=%p, init_func=%p\n", (void*)screen, (void*)init_func);

    if (screen != NULL && init_func != NULL) {
        // 获取当前页面的停留时间
        int delay = g_screen_config.pages[next_page].delay;
        if (delay <= 0) {
            delay = g_screen_config.default_delay;
        }

        // 更新定时器周期
        if (g_screen_manager.switch_timer != NULL) {
            lv_timer_set_period(g_screen_manager.switch_timer, delay);
        }

        printf("screen_manager_next: Calling _ui_screen_change and init_func\n");
        _ui_screen_change(screen, screen_manager_get_anim_type(), 200, 0, init_func);

        // 延迟更新页码显示 (等待页面初始化完成)
        lv_timer_create(update_page_number_timer_cb, 50, (void*)(intptr_t)next_page);

        printf("Switched to page: %s (%d/%d, delay: %dms)\n",
            page_type_to_name(next_page),
            g_screen_manager.current_page_index + 1,
            g_screen_config.enabled_page_count,
            delay);
    } else {
        printf("ERROR: screen=%p, init_func=%p - cannot switch page!\n",
               (void*)screen, (void*)init_func);
    }
}

// 切换到指定页面
void screen_manager_goto(page_type_t page)
{
    // 查找页面在page_order中的索引
    for (int i = 0; i < g_screen_config.enabled_page_count; i++) {
        if (g_screen_config.page_order[i] == page) {
            g_screen_manager.current_page_index = i;
            
            lv_obj_t **screen = screen_manager_get_screen(page);
            void (*init_func)(void) = screen_manager_get_init_func(page);
            
            if (screen != NULL && init_func != NULL) {
                _ui_screen_change(screen, screen_manager_get_anim_type(), 200, 0, init_func);
            }
            return;
        }
    }
}

// 获取当前页面
page_type_t screen_manager_current(void)
{
    if (g_screen_config.enabled_page_count == 0) {
        return PAGE_COUNT;
    }
    return g_screen_config.page_order[g_screen_manager.current_page_index];
}

// 初始化屏幕管理器
void screen_manager_init(void)
{
    g_screen_manager.current_page_index = -1;  // 还没有显示任何页面
    g_screen_manager.initialized = true;
    g_screen_manager.switch_timer = NULL;
    
    printf("Screen manager initialized with %d pages\n", g_screen_config.enabled_page_count);
}

// 启动自动屏幕切换
void screen_manager_start(void)
{
    if (g_screen_config.enabled_page_count == 0) {
        printf("No pages enabled, screen manager not started\n");
        return;
    }
    
    // 获取第一个页面的延时
    page_type_t first_page = g_screen_config.page_order[0];
    int delay = g_screen_config.pages[first_page].delay;
    if (delay <= 0) {
        delay = g_screen_config.default_delay;
    }
    
    // 创建定时器
    g_screen_manager.switch_timer = lv_timer_create(switch_timer_cb, delay, NULL);
    
    // 立即切换到第一个页面
    screen_manager_next();
    
    printf("Screen manager started\n");
}

// 停止自动屏幕切换
void screen_manager_stop(void)
{
    if (g_screen_manager.switch_timer != NULL) {
        lv_timer_delete(g_screen_manager.switch_timer);
        g_screen_manager.switch_timer = NULL;
    }
    
    printf("Screen manager stopped\n");
}

// 获取页面对应的页码标签
static lv_obj_t* get_page_number_label(page_type_t page)
{
    switch (page) {
        case PAGE_SYSTEM_INFO: return ui_txtSystemInfo1;
        case PAGE_SYSTEM_STATUS: return ui_txtSystemStatus1;
        case PAGE_NETWORK_INFO: return ui_txtNetworkInfo1;
        case PAGE_MODEM_INFO: return ui_txtModemInfo1;
        case PAGE_MODEM_SIGNAL: return ui_txtModemSignal1;
        default: return NULL;
    }
}

// 更新当前页面的页码显示
void screen_manager_update_page_number(page_type_t page, int current, int total)
{
    lv_obj_t* label = get_page_number_label(page);
    if (label != NULL) {
        char buf[16];
        snprintf(buf, sizeof(buf), "(%d/%d)", current, total);
        lv_label_set_text(label, buf);
    }
}
