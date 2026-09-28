#pragma once

#define VERSION 1.5

#include "framework.h"
#include <d3d11.h>
#include <dxgi.h>
#include <kiero.h>
#include <windows.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <fstream>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>

#include <OIS.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <detours.h>
#include <inicpp.h>
#include <ordered_map.h>
#include <psapi.h>
#include <ranges>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#define STR2(x) #x
#define STR(x) STR2(x)

struct FastIO {
    FastIO() {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
    }
};
inline FastIO fast_io_dummy;
