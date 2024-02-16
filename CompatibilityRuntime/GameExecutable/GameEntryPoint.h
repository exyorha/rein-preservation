#ifndef GAME_ENTRY_POINT_H
#define GAME_ENTRY_POINT_H

using GameInvokeUnity = int (*)(void *unityInvocationPackage);

int gameMain(int argc, char **argv, GameInvokeUnity unityEntryPoint, void *unityInvocationPackage);

bool gameEarlyInit();

#endif
