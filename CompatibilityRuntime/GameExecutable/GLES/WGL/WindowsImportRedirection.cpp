#include <GLES/WGL/WindowsImportRedirection.h>

#include <stdexcept>
#include <cstdint>
#include <string>

#include "CommonUtilities.h"

template<typename T>
static inline const T *rva(HMODULE mod, uint32_t rva) {
    return reinterpret_cast<const T *>(reinterpret_cast<const unsigned char *>(mod) + rva);
}

static const IMAGE_IMPORT_DESCRIPTOR *findImportDirectoryForModuleInModule(HMODULE importingModule, const char *importedModule) {

    auto dosHeaders = rva<IMAGE_DOS_HEADER>(importingModule, 0);
    auto ntHeaders = rva<IMAGE_NT_HEADERS>(importingModule, dosHeaders->e_lfanew);

    if(ntHeaders->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_IMPORT ||
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0)
        return nullptr;

    auto importDescriptors = rva<IMAGE_IMPORT_DESCRIPTOR>(importingModule,
                                                          ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    auto numberOfImportDescriptors =
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size / sizeof(IMAGE_IMPORT_DESCRIPTOR);

    for(size_t index = 0; index < numberOfImportDescriptors; index++) {
        if(importDescriptors[index].Name == 0)
            break;

        auto name = rva<char>(importingModule, importDescriptors[index].Name);
        if(stringsEqualIgnoreCaseAsciiOnly(name, importedModule))
            return &importDescriptors[index];
    }

    return nullptr;
}

void rebindModuleImport(HMODULE importingModule, const IMAGE_IMPORT_DESCRIPTOR *importedModule,
                               const ReplacementExport *rebindTo, size_t numberOfReboundExports) {

    auto importLookupTable = rva<uint64_t>(importingModule, importedModule->OriginalFirstThunk);
    auto importAddressTable = const_cast<uint64_t *>(rva<uint64_t>(importingModule, importedModule->FirstThunk));

    for(; *importLookupTable && *importAddressTable; importLookupTable++, importAddressTable++) {
        auto lookupEntry = *importLookupTable;
        if(lookupEntry & (UINT64_C(1) << 63))
            throw std::runtime_error("unsupported ordinal import");

        auto importByName = rva<IMAGE_IMPORT_BY_NAME>(importingModule, static_cast<uint32_t>(lookupEntry) & UINT32_C(0x7FFFFFFF));

        bool resolved = false;
        for(size_t functionIndex = 0; functionIndex < numberOfReboundExports; functionIndex++) {
            if(strcmp(importByName->Name, rebindTo[functionIndex].name) == 0) {
                size_t bytesWritten;

                if(!WriteProcessMemory(GetCurrentProcess(),
                                   importAddressTable,
                                   &rebindTo[functionIndex].address,
                                   sizeof(uintptr_t), &bytesWritten))
                    throw std::runtime_error("WriteProcessMemory has failed");

                if(bytesWritten != sizeof(uintptr_t))
                    throw std::runtime_error("WriteProcessMemory: short write");

                resolved = true;
                break;
            }
        }

        if(!resolved) {
            throw std::runtime_error("the function is not in the replacement export table: " + std::string(importByName->Name));
        }
    }

    if(static_cast<bool>(*importLookupTable) != static_cast<bool>(*importAddressTable)) {
        throw std::runtime_error("The lengths of ILT and IAT don't match\n");
    }
}

void rebindModuleImport(HMODULE importingModule, const char *importedModule,
                               const ReplacementExport *rebindTo, size_t numberOfReboundExports) {

    auto importDirectory = findImportDirectoryForModuleInModule(importingModule, importedModule);
    if(importDirectory == nullptr || importDirectory->OriginalFirstThunk == 0 || importDirectory->FirstThunk == 0)
        throw std::runtime_error("Unable to find the import directory for the requested module");

    rebindModuleImport(importingModule, importDirectory, rebindTo, numberOfReboundExports);
}
