#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <unordered_map>

inline const float PowerCoefLowGear = .45f;
inline const float PowerCoefLowPlusGear = 1.f;
inline const float PowerCoefLowMinusGear = .2f;

class String {
public:
    union {
        std::array<char, 16> pad_0000; // 0x0000
        char *Ptr;
    };
    uint64_t Size;     // 0x0010
    uint64_t Capacity; // 0x0018

    [[nodiscard]] inline bool Enlarged() const { return Capacity >= 16; }
    [[nodiscard]] const char *c_str() const {
        if (Enlarged()) {
            return Ptr;
        } else {
            return pad_0000.data();
        }
    }
}; // Size: 0x0020

class combine_combineTruckAction {
public:
    std::array<char, 48> pad_0000;  // 0x0000
    class Vehicle *Veh;             // 0x0030
    float PowerCoef;                // 0x0038
    bool IsInAutoMode;              // 0x003C
    std::array<char, 3> pad_003D;   // 0x003D
    float WheelTurn;                // 0x0040
    float Accel;                    // 0x0044
    bool Handbrake;                 // 0x0048
    bool AWD;                       // 0x0049
    bool Diff;                      // 0x004A
    std::array<char, 21> pad_004B;  // 0x004B
    class Unknown_1 *N0000005B;     // 0x0060
    std::array<char, 8> pad_0068;   // 0x0068
    std::int32_t Gear_1;            // 0x0070
    std::int32_t Gear_2;            // 0x0074
    std::array<char, 56> pad_0078;  // 0x0078
    float N00000065;                // 0x00B0
    float N00000319;                // 0x00B4
    float N00000066;                // 0x00B8
    float N00000325;                // 0x00BC
    std::array<char, 24> pad_00C0;  // 0x00C0
    float N0000006A;                // 0x00D8
    float SwitchThreshold;          // 0x00DC
    std::int32_t NextGear;          // 0x00E0
    std::array<char, 860> pad_00E4; // 0x00E4
}; // Size: 0x0440

class Vehicle {
public:
    std::array<char, 96> pad_0000; // 0x0000
    class combine_TRUCK_ADDON_MODEL
        *TruckAddonModel; // 0x0078 -- OFFSET NOT UPDATED
    class combine_combineTruckAction *TruckAction; // 0x0068
    class combine_combineTruckPostSimulationListener
        *TruckPostSimulationListener; // 0x0088 -- OFFSET NOT UPDATED
    std::array<char, 164> pad_0090;   // 0x0090 -- OFFSET NOT UPDATED
    float StallCounter;               // 0x011C
    std::array<char, 1496> pad_0138;  // 0x0138 -- OFFSET NOT UPDATED
    std::uint32_t Ignition;           // 0x0710 -- OFFSET NOT UPDATED
    std::array<char, 84> pad_0714;    // 0x0714 -- OFFSET NOT UPDATED
    std::uint32_t q_VehStateFlags;    // 0x0768 -- OFFSET NOT UPDATED
    std::array<char, 244> pad_076C;   // 0x076C -- NOT UPDATED
    void SetPowerCoef(float coef);

    [[nodiscard]] std::int32_t GetMaxGear() const;

    /**
     * @brief Switches to @p targetGear gear, if possible.
     *
     * @param targetGear gear to switch to
     * @param powerCoef power coef to set after switching. Low gears have
     * different coefs.
     * @return true - if gear was switched, false otherwise
     */
    bool ShiftToGear(std::int32_t targetGear, float powerCoef = 1.f);

    /**
     * @brief Switches to next gear, if possible.
     *
     * @param veh vehicle to switch the gear on
     * @return true - if gear was switched, false otherwise
     */
    bool ShiftToNextGear();

    /**
     * @brief Switches to previous gear, if possible.
     *
     * @param veh vehicle to switch the gear on
     * @return true - if gear was switched, false otherwise
     */
    bool ShiftToPrevGear();

    bool ShiftToHighGear();

    bool ShiftToReverseGear();

    bool ShiftToLowGear();

    bool ShiftToLowPlusGear();

    bool ShiftToLowMinusGear();
}; // Size: 0x0440

class combine_TRUCK_ADDON_MODEL {
public:
    std::array<char, 88> pad_0000;  // 0x0000
    class combine_SCENE *Scene;     // 0x0058
    class combine_XMESH *XMesh;     // 0x0060
    std::array<char, 144> pad_0068; // 0x0068
    class N0000059F *N00000462;     // 0x00F8
    std::array<char, 16> pad_0100;  // 0x0100
    class N00000579 *N00000465;     // 0x0110
    std::array<char, 200> pad_0118; // 0x0118
    class hkpRigidBody *N0000047F;  // 0x01E0
    std::array<char, 544> pad_01E8; // 0x01E8
}; // Size: 0x0408

class combine_XMESH {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class combine_SCENE {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class N00000579 {
public:
    class combine_TRUCK_WHEEL_MODEL *TruckWheelModel1; // 0x0000
    class N00000696 *N000005CA;                        // 0x0008
    void *N000005CB;                                   // 0x0010
    class N000006BB *N000005CC;                        // 0x0018
    std::array<char, 1000> pad_0020;                   // 0x0020
}; // Size: 0x0408

class N0000059F {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class combine_combineTruckPostSimulationListener {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class combine_TRUCK_WHEEL_MODEL {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class N00000696 {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class N000006BB {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class N000006C6 {
public:
    class combine_combineDriveCameraAction *N000006C7; // 0x0000
    class combine_combineSubstanceWheel *N000006C8;    // 0x0008
    class combine_combineSubstanceWheel *N000006C9;    // 0x0010
    class combine_combineSubstanceWheel *N000006CA;    // 0x0018
    class combine_combineSubstanceWheel *N000006CB;    // 0x0020
    class combine_combineSubstanceWheel *N000006CC;    // 0x0028
    class combine_combineSubstanceWheel *N000006CD;    // 0x0030
    class combine_combineSubstanceWheel *N000006CE;    // 0x0038
    class combine_combineSubstanceWheel *N000006CF;    // 0x0040
    std::array<char, 1016> pad_0048;                   // 0x0048
}; // Size: 0x0440

class combine_combineDriveCameraAction {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class combine_combineSubstanceWheel {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class combine_TRUCK_CONTROL {
public:
    std::array<char, 8> pad_0000;          // 0x0000
    class Vehicle *CurVehicle;             // 0x0008
    std::array<char, 96> pad_0010;         // 0x0010
    class combine_SOUND_OBJECT *N0000094C; // 0x0070
    class combine_SOUND_OBJECT *N0000094D; // 0x0078
    class combine_SOUND_OBJECT *N0000094E; // 0x0080
    std::array<char, 8> pad_0088;          // 0x0088
    class String *N00000950;               // 0x0090
    std::array<char, 936> pad_0098;        // 0x0098
}; // Size: 0x0440

class combine_SOUND_OBJECT {
public:
    std::array<char, 8> pad_0000; // 0x0000
}; // Size: 0x0008

class Unknown_1 {
public:
    std::array<char, 1032> pad_0000; // 0x0000
}; // Size: 0x0408

class hkpRigidBody {
public:
    std::array<char, 1032> pad_0000; // 0x0000
}; // Size: 0x0408

extern std::unordered_map<Vehicle *, std::atomic<bool>> IsInAuto;