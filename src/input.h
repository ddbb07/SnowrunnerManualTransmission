#pragma once

#include <OISJoyStick.h>
#include <set>

namespace SMT {
class JoyStickListener : public OIS::JoyStickListener {
public:
    bool buttonPressed(const OIS::JoyStickEvent &e, int button) override;

    bool buttonReleased(const OIS::JoyStickEvent &e, int button) override;

    bool axisMoved(const OIS::JoyStickEvent &e, int axis) override;

    bool sliderMoved(const OIS::JoyStickEvent &e, int sliderID) override;

    bool povMoved(const OIS::JoyStickEvent &e, int pov) override;
};
} // namespace SMT

extern void InitInput();
extern void ShutdownInput();
extern OIS::InputManager *inputManager;
extern std::vector<OIS::JoyStick *> joystickList;
extern std::set<std::string> tempPressed;
extern std::atomic<bool> keepAliveInput;
extern std::atomic<int32_t> range;
