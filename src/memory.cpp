#include <windows.h>

#include "game_data.h"
#include "memory.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <detours.h>
#include <psapi.h>
#include <spdlog/spdlog.h>
#include <vector>

// NOLINTBEGIN(performance-no-int-to-ptr)
HMODULE hModule = GetModuleHandleA(nullptr);
MODULEINFO mInfo;
bool temp = GetModuleInformation(GetCurrentProcess(), hModule, &mInfo,
                                 sizeof(MODULEINFO));
size_t base = reinterpret_cast<size_t>(mInfo.lpBaseOfDll);
size_t sizeOfImage =
    (reinterpret_cast<PIMAGE_NT_HEADERS>(
         reinterpret_cast<uint8_t *>(hModule) +
         (reinterpret_cast<PIMAGE_DOS_HEADER>(hModule))->e_lfanew))
        ->OptionalHeader.SizeOfCode;

uint32_t PatternScan(const char *signature, size_t begin = 0, size_t end = 0) {
    static auto pattern_to_byte = [](const char *pattern) {
        auto bytes = std::vector<char>{};
        auto start = const_cast<char *>(pattern);
        auto end = const_cast<char *>(pattern) + strlen(pattern);

        for (auto current = start; current < end; current++) {
            if (*current == '?') {
                current++;
                if (*current == '?')
                    current++;
                bytes.push_back('\?');
            } else {
                bytes.push_back(
                    static_cast<char>(strtoul(current, &current, 16)));
            }
        }
        return bytes;
    };

    auto patternBytes = pattern_to_byte(signature);

    size_t patternLength = patternBytes.size();
    auto data = patternBytes.data();

    uint32_t result;
    size_t count = 0;

    if (!end) {
        end = sizeOfImage;
    }

    for (size_t i = begin; i < end - patternLength; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLength; j++) {
            char a = '\?';
            char b = *reinterpret_cast<char *>(base + i + j);
            found &= data[j] == a || data[j] == b;
        }
        if (found) {
            result = i;
            count++;
        }
    }
    if (count == 1) {
        return result;
    }
    return 0;
}

uint32_t ToLittleEndian(uint32_t value) {
    uint8_t b0 = (value >> 0) & 0xFF;
    uint8_t b1 = (value >> 8) & 0xFF;
    uint8_t b2 = (value >> 16) & 0xFF;
    uint8_t b3 = (value >> 24) & 0xFF;

    return (static_cast<uint32_t>(b0) << 0) | (static_cast<uint32_t>(b1) << 8) |
           (static_cast<uint32_t>(b2) << 16) |
           (static_cast<uint32_t>(b3) << 24);
}

int32_t DigAHole(uintptr_t result) {
    result += base;
    uintptr_t address = result;
    for (; *reinterpret_cast<uint8_t *>(address) != 0xE8; address++) {
    }
    address++;
    auto value = static_cast<int32_t>(
        ToLittleEndian(*reinterpret_cast<int32_t *>(address)));
    address += 4;
    value = result - base + value + (address - result);
    address = base + value;
    for (; *reinterpret_cast<uint8_t *>(address) != 0x05; address++) {
    }
    address++;
    value = static_cast<int32_t>(
        ToLittleEndian(*reinterpret_cast<int32_t *>(address)));
    address += 4;
    value = address - base + value;
    return value;
}

combine_TRUCK_CONTROL **TruckControlPtr = nullptr;
Fnc_ShiftGear *ShiftGearO = nullptr;
Fnc_ShiftToAutoGear *ShiftToAutoGearO = nullptr;
Fnc_ShiftToHigh *ShiftToHighO = nullptr;
Fnc_ShiftToReverse *ShiftToReverseO = nullptr;
Fnc_ShiftToNeutral *ShiftToNeutralO = nullptr;
Fnc_GetMaxGear *GetMaxGearO = nullptr;
Fnc_DisableAutoAndShift *DisableAutoAndShiftO = nullptr;
Fnc_SetPowerCoef *SetPowerCoefO = nullptr;
Fnc_SetCurrentVehicle *SetCurrentVehicleO = nullptr;

Vehicle *GetCurrentVehicle() {
    combine_TRUCK_CONTROL *truckCtrl = *TruckControlPtr;
    if (truckCtrl == nullptr) {
        return nullptr;
    }
    return truckCtrl->CurVehicle;
}

