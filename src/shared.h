#pragma once

#define VERSION 1.5

#include "framework.h"
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <kiero.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <fstream>

#include <cstdio>
#include <cstdint>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <inicpp.h>
#include <ordered_map.h>
#include <OIS.h>
#include <utility>
#include <detours.h>
#include <psapi.h>
#include <ranges>
#include <spdlog/spdlog.h>

#define STR2(x) #x
#define STR(x) STR2(x)

struct FastIO {
	FastIO() {
		std::ios_base::sync_with_stdio(false);
		std::cin.tie(nullptr);
	}
};
inline FastIO fast_io_dummy;

