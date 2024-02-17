#include "il2cpp-api-types.h"
#include <il2cpp-api.h>
#include <il2cpp-tabledefs.h>
#include <translator_api.h>

#include <cstdio>
#include <cstdlib>

static void dumpCustomAttributes(Il2CppArray *array) {
    auto count = il2cpp_array_length(array);
    Il2CppObject **elements = reinterpret_cast<Il2CppObject **>(reinterpret_cast<unsigned char *>(array) + il2cpp_array_object_header_size());

    for(auto index = 0; index < count; index++) {
        auto element = elements[index];
        auto elementClass = il2cpp_object_get_class(element);

        auto namespaceName = il2cpp_class_get_namespace(elementClass);
        auto className = il2cpp_class_get_name(elementClass);

        printf("      attribute %s.%s\n", namespaceName, className);
    }
}

static void dumpCustomAttributes(Il2CppCustomAttrInfo *attrInfo) {
    auto attrArray = il2cpp_custom_attrs_construct(attrInfo);
    il2cpp_custom_attrs_free(attrInfo);

    dumpCustomAttributes(attrArray);
}

int tool_main(void *arg) {
    (void)arg;
    il2cpp_set_data_dir("graft/NieR_Data/il2cpp_data");
    auto result = il2cpp_init("IL2CPP Root Domain");
    if(!result) {
        fprintf(stderr, "il2cpp has failed to initialize.\n");
        return 1;
    }

    printf("il2cpp has been successfully initialized.\n");

    // TODO: possibly use il2cpp_class_for_each


    auto domain = il2cpp_domain_get();

    size_t numberOfAssemblies;
    auto assemblies = il2cpp_domain_get_assemblies(domain, &numberOfAssemblies);
    printf("the default domain %p contains %zu assemblies.\n", domain, numberOfAssemblies);
    for(size_t index = 0; index <numberOfAssemblies; index++) {
        auto assembly = assemblies[index];
        auto image = il2cpp_assembly_get_image(assembly);
        auto assemblyName = il2cpp_image_get_name(image);

        auto classCount = il2cpp_image_get_class_count(image);

        printf("assembly no. %zu: assembly %p, image %p: '%s', containing %zu classes total:\n", index, assembly, image, assemblyName, classCount);

        for(size_t classIndex = 0; classIndex < classCount; classIndex++) {
            auto classDesc = const_cast<Il2CppClass *>(il2cpp_image_get_class(image, classIndex));
            auto className = il2cpp_class_get_name(classDesc);
            auto namespaceName = il2cpp_class_get_namespace(classDesc);

            bool classHeading = false;

            printf("  class no. %zu: %s#%s\n", classIndex, namespaceName, className);

            dumpCustomAttributes(il2cpp_custom_attrs_from_class(classDesc));

            bool isEnum = il2cpp_class_is_enum(classDesc);
            if(isEnum) {
                auto enumBase = il2cpp_class_enum_basetype(classDesc);
                auto baseName = il2cpp_type_get_name(enumBase);
                printf("    enum, base: '%s\n", baseName);
            }

            void *iter = nullptr;
            FieldInfo *field;
            while((field = il2cpp_class_get_fields(classDesc, &iter)) != nullptr) {
                auto name = il2cpp_field_get_name(field);

                uint32_t flags = il2cpp_field_get_flags(field);
                auto offset = il2cpp_field_get_offset(field);
                auto fieldType = il2cpp_field_get_type(field);
                auto fieldTypeName = il2cpp_type_get_name(fieldType);

                printf("    field: '%s', flags: 0x%08X, at %zu, '%s'\n", name, flags, offset, fieldTypeName);

                if((flags & (FIELD_ATTRIBUTE_HAS_DEFAULT | FIELD_ATTRIBUTE_STATIC)) == (FIELD_ATTRIBUTE_HAS_DEFAULT | FIELD_ATTRIBUTE_STATIC) && isEnum) {
                    int64_t value = 0;
                    il2cpp_field_static_get_value(field, &value);

                    printf("      enum value: %ld\n", value);
                }
            }

            iter = nullptr;
            const PropertyInfo *property;
            while((property = il2cpp_class_get_properties(classDesc, &iter)) != nullptr) {

            }

            iter = nullptr;
            const MethodInfo *method;
            while((method = il2cpp_class_get_methods(classDesc, &iter)) != nullptr) {
                auto name = il2cpp_method_get_name(method);
                uint32_t iflags;
                uint32_t flags = il2cpp_method_get_flags(method, &iflags);

                //if(!(iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL) && false)
                //    continue;

                //if(!assemblyHeading) {
                //    assemblyHeading = true;
                //}

               // if(!classHeading) {
                //    classHeading = true;
               // }

                printf("    method: '%s', flags: 0x%08X, iflags: 0x%08X\n", name, flags, iflags);

                dumpCustomAttributes(il2cpp_custom_attrs_from_method(method));

                auto returnType = il2cpp_method_get_return_type(method);
                auto returnTypeCategory = il2cpp_type_get_type(returnType);
                auto returnTypeName = il2cpp_type_get_name(returnType);
                printf("      return type: '%s', category: %d; pointer: %d, byref: %d\n", returnTypeName, returnTypeCategory,
                       il2cpp_type_is_pointer_type(returnType), il2cpp_type_is_byref(returnType));

                size_t argCount = il2cpp_method_get_param_count(method);
                for(size_t argIndex = 0; argIndex < argCount; argIndex++) {
                    auto argumentType = il2cpp_method_get_param(method, argIndex);
                    auto argumentTypeCategory = il2cpp_type_get_type(argumentType);
                    auto argumentTypeName = il2cpp_type_get_name(argumentType);

                    printf("      argument %zu type: '%s', category: %d; pointer: %d, byref: %d\n", argIndex, argumentTypeName, argumentTypeCategory,
                           il2cpp_type_is_pointer_type(argumentType),
                           il2cpp_type_is_byref(argumentType));
                }
            }
        }

    }

    exit(0);
    return 0;
}


int main(int argc, char *argv[]) {
    return translator_main(tool_main, nullptr);
}