// Game code is still unpacking when this runs, so retry until it resolves.
int32_t ScanQuiet(const char *signature, size_t begin = 0, size_t end = 0) {
    int32_t offset;
    int32_t retries = 0;
    while (
        !(offset = static_cast<int32_t>(PatternScan(signature, begin, end)))) {
        retries++;
        Sleep(100);
    }
    if (retries > 0) {
        spdlog::info("Took {} retries to resolve", retries);
    }
    return offset;
}

int32_t Scan(const char *name, const char *signature, size_t begin = 0,
             size_t end = 0) {
    int32_t offset = ScanQuiet(signature, begin, end);
    spdlog::info("{} {:08x}", name, offset);
    return offset;
}

// DigAHole can't tell a good read from a bad one, so retry until two calls
// agree.
int32_t DigAHoleStable(uintptr_t result) {
    int32_t value = DigAHole(result);
    int32_t retries = 0;
    while (true) {
        Sleep(100);
        int32_t recheck = DigAHole(result);
        if (recheck == value) {
            break;
        }
        value = recheck;
        retries++;
    }
    if (retries > 0) {
        spdlog::info("Took {} retries to resolve", retries);
    }
    return value;
}

void InitMemory() {
    int32_t ShiftGearOffset =
        Scan("ShiftGear:",
             "48 89 74 24 10 48 89 7C 24 18 41 56 48 83 EC 20 48 8B 81 48 01 "
             "00 00 48 8B F1 48 B9 FF FF FF FF FF FF 00 FF 8B FA 48 23 C1 74");
    int32_t ShiftToAutoGearOffset =
        Scan("ShiftToAutoGear:",
             "40 57 48 81 EC 80 00 00 00 48 8B 41 68 48 8B F9 48 89 9C 24 90 "
             "00 00 00 0F 29 7C 24 60 C6 40 3C 01 48 8B 41 60 48 8B 90 30 02 "
             "00 00 0F 10 8A 30 02 00 00 0F 59 8A 70 01 00 00");
    int32_t ShiftToHighOffset =
        Scan("ShiftToHigh:", "40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B CB "
                             "8D 50 01 48 83 C4 20 5B");
    int32_t ShiftToReverseOffset =
        Scan("ShiftToReverse:", "BA FF FF FF FF E9 ? ? ? ? CC");
    int32_t ShiftToNeutralOffset =
        Scan("ShiftToNeutral:", "33 D2 E9 ? ? ? ? CC", ShiftToHighOffset,
             ShiftToReverseOffset);
    int32_t GetMaxGearOffset =
        Scan("GetMaxGear:", "48 8B 41 68 48 8B 50 58 48 8B 48 60 48 3B D1 75 ? "
                            "33 C0 C3 48 2B CA 48 C1 F9 02 8D 41 FE");
    int32_t DisableAutoAndShiftOffset =
        Scan("DisableAutoAndShift:", "48 8B 41 68 C6 40 3C 00 E9");
    int32_t SetPowerCoefOffset =
        Scan("SetPowerCoef:", "48 8B 41 68 F3 0F 11 48 38 C3");
    int32_t SetCurrentVehicleOffset = Scan(
        "SetCurrentVehicle:", "48 8B C4 53 57 48 81 EC 98 00 00 00 48 8B FA 48 "
                              "8B D9 48 39 51 08 0F 84 ? ? ? ? 48 89 68 E8 48 "
                              "83 C1 70 48 89 70 E0 4C 89 70 D8 4C 89 78 D0");
    int32_t combine_TRUCK_CONTROLOffset = DigAHoleStable(
        ScanQuiet("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 33 C9 48 89 18"));
    spdlog::info("combine_TRUCK_CONTROL: {:08x}", combine_TRUCK_CONTROLOffset);

    TruckControlPtr = reinterpret_cast<combine_TRUCK_CONTROL **>(
        base + combine_TRUCK_CONTROLOffset);
    ShiftGearO = reinterpret_cast<Fnc_ShiftGear *>(base + ShiftGearOffset);
    ShiftToAutoGearO =
        reinterpret_cast<Fnc_ShiftToAutoGear *>(base + ShiftToAutoGearOffset);
    ShiftToHighO =
        reinterpret_cast<Fnc_ShiftToHigh *>(base + ShiftToHighOffset);
    ShiftToReverseO =
        reinterpret_cast<Fnc_ShiftToReverse *>(base + ShiftToReverseOffset);
    ShiftToNeutralO =
        reinterpret_cast<Fnc_ShiftToNeutral *>(base + ShiftToNeutralOffset);
    GetMaxGearO = reinterpret_cast<Fnc_GetMaxGear *>(base + GetMaxGearOffset);
    DisableAutoAndShiftO = reinterpret_cast<Fnc_DisableAutoAndShift *>(
        base + DisableAutoAndShiftOffset);
    SetPowerCoefO =
        reinterpret_cast<Fnc_SetPowerCoef *>(base + SetPowerCoefOffset);
    SetCurrentVehicleO = reinterpret_cast<Fnc_SetCurrentVehicle *>(
        base + SetCurrentVehicleOffset);

    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(reinterpret_cast<PVOID *>(&ShiftGearO),
                 reinterpret_cast<PVOID>(Hooked_ShiftGear));
    DetourAttach(reinterpret_cast<PVOID *>(&ShiftToAutoGearO),
                 reinterpret_cast<PVOID>(Hooked_ShiftToAutoGear));
    DetourAttach(reinterpret_cast<PVOID *>(&ShiftToHighO),
                 reinterpret_cast<PVOID>(Hooked_ShiftToHigh));
    DetourAttach(reinterpret_cast<PVOID *>(&ShiftToReverseO),
                 reinterpret_cast<PVOID>(Hooked_ShiftToReverse));
    DetourAttach(reinterpret_cast<PVOID *>(&ShiftToNeutralO),
                 reinterpret_cast<PVOID>(Hooked_ShiftToNeutral));
    DetourAttach(reinterpret_cast<PVOID *>(&GetMaxGearO),
                 reinterpret_cast<PVOID>(Hooked_GetMaxGear));
    DetourAttach(reinterpret_cast<PVOID *>(&DisableAutoAndShiftO),
                 reinterpret_cast<PVOID>(Hooked_DisableAutoAndShift));
    DetourAttach(reinterpret_cast<PVOID *>(&SetPowerCoefO),
                 reinterpret_cast<PVOID>(Hooked_SetPowerCoef));
    DetourAttach(reinterpret_cast<PVOID *>(&SetCurrentVehicleO),
                 reinterpret_cast<PVOID>(Hooked_SetCurrentVehicle));
    DetourTransactionCommit();

    if (Vehicle *veh = GetCurrentVehicle()) {
        IsInAuto[veh] = veh->TruckAction->IsInAutoMode;
        veh->TruckAction->IsInAutoMode = false;
        veh->ShiftToGear(1, 1.05);
    }

    spdlog::info("init {}", base);
}

