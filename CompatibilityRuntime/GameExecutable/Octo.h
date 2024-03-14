#ifndef OCTO_H
#define OCTO_H

#include <il2cpp-api.h>

#include <filesystem>

class OctoContentStorage;

void InitializeOcto();

std::filesystem::path getGamePersistentDataPath();

extern OctoContentStorage *contentStorageInstance;

#endif
