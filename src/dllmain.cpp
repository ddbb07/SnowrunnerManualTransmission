#include "config.h"
#include "gui.h"
#include "input.h"
#include "memory.h"
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sec_api/stdio_s.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <windows.h>

HMODULE g_hModule = nullptr;
std::atomic<bool> hasConsole = false;
std::atomic<bool> alive = true;

void AttachConsole() {
    if (!hasConsole) {
        AllocConsole();
        hasConsole = true;
        freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
        freopen_s((FILE **)stdin, "CONIN$", "r", stdin);
        SetConsoleTitleA("Logging Console");

        std::cout.clear();
        std::cerr.clear();
        std::clog.clear();

        spdlog::info("Started Logging");
    }
}

void DetachConsole() {
    if (hasConsole) {
        spdlog::info("Console Detached. You can close this window.");
        FreeConsole();
        hasConsole = false;
    }
}

void DetachDLL() {
    CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto hMod = (HMODULE)lpParam;
            FreeLibraryAndExitThread(hMod, 0);
            return 0;
        },
        g_hModule, 0, nullptr);
}

DWORD WINAPI MainThread(LPVOID lpReserved) {
    auto logger = std::make_shared<spdlog::logger>(
        "SMT",
        spdlog::sinks_init_list{
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("SMT_LOG.txt"),
            std::make_shared<spdlog::sinks::stdout_sink_mt>()});
    logger->set_pattern("[%H:%M:%S.%e] %v");
    logger->flush_on(spdlog::level::info);
    spdlog::set_default_logger(logger);
    LoadIniConfig();
    InitMemory();
    InitGui();
    while (!isGuiInitialized) {
        Sleep(100);
    }
    InitInput();
    return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        g_hModule = hMod;
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        ShutdownInput();
        do {
            Sleep(100);
        } while (keepAliveInput);
        ShutdownMemory();
        ShutdownGui();
        DetachConsole();
        spdlog::shutdown();
        break;
    }
    return TRUE;
}