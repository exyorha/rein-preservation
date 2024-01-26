#include <ICall/InternalCallThunk.h>
#include <il2cpp-tabledefs.h>
#include <il2cpp-blob.h>
#include <ffi.h>

#include <cstdio>

#include <Translator/JITThreadContext.h>
#include <Translator/thunking.h>

InternalCallThunk::InternalCallThunk(const char *name, Il2CppMethodPointer x86Method) : m_name(name), m_x86Method(x86Method) {

}

InternalCallThunk::~InternalCallThunk() = default;

void InternalCallThunk::thunkCall() {
    static_cast<InternalCallThunk *>(thunkUtilitySlot)->execute();
}

void InternalCallThunk::execute() {
    auto &thread = JITThreadContext::get();

    /*
     * Copy the input arguments, since we're going to trash them by calling other ARM functions.
     */

    std::array<std::uint64_t, 8> intArgs;
    memcpy(&intArgs, &thread.gprs, sizeof(intArgs));

    std::array<Dynarmic::Vector, 8> vectorArgs;
    memcpy(&vectorArgs, &thread.vectors, sizeof(vectorArgs));

    auto methodNameDelim = m_name.find("::");
    if(methodNameDelim == std::string::npos)
        panic("icall method name has no class-method delimiter: %s\n", m_name.c_str());

    auto namespaceDelim = m_name.rfind('.');
    if(namespaceDelim >= methodNameDelim)
        panic("icall method name has no namespace-class delimiter: %s\n", m_name.c_str());

    std::string namespaceName(m_name.substr(0, namespaceDelim));
    std::string className(m_name.substr(namespaceDelim + 1, methodNameDelim - namespaceDelim - 1));
    std::string methodName(m_name.substr(methodNameDelim + 2));

    Il2CppClass *classDef = nullptr;

    size_t assemblyCount;
    auto assemblies = il2cpp_domain_get_assemblies(il2cpp_domain_get(), &assemblyCount);
    for(size_t assemblyIndex = 0; assemblyIndex < assemblyCount; assemblyIndex++) {
        auto image = il2cpp_assembly_get_image(assemblies[assemblyIndex]);

        classDef = il2cpp_class_from_name(image, namespaceName.c_str(), className.c_str());
        if(classDef)
            break;
    }

    if(!classDef) {
        panic("icall: unable to find the class in any assembly when calling '%s'\n", m_name.c_str());
    }

    void *iter = nullptr;
    const MethodInfo *methodDef = nullptr;
    const MethodInfo *itMethod;

    while((itMethod = il2cpp_class_get_methods(classDef, &iter)) != nullptr) {
        uint32_t iflags;
        auto flags = il2cpp_method_get_flags(itMethod, &iflags);

        if(!(iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL))
            continue;

        auto name = il2cpp_method_get_name(itMethod);
        if(name == methodName) {
            if(methodDef) {
                panic("icall: multiple icall methods named %s", m_name.c_str());
            }

            methodDef = itMethod;
        }
    }

    if(!methodDef) {
        panic("icall: unable to find the method in the class when calling '%s'\n", m_name.c_str());
    }

    uint32_t iflags;
    auto flags = il2cpp_method_get_flags(methodDef, &iflags);

    if(!(iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL))
        panic("icall: the located method was not marked for internal call when calling '%s'\n", m_name.c_str());

    printf("InternalCallThunk::execute: thunk %p called to call namespace %s class %s method %s at %p; il2cpp class %p, method %p; flags 0x%08X, iflags 0x%08X\n",
          this, namespaceName.c_str(), className.c_str(), methodName.c_str(), m_x86Method,
          classDef, methodDef,
           flags, iflags);

    std::vector<void *> argumentPointers;
    std::vector<ffi_type *> argumentTypes;

    auto argumentCount = il2cpp_method_get_param_count(methodDef);

    size_t argumentsToReserve = argumentCount;

    bool hasThis = !(flags & METHOD_ATTRIBUTE_STATIC);

    if(hasThis)
        argumentsToReserve++;

    argumentPointers.reserve(argumentsToReserve);
    argumentTypes.reserve(argumentsToReserve);

    auto returnType = &ffi_type_void;
    void *returnPointer = nullptr;

    unsigned int integerArgumentSlot = 0;
    unsigned int vectorArgumentSlot = 0;

    if(hasThis) {
        printf("the method is not static, getting the this pointer\n");

        argumentPointers.emplace_back(&intArgs[integerArgumentSlot]);
        integerArgumentSlot++;

        argumentTypes.emplace_back(&ffi_type_pointer);
    }

    printf("  %u arguments\n", argumentCount);
    for(unsigned int argumentIndex = 0; argumentIndex < argumentCount; argumentIndex++) {
        auto argumentType = il2cpp_method_get_param(methodDef, argumentIndex);

        ffi_type *ffiType;
        auto category = getValueCategory(argumentType, &ffiType);

        if(category == ValueCategory::Integer) {
            if(integerArgumentSlot >= intArgs.size()) {
                panic("fetching integer args from stack is not implemented yet\n");
            } else {
                argumentPointers.emplace_back(&intArgs[integerArgumentSlot]);
                integerArgumentSlot++;
            }
        } else if(category == ValueCategory::Vector) {
            if(vectorArgumentSlot >= vectorArgs.size()) {
                panic("fetching vector args from stack is not implemented yet\n");
            } else {
                argumentPointers.emplace_back(&vectorArgs[vectorArgumentSlot]);
                vectorArgumentSlot++;
            }
        } else {
            panic("unsupported argument return category\n");
        }

        argumentTypes.emplace_back(ffiType);
    }

    auto returnCategory = getValueCategory(il2cpp_method_get_return_type(methodDef), &returnType);
    if(returnCategory == ValueCategory::Void) {
        returnPointer = nullptr;
    } else if(returnCategory == ValueCategory::Integer) {
        returnPointer = &thread.gprs[0];
    } else if(returnCategory == ValueCategory::Vector) {
        returnPointer = &thread.vectors[0];
    } else {
        panic("unsupported return value category\n");
    }

    ffi_cif cif;
    auto result = ffi_prep_cif(&cif,
                               FFI_DEFAULT_ABI,
                               argumentTypes.size(),
                               returnType,
                               argumentTypes.data());

    if(result != FFI_OK) {
        panic("ffi_prep_cif has failed: %d\n", result);
    }

    if(m_name == "UnityEngine.Networking.UnityWebRequest::SetUrl") {
        auto url = static_cast<Il2CppString **>(argumentPointers.at(1));
        printf("string at (probably) %p\n", *url);
        auto length = il2cpp_string_length(*url);
        auto chars = il2cpp_string_chars(*url);
        printf("length %p, chars %u\n", chars, length);
        std::string conv;
        for(size_t index = 0; index < length; index++) {
            conv.push_back(chars[index]);
        }

        printf("Web Request for '%s'\n", conv.c_str());

        if(conv.starts_with("jar:")) {
            printf("Requesting a file from the game APK, repairing the path: '%s'\n", conv.c_str());

            conv.erase(conv.begin(), conv.begin() + 4);

            auto bang = conv.find('!');
            if(bang != std::string::npos) {
                conv.erase(bang, 1);
            }

            printf("Redirecting to %s\n", conv.c_str());

            *url = il2cpp_string_new_len(conv.data(), conv.size());
        }
    }
#if 0
else if(m_name == "UnityEngine.AndroidJNI::FindClass") {
        auto url = static_cast<Il2CppString **>(argumentPointers.at(0));
        printf("string at (probably) %p\n", *url);
        auto length = il2cpp_string_length(*url);
        auto chars = il2cpp_string_chars(*url);
        printf("length %p, chars %u\n", chars, length);
        std::string conv;
        for(size_t index = 0; index < length; index++) {
            conv.push_back(chars[index]);
        }

        printf("Java: %s: '%s'\n", m_name.c_str(), conv.c_str());
    } else if(m_name == "UnityEngine.AndroidJNI::GetStaticMethodID" || m_name == "UnityEngine.AndroidJNI::GetMethodID" ||
              m_name == "UnityEngine.AndroidJNI::GetFieldID" || m_name == "UnityEngine.AndroidJNI::GetStaticFieldID"
    ) {
        auto url = static_cast<Il2CppString **>(argumentPointers.at(1));
        printf("string at (probably) %p\n", *url);
        auto length = il2cpp_string_length(*url);
        auto chars = il2cpp_string_chars(*url);
        printf("length %p, chars %u\n", chars, length);
        std::string conv;
        for(size_t index = 0; index < length; index++) {
            conv.push_back(chars[index]);
        }

        printf("Java: '%s': '%s'\n", m_name.c_str(), conv.c_str());

    }
#endif

    printf("calling!\n");

    ffi_call(
        &cif,
        m_x86Method,
        returnPointer,
        argumentPointers.data());
#if 0
    if(m_name == "UnityEngine.AndroidJNI::FindClass" || m_name == "UnityEngine.AndroidJNI::NewGlobalRef") {
        static unsigned char fake;
        *reinterpret_cast<void **>(returnPointer) = &fake;
        printf("Java: faked the class, R0 = %016" PRIx64 "\n", thread.gprs[0]);
    } else if(m_name == "UnityEngine.AndroidJNI::ExceptionOccurred") {
        printf("Java: exception occurred: %016" PRIx64 "\n", thread.gprs[0]);
    }
#endif

    printf("done!\n");
}

