#ifndef OCTO_H
#define OCTO_H

#include <il2cpp-api.h>

#include <filesystem>

namespace ClientDataAccess {
    class OctoContentStorage;
}

void InitializeOcto();

std::filesystem::path getGamePersistentDataPath();

extern ClientDataAccess::OctoContentStorage *contentStorageInstance;

#endif
