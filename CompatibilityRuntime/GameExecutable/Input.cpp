#include "Input.h"
#include "translator_api.h"
#include "GameServerInterface.h"
#include "Input/TouchPacket.h"
#include "Input/TouchEmulator.h"
#include "Input/UnityTouchInputReceiver.h"

#include <Il2CppUtilities.h>

#include <cstdio>
#include <cmath>
#include <optional>

struct Dark_Animation_ActorControllerWithGesturePan {
    unsigned char base[16];         // +0: Il2CppObject
    unsigned char pad[32];
    unsigned char ApplyingInput;    // +0x30
    unsigned char pad2[3];
    UnityEngine_Vector2 InputDirection; // +0x34
    unsigned char pad3[16];
    uint32_t PanLevel; // 0x4C
};

static Il2CppString *interned_Horizontal; // negative: 'left'/'A', positive: 'right'/'D'
static Il2CppString *interned_Vertical;   // negative: 'down'/'S', positive: 'up'/'W'

static Il2CppString *interned_tilde;

using GetAxisRawPtr = float (*)(Il2CppString *axisName);
static GetAxisRawPtr icall_GetAxisRaw;

using GetKeyDownPtr = bool (*)(Il2CppString *keyName);
static GetKeyDownPtr icall_GetKeyDown;

using GetMouseButtonPtr = bool (*)(int32_t index);
static GetMouseButtonPtr icall_GetMouseButton;

using get_mousePosition_InjectedPtr = void (*)(UnityEngine_Vector3 *position);
static get_mousePosition_InjectedPtr icall_get_mousePosition_Injected;

using OpenURLPtr = void (*)(Il2CppString *url);
static OpenURLPtr icall_OpenURL;

bool EmulateTouchInput = true;

TouchEmulator touchEmulator;
static UnityTouchInputReceiver unityTouchReceiver;

static void Dark_Animation_ActorControllerWithGesturePan_Update(Il2CppObject *this_, void (*original)(Il2CppObject *this_)) {

    auto horizontal = icall_GetAxisRaw(interned_Horizontal);
    auto vertical = icall_GetAxisRaw(interned_Vertical);

    auto contents = reinterpret_cast<Dark_Animation_ActorControllerWithGesturePan *>(this_);
    contents->ApplyingInput = fabsf(horizontal) >= 0.01f || fabsf(vertical) >= 0.01f;
    contents->InputDirection.x = horizontal;
    contents->InputDirection.y = vertical;

    /*
     * Pan level is the distance between the initial point of the pan
     * and the current position (think, 'width' of the gesture) in integer
     * centimeters, rounded up.
     */
    if(contents->ApplyingInput)
        contents->PanLevel = 15;
    else
        contents->PanLevel = 0;

    original(this_);
}

static int32_t UnityEngine_Input_get_touchCount(int32_t (*original)()) {
    if(!EmulateTouchInput)
        return original();

    if(touchEmulator.currentTouch(&unityTouchReceiver))
        return 1;
    else
        return 0;
}
INTERPOSE_ICALL("UnityEngine.Input::get_touchCount", UnityEngine_Input_get_touchCount);

static bool UnityEngine_Input_get_touchSupported(bool (*original)()) {
    if(!EmulateTouchInput)
        return original();

    return true;
}
INTERPOSE_ICALL("UnityEngine.Input::get_touchSupported", UnityEngine_Input_get_touchSupported);

static void UnityEngine_Input_GetTouch_Injected(int32_t index, UnityEngine_Touch *touch,
                                                void (*original)(int32_t index, UnityEngine_Touch *touch)) {
    if(!EmulateTouchInput)
        return original(index, touch);

    if(index == 0) {
        auto current = touchEmulator.currentTouch(&unityTouchReceiver);
        if(current)
            *touch = *current;
    }
}
INTERPOSE_ICALL("UnityEngine.Input::GetTouch_Injected", UnityEngine_Input_GetTouch_Injected);

static void UnityEngine_SendMouseEvent_DoSendMouseEvents(int32_t unknown, void (*original)(int32_t unknown)) {
    bool shouldPass = true;

    if(EmulateTouchInput) {
        UnityEngine_Vector3 pos;

        auto pressed = icall_GetMouseButton(0);
        icall_get_mousePosition_Injected(&pos);

        shouldPass = touchEmulator.emulateTouchInput(pos.x, pos.y, pressed);

    }

    if(shouldPass) {
        original(unknown);
    }

    if(icall_GetKeyDown(interned_tilde)) {
        icall_OpenURL(stringFromUtf8(getGameServerEndpoint() + "/database"));
    }
}

void InitializeInput(void) {
    interned_Horizontal = il2cpp_string_intern(il2cpp_string_new("Horizontal"));
        // TODO: for the joystick, also consume '3rd axis' with the same sense

    interned_Vertical = il2cpp_string_intern(il2cpp_string_new("Vertical"));
        // TODO: for the joystick, also consume '4th axis' with the same sense

    interned_tilde = il2cpp_string_intern(il2cpp_string_new("`"));

    /*
     * The game does its own filtering, so using GetAxis just makes the movement sluggish.
     */
    icall_GetAxisRaw = reinterpret_cast<GetAxisRawPtr>(translator_resolve_native_icall("UnityEngine.Input::GetAxisRaw(System.String)"));

    icall_GetKeyDown = reinterpret_cast<GetKeyDownPtr>(translator_resolve_native_icall("UnityEngine.Input::GetKeyDownString"));

    translator_divert_method("Assembly-CSharp.dll::Dark.Animation.ActorControllerWithGesturePan::Update",
                             Dark_Animation_ActorControllerWithGesturePan_Update);

    if(EmulateTouchInput) {
        icall_GetMouseButton = reinterpret_cast<GetMouseButtonPtr>(translator_resolve_native_icall("UnityEngine.Input::GetMouseButton(System.Int32)"));
        icall_get_mousePosition_Injected = reinterpret_cast<get_mousePosition_InjectedPtr>(
            translator_resolve_native_icall("UnityEngine.Input::get_mousePosition_Injected(UnityEngine.Vector3&)"));

        touchEmulator.setBackgroundLayerInputReceiver(&unityTouchReceiver);
    }

    icall_OpenURL = reinterpret_cast<OpenURLPtr>(translator_resolve_native_icall("UnityEngine.Application::OpenURL(System.String)"));

    translator_divert_method("UnityEngine.InputLegacyModule.dll::UnityEngine.SendMouseEvents::DoSendMouseEvents",
                                UnityEngine_SendMouseEvent_DoSendMouseEvents);
}
