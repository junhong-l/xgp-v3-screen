// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 zzzz0317

#include "screen_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

// 全局配置实例
screen_config_t g_screen_config;

// 初始化默认配置
void init_default_config(void)
{
    g_screen_config.enabled = true;
    g_screen_config.scroll_type = SCROLL_TYPE_SLIDE;
    g_screen_config.default_delay = 5000;
    g_screen_config.skip_boot = false;
    g_screen_config.skip_splash = false;
    
    // 默认所有页面启用
    for (int i = 0; i < PAGE_COUNT; i++) {
        g_screen_config.pages[i].type = (page_type_t)i;
        g_screen_config.pages[i].enabled = true;
        g_screen_config.pages[i].delay = 5000;
        g_screen_config.pages[i].order = i + 1;
        g_screen_config.page_order[i] = (page_type_t)i;
    }
    g_screen_config.enabled_page_count = PAGE_COUNT;
}

// 页面类型转换函数
const char* page_type_to_name(page_type_t type)
{
    switch (type) {
        case PAGE_SYSTEM_INFO: return "system_info";
        case PAGE_SYSTEM_STATUS: return "system_status";
        case PAGE_NETWORK_INFO: return "network_info";
        case PAGE_MODEM_INFO: return "modem_info";
        case PAGE_MODEM_SIGNAL: return "modem_signal";
        default: return "unknown";
    }
}

page_type_t page_name_to_type(const char* name)
{
    if (strcmp(name, "system_info") == 0) return PAGE_SYSTEM_INFO;
    if (strcmp(name, "system_status") == 0) return PAGE_SYSTEM_STATUS;
    if (strcmp(name, "network_info") == 0) return PAGE_NETWORK_INFO;
    if (strcmp(name, "modem_info") == 0) return PAGE_MODEM_INFO;
    if (strcmp(name, "modem_signal") == 0) return PAGE_MODEM_SIGNAL;
    return PAGE_COUNT; // 无效
}

scroll_type_t scroll_name_to_type(const char* name)
{
    if (strcmp(name, "none") == 0) return SCROLL_TYPE_NONE;
    if (strcmp(name, "fade") == 0) return SCROLL_TYPE_FADE;
    if (strcmp(name, "slide_up") == 0 || strcmp(name, "slide") == 0) return SCROLL_TYPE_SLIDE_UP;
    if (strcmp(name, "slide_down") == 0) return SCROLL_TYPE_SLIDE_DOWN;
    if (strcmp(name, "slide_left") == 0) return SCROLL_TYPE_SLIDE_LEFT;
    if (strcmp(name, "slide_right") == 0) return SCROLL_TYPE_SLIDE_RIGHT;
    if (strcmp(name, "over_up") == 0) return SCROLL_TYPE_OVER_UP;
    if (strcmp(name, "over_down") == 0) return SCROLL_TYPE_OVER_DOWN;
    if (strcmp(name, "over_left") == 0) return SCROLL_TYPE_OVER_LEFT;
    if (strcmp(name, "over_right") == 0) return SCROLL_TYPE_OVER_RIGHT;
    return SCROLL_TYPE_SLIDE_UP; // 默认
}

// 去除字符串首尾空白
static char* trim(char* str)
{
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// 去除引号
static void strip_quotes(char* str)
{
    size_t len = strlen(str);
    if (len >= 2 && str[0] == '\'' && str[len-1] == '\'') {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

// 比较页面order用于排序
static int compare_page_order(const void* a, const void* b)
{
    page_type_t type_a = *(page_type_t*)a;
    page_type_t type_b = *(page_type_t*)b;
    return g_screen_config.pages[type_a].order - g_screen_config.pages[type_b].order;
}

// 重新计算启用的页面顺序
static void recalculate_page_order(void)
{
    g_screen_config.enabled_page_count = 0;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (g_screen_config.pages[i].enabled) {
            g_screen_config.page_order[g_screen_config.enabled_page_count++] = (page_type_t)i;
        }
    }
    // 按order排序
    qsort(g_screen_config.page_order, g_screen_config.enabled_page_count, 
          sizeof(page_type_t), compare_page_order);
}

// 解析UCI配置文件
int load_screen_config(void)
{
    FILE *fp;
    char line[256];
    char current_section[64] = "";
    char current_section_name[64] = "";
    
    init_default_config();
    
    fp = fopen(CONFIG_FILE, "r");
    if (fp == NULL) {
        printf("Config file not found, using defaults\n");
        return 0;
    }
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        char* trimmed = trim(line);
        
        // 跳过空行和注释
        if (trimmed[0] == '\0' || trimmed[0] == '#') continue;
        
        // 检查是否是config行
        if (strncmp(trimmed, "config ", 7) == 0) {
            char type[64], name[64];
            if (sscanf(trimmed, "config %63s '%63[^']'", type, name) >= 1) {
                strcpy(current_section, type);
                if (sscanf(trimmed, "config %63s '%63[^']'", type, name) == 2) {
                    strcpy(current_section_name, name);
                } else {
                    current_section_name[0] = '\0';
                }
            }
            continue;
        }
        
        // 检查是否是option行
        if (strncmp(trimmed, "option ", 7) == 0) {
            char key[64], value[128];
            if (sscanf(trimmed, "option %63s '%127[^']'", key, value) == 2 ||
                sscanf(trimmed, "option %63s %127s", key, value) == 2) {
                
                strip_quotes(value);
                
                if (strcmp(current_section, "screen") == 0) {
                    // 主屏幕配置
                    if (strcmp(key, "enabled") == 0) {
                        g_screen_config.enabled = (atoi(value) != 0);
                    } else if (strcmp(key, "scroll_type") == 0) {
                        g_screen_config.scroll_type = scroll_name_to_type(value);
                    } else if (strcmp(key, "default_delay") == 0) {
                        g_screen_config.default_delay = atoi(value);
                    } else if (strcmp(key, "skip_boot") == 0) {
                        g_screen_config.skip_boot = (atoi(value) != 0);
                    } else if (strcmp(key, "skip_splash") == 0) {
                        g_screen_config.skip_splash = (atoi(value) != 0);
                    }
                } else if (strcmp(current_section, "page") == 0) {
                    // 页面配置
                    page_type_t page_type = page_name_to_type(current_section_name);
                    if (page_type < PAGE_COUNT) {
                        if (strcmp(key, "enabled") == 0) {
                            g_screen_config.pages[page_type].enabled = (atoi(value) != 0);
                        } else if (strcmp(key, "delay") == 0) {
                            g_screen_config.pages[page_type].delay = atoi(value);
                        } else if (strcmp(key, "order") == 0) {
                            g_screen_config.pages[page_type].order = atoi(value);
                        }
                    }
                }
            }
        }
    }
    
    fclose(fp);
    
    // 重新计算页面顺序
    recalculate_page_order();
    
    printf("Config loaded: enabled=%d, scroll=%d, pages=%d\n",
           g_screen_config.enabled,
           g_screen_config.scroll_type, g_screen_config.enabled_page_count);
    
    return 0;
}
