#include "Input.h"
#include "translator_api.h"

#include <cstdio>
#include <cmath>

struct UnityEngine_Vector2 {
    float x;
    float y;
};

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

using GetAxisRawPtr = float (*)(Il2CppString *axisName);
static GetAxisRawPtr icall_GetAxisRaw;

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

void InitializeInput(void) {
    interned_Horizontal = il2cpp_string_intern(il2cpp_string_new("Horizontal"));
        // TODO: for the joystick, also consume '3rd axis' with the same sense

    interned_Vertical = il2cpp_string_intern(il2cpp_string_new("Vertical"));
        // TODO: for the joystick, also consume '4th axis' with the same sense

    /*
     * The game does its own filtering, so using GetAxis just makes the movement sluggish.
     */
    icall_GetAxisRaw = reinterpret_cast<GetAxisRawPtr>(translator_resolve_native_icall("UnityEngine.Input::GetAxisRaw(System.String)"));

    translator_divert_method("Assembly-CSharp.dll::Dark.Animation.ActorControllerWithGesturePan::Update",
                             Dark_Animation_ActorControllerWithGesturePan_Update);
}
