-- SPDX-License-Identifier: GPL-3.0-or-later
-- Copyright (C) 2025 zzzz0317

module("luci.controller.xgp_screen", package.seeall)

function index()
    entry({"admin", "services", "xgp_screen"}, cbi("xgp_screen"), _("XGP Screen"), 90)
end
