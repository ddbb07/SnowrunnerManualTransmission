#pragma once

#include <atomic>
#include <dxgi.h>
#include <windows.h>

using Present = HRESULT(__stdcall *)(IDXGISwapChain *pSwapChain,
                                     UINT SyncInterval, UINT Flags);
using WNDPROC = LRESULT(CALLBACK *)(HWND, UINT, WPARAM, LPARAM);
using PTR = uintptr_t;

extern Present originalPresent;
extern HRESULT __stdcall hookedPresent(IDXGISwapChain *pSwapChain,
                                       UINT SyncInterval, UINT Flags);
extern std::atomic<bool> showGui;
extern void InitGui();
extern void ShutdownGui();
extern HWND window;
extern std::atomic<bool> isGuiInitialized;