auto InternalCallThunk::getValueCategory(const Il2CppType *type, ffi_type **ffi) -> ValueCategory {
    auto typeCategory = il2cpp_type_get_type(type);
    auto typeName = il2cpp_type_get_name(type);

    auto ptr = il2cpp_type_is_pointer_type(type);
    auto ref = il2cpp_type_is_byref(type);

    printf("sorting out type: '%s', category: %d; pointer: %d, byref: %d\n", typeName, typeCategory, ptr, ref);

    if(typeCategory == IL2CPP_TYPE_VOID) {
        *ffi = &ffi_type_void;
        return ValueCategory::Void;
    } else if(typeCategory == IL2CPP_TYPE_BOOLEAN) {
        *ffi = &ffi_type_uint8;
        return ValueCategory::Integer;

    } else if(
        ptr ||
        ref ||
        typeCategory == IL2CPP_TYPE_CLASS ||
        typeCategory == IL2CPP_TYPE_STRING ||
        typeCategory == IL2CPP_TYPE_SZARRAY ||
        typeCategory == IL2CPP_TYPE_I || // intptr_t, technically not a pointer but it doesn't matter
        typeCategory == IL2CPP_TYPE_OBJECT
    ) {
        *ffi = &ffi_type_pointer;
        return ValueCategory::Integer;
    } else if(typeCategory == IL2CPP_TYPE_I4) {
        *ffi = &ffi_type_sint32;
        return ValueCategory::Integer;

    } else if(typeCategory == IL2CPP_TYPE_VALUETYPE) {
        auto typeClass = il2cpp_type_get_class_or_element_class(type);
        uint32_t valueAlign;
        auto valueSize = il2cpp_class_value_size(typeClass, &valueAlign);

        if(valueSize == 4 && valueAlign == 4) {
            *ffi = &ffi_type_uint32;
            return ValueCategory::Integer;
        } else {

            panic("value type passed by value; size: %u, align: %u\n", valueSize, valueAlign);
        }

    } else if(typeCategory == IL2CPP_TYPE_R4) {
        *ffi = &ffi_type_float;
        return ValueCategory::Vector;

    } else {
        panic("unsupported argument type\n");
    }

}
