#include <ICall/PreparedInternalCall.h>

#include <il2cpp-api.h>
#include <il2cpp-tabledefs.h>

#include "support.h"

PreparedInternalCall::PreparedInternalCall(const std::string &fullName) {

    auto methodNameDelim = fullName.find("::");
    if(methodNameDelim == std::string::npos)
        panic("icall method name has no class-method delimiter: %s\n", fullName.c_str());

    auto namespaceDelim = fullName.rfind('.');
    if(namespaceDelim >= methodNameDelim)
        panic("icall method name has no namespace-class delimiter: %s\n", fullName.c_str());

    std::string namespaceName(fullName.substr(0, namespaceDelim));
    std::string className(fullName.substr(namespaceDelim + 1, methodNameDelim - namespaceDelim - 1));
    std::string methodName(fullName.substr(methodNameDelim + 2));

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
        panic("icall: unable to find the class in any assembly when calling '%s'\n", fullName.c_str());
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
                panic("icall: multiple icall methods named %s", fullName.c_str());
            }

            methodDef = itMethod;
        }
    }

    if(!methodDef) {
        panic("icall: unable to find the method in the class when calling '%s'\n", fullName.c_str());
    }

    uint32_t iflags;
    auto flags = il2cpp_method_get_flags(methodDef, &iflags);

    if(!(iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL))
        panic("icall: the located method was not marked for internal call when calling '%s'\n", fullName.c_str());

    printf("PreparedInternalCall: preparing a thunk called to call namespace %s class %s method %s; il2cpp class %p, method %p; flags 0x%08X, iflags 0x%08X\n",
          namespaceName.c_str(), className.c_str(), methodName.c_str(),
          classDef, methodDef,
           flags, iflags);

    auto argumentCount = il2cpp_method_get_param_count(methodDef);

    size_t argumentsToReserve = argumentCount;

    bool hasThis = !(flags & METHOD_ATTRIBUTE_STATIC);

    if(hasThis)
        argumentsToReserve++;

    ARMArgumentPacker argumentPacker(argumentsToReserve);
    ARMArgumentPacker resultPacker(1);

    if(hasThis) {
        printf("the method is not static, getting the this pointer\n");

        argumentPacker.packPointer();

    }

    printf("  %u arguments\n", argumentCount);
    for(unsigned int argumentIndex = 0; argumentIndex < argumentCount; argumentIndex++) {
        argumentPacker.pack(il2cpp_method_get_param(methodDef, argumentIndex));

    }

    resultPacker.pack(il2cpp_method_get_return_type(methodDef));

    m_arguments = std::move(argumentPacker.finish());
    m_result = resultPacker.finish().makeIntoSingleArgument();

    auto result = ffi_prep_cif(&m_cif,
                               FFI_DEFAULT_ABI,
                               m_arguments.size(),
                               m_result.type(),
                               m_arguments.types());

    if(result != FFI_OK) {
        panic("ffi_prep_cif has failed: %d\n", result);
    }

}
#if 0
    if(fullName == "UnityEngine.Networking.UnityWebRequest::SetUrl") {
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
#endif
#if 0
else if(fullName == "UnityEngine.AndroidJNI::FindClass") {
        auto url = static_cast<Il2CppString **>(argumentPointers.at(0));
        printf("string at (probably) %p\n", *url);
        auto length = il2cpp_string_length(*url);
        auto chars = il2cpp_string_chars(*url);
        printf("length %p, chars %u\n", chars, length);
        std::string conv;
        for(size_t index = 0; index < length; index++) {
            conv.push_back(chars[index]);
        }

        printf("Java: %s: '%s'\n", fullName.c_str(), conv.c_str());
    } else if(fullName == "UnityEngine.AndroidJNI::GetStaticMethodID" || fullName == "UnityEngine.AndroidJNI::GetMethodID" ||
              fullName == "UnityEngine.AndroidJNI::GetFieldID" || fullName == "UnityEngine.AndroidJNI::GetStaticFieldID"
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

        printf("Java: '%s': '%s'\n", fullName.c_str(), conv.c_str());

    }
#endif

PreparedInternalCall::~PreparedInternalCall() = default;

void PreparedInternalCall::invokeInternal(Il2CppMethodPointer method, JITThreadContext &context, SavedICallContext *contextWithInputArgs) const {


    printf("calling!\n");

    std::vector<void *> args;
    if(contextWithInputArgs) {
        args = m_arguments.getPointers(*contextWithInputArgs);
    } else {
        args = m_arguments.getPointers(context);
    }

    auto returnPointer = m_result.getPointer(context);

    ffi_call(
        &m_cif,
        method,
        returnPointer,
        args.data());
#if 0
    if(fullName == "UnityEngine.AndroidJNI::FindClass" || fullName == "UnityEngine.AndroidJNI::NewGlobalRef") {
        static unsigned char fake;
        *reinterpret_cast<void **>(returnPointer) = &fake;
        printf("Java: faked the class, R0 = %016" PRIx64 "\n", thread.gprs[0]);
    } else if(fullName == "UnityEngine.AndroidJNI::ExceptionOccurred") {
        printf("Java: exception occurred: %016" PRIx64 "\n", thread.gprs[0]);
    }
#endif

    printf("done!\n");
}
