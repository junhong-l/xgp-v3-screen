-- SPDX-License-Identifier: GPL-3.0-or-later
-- Copyright (C) 2025 zzzz0317

local m, s, o

m = Map("xgp_screen", translate("XGP 屏幕设置"), 
    translate("配置 XiGuaPi V3 TFT 屏幕显示参数"))

-- 主设置部分
s = m:section(NamedSection, "main", "screen", translate("基本设置"))
s.addremove = false

o = s:option(Flag, "enabled", translate("启用屏幕"), translate("关闭后将停止程序并关闭背光"))
o.rmempty = false
o.default = "1"

o = s:option(Value, "fb_device", translate("Framebuffer设备"), 
    translate("留空自动检测，或手动指定如 /dev/fb0 或 /dev/fb1"))
o.placeholder = "自动检测"
o.rmempty = true

o = s:option(ListValue, "scroll_type", translate("切换动画"))
o:value("none", translate("无动画"))
o:value("fade", translate("淡入淡出"))
o:value("slide_up", translate("向上滑动"))
o:value("slide_down", translate("向下滑动"))
o:value("slide_left", translate("向左滑动"))
o:value("slide_right", translate("向右滑动"))
o:value("over_up", translate("从下往上覆盖"))
o:value("over_down", translate("从上往下覆盖"))
o:value("over_left", translate("从右往左覆盖"))
o:value("over_right", translate("从左往右覆盖"))
o.default = "slide_up"

o = s:option(Value, "default_delay", translate("默认停留时间"), translate("毫秒"))
o.datatype = "uinteger"
o.default = "5000"

o = s:option(Flag, "skip_boot", translate("跳过启动画面"))
o.rmempty = false
o.default = "0"

o = s:option(Flag, "skip_splash", translate("跳过闪屏画面"))
o.rmempty = false
o.default = "0"

-- 系统信息页面
s = m:section(NamedSection, "system_info", "page", translate("系统信息页面"))
s.addremove = false

o = s:option(Flag, "enabled", translate("启用"))
o.rmempty = false
o.default = "1"

o = s:option(Value, "delay", translate("停留时间"), translate("毫秒"))
o.datatype = "uinteger"
o.default = "5000"

o = s:option(Value, "order", translate("显示顺序"))
o.datatype = "uinteger"
o.default = "1"

-- 系统状态页面
s = m:section(NamedSection, "system_status", "page", translate("系统状态页面"))
s.addremove = false

o = s:option(Flag, "enabled", translate("启用"))
o.rmempty = false
o.default = "1"

o = s:option(Value, "delay", translate("停留时间"), translate("毫秒"))
o.datatype = "uinteger"
o.default = "5000"

o = s:option(Value, "order", translate("显示顺序"))
o.datatype = "uinteger"
o.default = "2"

-- 网络信息页面
s = m:section(NamedSection, "network_info", "page", translate("网络信息页面"))
s.addremove = false

o = s:option(Flag, "enabled", translate("启用"))
o.rmempty = false
o.default = "1"

o = s:option(Value, "delay", translate("停留时间"), translate("毫秒"))
o.datatype = "uinteger"
o.default = "5000"

o = s:option(Value, "order", translate("显示顺序"))
o.datatype = "uinteger"
o.default = "3"

-- 调制解调器信息页面
s = m:section(NamedSection, "modem_info", "page", translate("调制解调器信息页面"))
s.addremove = false

o = s:option(Flag, "enabled", translate("启用"))
o.rmempty = false
o.default = "1"

o = s:option(Value, "delay", translate("停留时间"), translate("毫秒"))
o.datatype = "uinteger"
o.default = "5000"

o = s:option(Value, "order", translate("显示顺序"))
o.datatype = "uinteger"
o.default = "4"

-- 调制解调器信号页面
s = m:section(NamedSection, "modem_signal", "page", translate("调制解调器信号页面"))
s.addremove = false

o = s:option(Flag, "enabled", translate("启用"))
o.rmempty = false
o.default = "1"

o = s:option(Value, "delay", translate("停留时间"), translate("毫秒"))
o.datatype = "uinteger"
o.default = "5000"

o = s:option(Value, "order", translate("显示顺序"))
o.datatype = "uinteger"
o.default = "5"

-- 应用后重启服务
m.on_after_commit = function(self)
    luci.sys.call("/etc/init.d/zz_xgp_screen restart >/dev/null 2>&1")
end

return m