void ShutdownMemory() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(reinterpret_cast<PVOID *>(&ShiftGearO),
                 reinterpret_cast<PVOID>(Hooked_ShiftGear));
    DetourDetach(reinterpret_cast<PVOID *>(&ShiftToAutoGearO),
                 reinterpret_cast<PVOID>(Hooked_ShiftToAutoGear));
    DetourDetach(reinterpret_cast<PVOID *>(&ShiftToHighO),
                 reinterpret_cast<PVOID>(Hooked_ShiftToHigh));
    DetourDetach(reinterpret_cast<PVOID *>(&ShiftToReverseO),
                 reinterpret_cast<PVOID>(Hooked_ShiftToReverse));
    DetourDetach(reinterpret_cast<PVOID *>(&ShiftToNeutralO),
                 reinterpret_cast<PVOID>(Hooked_ShiftToNeutral));
    DetourDetach(reinterpret_cast<PVOID *>(&GetMaxGearO),
                 reinterpret_cast<PVOID>(Hooked_GetMaxGear));
    DetourDetach(reinterpret_cast<PVOID *>(&DisableAutoAndShiftO),
                 reinterpret_cast<PVOID>(Hooked_DisableAutoAndShift));
    DetourDetach(reinterpret_cast<PVOID *>(&SetPowerCoefO),
                 reinterpret_cast<PVOID>(Hooked_SetPowerCoef));
    DetourDetach(reinterpret_cast<PVOID *>(&SetCurrentVehicleO),
                 reinterpret_cast<PVOID>(Hooked_SetCurrentVehicle));
    DetourTransactionCommit();
}
// NOLINTEND(performance-no-int-to-ptr)
