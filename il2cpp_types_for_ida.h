       

       

typedef struct String_t String_t;
struct Type_t;
struct Exception_t;
struct StringBuilder_t;
struct MulticastDelegate_t;
struct MethodBase_t;
struct Assembly_t;

       

       



       

       




       



       






typedef struct Il2CppClass Il2CppClass;
typedef struct Il2CppType Il2CppType;
typedef struct EventInfo EventInfo;
typedef struct MethodInfo MethodInfo;
typedef struct FieldInfo FieldInfo;
typedef struct PropertyInfo PropertyInfo;

typedef struct Il2CppAssembly Il2CppAssembly;
typedef struct Il2CppArray Il2CppArray;
typedef struct Il2CppDelegate Il2CppDelegate;
typedef struct Il2CppDomain Il2CppDomain;
typedef struct Il2CppImage Il2CppImage;
typedef struct Il2CppException Il2CppException;
typedef struct Il2CppProfiler Il2CppProfiler;
typedef struct Il2CppObject Il2CppObject;
typedef struct Il2CppReflectionMethod Il2CppReflectionMethod;
typedef struct Il2CppReflectionType Il2CppReflectionType;
typedef struct Il2CppString Il2CppString;
typedef struct Il2CppThread Il2CppThread;
typedef struct Il2CppAsyncResult Il2CppAsyncResult;
typedef struct Il2CppManagedMemorySnapshot Il2CppManagedMemorySnapshot;
typedef struct Il2CppCustomAttrInfo Il2CppCustomAttrInfo;

typedef enum
{
    IL2CPP_PROFILE_NONE = 0,
    IL2CPP_PROFILE_APPDOMAIN_EVENTS = 1 << 0,
    IL2CPP_PROFILE_ASSEMBLY_EVENTS = 1 << 1,
    IL2CPP_PROFILE_MODULE_EVENTS = 1 << 2,
    IL2CPP_PROFILE_CLASS_EVENTS = 1 << 3,
    IL2CPP_PROFILE_JIT_COMPILATION = 1 << 4,
    IL2CPP_PROFILE_INLINING = 1 << 5,
    IL2CPP_PROFILE_EXCEPTIONS = 1 << 6,
    IL2CPP_PROFILE_ALLOCATIONS = 1 << 7,
    IL2CPP_PROFILE_GC = 1 << 8,
    IL2CPP_PROFILE_THREADS = 1 << 9,
    IL2CPP_PROFILE_REMOTING = 1 << 10,
    IL2CPP_PROFILE_TRANSITIONS = 1 << 11,
    IL2CPP_PROFILE_ENTER_LEAVE = 1 << 12,
    IL2CPP_PROFILE_COVERAGE = 1 << 13,
    IL2CPP_PROFILE_INS_COVERAGE = 1 << 14,
    IL2CPP_PROFILE_STATISTICAL = 1 << 15,
    IL2CPP_PROFILE_METHOD_EVENTS = 1 << 16,
    IL2CPP_PROFILE_MONITOR_EVENTS = 1 << 17,
    IL2CPP_PROFILE_IOMAP_EVENTS = 1 << 18,
    IL2CPP_PROFILE_GC_MOVES = 1 << 19,
    IL2CPP_PROFILE_FILEIO = 1 << 20
} Il2CppProfileFlags;

typedef enum
{
    IL2CPP_PROFILE_FILEIO_WRITE = 0,
    IL2CPP_PROFILE_FILEIO_READ
} Il2CppProfileFileIOKind;

typedef enum
{
    IL2CPP_GC_EVENT_START,
    IL2CPP_GC_EVENT_MARK_START,
    IL2CPP_GC_EVENT_MARK_END,
    IL2CPP_GC_EVENT_RECLAIM_START,
    IL2CPP_GC_EVENT_RECLAIM_END,
    IL2CPP_GC_EVENT_END,
    IL2CPP_GC_EVENT_PRE_STOP_WORLD,
    IL2CPP_GC_EVENT_POST_STOP_WORLD,
    IL2CPP_GC_EVENT_PRE_START_WORLD,
    IL2CPP_GC_EVENT_POST_START_WORLD
} Il2CppGCEvent;

typedef enum
{
    IL2CPP_STAT_NEW_OBJECT_COUNT,
    IL2CPP_STAT_INITIALIZED_CLASS_COUNT,


    IL2CPP_STAT_METHOD_COUNT,

    IL2CPP_STAT_CLASS_STATIC_DATA_SIZE,
    IL2CPP_STAT_GENERIC_INSTANCE_COUNT,
    IL2CPP_STAT_GENERIC_CLASS_COUNT,
    IL2CPP_STAT_INFLATED_METHOD_COUNT,
    IL2CPP_STAT_INFLATED_TYPE_COUNT,





} Il2CppStat;

typedef enum
{
    IL2CPP_UNHANDLED_POLICY_LEGACY,
    IL2CPP_UNHANDLED_POLICY_CURRENT
} Il2CppRuntimeUnhandledExceptionPolicy;

typedef struct Il2CppStackFrameInfo
{
    const MethodInfo *method;
} Il2CppStackFrameInfo;

typedef void(*Il2CppMethodPointer)();

typedef struct Il2CppMethodDebugInfo
{
    Il2CppMethodPointer methodPointer;
    int32_t code_size;
    const char *file;
} Il2CppMethodDebugInfo;

typedef struct
{
    void* (*malloc_func)(size_t size);
    void* (*aligned_malloc_func)(size_t size, size_t alignment);
    void (*free_func)(void *ptr);
    void (*aligned_free_func)(void *ptr);
    void* (*calloc_func)(size_t nmemb, size_t size);
    void* (*realloc_func)(void *ptr, size_t size);
    void* (*aligned_realloc_func)(void *ptr, size_t size, size_t alignment);
} Il2CppMemoryCallbacks;

typedef struct
{
    const char *name;
    void(*connect)(const char *address);
    int(*wait_for_attach)(void);
    void(*close1)(void);
    void(*close2)(void);
    int(*send)(void *buf, int len);
    int(*recv)(void *buf, int len);
} Il2CppDebuggerTransport;
typedef uint16_t Il2CppChar;






typedef char Il2CppNativeChar;



typedef void (*il2cpp_register_object_callback)(Il2CppObject** arr, int size, void* userdata);
typedef void (*il2cpp_WorldChangedCallback)();
typedef void (*Il2CppFrameWalkFunc) (const Il2CppStackFrameInfo *info, void *user_data);
typedef void (*Il2CppProfileFunc) (Il2CppProfiler* prof);
typedef void (*Il2CppProfileMethodFunc) (Il2CppProfiler* prof, const MethodInfo *method);
typedef void (*Il2CppProfileAllocFunc) (Il2CppProfiler* prof, Il2CppObject *obj, Il2CppClass *klass);
typedef void (*Il2CppProfileGCFunc) (Il2CppProfiler* prof, Il2CppGCEvent event, int generation);
typedef void (*Il2CppProfileGCResizeFunc) (Il2CppProfiler* prof, int64_t new_size);
typedef void (*Il2CppProfileFileIOFunc) (Il2CppProfiler* prof, Il2CppProfileFileIOKind kind, int count);
typedef void (*Il2CppProfileThreadFunc) (Il2CppProfiler *prof, unsigned long tid);

typedef const Il2CppNativeChar* (*Il2CppSetFindPlugInCallback)(const Il2CppNativeChar*);
typedef void (*Il2CppLogCallback)(const char*);

typedef size_t(*Il2CppBacktraceFunc) (Il2CppMethodPointer* buffer, size_t maxSize);

struct Il2CppManagedMemorySnapshot;

typedef uintptr_t il2cpp_array_size_t;

       
typedef void ( *SynchronizationContextCallback)(intptr_t arg);
typedef uint32_t Il2CppMethodSlot;
static const uint32_t kInvalidIl2CppMethodSlot = 65535;
static const int32_t kIl2CppInt32Min = INT32_MIN;
static const int32_t kIl2CppInt32Max = INT32_MAX;
static const uint32_t kIl2CppUInt32Max = UINT32_MAX;
static const int64_t kIl2CppInt64Min = INT64_MIN;
static const int64_t kIl2CppInt64Max = INT64_MAX;
static const uint64_t kIl2CppUInt64Max = UINT64_MAX;


static const intptr_t kIl2CppIntPtrMin = INT64_MIN;
static const intptr_t kIl2CppIntPtrMax = INT64_MAX;
static const uintptr_t kIl2CppUIntPtrMax = UINT64_MAX;






static const int ipv6AddressSize = 16;
typedef int32_t il2cpp_hresult_t;
static const Il2CppChar kIl2CppNewLine[] = { '\n', '\0' };



       

       




typedef enum Il2CppTypeEnum
{
    IL2CPP_TYPE_END = 0x00,
    IL2CPP_TYPE_VOID = 0x01,
    IL2CPP_TYPE_BOOLEAN = 0x02,
    IL2CPP_TYPE_CHAR = 0x03,
    IL2CPP_TYPE_I1 = 0x04,
    IL2CPP_TYPE_U1 = 0x05,
    IL2CPP_TYPE_I2 = 0x06,
    IL2CPP_TYPE_U2 = 0x07,
    IL2CPP_TYPE_I4 = 0x08,
    IL2CPP_TYPE_U4 = 0x09,
    IL2CPP_TYPE_I8 = 0x0a,
    IL2CPP_TYPE_U8 = 0x0b,
    IL2CPP_TYPE_R4 = 0x0c,
    IL2CPP_TYPE_R8 = 0x0d,
    IL2CPP_TYPE_STRING = 0x0e,
    IL2CPP_TYPE_PTR = 0x0f,
    IL2CPP_TYPE_BYREF = 0x10,
    IL2CPP_TYPE_VALUETYPE = 0x11,
    IL2CPP_TYPE_CLASS = 0x12,
    IL2CPP_TYPE_VAR = 0x13,
    IL2CPP_TYPE_ARRAY = 0x14,
    IL2CPP_TYPE_GENERICINST = 0x15,
    IL2CPP_TYPE_TYPEDBYREF = 0x16,
    IL2CPP_TYPE_I = 0x18,
    IL2CPP_TYPE_U = 0x19,
    IL2CPP_TYPE_FNPTR = 0x1b,
    IL2CPP_TYPE_OBJECT = 0x1c,
    IL2CPP_TYPE_SZARRAY = 0x1d,
    IL2CPP_TYPE_MVAR = 0x1e,
    IL2CPP_TYPE_CMOD_REQD = 0x1f,
    IL2CPP_TYPE_CMOD_OPT = 0x20,
    IL2CPP_TYPE_INTERNAL = 0x21,

    IL2CPP_TYPE_MODIFIER = 0x40,
    IL2CPP_TYPE_SENTINEL = 0x41,
    IL2CPP_TYPE_PINNED = 0x45,

    IL2CPP_TYPE_ENUM = 0x55
} Il2CppTypeEnum;
       


       

typedef enum
{
    IL2CPP_TOKEN_MODULE = 0x00000000,
    IL2CPP_TOKEN_TYPE_REF = 0x01000000,
    IL2CPP_TOKEN_TYPE_DEF = 0x02000000,
    IL2CPP_TOKEN_FIELD_DEF = 0x04000000,
    IL2CPP_TOKEN_METHOD_DEF = 0x06000000,
    IL2CPP_TOKEN_PARAM_DEF = 0x08000000,
    IL2CPP_TOKEN_INTERFACE_IMPL = 0x09000000,
    IL2CPP_TOKEN_MEMBER_REF = 0x0a000000,
    IL2CPP_TOKEN_CUSTOM_ATTRIBUTE = 0x0c000000,
    IL2CPP_TOKEN_PERMISSION = 0x0e000000,
    IL2CPP_TOKEN_SIGNATURE = 0x11000000,
    IL2CPP_TOKEN_EVENT = 0x14000000,
    IL2CPP_TOKEN_PROPERTY = 0x17000000,
    IL2CPP_TOKEN_MODULE_REF = 0x1a000000,
    IL2CPP_TOKEN_TYPE_SPEC = 0x1b000000,
    IL2CPP_TOKEN_ASSEMBLY = 0x20000000,
    IL2CPP_TOKEN_ASSEMBLY_REF = 0x23000000,
    IL2CPP_TOKEN_FILE = 0x26000000,
    IL2CPP_TOKEN_EXPORTED_TYPE = 0x27000000,
    IL2CPP_TOKEN_MANIFEST_RESOURCE = 0x28000000,
    IL2CPP_TOKEN_GENERIC_PARAM = 0x2a000000,
    IL2CPP_TOKEN_METHOD_SPEC = 0x2b000000,
} Il2CppTokenType;
typedef int32_t TypeIndex;
typedef int32_t TypeDefinitionIndex;
typedef int32_t FieldIndex;
typedef int32_t DefaultValueIndex;
typedef int32_t DefaultValueDataIndex;
typedef int32_t CustomAttributeIndex;
typedef int32_t ParameterIndex;
typedef int32_t MethodIndex;
typedef int32_t GenericMethodIndex;
typedef int32_t PropertyIndex;
typedef int32_t EventIndex;
typedef int32_t GenericContainerIndex;
typedef int32_t GenericParameterIndex;
typedef int16_t GenericParameterConstraintIndex;
typedef int32_t NestedTypeIndex;
typedef int32_t InterfacesIndex;
typedef int32_t VTableIndex;
typedef int32_t InterfaceOffsetIndex;
typedef int32_t RGCTXIndex;
typedef int32_t StringIndex;
typedef int32_t StringLiteralIndex;
typedef int32_t GenericInstIndex;
typedef int32_t ImageIndex;
typedef int32_t AssemblyIndex;
typedef int32_t InteropDataIndex;

static const TypeIndex kTypeIndexInvalid = -1;
static const TypeDefinitionIndex kTypeDefinitionIndexInvalid = -1;
static const DefaultValueDataIndex kDefaultValueIndexNull = -1;
static const CustomAttributeIndex kCustomAttributeIndexInvalid = -1;
static const EventIndex kEventIndexInvalid = -1;
static const FieldIndex kFieldIndexInvalid = -1;
static const MethodIndex kMethodIndexInvalid = -1;
static const PropertyIndex kPropertyIndexInvalid = -1;
static const GenericContainerIndex kGenericContainerIndexInvalid = -1;
static const GenericParameterIndex kGenericParameterIndexInvalid = -1;
static const RGCTXIndex kRGCTXIndexInvalid = -1;
static const StringLiteralIndex kStringLiteralIndexInvalid = -1;
static const InteropDataIndex kInteropDataIndexInvalid = -1;
typedef uint32_t EncodedMethodIndex;

enum Il2CppMetadataUsage
{
    kIl2CppMetadataUsageInvalid,
    kIl2CppMetadataUsageTypeInfo,
    kIl2CppMetadataUsageIl2CppType,
    kIl2CppMetadataUsageMethodDef,
    kIl2CppMetadataUsageFieldInfo,
    kIl2CppMetadataUsageStringLiteral,
    kIl2CppMetadataUsageMethodRef,
};
struct Il2CppImage;
struct Il2CppType;
struct Il2CppTypeDefinitionMetadata;

typedef union Il2CppRGCTXDefinitionData
{
    int32_t rgctxDataDummy;
    MethodIndex methodIndex;
    TypeIndex typeIndex;
} Il2CppRGCTXDefinitionData;

typedef enum Il2CppRGCTXDataType
{
    IL2CPP_RGCTX_DATA_INVALID,
    IL2CPP_RGCTX_DATA_TYPE,
    IL2CPP_RGCTX_DATA_CLASS,
    IL2CPP_RGCTX_DATA_METHOD,
    IL2CPP_RGCTX_DATA_ARRAY,
} Il2CppRGCTXDataType;
typedef struct Il2CppRGCTXDefinition
{
    Il2CppRGCTXDataType type;
    Il2CppRGCTXDefinitionData data;
} Il2CppRGCTXDefinition;



typedef struct Il2CppInterfaceOffsetPair
{
    TypeIndex interfaceTypeIndex;
    int32_t offset;
} Il2CppInterfaceOffsetPair;

typedef struct Il2CppTypeDefinition
{
    StringIndex nameIndex;
    StringIndex namespaceIndex;
    TypeIndex byvalTypeIndex;
    TypeIndex byrefTypeIndex;

    TypeIndex declaringTypeIndex;
    TypeIndex parentIndex;
    TypeIndex elementTypeIndex;

    GenericContainerIndex genericContainerIndex;

    uint32_t flags;

    FieldIndex fieldStart;
    MethodIndex methodStart;
    EventIndex eventStart;
    PropertyIndex propertyStart;
    NestedTypeIndex nestedTypesStart;
    InterfacesIndex interfacesStart;
    VTableIndex vtableStart;
    InterfacesIndex interfaceOffsetsStart;

    uint16_t method_count;
    uint16_t property_count;
    uint16_t field_count;
    uint16_t event_count;
    uint16_t nested_type_count;
    uint16_t vtable_count;
    uint16_t interfaces_count;
    uint16_t interface_offsets_count;
    uint32_t bitfield;
    uint32_t token;
} Il2CppTypeDefinition;

typedef struct Il2CppFieldDefinition
{
    StringIndex nameIndex;
    TypeIndex typeIndex;
    uint32_t token;
} Il2CppFieldDefinition;

typedef struct Il2CppFieldDefaultValue
{
    FieldIndex fieldIndex;
    TypeIndex typeIndex;
    DefaultValueDataIndex dataIndex;
} Il2CppFieldDefaultValue;

typedef struct Il2CppFieldMarshaledSize
{
    FieldIndex fieldIndex;
    TypeIndex typeIndex;
    int32_t size;
} Il2CppFieldMarshaledSize;

typedef struct Il2CppFieldRef
{
    TypeIndex typeIndex;
    FieldIndex fieldIndex;
} Il2CppFieldRef;

typedef struct Il2CppParameterDefinition
{
    StringIndex nameIndex;
    uint32_t token;
    TypeIndex typeIndex;
} Il2CppParameterDefinition;

typedef struct Il2CppParameterDefaultValue
{
    ParameterIndex parameterIndex;
    TypeIndex typeIndex;
    DefaultValueDataIndex dataIndex;
} Il2CppParameterDefaultValue;

typedef struct Il2CppMethodDefinition
{
    StringIndex nameIndex;
    TypeDefinitionIndex declaringType;
    TypeIndex returnType;
    ParameterIndex parameterStart;
    GenericContainerIndex genericContainerIndex;
    uint32_t token;
    uint16_t flags;
    uint16_t iflags;
    uint16_t slot;
    uint16_t parameterCount;
} Il2CppMethodDefinition;

typedef struct Il2CppEventDefinition
{
    StringIndex nameIndex;
    TypeIndex typeIndex;
    MethodIndex add;
    MethodIndex remove;
    MethodIndex raise;
    uint32_t token;
} Il2CppEventDefinition;

typedef struct Il2CppPropertyDefinition
{
    StringIndex nameIndex;
    MethodIndex get;
    MethodIndex set;
    uint32_t attrs;
    uint32_t token;
} Il2CppPropertyDefinition;

typedef struct Il2CppMethodSpec
{
    MethodIndex methodDefinitionIndex;
    GenericInstIndex classIndexIndex;
    GenericInstIndex methodIndexIndex;
} Il2CppMethodSpec;

typedef struct Il2CppStringLiteral
{
    uint32_t length;
    StringLiteralIndex dataIndex;
} Il2CppStringLiteral;

typedef struct
{
    MethodIndex methodIndex;
    MethodIndex invokerIndex;
    MethodIndex adjustorThunkIndex;
} Il2CppGenericMethodIndices;

typedef struct Il2CppGenericMethodFunctionsDefinitions
{
    GenericMethodIndex genericMethodIndex;
    Il2CppGenericMethodIndices indices;
} Il2CppGenericMethodFunctionsDefinitions;


static const int kPublicKeyByteLength = 8;

typedef struct Il2CppAssemblyNameDefinition
{
    StringIndex nameIndex;
    StringIndex cultureIndex;
    StringIndex publicKeyIndex;
    uint32_t hash_alg;
    int32_t hash_len;
    uint32_t flags;
    int32_t major;
    int32_t minor;
    int32_t build;
    int32_t revision;
    uint8_t public_key_token[8];
} Il2CppAssemblyNameDefinition;

typedef struct Il2CppImageDefinition
{
    StringIndex nameIndex;
    AssemblyIndex assemblyIndex;

    TypeDefinitionIndex typeStart;
    uint32_t typeCount;

    TypeDefinitionIndex exportedTypeStart;
    uint32_t exportedTypeCount;

    MethodIndex entryPointIndex;
    uint32_t token;

    CustomAttributeIndex customAttributeStart;
    uint32_t customAttributeCount;
} Il2CppImageDefinition;

typedef struct Il2CppAssemblyDefinition
{
    ImageIndex imageIndex;
    uint32_t token;
    int32_t referencedAssemblyStart;
    int32_t referencedAssemblyCount;
    Il2CppAssemblyNameDefinition aname;
} Il2CppAssemblyDefinition;

typedef struct Il2CppMetadataUsageList
{
    uint32_t start;
    uint32_t count;
} Il2CppMetadataUsageList;

typedef struct Il2CppMetadataUsagePair
{
    uint32_t destinationIndex;
    uint32_t encodedSourceIndex;
} Il2CppMetadataUsagePair;

typedef struct Il2CppCustomAttributeTypeRange
{
    uint32_t token;
    int32_t start;
    int32_t count;
} Il2CppCustomAttributeTypeRange;

typedef struct Il2CppRange
{
    int32_t start;
    int32_t length;
} Il2CppRange;

typedef struct Il2CppWindowsRuntimeTypeNamePair
{
    StringIndex nameIndex;
    TypeIndex typeIndex;
} Il2CppWindowsRuntimeTypeNamePair;

typedef struct Il2CppGlobalMetadataHeader
{
    int32_t sanity;
    int32_t version;
    int32_t stringLiteralOffset;
    int32_t stringLiteralCount;
    int32_t stringLiteralDataOffset;
    int32_t stringLiteralDataCount;
    int32_t stringOffset;
    int32_t stringCount;
    int32_t eventsOffset;
    int32_t eventsCount;
    int32_t propertiesOffset;
    int32_t propertiesCount;
    int32_t methodsOffset;
    int32_t methodsCount;
    int32_t parameterDefaultValuesOffset;
    int32_t parameterDefaultValuesCount;
    int32_t fieldDefaultValuesOffset;
    int32_t fieldDefaultValuesCount;
    int32_t fieldAndParameterDefaultValueDataOffset;
    int32_t fieldAndParameterDefaultValueDataCount;
    int32_t fieldMarshaledSizesOffset;
    int32_t fieldMarshaledSizesCount;
    int32_t parametersOffset;
    int32_t parametersCount;
    int32_t fieldsOffset;
    int32_t fieldsCount;
    int32_t genericParametersOffset;
    int32_t genericParametersCount;
    int32_t genericParameterConstraintsOffset;
    int32_t genericParameterConstraintsCount;
    int32_t genericContainersOffset;
    int32_t genericContainersCount;
    int32_t nestedTypesOffset;
    int32_t nestedTypesCount;
    int32_t interfacesOffset;
    int32_t interfacesCount;
    int32_t vtableMethodsOffset;
    int32_t vtableMethodsCount;
    int32_t interfaceOffsetsOffset;
    int32_t interfaceOffsetsCount;
    int32_t typeDefinitionsOffset;
    int32_t typeDefinitionsCount;
    int32_t imagesOffset;
    int32_t imagesCount;
    int32_t assembliesOffset;
    int32_t assembliesCount;
    int32_t metadataUsageListsOffset;
    int32_t metadataUsageListsCount;
    int32_t metadataUsagePairsOffset;
    int32_t metadataUsagePairsCount;
    int32_t fieldRefsOffset;
    int32_t fieldRefsCount;
    int32_t referencedAssembliesOffset;
    int32_t referencedAssembliesCount;
    int32_t attributesInfoOffset;
    int32_t attributesInfoCount;
    int32_t attributeTypesOffset;
    int32_t attributeTypesCount;
    int32_t unresolvedVirtualCallParameterTypesOffset;
    int32_t unresolvedVirtualCallParameterTypesCount;
    int32_t unresolvedVirtualCallParameterRangesOffset;
    int32_t unresolvedVirtualCallParameterRangesCount;
    int32_t windowsRuntimeTypeNamesOffset;
    int32_t windowsRuntimeTypeNamesSize;
    int32_t exportedTypeDefinitionsOffset;
    int32_t exportedTypeDefinitionsCount;
} Il2CppGlobalMetadataHeader;

struct Il2CppClass;
struct MethodInfo;
struct Il2CppType;

typedef struct Il2CppArrayType
{
    const Il2CppType* etype;
    uint8_t rank;
    uint8_t numsizes;
    uint8_t numlobounds;
    int *sizes;
    int *lobounds;
} Il2CppArrayType;

typedef struct Il2CppGenericInst
{
    uint32_t type_argc;
    const Il2CppType **type_argv;
} Il2CppGenericInst;

typedef struct Il2CppGenericContext
{

    const Il2CppGenericInst *class_inst;

    const Il2CppGenericInst *method_inst;
} Il2CppGenericContext;

typedef struct Il2CppGenericParameter
{
    GenericContainerIndex ownerIndex;
    StringIndex nameIndex;
    GenericParameterConstraintIndex constraintsStart;
    int16_t constraintsCount;
    uint16_t num;
    uint16_t flags;
} Il2CppGenericParameter;

typedef struct Il2CppGenericContainer
{

    int32_t ownerIndex;
    int32_t type_argc;

    int32_t is_method;

    GenericParameterIndex genericParameterStart;
} Il2CppGenericContainer;

typedef struct Il2CppGenericClass
{
    TypeDefinitionIndex typeDefinitionIndex;
    Il2CppGenericContext context;
    Il2CppClass *cached_class;
} Il2CppGenericClass;

typedef struct Il2CppGenericMethod
{
    const MethodInfo* methodDefinition;
    Il2CppGenericContext context;
} Il2CppGenericMethod;

typedef struct Il2CppType
{
    union
    {

        void* dummy;
        TypeDefinitionIndex klassIndex;
        const Il2CppType *type;
        Il2CppArrayType *array;

        GenericParameterIndex genericParameterIndex;
        Il2CppGenericClass *generic_class;
    } data;
    unsigned int attrs : 16;
    Il2CppTypeEnum type : 8;
    unsigned int num_mods : 6;
    unsigned int byref : 1;
    unsigned int pinned : 1;

} Il2CppType;

       


typedef enum Il2CppCallConvention
{
    IL2CPP_CALL_DEFAULT,
    IL2CPP_CALL_C,
    IL2CPP_CALL_STDCALL,
    IL2CPP_CALL_THISCALL,
    IL2CPP_CALL_FASTCALL,
    IL2CPP_CALL_VARARG
} Il2CppCallConvention;

typedef enum Il2CppCharSet
{
    CHARSET_ANSI,
    CHARSET_UNICODE,
    CHARSET_NOT_SPECIFIED
} Il2CppCharSet;

       


typedef struct Il2CppHString__
{
    int unused;
} Il2CppHString__;

typedef Il2CppHString__* Il2CppHString;

typedef struct Il2CppHStringHeader
{
    union
    {
        void* Reserved1;

        char Reserved2[24];



    } Reserved;
} Il2CppHStringHeader;


typedef struct Il2CppGuid
{
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
} Il2CppGuid;

typedef struct Il2CppSafeArrayBound
{
    uint32_t element_count;
    int32_t lower_bound;
} Il2CppSafeArrayBound;

typedef struct Il2CppSafeArray
{
    uint16_t dimension_count;
    uint16_t features;
    uint32_t element_size;
    uint32_t lock_count;
    void* data;
    Il2CppSafeArrayBound bounds[1];
} Il2CppSafeArray;

typedef struct Il2CppWin32Decimal
{
    uint16_t reserved;
    union
    {
        struct
        {
            uint8_t scale;
            uint8_t sign;
        } s;
        uint16_t signscale;
    } u;
    uint32_t hi32;
    union
    {
        struct
        {
            uint32_t lo32;
            uint32_t mid32;
        } s2;
        uint64_t lo64;
    } u2;
} Il2CppWin32Decimal;

typedef int16_t IL2CPP_VARIANT_BOOL;




typedef enum Il2CppVarType
{
    IL2CPP_VT_EMPTY = 0,
    IL2CPP_VT_NULL = 1,
    IL2CPP_VT_I2 = 2,
    IL2CPP_VT_I4 = 3,
    IL2CPP_VT_R4 = 4,
    IL2CPP_VT_R8 = 5,
    IL2CPP_VT_CY = 6,
    IL2CPP_VT_DATE = 7,
    IL2CPP_VT_BSTR = 8,
    IL2CPP_VT_DISPATCH = 9,
    IL2CPP_VT_ERROR = 10,
    IL2CPP_VT_BOOL = 11,
    IL2CPP_VT_VARIANT = 12,
    IL2CPP_VT_UNKNOWN = 13,
    IL2CPP_VT_DECIMAL = 14,
    IL2CPP_VT_I1 = 16,
    IL2CPP_VT_UI1 = 17,
    IL2CPP_VT_UI2 = 18,
    IL2CPP_VT_UI4 = 19,
    IL2CPP_VT_I8 = 20,
    IL2CPP_VT_UI8 = 21,
    IL2CPP_VT_INT = 22,
    IL2CPP_VT_UINT = 23,
    IL2CPP_VT_VOID = 24,
    IL2CPP_VT_HRESULT = 25,
    IL2CPP_VT_PTR = 26,
    IL2CPP_VT_SAFEARRAY = 27,
    IL2CPP_VT_CARRAY = 28,
    IL2CPP_VT_USERDEFINED = 29,
    IL2CPP_VT_LPSTR = 30,
    IL2CPP_VT_LPWSTR = 31,
    IL2CPP_VT_RECORD = 36,
    IL2CPP_VT_INT_PTR = 37,
    IL2CPP_VT_UINT_PTR = 38,
    IL2CPP_VT_FILETIME = 64,
    IL2CPP_VT_BLOB = 65,
    IL2CPP_VT_STREAM = 66,
    IL2CPP_VT_STORAGE = 67,
    IL2CPP_VT_STREAMED_OBJECT = 68,
    IL2CPP_VT_STORED_OBJECT = 69,
    IL2CPP_VT_BLOB_OBJECT = 70,
    IL2CPP_VT_CF = 71,
    IL2CPP_VT_CLSID = 72,
    IL2CPP_VT_VERSIONED_STREAM = 73,
    IL2CPP_VT_BSTR_BLOB = 0xfff,
    IL2CPP_VT_VECTOR = 0x1000,
    IL2CPP_VT_ARRAY = 0x2000,
    IL2CPP_VT_BYREF = 0x4000,
    IL2CPP_VT_RESERVED = 0x8000,
    IL2CPP_VT_ILLEGAL = 0xffff,
    IL2CPP_VT_ILLEGALMASKED = 0xfff,
    IL2CPP_VT_TYPEMASK = 0xfff,
} Il2CppVarType;

typedef struct Il2CppVariant Il2CppVariant;
typedef struct Il2CppIUnknown Il2CppIUnknown;

typedef struct Il2CppVariant
{
    union
    {
        struct __tagVARIANT
        {
            uint16_t type;
            uint16_t reserved1;
            uint16_t reserved2;
            uint16_t reserved3;
            union
            {
                int64_t llVal;
                int32_t lVal;
                uint8_t bVal;
                int16_t iVal;
                float fltVal;
                double dblVal;
                IL2CPP_VARIANT_BOOL boolVal;
                int32_t scode;
                int64_t cyVal;
                double date;
                Il2CppChar* bstrVal;
                Il2CppIUnknown* punkVal;
                void* pdispVal;
                Il2CppSafeArray* parray;
                uint8_t* pbVal;
                int16_t* piVal;
                int32_t* plVal;
                int64_t* pllVal;
                float* pfltVal;
                double* pdblVal;
                IL2CPP_VARIANT_BOOL* pboolVal;
                int32_t* pscode;
                int64_t* pcyVal;
                double* pdate;
                Il2CppChar* pbstrVal;
                Il2CppIUnknown** ppunkVal;
                void** ppdispVal;
                Il2CppSafeArray** pparray;
                Il2CppVariant* pvarVal;
                void* byref;
                char cVal;
                uint16_t uiVal;
                uint32_t ulVal;
                uint64_t ullVal;
                int intVal;
                unsigned int uintVal;
                Il2CppWin32Decimal* pdecVal;
                char* pcVal;
                uint16_t* puiVal;
                uint32_t* pulVal;
                uint64_t* pullVal;
                int* pintVal;
                unsigned int* puintVal;
                struct __tagBRECORD
                {
                    void* pvRecord;
                    void* pRecInfo;
                } n4;
            } n3;
        } n2;
        Il2CppWin32Decimal decVal;
    } n1;
} Il2CppVariant;

typedef struct Il2CppFileTime
{
    uint32_t low;
    uint32_t high;
} Il2CppFileTime;

typedef struct Il2CppStatStg
{
    Il2CppChar* name;
    uint32_t type;
    uint64_t size;
    Il2CppFileTime mtime;
    Il2CppFileTime ctime;
    Il2CppFileTime atime;
    uint32_t mode;
    uint32_t locks;
    Il2CppGuid clsid;
    uint32_t state;
    uint32_t reserved;
} Il2CppStatStg;

enum Il2CppWindowsRuntimeTypeKind
{
    kTypeKindPrimitive = 0,
    kTypeKindMetadata,
    kTypeKindCustom
};

struct Il2CppWindowsRuntimeTypeName
{
    Il2CppHString typeName;
    enum Il2CppWindowsRuntimeTypeKind typeKind;
};

typedef void (*PInvokeMarshalToNativeFunc)(void* managedStructure, void* marshaledStructure);
typedef void (*PInvokeMarshalFromNativeFunc)(void* marshaledStructure, void* managedStructure);
typedef void (*PInvokeMarshalCleanupFunc)(void* marshaledStructure);
typedef struct Il2CppIUnknown* (*CreateCCWFunc)(Il2CppObject* obj);





typedef struct Il2CppInteropData
{
    Il2CppMethodPointer delegatePInvokeWrapperFunction;
    PInvokeMarshalToNativeFunc pinvokeMarshalToNativeFunction;
    PInvokeMarshalFromNativeFunc pinvokeMarshalFromNativeFunction;
    PInvokeMarshalCleanupFunc pinvokeMarshalCleanupFunction;
    CreateCCWFunc createCCWFunction;
    const Il2CppGuid* guid;




    const Il2CppType* type;

} Il2CppInteropData;





typedef struct Il2CppClass Il2CppClass;
typedef struct Il2CppGuid Il2CppGuid;
typedef struct Il2CppImage Il2CppImage;
typedef struct Il2CppAppDomain Il2CppAppDomain;
typedef struct Il2CppAppDomainSetup Il2CppAppDomainSetup;
typedef struct Il2CppDelegate Il2CppDelegate;
typedef struct Il2CppAppContext Il2CppAppContext;
typedef struct Il2CppNameToTypeDefinitionIndexHashTable Il2CppNameToTypeDefinitionIndexHashTable;
typedef struct Il2CppCodeGenModule Il2CppCodeGenModule;
typedef struct VirtualInvokeData
{
    Il2CppMethodPointer methodPtr;



    const MethodInfo* method;

} VirtualInvokeData;

typedef enum Il2CppTypeNameFormat
{
    IL2CPP_TYPE_NAME_FORMAT_IL,
    IL2CPP_TYPE_NAME_FORMAT_REFLECTION,
    IL2CPP_TYPE_NAME_FORMAT_FULL_NAME,
    IL2CPP_TYPE_NAME_FORMAT_ASSEMBLY_QUALIFIED
} Il2CppTypeNameFormat;


typedef struct Il2CppDefaults
{
    Il2CppImage *corlib;
    Il2CppClass *object_class;
    Il2CppClass *byte_class;
    Il2CppClass *void_class;
    Il2CppClass *boolean_class;
    Il2CppClass *sbyte_class;
    Il2CppClass *int16_class;
    Il2CppClass *uint16_class;
    Il2CppClass *int32_class;
    Il2CppClass *uint32_class;
    Il2CppClass *int_class;
    Il2CppClass *uint_class;
    Il2CppClass *int64_class;
    Il2CppClass *uint64_class;
    Il2CppClass *single_class;
    Il2CppClass *double_class;
    Il2CppClass *char_class;
    Il2CppClass *string_class;
    Il2CppClass *enum_class;
    Il2CppClass *array_class;
    Il2CppClass *delegate_class;
    Il2CppClass *multicastdelegate_class;
    Il2CppClass *asyncresult_class;
    Il2CppClass *manualresetevent_class;
    Il2CppClass *typehandle_class;
    Il2CppClass *fieldhandle_class;
    Il2CppClass *methodhandle_class;
    Il2CppClass *systemtype_class;
    Il2CppClass *monotype_class;
    Il2CppClass *exception_class;
    Il2CppClass *threadabortexception_class;
    Il2CppClass *thread_class;
    Il2CppClass *internal_thread_class;



    Il2CppClass *appdomain_class;
    Il2CppClass *appdomain_setup_class;
    Il2CppClass *field_info_class;
    Il2CppClass *method_info_class;
    Il2CppClass *property_info_class;
    Il2CppClass *event_info_class;
    Il2CppClass *mono_event_info_class;
    Il2CppClass *stringbuilder_class;

    Il2CppClass *stack_frame_class;
    Il2CppClass *stack_trace_class;
    Il2CppClass *marshal_class;



    Il2CppClass *typed_reference_class;

    Il2CppClass *marshalbyrefobject_class;





    Il2CppClass *generic_ilist_class;
    Il2CppClass *generic_icollection_class;
    Il2CppClass *generic_ienumerable_class;
    Il2CppClass *generic_ireadonlylist_class;
    Il2CppClass *generic_ireadonlycollection_class;
    Il2CppClass *runtimetype_class;
    Il2CppClass *generic_nullable_class;


    Il2CppClass *il2cpp_com_object_class;





    Il2CppClass *attribute_class;
    Il2CppClass *customattribute_data_class;

    Il2CppClass *version;
    Il2CppClass *culture_info;
    Il2CppClass *async_call_class;
    Il2CppClass *assembly_class;
    Il2CppClass *mono_assembly_class;
    Il2CppClass *assembly_name_class;
    Il2CppClass *mono_field_class;
    Il2CppClass *mono_method_class;
    Il2CppClass *mono_method_info_class;
    Il2CppClass *mono_property_info_class;
    Il2CppClass *parameter_info_class;
    Il2CppClass *mono_parameter_info_class;
    Il2CppClass *module_class;
    Il2CppClass *pointer_class;
    Il2CppClass *system_exception_class;
    Il2CppClass *argument_exception_class;
    Il2CppClass *wait_handle_class;
    Il2CppClass *safe_handle_class;
    Il2CppClass *sort_key_class;
    Il2CppClass *dbnull_class;
    Il2CppClass *error_wrapper_class;
    Il2CppClass *missing_class;
    Il2CppClass *value_type_class;


    Il2CppClass *threadpool_wait_callback_class;
    MethodInfo *threadpool_perform_wait_callback_method;
    Il2CppClass *mono_method_message_class;


    Il2CppClass* ireference_class;

    Il2CppClass* ireferencearray_class;

    Il2CppClass* ikey_value_pair_class;

    Il2CppClass* key_value_pair_class;

    Il2CppClass* windows_foundation_uri_class;

    Il2CppClass* windows_foundation_iuri_runtime_class_class;

    Il2CppClass* system_uri_class;

    Il2CppClass* system_guid_class;

    Il2CppClass* sbyte_shared_enum;
    Il2CppClass* int16_shared_enum;
    Il2CppClass* int32_shared_enum;
    Il2CppClass* int64_shared_enum;

    Il2CppClass* byte_shared_enum;
    Il2CppClass* uint16_shared_enum;
    Il2CppClass* uint32_shared_enum;
    Il2CppClass* uint64_shared_enum;
} Il2CppDefaults;

extern Il2CppDefaults il2cpp_defaults;

struct Il2CppClass;
struct MethodInfo;
struct FieldInfo;
struct Il2CppObject;
struct MemberInfo;

typedef struct CustomAttributesCache
{
    int count;
    Il2CppObject** attributes;
} CustomAttributesCache;

typedef void (*CustomAttributesCacheGenerator)(CustomAttributesCache*);





typedef struct FieldInfo
{
    const char* name;
    const Il2CppType* type;
    Il2CppClass *parent;
    int32_t offset;
    uint32_t token;
} FieldInfo;

typedef struct PropertyInfo
{
    Il2CppClass *parent;
    const char *name;
    const MethodInfo *get;
    const MethodInfo *set;
    uint32_t attrs;
    uint32_t token;
} PropertyInfo;

typedef struct EventInfo
{
    const char* name;
    const Il2CppType* eventType;
    Il2CppClass* parent;
    const MethodInfo* add;
    const MethodInfo* remove;
    const MethodInfo* raise;
    uint32_t token;
} EventInfo;

typedef struct ParameterInfo
{
    const char* name;
    int32_t position;
    uint32_t token;
    const Il2CppType* parameter_type;
} ParameterInfo;




typedef void* (*InvokerMethod)(Il2CppMethodPointer, const MethodInfo*, void*, void**);


typedef enum MethodVariableKind
{
    kMethodVariableKind_This,
    kMethodVariableKind_Parameter,
    kMethodVariableKind_LocalVariable
} MethodVariableKind;

typedef enum SequencePointKind
{
    kSequencePointKind_Normal,
    kSequencePointKind_StepOut
} SequencePointKind;

typedef struct Il2CppMethodExecutionContextInfo
{
    TypeIndex typeIndex;
    int32_t nameIndex;
    int32_t scopeIndex;
} Il2CppMethodExecutionContextInfo;

typedef struct Il2CppMethodExecutionContextInfoIndex
{
    int32_t startIndex;
    int32_t count;
} Il2CppMethodExecutionContextInfoIndex;

typedef struct Il2CppMethodScope
{
    int32_t startOffset;
    int32_t endOffset;
} Il2CppMethodScope;

typedef struct Il2CppMethodHeaderInfo
{
    int32_t code_size;
    int32_t startScope;
    int32_t numScopes;
} Il2CppMethodHeaderInfo;

typedef struct Il2CppSequencePointSourceFile
{
    const char *file;
    uint8_t hash[16];
} Il2CppSequencePointSourceFile;

typedef struct Il2CppTypeSourceFilePair
{
    TypeDefinitionIndex klassIndex;
    int32_t sourceFileIndex;
} Il2CppTypeSourceFilePair;

typedef struct Il2CppSequencePoint
{
    MethodIndex methodDefinitionIndex;
    int32_t sourceFileIndex;
    int32_t lineStart, lineEnd;
    int32_t columnStart, columnEnd;
    int32_t ilOffset;
    SequencePointKind kind;
    int32_t isActive;
    int32_t id;
} Il2CppSequencePoint;

typedef struct Il2CppCatchPoint
{
    MethodIndex methodDefinitionIndex;
    TypeIndex catchTypeIndex;
    int32_t ilOffset;
    int32_t tryId;
    int32_t parentTryId;
} Il2CppCatchPoint;

typedef struct Il2CppDebuggerMetadataRegistration
{
    Il2CppMethodExecutionContextInfo* methodExecutionContextInfos;
    Il2CppMethodExecutionContextInfoIndex* methodExecutionContextInfoIndexes;
    Il2CppMethodScope* methodScopes;
    Il2CppMethodHeaderInfo* methodHeaderInfos;
    Il2CppSequencePointSourceFile* sequencePointSourceFiles;
    int32_t numSequencePoints;
    Il2CppSequencePoint* sequencePoints;
    int32_t numCatchPoints;
    Il2CppCatchPoint* catchPoints;
    int32_t numTypeSourceFileEntries;
    Il2CppTypeSourceFilePair* typeSourceFiles;
    const char** methodExecutionContextInfoStrings;
} Il2CppDebuggerMetadataRegistration;

typedef union Il2CppRGCTXData
{
    void* rgctxDataDummy;
    const MethodInfo* method;
    const Il2CppType* type;
    Il2CppClass* klass;
} Il2CppRGCTXData;

typedef struct MethodInfo
{
    Il2CppMethodPointer methodPointer;
    InvokerMethod invoker_method;
    const char* name;
    Il2CppClass *klass;
    const Il2CppType *return_type;
    const ParameterInfo* parameters;

    union
    {
        const Il2CppRGCTXData* rgctx_data;
        const Il2CppMethodDefinition* methodDefinition;
    };


    union
    {
        const Il2CppGenericMethod* genericMethod;
        const Il2CppGenericContainer* genericContainer;
    };

    uint32_t token;
    uint16_t flags;
    uint16_t iflags;
    uint16_t slot;
    uint8_t parameters_count;
    uint8_t is_generic : 1;
    uint8_t is_inflated : 1;
    uint8_t wrapper_type : 1;
    uint8_t is_marshaled_from_native : 1;
} MethodInfo;

typedef struct Il2CppRuntimeInterfaceOffsetPair
{
    Il2CppClass* interfaceType;
    int32_t offset;
} Il2CppRuntimeInterfaceOffsetPair;
typedef struct Il2CppClass
{

    const Il2CppImage* image;
    void* gc_desc;
    const char* name;
    const char* namespaze;
    Il2CppType byval_arg;
    Il2CppType this_arg;
    Il2CppClass* element_class;
    Il2CppClass* castClass;
    Il2CppClass* declaringType;
    Il2CppClass* parent;
    Il2CppGenericClass *generic_class;
    const Il2CppTypeDefinition* typeDefinition;
    const Il2CppInteropData* interopData;
    Il2CppClass* klass;



    FieldInfo* fields;
    const EventInfo* events;
    const PropertyInfo* properties;
    const MethodInfo** methods;
    Il2CppClass** nestedTypes;
    Il2CppClass** implementedInterfaces;
    Il2CppRuntimeInterfaceOffsetPair* interfaceOffsets;
    void* static_fields;
    const Il2CppRGCTXData* rgctx_data;

    Il2CppClass** typeHierarchy;


    void *unity_user_data;

    uint32_t initializationExceptionGCHandle;

    uint32_t cctor_started;
    uint32_t cctor_finished;
    __attribute__((aligned(8))) size_t cctor_thread;


    GenericContainerIndex genericContainerIndex;
    uint32_t instance_size;
    uint32_t actualSize;
    uint32_t element_size;
    int32_t native_size;
    uint32_t static_fields_size;
    uint32_t thread_static_fields_size;
    int32_t thread_static_fields_offset;
    uint32_t flags;
    uint32_t token;

    uint16_t method_count;
    uint16_t property_count;
    uint16_t field_count;
    uint16_t event_count;
    uint16_t nested_type_count;
    uint16_t vtable_count;
    uint16_t interfaces_count;
    uint16_t interface_offsets_count;

    uint8_t typeHierarchyDepth;
    uint8_t genericRecursionDepth;
    uint8_t rank;
    uint8_t minimumAlignment;
    uint8_t naturalAligment;
    uint8_t packingSize;



    uint8_t initialized_and_no_error : 1;

    uint8_t valuetype : 1;
    uint8_t initialized : 1;
    uint8_t enumtype : 1;
    uint8_t is_generic : 1;
    uint8_t has_references : 1;
    uint8_t init_pending : 1;
    uint8_t size_inited : 1;
    uint8_t has_finalize : 1;
    uint8_t has_cctor : 1;
    uint8_t is_blittable : 1;
    uint8_t is_import_or_windows_runtime : 1;
    uint8_t is_vtable_initialized : 1;
    uint8_t has_initialization_error : 1;
    VirtualInvokeData vtable[0];
} Il2CppClass;
typedef struct Il2CppTypeDefinitionSizes
{
    uint32_t instance_size;
    int32_t native_size;
    uint32_t static_fields_size;
    uint32_t thread_static_fields_size;
} Il2CppTypeDefinitionSizes;

typedef struct Il2CppDomain
{
    Il2CppAppDomain* domain;
    Il2CppAppDomainSetup* setup;
    Il2CppAppContext* default_context;
    const char* friendly_name;
    uint32_t domain_id;

    volatile int threadpool_jobs;
    void* agent_info;
} Il2CppDomain;

typedef struct Il2CppAssemblyName
{
    const char* name;
    const char* culture;
    const uint8_t* public_key;
    uint32_t hash_alg;
    int32_t hash_len;
    uint32_t flags;
    int32_t major;
    int32_t minor;
    int32_t build;
    int32_t revision;
    uint8_t public_key_token[8];
} Il2CppAssemblyName;

typedef struct Il2CppImage
{
    const char* name;
    const char *nameNoExt;
    Il2CppAssembly* assembly;

    TypeDefinitionIndex typeStart;
    uint32_t typeCount;

    TypeDefinitionIndex exportedTypeStart;
    uint32_t exportedTypeCount;

    CustomAttributeIndex customAttributeStart;
    uint32_t customAttributeCount;

    MethodIndex entryPointIndex;




    Il2CppNameToTypeDefinitionIndexHashTable * nameToClassHashTable;

    const Il2CppCodeGenModule* codeGenModule;

    uint32_t token;
    uint8_t dynamic;
} Il2CppImage;

typedef struct Il2CppAssembly
{
    Il2CppImage* image;
    uint32_t token;
    int32_t referencedAssemblyStart;
    int32_t referencedAssemblyCount;
    Il2CppAssemblyName aname;
} Il2CppAssembly;

typedef struct Il2CppCodeGenOptions
{
    uint8_t enablePrimitiveValueTypeGenericSharing;
    int maximumRuntimeGenericDepth;
} Il2CppCodeGenOptions;

typedef struct Il2CppTokenIndexPair
{
    uint32_t token;
    int32_t index;
} Il2CppTokenIndexPair;


typedef struct Il2CppTokenRangePair
{
    uint32_t token;
    Il2CppRange range;
} Il2CppTokenRangePair;

typedef struct Il2CppTokenIndexMethodTuple
{
    uint32_t token;
    int32_t index;
    void** method;
    uint32_t genericMethodIndex;
} Il2CppTokenIndexMethodTuple;

typedef struct Il2CppTokenAdjustorThunkPair
{
    uint32_t token;
    Il2CppMethodPointer adjustorThunk;
} Il2CppTokenAdjustorThunkPair;

typedef struct Il2CppWindowsRuntimeFactoryTableEntry
{
    const Il2CppType* type;
    Il2CppMethodPointer createFactoryFunction;
} Il2CppWindowsRuntimeFactoryTableEntry;

typedef struct Il2CppCodeGenModule
{
    const char* moduleName;
    const uint32_t methodPointerCount;
    const Il2CppMethodPointer* methodPointers;
    const uint32_t adjustorThunkCount;
    const Il2CppTokenAdjustorThunkPair* adjustorThunks;
    const int32_t* invokerIndices;
    const uint32_t reversePInvokeWrapperCount;
    const Il2CppTokenIndexMethodTuple* reversePInvokeWrapperIndices;
    const uint32_t rgctxRangesCount;
    const Il2CppTokenRangePair* rgctxRanges;
    const uint32_t rgctxsCount;



    const Il2CppRGCTXDefinition* rgctxs;

    const Il2CppDebuggerMetadataRegistration *debuggerMetadata;
} Il2CppCodeGenModule;

typedef struct Il2CppCodeRegistration
{
    uint32_t reversePInvokeWrapperCount;
    const Il2CppMethodPointer* reversePInvokeWrappers;
    uint32_t genericMethodPointersCount;
    const Il2CppMethodPointer* genericMethodPointers;
    const Il2CppMethodPointer* genericAdjustorThunks;
    uint32_t invokerPointersCount;
    const InvokerMethod* invokerPointers;
    CustomAttributeIndex customAttributeCount;
    const CustomAttributesCacheGenerator* customAttributeGenerators;
    uint32_t unresolvedVirtualCallCount;
    const Il2CppMethodPointer* unresolvedVirtualCallPointers;
    uint32_t interopDataCount;
    Il2CppInteropData* interopData;
    uint32_t windowsRuntimeFactoryCount;
    Il2CppWindowsRuntimeFactoryTableEntry* windowsRuntimeFactoryTable;
    uint32_t codeGenModulesCount;
    const Il2CppCodeGenModule** codeGenModules;
} Il2CppCodeRegistration;

typedef struct Il2CppMetadataRegistration
{
    int32_t genericClassesCount;
    Il2CppGenericClass* const * genericClasses;
    int32_t genericInstsCount;
    const Il2CppGenericInst* const * genericInsts;
    int32_t genericMethodTableCount;
    const Il2CppGenericMethodFunctionsDefinitions* genericMethodTable;
    int32_t typesCount;
    const Il2CppType* const * types;
    int32_t methodSpecsCount;
    const Il2CppMethodSpec* methodSpecs;

    FieldIndex fieldOffsetsCount;
    const int32_t** fieldOffsets;

    TypeDefinitionIndex typeDefinitionsSizesCount;
    const Il2CppTypeDefinitionSizes** typeDefinitionsSizes;
    const size_t metadataUsagesCount;
    void** const* metadataUsages;
} Il2CppMetadataRegistration;







typedef struct Il2CppPerfCounters
{

    uint32_t jit_methods;
    uint32_t jit_bytes;
    uint32_t jit_time;
    uint32_t jit_failures;

    uint32_t exceptions_thrown;
    uint32_t exceptions_filters;
    uint32_t exceptions_finallys;
    uint32_t exceptions_depth;
    uint32_t aspnet_requests_queued;
    uint32_t aspnet_requests;

    uint32_t gc_collections0;
    uint32_t gc_collections1;
    uint32_t gc_collections2;
    uint32_t gc_promotions0;
    uint32_t gc_promotions1;
    uint32_t gc_promotion_finalizers;
    uint32_t gc_gen0size;
    uint32_t gc_gen1size;
    uint32_t gc_gen2size;
    uint32_t gc_lossize;
    uint32_t gc_fin_survivors;
    uint32_t gc_num_handles;
    uint32_t gc_allocated;
    uint32_t gc_induced;
    uint32_t gc_time;
    uint32_t gc_total_bytes;
    uint32_t gc_committed_bytes;
    uint32_t gc_reserved_bytes;
    uint32_t gc_num_pinned;
    uint32_t gc_sync_blocks;

    uint32_t remoting_calls;
    uint32_t remoting_channels;
    uint32_t remoting_proxies;
    uint32_t remoting_classes;
    uint32_t remoting_objects;
    uint32_t remoting_contexts;

    uint32_t loader_classes;
    uint32_t loader_total_classes;
    uint32_t loader_appdomains;
    uint32_t loader_total_appdomains;
    uint32_t loader_assemblies;
    uint32_t loader_total_assemblies;
    uint32_t loader_failures;
    uint32_t loader_bytes;
    uint32_t loader_appdomains_uloaded;

    uint32_t thread_contentions;
    uint32_t thread_queue_len;
    uint32_t thread_queue_max;
    uint32_t thread_num_logical;
    uint32_t thread_num_physical;
    uint32_t thread_cur_recognized;
    uint32_t thread_num_recognized;

    uint32_t interop_num_ccw;
    uint32_t interop_num_stubs;
    uint32_t interop_num_marshals;

    uint32_t security_num_checks;
    uint32_t security_num_link_checks;
    uint32_t security_time;
    uint32_t security_depth;
    uint32_t unused;

    uint64_t threadpool_workitems;
    uint64_t threadpool_ioworkitems;
    unsigned int threadpool_threads;
    unsigned int threadpool_iothreads;
} Il2CppPerfCounters;
struct TypeInfo;
struct MethodInfo;
struct FieldInfo;
struct Il2CppType;
typedef Il2CppClass RuntimeClass;
typedef MethodInfo RuntimeMethod;
typedef FieldInfo RuntimeField;
typedef Il2CppType RuntimeType;
typedef Il2CppObject RuntimeObject;
typedef Il2CppImage RuntimeImage;
typedef Il2CppException RuntimeException;
typedef Il2CppArray RuntimeArray;
typedef Il2CppAssembly RuntimeAssembly;
typedef Il2CppString RuntimeString;
typedef Il2CppDelegate RuntimeDelegate;





struct Il2CppStringBuilder;
typedef Il2CppStringBuilder RuntimeStringBuilder;
       

       




       





__attribute__ ((visibility ("default"))) void il2cpp_gc_wbarrier_set_field(Il2CppObject * obj, void **targetAddress, void *object);




typedef struct Il2CppClass Il2CppClass;
typedef struct MethodInfo MethodInfo;
typedef struct PropertyInfo PropertyInfo;
typedef struct FieldInfo FieldInfo;
typedef struct EventInfo EventInfo;
typedef struct Il2CppType Il2CppType;
typedef struct Il2CppAssembly Il2CppAssembly;
typedef struct Il2CppException Il2CppException;
typedef struct Il2CppImage Il2CppImage;
typedef struct Il2CppDomain Il2CppDomain;
typedef struct Il2CppString Il2CppString;
typedef struct Il2CppReflectionMethod Il2CppReflectionMethod;
typedef struct Il2CppAsyncCall Il2CppAsyncCall;
typedef struct Il2CppIUnknown Il2CppIUnknown;
typedef struct Il2CppWaitHandle Il2CppWaitHandle;
typedef struct MonitorData MonitorData;
typedef struct Il2CppReflectionAssembly Il2CppReflectionAssembly;

typedef Il2CppClass Il2CppVTable;
typedef struct Il2CppObject
{
    union
    {
        Il2CppClass *klass;
        Il2CppVTable *vtable;
    };
    MonitorData *monitor;
} Il2CppObject;

typedef int32_t il2cpp_array_lower_bound_t;



typedef struct Il2CppArrayBounds
{
    il2cpp_array_size_t length;
    il2cpp_array_lower_bound_t lower_bound;
} Il2CppArrayBounds;
typedef struct Il2CppArray
{
    Il2CppObject obj;


    Il2CppArrayBounds *bounds;

    il2cpp_array_size_t max_length;
} Il2CppArray;






typedef struct Il2CppArraySize
{
    Il2CppObject obj;
    Il2CppArrayBounds *bounds;
    il2cpp_array_size_t max_length;

    __attribute__((aligned(8))) void* vector[0];
} Il2CppArraySize;

static const size_t kIl2CppSizeOfArray = (offsetof(Il2CppArraySize, vector));
static const size_t kIl2CppOffsetOfArrayBounds = (offsetof(Il2CppArray, bounds));
static const size_t kIl2CppOffsetOfArrayLength = (offsetof(Il2CppArray, max_length));



typedef struct Il2CppString
{
    Il2CppObject object;
    int32_t length;
    Il2CppChar chars[0];
} Il2CppString;
typedef struct Il2CppReflectionType
{
    Il2CppObject object;
    const Il2CppType *type;
} Il2CppReflectionType;




typedef struct Il2CppReflectionRuntimeType
{
    Il2CppReflectionType type;
    Il2CppObject *type_info;
    Il2CppObject* genericCache;
    Il2CppObject* serializationCtor;
} Il2CppReflectionRuntimeType;


typedef struct Il2CppReflectionMonoType
{
    Il2CppReflectionRuntimeType type;
} Il2CppReflectionMonoType;


typedef struct Il2CppReflectionEvent
{
    Il2CppObject object;
    Il2CppObject *cached_add_event;
} Il2CppReflectionEvent;


typedef struct Il2CppReflectionMonoEvent
{
    Il2CppReflectionEvent event;
    Il2CppReflectionType* reflectedType;
    const EventInfo* eventInfo;
} Il2CppReflectionMonoEvent;


typedef struct Il2CppReflectionMonoEventInfo
{
    Il2CppReflectionType* declaringType;
    Il2CppReflectionType* reflectedType;
    Il2CppString* name;
    Il2CppReflectionMethod* addMethod;
    Il2CppReflectionMethod* removeMethod;
    Il2CppReflectionMethod* raiseMethod;
    uint32_t eventAttributes;
    Il2CppArray* otherMethods;
} Il2CppReflectionMonoEventInfo;


typedef struct Il2CppReflectionField
{
    Il2CppObject object;
    Il2CppClass *klass;
    FieldInfo *field;
    Il2CppString *name;
    Il2CppReflectionType *type;
    uint32_t attrs;
} Il2CppReflectionField;


typedef struct Il2CppReflectionProperty
{
    Il2CppObject object;
    Il2CppClass *klass;
    const PropertyInfo *property;
} Il2CppReflectionProperty;


typedef struct Il2CppReflectionMethod
{
    Il2CppObject object;
    const MethodInfo *method;
    Il2CppString *name;
    Il2CppReflectionType *reftype;
} Il2CppReflectionMethod;


typedef struct Il2CppReflectionGenericMethod
{
    Il2CppReflectionMethod base;
} Il2CppReflectionGenericMethod;


typedef struct Il2CppMethodInfo
{
    Il2CppReflectionType *parent;
    Il2CppReflectionType *ret;
    uint32_t attrs;
    uint32_t implattrs;
    uint32_t callconv;
} Il2CppMethodInfo;


typedef struct Il2CppPropertyInfo
{
    Il2CppReflectionType* parent;
    Il2CppReflectionType* declaringType;
    Il2CppString *name;
    Il2CppReflectionMethod *get;
    Il2CppReflectionMethod *set;
    uint32_t attrs;
} Il2CppPropertyInfo;


typedef struct Il2CppReflectionParameter
{
    Il2CppObject object;
    Il2CppReflectionType *ClassImpl;
    Il2CppObject *DefaultValueImpl;
    Il2CppObject *MemberImpl;
    Il2CppString *NameImpl;
    int32_t PositionImpl;
    uint32_t AttrsImpl;
    Il2CppObject *MarshalAsImpl;
} Il2CppReflectionParameter;


typedef struct Il2CppReflectionModule
{
    Il2CppObject obj;
    const Il2CppImage* image;
    Il2CppReflectionAssembly* assembly;
    Il2CppString* fqname;
    Il2CppString* name;
    Il2CppString* scopename;
    uint8_t is_resource;
    uint32_t token;
} Il2CppReflectionModule;


typedef struct Il2CppReflectionAssemblyName
{
    Il2CppObject obj;
    Il2CppString *name;
    Il2CppString *codebase;
    int32_t major, minor, build, revision;
    Il2CppObject *cultureInfo;
    uint32_t flags;
    uint32_t hashalg;
    Il2CppObject *keypair;
    Il2CppArray *publicKey;
    Il2CppArray *keyToken;
    uint32_t versioncompat;
    Il2CppObject *version;
    uint32_t processor_architecture;
    uint32_t contentType;
} Il2CppReflectionAssemblyName;


typedef struct Il2CppReflectionAssembly
{
    Il2CppObject object;
    const Il2CppAssembly *assembly;
    Il2CppObject *resolve_event_holder;

    Il2CppObject *evidence;
    Il2CppObject *minimum;
    Il2CppObject *optional;
    Il2CppObject *refuse;
    Il2CppObject *granted;
    Il2CppObject *denied;

    uint8_t from_byte_array;
    Il2CppString *name;
} Il2CppReflectionAssembly;


typedef struct Il2CppReflectionMarshal
{
    Il2CppObject object;
    int32_t count;
    int32_t type;
    int32_t eltype;
    Il2CppString* guid;
    Il2CppString* mcookie;
    Il2CppString* marshaltype;
    Il2CppObject* marshaltyperef;
    int32_t param_num;
    uint8_t has_size;
} Il2CppReflectionMarshal;


typedef struct Il2CppReflectionPointer
{
    Il2CppObject object;
    void* data;
    Il2CppReflectionType* type;
} Il2CppReflectionPointer;


typedef struct Il2CppInternalThread
{
    Il2CppObject obj;
    int lock_thread_id;



    void* handle;

    void* native_handle;
    Il2CppArray* cached_culture_info;
    Il2CppChar* name;
    int name_len;
    uint32_t state;
    Il2CppObject* abort_exc;
    int abort_state_handle;
    uint64_t tid;
    intptr_t debugger_thread;
    void** static_data;
    void* runtime_thread_info;
    Il2CppObject* current_appcontext;
    Il2CppObject* root_domain_thread;
    Il2CppArray* _serialized_principal;
    int _serialized_principal_version;
    void* appdomain_refs;
    int32_t interruption_requested;



    void* synch_cs;

    uint8_t threadpool_thread;
    uint8_t thread_interrupt_requested;
    int stack_size;
    uint8_t apartment_state;
    int critical_region_level;
    int managed_id;
    uint32_t small_id;
    void* manage_callback;
    void* interrupt_on_stop;
    intptr_t flags;
    void* thread_pinning_ref;
    void* abort_protected_block_count;
    int32_t priority;
    void* owned_mutexes;
    void * suspended;
    int32_t self_suspended;
    size_t thread_state;
    size_t unused2;
    void* last;
} Il2CppInternalThread;


typedef struct Il2CppIOSelectorJob
{
    Il2CppObject object;
    int32_t operation;
    Il2CppObject *callback;
    Il2CppObject *state;
} Il2CppIOSelectorJob;


typedef enum
{
    Il2Cpp_CallType_Sync = 0,
    Il2Cpp_CallType_BeginInvoke = 1,
    Il2Cpp_CallType_EndInvoke = 2,
    Il2Cpp_CallType_OneWay = 3
} Il2CppCallType;

typedef struct Il2CppMethodMessage
{
    Il2CppObject obj;
    Il2CppReflectionMethod *method;
    Il2CppArray *args;
    Il2CppArray *names;
    Il2CppArray *arg_types;
    Il2CppObject *ctx;
    Il2CppObject *rval;
    Il2CppObject *exc;
    Il2CppAsyncResult *async_result;
    uint32_t call_type;
} Il2CppMethodMessage;


typedef struct Il2CppAppDomainSetup
{
    Il2CppObject object;
    Il2CppString* application_base;
    Il2CppString* application_name;
    Il2CppString* cache_path;
    Il2CppString* configuration_file;
    Il2CppString* dynamic_base;
    Il2CppString* license_file;
    Il2CppString* private_bin_path;
    Il2CppString* private_bin_path_probe;
    Il2CppString* shadow_copy_directories;
    Il2CppString* shadow_copy_files;
    uint8_t publisher_policy;
    uint8_t path_changed;
    int loader_optimization;
    uint8_t disallow_binding_redirects;
    uint8_t disallow_code_downloads;
    Il2CppObject* activation_arguments;
    Il2CppObject* domain_initializer;
    Il2CppObject* application_trust;
    Il2CppArray* domain_initializer_args;
    uint8_t disallow_appbase_probe;
    Il2CppArray* configuration_bytes;
    Il2CppArray* serialized_non_primitives;
} Il2CppAppDomainSetup;



typedef struct Il2CppThread
{
    Il2CppObject obj;
    Il2CppInternalThread* internal_thread;
    Il2CppObject* start_obj;
    Il2CppException* pending_exception;
    Il2CppObject* principal;
    int32_t principal_version;
    Il2CppDelegate* delegate;
    Il2CppObject* executionContext;
    uint8_t executionContextBelongsToOuterScope;
} Il2CppThread;






typedef struct Il2CppException
{
    Il2CppObject object;

    Il2CppString* className;
    Il2CppString* message;
    Il2CppObject* _data;
    Il2CppException* inner_ex;
    Il2CppString* _helpURL;
    Il2CppArray* trace_ips;
    Il2CppString* stack_trace;
    Il2CppString* remote_stack_trace;
    int remote_stack_index;
    Il2CppObject* _dynamicMethods;
    il2cpp_hresult_t hresult;
    Il2CppString* source;
    Il2CppObject* safeSerializationManager;
    Il2CppArray* captured_traces;
    Il2CppArray* native_trace_ips;
} Il2CppException;


typedef struct Il2CppSystemException
{
    Il2CppException base;
} Il2CppSystemException;


typedef struct Il2CppArgumentException
{
    Il2CppException base;
    Il2CppString *argName;
} Il2CppArgumentException;


typedef struct Il2CppTypedRef
{
    const Il2CppType *type;
    void* value;
    Il2CppClass *klass;
} Il2CppTypedRef;


typedef struct Il2CppDelegate
{
    Il2CppObject object;


    Il2CppMethodPointer method_ptr;

    InvokerMethod invoke_impl;
    Il2CppObject *target;




    const MethodInfo *method;


    void* delegate_trampoline;

    intptr_t extraArg;





    uint8_t **method_code;
    Il2CppReflectionMethod *method_info;
    Il2CppReflectionMethod *original_method_info;
    Il2CppObject *data;

    uint8_t method_is_virtual;






} Il2CppDelegate;

typedef struct Il2CppMulticastDelegate
{
    Il2CppDelegate delegate;
    Il2CppArray *delegates;



} Il2CppMulticastDelegate;


typedef struct Il2CppMarshalByRefObject
{
    Il2CppObject obj;
    Il2CppObject *identity;
} Il2CppMarshalByRefObject;
typedef struct Il2CppAppDomain
{
    Il2CppMarshalByRefObject mbr;
    Il2CppDomain *data;
} Il2CppAppDomain;


typedef struct Il2CppStackFrame
{
    Il2CppObject obj;
    int32_t il_offset;
    int32_t native_offset;
    uint64_t methodAddress;
    uint32_t methodIndex;
    Il2CppReflectionMethod *method;
    Il2CppString *filename;
    int32_t line;
    int32_t column;
    Il2CppString *internal_method_name;
} Il2CppStackFrame;


typedef struct Il2CppDateTimeFormatInfo
{
    Il2CppObject obj;
    Il2CppObject* CultureData;
    Il2CppString* Name;
    Il2CppString* LangName;
    Il2CppObject* CompareInfo;
    Il2CppObject* CultureInfo;
    Il2CppString* AMDesignator;
    Il2CppString* PMDesignator;
    Il2CppString* DateSeparator;
    Il2CppString* GeneralShortTimePattern;
    Il2CppString* GeneralLongTimePattern;
    Il2CppString* TimeSeparator;
    Il2CppString* MonthDayPattern;
    Il2CppString* DateTimeOffsetPattern;
    Il2CppObject* Calendar;
    uint32_t FirstDayOfWeek;
    uint32_t CalendarWeekRule;
    Il2CppString* FullDateTimePattern;
    Il2CppArray* AbbreviatedDayNames;
    Il2CppArray* ShortDayNames;
    Il2CppArray* DayNames;
    Il2CppArray* AbbreviatedMonthNames;
    Il2CppArray* MonthNames;
    Il2CppArray* GenitiveMonthNames;
    Il2CppArray* GenitiveAbbreviatedMonthNames;
    Il2CppArray* LeapYearMonthNames;
    Il2CppString* LongDatePattern;
    Il2CppString* ShortDatePattern;
    Il2CppString* YearMonthPattern;
    Il2CppString* LongTimePattern;
    Il2CppString* ShortTimePattern;
    Il2CppArray* YearMonthPatterns;
    Il2CppArray* ShortDatePatterns;
    Il2CppArray* LongDatePatterns;
    Il2CppArray* ShortTimePatterns;
    Il2CppArray* LongTimePatterns;
    Il2CppArray* EraNames;
    Il2CppArray* AbbrevEraNames;
    Il2CppArray* AbbrevEnglishEraNames;
    Il2CppArray* OptionalCalendars;
    uint8_t readOnly;
    int32_t FormatFlags;
    int32_t CultureID;
    uint8_t UseUserOverride;
    uint8_t UseCalendarInfo;
    int32_t DataItem;
    uint8_t IsDefaultCalendar;
    Il2CppArray* DateWords;
    Il2CppString* FullTimeSpanPositivePattern;
    Il2CppString* FullTimeSpanNegativePattern;
    Il2CppArray* dtfiTokenHash;
} Il2CppDateTimeFormatInfo;


typedef struct Il2CppNumberFormatInfo
{
    Il2CppObject obj;
    Il2CppArray* numberGroupSizes;
    Il2CppArray* currencyGroupSizes;
    Il2CppArray* percentGroupSizes;
    Il2CppString* positiveSign;
    Il2CppString* negativeSign;
    Il2CppString* numberDecimalSeparator;
    Il2CppString* numberGroupSeparator;
    Il2CppString* currencyGroupSeparator;
    Il2CppString* currencyDecimalSeparator;
    Il2CppString* currencySymbol;
    Il2CppString* ansiCurrencySymbol;
    Il2CppString* naNSymbol;
    Il2CppString* positiveInfinitySymbol;
    Il2CppString* negativeInfinitySymbol;
    Il2CppString* percentDecimalSeparator;
    Il2CppString* percentGroupSeparator;
    Il2CppString* percentSymbol;
    Il2CppString* perMilleSymbol;
    Il2CppArray* nativeDigits;
    int dataItem;
    int numberDecimalDigits;
    int currencyDecimalDigits;
    int currencyPositivePattern;
    int currencyNegativePattern;
    int numberNegativePattern;
    int percentPositivePattern;
    int percentNegativePattern;
    int percentDecimalDigits;
    int digitSubstitution;
    uint8_t readOnly;
    uint8_t useUserOverride;
    uint8_t isInvariant;
    uint8_t validForParseAsNumber;
    uint8_t validForParseAsCurrency;
} Il2CppNumberFormatInfo;

typedef struct Il2CppCultureData
{
    Il2CppObject obj;
    Il2CppString *AMDesignator;
    Il2CppString *PMDesignator;
    Il2CppString *TimeSeparator;
    Il2CppArray *LongTimePatterns;
    Il2CppArray *ShortTimePatterns;
    uint32_t FirstDayOfWeek;
    uint32_t CalendarWeekRule;
} Il2CppCultureData;

typedef struct Il2CppCalendarData
{
    Il2CppObject obj;
    Il2CppString *NativeName;
    Il2CppArray *ShortDatePatterns;
    Il2CppArray *YearMonthPatterns;
    Il2CppArray *LongDatePatterns;
    Il2CppString *MonthDayPattern;

    Il2CppArray *EraNames;
    Il2CppArray *AbbreviatedEraNames;
    Il2CppArray *AbbreviatedEnglishEraNames;
    Il2CppArray *DayNames;
    Il2CppArray *AbbreviatedDayNames;
    Il2CppArray *SuperShortDayNames;
    Il2CppArray *MonthNames;
    Il2CppArray *AbbreviatedMonthNames;
    Il2CppArray *GenitiveMonthNames;
    Il2CppArray *GenitiveAbbreviatedMonthNames;
} Il2CppCalendarData;


typedef struct Il2CppCultureInfo
{
    Il2CppObject obj;
    uint8_t is_read_only;
    int32_t lcid;
    int32_t parent_lcid;

    int32_t datetime_index;
    int32_t number_index;

    int32_t default_calendar_type;

    uint8_t use_user_override;
    Il2CppNumberFormatInfo* number_format;
    Il2CppDateTimeFormatInfo* datetime_format;
    Il2CppObject* textinfo;
    Il2CppString* name;

    Il2CppString* englishname;
    Il2CppString* nativename;
    Il2CppString* iso3lang;
    Il2CppString* iso2lang;

    Il2CppString* win3lang;
    Il2CppString* territory;

    Il2CppArray* native_calendar_names;

    Il2CppString* compareinfo;

    const void* text_info_data;

    int dataItem;
    Il2CppObject* calendar;
    Il2CppObject* parent_culture;
    uint8_t constructed;
    Il2CppArray* cached_serialized_form;
    Il2CppObject* cultureData;
    uint8_t isInherited;
} Il2CppCultureInfo;


typedef struct Il2CppRegionInfo
{
    Il2CppObject obj;
    int32_t geo_id;
    Il2CppString* iso2name;
    Il2CppString* iso3name;
    Il2CppString* win3name;
    Il2CppString* english_name;
    Il2CppString* native_name;
    Il2CppString* currency_symbol;
    Il2CppString* iso_currency_symbol;
    Il2CppString* currency_english_name;
    Il2CppString* currency_native_name;
} Il2CppRegionInfo;



typedef struct Il2CppSafeHandle
{
    Il2CppObject base;
    void* handle;

    int state;
    uint8_t owns_handle;
    uint8_t fullyInitialized;
} Il2CppSafeHandle;


typedef struct Il2CppStringBuilder Il2CppStringBuilder;
typedef struct Il2CppStringBuilder
{
    Il2CppObject object;

    Il2CppArray* chunkChars;
    Il2CppStringBuilder* chunkPrevious;
    int chunkLength;
    int chunkOffset;
    int maxCapacity;
} Il2CppStringBuilder;


typedef struct Il2CppSocketAddress
{
    Il2CppObject base;
    int m_Size;
    Il2CppArray* data;
    uint8_t m_changed;
    int m_hash;
} Il2CppSocketAddress;


typedef struct Il2CppSortKey
{
    Il2CppObject base;
    Il2CppString *str;
    Il2CppArray *key;
    int32_t options;
    int32_t lcid;
} Il2CppSortKey;


typedef struct Il2CppErrorWrapper
{
    Il2CppObject base;
    int32_t errorCode;
} Il2CppErrorWrapper;


typedef struct Il2CppAsyncResult
{
    Il2CppObject base;
    Il2CppObject *async_state;
    Il2CppWaitHandle *handle;
    Il2CppDelegate *async_delegate;
    void* data;
    Il2CppAsyncCall *object_data;
    uint8_t sync_completed;
    uint8_t completed;
    uint8_t endinvoke_called;
    Il2CppObject *async_callback;
    Il2CppObject *execution_context;
    Il2CppObject *original_context;
} Il2CppAsyncResult;


typedef struct Il2CppAsyncCall
{
    Il2CppObject base;

    Il2CppMethodMessage *msg;

    MethodInfo *cb_method;
    Il2CppDelegate *cb_target;
    Il2CppObject *state;
    Il2CppObject *res;
    Il2CppArray *out_args;
} Il2CppAsyncCall;
typedef struct Il2CppExceptionWrapper Il2CppExceptionWrapper;
typedef struct Il2CppExceptionWrapper
{






    Il2CppException* ex;




} Il2CppExceptionWrapper;

typedef struct Il2CppIOAsyncResult
{
    Il2CppObject base;
    Il2CppDelegate* callback;
    Il2CppObject* state;
    Il2CppWaitHandle* wait_handle;
    uint8_t completed_synchronously;
    uint8_t completed;
} Il2CppIOAsyncResult;



typedef struct Il2CppSocketAsyncResult
{
    Il2CppIOAsyncResult base;
    Il2CppObject* socket;
    int32_t operation;
    Il2CppException* delayedException;
    Il2CppObject* endPoint;
    Il2CppArray* buffer;
    int32_t offset;
    int32_t size;
    int32_t socket_flags;
    Il2CppObject* acceptSocket;
    Il2CppArray* addresses;
    int32_t port;
    Il2CppObject* buffers;
    uint8_t reuseSocket;
    int32_t currentAddress;
    Il2CppObject* acceptedSocket;
    int32_t total;
    int32_t error;
    int32_t endCalled;
} Il2CppSocketAsyncResult;

typedef enum Il2CppResourceLocation
{
    IL2CPP_RESOURCE_LOCATION_EMBEDDED = 1,
    IL2CPP_RESOURCE_LOCATION_ANOTHER_ASSEMBLY = 2,
    IL2CPP_RESOURCE_LOCATION_IN_MANIFEST = 4
} Il2CppResourceLocation;


typedef struct Il2CppManifestResourceInfo
{
    Il2CppObject object;
    Il2CppReflectionAssembly* assembly;
    Il2CppString* filename;
    uint32_t location;
} Il2CppManifestResourceInfo;
typedef struct Il2CppAppContext
{
    Il2CppObject obj;
    int32_t domain_id;
    int32_t context_id;
    void* static_data;
} Il2CppAppContext;

typedef struct Il2CppDecimal
{
    uint16_t reserved;
    union
    {
        struct
        {
            uint8_t scale;
            uint8_t sign;
        } u;
        uint16_t signscale;
    } u;

    uint32_t Hi32;
    union
    {
        struct
        {
            uint32_t Lo32;
            uint32_t Mid32;
        } v;
        uint64_t Lo64;
    } v;
} Il2CppDecimal;


typedef struct Il2CppDouble
{






    uint32_t mantLo : 32;
    uint32_t mantHi : 20;
    uint32_t exp : 11;
    uint32_t sign : 1;

} Il2CppDouble;

typedef union Il2CppDouble_double
{
    Il2CppDouble s;
    double d;
} Il2CppDouble_double;

typedef enum Il2CppDecimalCompareResult
{
    IL2CPP_DECIMAL_CMP_LT = -1,
    IL2CPP_DECIMAL_CMP_EQ,
    IL2CPP_DECIMAL_CMP_GT
} Il2CppDecimalCompareResult;


typedef struct Il2CppSingle
{





    uint32_t mant : 23;
    uint32_t exp : 8;
    uint32_t sign : 1;

} Il2CppSingle;

typedef union Il2CppSingle_float
{
    Il2CppSingle s;
    float f;
} Il2CppSingle_float;

       

       


struct Il2CppSequencePoint;
struct Il2CppCatchPoint;
struct Il2CppSequencePointExecutionContext;
struct Il2CppThreadUnwindState;

typedef void(*DebugInfoInitialization)();
typedef void(*ThreadCallback)(void*, uintptr_t);

typedef struct Il2CppSequencePointExecutionContext
{
    const MethodInfo* method;
    void** thisArg;
    void** params;
    void** locals;
    Il2CppSequencePoint* currentSequencePoint;
    int32_t tryId;





} Il2CppSequencePointExecutionContext;


typedef struct Il2CppThreadUnwindState
{
    Il2CppSequencePointExecutionContext** executionContexts;
    uint32_t frameCount;
    uint32_t frameCapacity;
} Il2CppThreadUnwindState;
       


namespace il2cpp
{
namespace utils
{
    class LeaveTargetStack
    {
    public:
        LeaveTargetStack(void* storage) : m_Storage((int32_t*)storage), m_currentIndex(-1)
        {
        }

        void push(int32_t value)
        {




            m_currentIndex++;
            m_Storage[m_currentIndex] = value;
        }

        void pop()
        {
            if (m_currentIndex >= 0)
                m_currentIndex--;
        }

        int32_t top() const
        {
            return m_Storage[m_currentIndex];
        }

        uint8_t empty() const
        {
            return m_currentIndex == -1;
        }

    private:
        int32_t* m_Storage;
        int m_currentIndex;
    };
}
}
       



struct Il2CppString;

namespace il2cpp
{
namespace utils
{
    class Output
    {
    public:
        static void WriteToStdout(const char* message);
        static void WriteToStderr(const char* message);
    };
}
}

__attribute__ ((noreturn)) __attribute__ ((noinline)) void il2cpp_codegen_no_return();
inline double bankers_round(double x)
{
    double integerPart;
    if (x >= 0.0)
    {
        if (modf(x, &integerPart) == 0.5)
            return (int64_t)integerPart % 2 == 0 ? integerPart : integerPart + 1.0;
        return floor(x + 0.5);
    }
    else
    {
        if (modf(x, &integerPart) == -0.5)
            return (int64_t)integerPart % 2 == 0 ? integerPart : integerPart - 1.0;
        return ceil(x - 0.5);
    }
}

inline float bankers_roundf(float x)
{
    double integerPart;
    if (x >= 0.0f)
    {
        if (modf(x, &integerPart) == 0.5)
            return (int64_t)integerPart % 2 == 0 ? (float)integerPart : (float)integerPart + 1.0f;
        return floorf(x + 0.5f);
    }
    else
    {
        if (modf(x, &integerPart) == -0.5)
            return (int64_t)integerPart % 2 == 0 ? (float)integerPart : (float)integerPart - 1.0f;
        return ceilf(x - 0.5f);
    }
}




inline uint8_t il2cpp_codegen_check_mul_overflow_i64(int64_t a, int64_t b, int64_t imin, int64_t imax)
{

    uint64_t ua = (uint64_t)llabs(a);
    uint64_t ub = (uint64_t)llabs(b);

    uint64_t c;
    if ((a > 0 && b > 0) || (a <= 0 && b <= 0))
        c = (uint64_t)llabs(imax);
    else
        c = (uint64_t)llabs(imin);

    return ua != 0 && ub > c / ua;
}

inline uint8_t il2cpp_codegen_check_mul_oveflow_u64(uint64_t a, uint64_t b)
{
    return b != 0 && (a * b) / b != a;
}

inline int32_t il2cpp_codegen_abs(uint32_t value)
{
    return abs(static_cast<int32_t>(value));
}

inline int32_t il2cpp_codegen_abs(int32_t value)
{
    return abs(value);
}

inline int64_t il2cpp_codegen_abs(uint64_t value)
{
    return llabs(static_cast<int64_t>(value));
}

inline int64_t il2cpp_codegen_abs(int64_t value)
{
    return llabs(value);
}

template<typename TInput, typename TOutput, typename TFloat>
inline TOutput il2cpp_codegen_cast_floating_point(TFloat value)
{
    return (TOutput)((TInput)value);
}






template<typename T>
inline T il2cpp_codegen_cast_double_to_int(double value)
{




    return (T)value;
}
inline void Il2CppCodeGenWriteBarrier(void** targetAddress, void* object) {}


void il2cpp_codegen_memory_barrier();

template<typename T>
inline T VolatileRead(T* location)
{
    T result = *location;
    il2cpp_codegen_memory_barrier();
    return result;
}

template<typename T, typename U>
inline void VolatileWrite(T** location, U* value)
{
    il2cpp_codegen_memory_barrier();
    *location = value;
    Il2CppCodeGenWriteBarrier((void**)location, value);
}

template<typename T, typename U>
inline void VolatileWrite(T* location, U value)
{
    il2cpp_codegen_memory_barrier();
    *location = value;
}

inline void il2cpp_codegen_write_to_stdout(const char* str)
{
    il2cpp::utils::Output::WriteToStdout(str);
}
inline void il2cpp_codegen_write_to_stderr(const char* str)
{
    il2cpp::utils::Output::WriteToStderr(str);
}
__attribute__ ((noreturn)) void il2cpp_codegen_abort();

inline uint8_t il2cpp_codegen_check_add_overflow(int64_t left, int64_t right)
{
    return (right >= 0 && left > kIl2CppInt64Max - right) ||
        (left < 0 && right < kIl2CppInt64Min - left);
}

inline uint8_t il2cpp_codegen_check_sub_overflow(int64_t left, int64_t right)
{
    return (right >= 0 && left < kIl2CppInt64Min + right) ||
        (right < 0 && left > kIl2CppInt64Max + right);
}

template<uint8_t, class T, class U>
struct pick_first;

template<class T, class U>
struct pick_first<true, T, U>
{
    typedef T type;
};

template<class T, class U>
struct pick_first<false, T, U>
{
    typedef U type;
};

template<class T, class U>
struct pick_bigger
{
    typedef typename pick_first<(sizeof(T) >= sizeof(U)), T, U>::type type;
};

template<typename T, typename U>
inline typename pick_bigger<T, U>::type il2cpp_codegen_multiply(T left, U right)
{
    return left * right;
}

template<typename T, typename U>
inline typename pick_bigger<T, U>::type il2cpp_codegen_add(T left, U right)
{
    return left + right;
}

template<typename T, typename U>
inline typename pick_bigger<T, U>::type il2cpp_codegen_subtract(T left, U right)
{
    return left - right;
}

inline void il2cpp_codegen_memcpy(void* dest, const void* src, size_t count)
{
    memcpy(dest, src, count);
}

inline void il2cpp_codegen_memset(void* ptr, int value, size_t num)
{
    memset(ptr, value, num);
}

inline void il2cpp_codegen_register_debugger_data(const Il2CppDebuggerMetadataRegistration *data)
{



}

inline void il2cpp_codegen_check_sequence_point(Il2CppSequencePointExecutionContext* executionContext, Il2CppSequencePoint* seqPoint)
{



}

inline void il2cpp_codegen_check_sequence_point_entry(Il2CppSequencePointExecutionContext* executionContext, Il2CppSequencePoint* seqPoint)
{



}

inline void il2cpp_codegen_check_sequence_point_exit(Il2CppSequencePointExecutionContext* executionContext, Il2CppSequencePoint* seqPoint)
{



}

inline void il2cpp_codegen_check_pause_point()
{



}

class MethodExitSequencePointChecker
{
private:
    Il2CppSequencePoint* m_seqPoint;
    Il2CppSequencePointExecutionContext* m_seqPointStorage;

public:
    MethodExitSequencePointChecker(Il2CppSequencePointExecutionContext* seqPointStorage, Il2CppSequencePoint* seqPoint) :
        m_seqPointStorage(seqPointStorage), m_seqPoint(seqPoint)
    {
    }

    ~MethodExitSequencePointChecker()
    {



    }
};
inline uint8_t il2cpp_codegen_object_reference_equals(const RuntimeObject *obj1, const RuntimeObject *obj2)
{
    return obj1 == obj2;
}

inline uint8_t il2cpp_codegen_platform_is_osx_or_ios()
{
    return 0 != 0 || 0 != 0;
}

inline uint8_t il2cpp_codegen_platform_is_freebsd()
{

    return false;
}

inline uint8_t il2cpp_codegen_platform_disable_libc_pinvoke()
{
    return 0;
}

       




namespace il2cpp
{
namespace icalls
{
namespace mscorlib
{
namespace System
{
namespace Threading
{
    class Interlocked
    {
    public:
        static int32_t Add(int32_t* location1, int32_t value);
        static int64_t Add64(int64_t* location1, int64_t value);
        static int32_t CompareExchange(int32_t* location, int32_t value, int32_t comparand);
        static int64_t CompareExchange64(int64_t* location1, int64_t value, int64_t comparand);
        static double CompareExchangeDouble(double* location1, double value, double comparand);
        static intptr_t CompareExchangeIntPtr(intptr_t* location, intptr_t value, intptr_t comparand);
        static float CompareExchangeSingle(float* location1, float value, float comparand);
        static void* CompareExchange_T(void** location, void* value, void* comparand);
        static int32_t Decrement(int32_t* location);
        static int64_t Decrement64(int64_t* location);
        static intptr_t ExchangeIntPtr(intptr_t* location, intptr_t value);
        static int32_t Exchange(int32_t* location1, int32_t value);
        static int64_t Exchange64(int64_t* location1, int64_t value);
        static double ExchangeDouble(double* location1, double value);
        static void* ExchangePointer(void** location1, void* value);
        static float ExchangeSingle(float* location1, float value);
        static int32_t Increment(int32_t* value);
        static int64_t Increment64(int64_t* location);
        static int64_t Read(int64_t* location);
        static int32_t CompareExchange(int32_t* location1, int32_t value, int32_t comparand, uint8_t* succeeded);
    };
}
}
}
}
}
       




struct mscorlib_System_Guid;
struct mscorlib_System_Reflection_MemberInfo;

struct Il2CppObject;
struct Il2CppDelegate;
struct Il2CppReflectionType;
struct Il2CppString;

namespace il2cpp
{
namespace icalls
{
namespace mscorlib
{
namespace System
{
namespace Runtime
{
namespace InteropServices
{
    class Marshal
    {
    public:
        static int32_t GetLastWin32Error();
        static int32_t AddRefInternal(intptr_t pUnk);
        static intptr_t AllocCoTaskMem(int32_t size);
        static intptr_t AllocHGlobal(intptr_t size);
        static void DestroyStructure(intptr_t ptr, Il2CppReflectionType* structureType);
        static void FreeBSTR(intptr_t ptr);
        static void FreeCoTaskMem(intptr_t ptr);
        static void FreeHGlobal(intptr_t hglobal);
        static intptr_t GetCCW(Il2CppObject* o, Il2CppReflectionType * T);
        static int32_t GetComSlotForMethodInfoInternal(mscorlib_System_Reflection_MemberInfo * m);
        static Il2CppDelegate* GetDelegateForFunctionPointerInternal(intptr_t ptr, Il2CppReflectionType* t);
        static intptr_t GetFunctionPointerForDelegateInternal(Il2CppDelegate* d);
        static intptr_t GetIDispatchForObjectInternal(Il2CppObject* o);
        static intptr_t GetIUnknownForObjectInternal(Il2CppObject* o);
        static Il2CppObject* GetObjectForCCW(intptr_t pUnk);
        static uint8_t IsComObject(Il2CppObject* o);
        static intptr_t OffsetOf(Il2CppReflectionType* t, Il2CppString* fieldName);
        static void Prelink(Il2CppReflectionMethod* m);
        static void PrelinkAll(Il2CppReflectionType* c);
        static Il2CppString* PtrToStringAnsi_mscorlib_System_String_mscorlib_System_IntPtr(intptr_t ptr);
        static Il2CppString* PtrToStringAnsi_mscorlib_System_String_mscorlib_System_IntPtr_mscorlib_System_Int32(intptr_t ptr, int32_t len);
        static Il2CppString* PtrToStringBSTR(intptr_t ptr);
        static Il2CppString* PtrToStringUni_mscorlib_System_String_mscorlib_System_IntPtr(intptr_t ptr);
        static Il2CppString* PtrToStringUni_mscorlib_System_String_mscorlib_System_IntPtr_mscorlib_System_Int32(intptr_t ptr, int32_t len);
        static Il2CppObject* PtrToStructure(intptr_t ptr, Il2CppReflectionType * structureType);
        static void PtrToStructureObject(intptr_t ptr, Il2CppObject* structure);
        static int32_t QueryInterfaceInternal(intptr_t pUnk, mscorlib_System_Guid * iid, intptr_t* ppv);
        static intptr_t ReAllocCoTaskMem(intptr_t ptr, int32_t size);
        static intptr_t ReAllocHGlobal(intptr_t ptr, intptr_t size);
        static uint8_t ReadByte(intptr_t ptr, int32_t ofs);
        static int16_t ReadInt16(intptr_t ptr, int32_t ofs);
        static int32_t ReadInt32(intptr_t ptr, int32_t ofs);
        static int64_t ReadInt64(intptr_t ptr, int32_t ofs);
        static intptr_t ReadIntPtr(intptr_t ptr, int32_t ofs);
        static int32_t ReleaseComObjectInternal(Il2CppObject* co);
        static int32_t ReleaseInternal(intptr_t pUnk);
        static int SizeOf(Il2CppReflectionType * rtype);
        static intptr_t StringToBSTR(Il2CppString* s);
        static intptr_t StringToHGlobalAnsi(Il2CppString* s);
        static intptr_t StringToHGlobalUni(Il2CppString* s);
        static void StructureToPtr(Il2CppObject* structure, intptr_t ptr, uint8_t deleteOld);
        static intptr_t UnsafeAddrOfPinnedArrayElement(Il2CppArray* arr, int32_t index);
        static void WriteByte(intptr_t ptr, int32_t ofs, uint8_t val);
        static void WriteInt16(intptr_t ptr, int32_t ofs, int16_t val);
        static void WriteInt32(intptr_t ptr, int32_t ofs, int32_t val);
        static void WriteInt64(intptr_t ptr, int32_t ofs, int64_t val);
        static void copy_from_unmanaged(intptr_t, int, Il2CppArray *, int);
        static void copy_to_unmanaged(Il2CppArray * source, int32_t startIndex, intptr_t destination, int32_t length);
        static void WriteIntPtr(intptr_t ptr, int32_t ofs, intptr_t val);

        static intptr_t BufferToBSTR(Il2CppArray* ptr, int32_t slen);

        static int32_t GetHRForException_WinRT(Il2CppException* e);
        static intptr_t GetRawIUnknownForComObjectNoAddRef(Il2CppObject* o);
        static Il2CppObject* GetNativeActivationFactory(Il2CppObject* type);

        static intptr_t AllocCoTaskMemSize(intptr_t sizet);
    };
}
}
}
}
}
}
       


       







namespace il2cpp
{
namespace utils
{
    template<typename CharType>
    class StringView
    {
    private:
        const CharType* m_String;
        size_t m_Length;


        inline StringView() :
            m_String(NULL),
            m_Length(0)
        {
        }

    public:
        template<size_t Length>
        inline StringView(const CharType(&str)[Length]) :
            m_String(str), m_Length(Length - 1)
        {
        }

        inline StringView(const CharType* str, size_t length) :
            m_String(str), m_Length(length)
        {
            (void(0));
        }

        inline StringView(const CharType* str, size_t startIndex, size_t length) :
            m_String(str + startIndex), m_Length(length)
        {
            (void(0));
        }

        inline StringView(const StringView<CharType>& str, size_t startIndex, size_t length) :
            m_String(str.Str() + startIndex),
            m_Length(length)
        {
            (void(0));
        }
        inline const CharType* Str() const
        {
            return m_String;
        }

        inline size_t Length() const
        {
            return m_Length;
        }

        inline CharType operator[](size_t index) const
        {
            return m_String[index];
        }

        inline uint8_t IsNullTerminated() const
        {
            return m_String[m_Length] == 0;
        }

        inline uint8_t IsEmpty() const
        {
            return Length() == 0;
        }

        static inline StringView<CharType> Empty()
        {
            return StringView<CharType>();
        }

        inline size_t Find(CharType c, size_t startIndex = 0) const
        {
            const CharType* end = m_String + m_Length;
            for (const CharType* ptr = m_String + startIndex; ptr < end; ptr++)
            {
                if (*ptr == c)
                    return ptr - m_String;
            }

            return NPos();
        }

        inline size_t RFind(CharType c) const
        {
            for (const CharType* ptr = m_String + m_Length; ptr-- > m_String;)
            {
                if (*ptr == c)
                    return ptr - m_String;
            }

            return NPos();
        }

        inline StringView<CharType> SubStr(size_t startIndex, size_t length)
        {
            return StringView<CharType>(*this, startIndex, length);
        }

        inline StringView<CharType> SubStr(size_t startIndex)
        {
            return StringView<CharType>(*this, startIndex, Length() - startIndex);
        }

        inline static size_t NPos()
        {
            return static_cast<size_t>(-1);
        }

        inline uint8_t TryParseAsInt(int& outResult)
        {
            if (Length() == 0)
                return false;

            int result = 0;
            uint8_t isNegative = false;
            const CharType* ptr = m_String;
            const CharType* end = m_String + m_Length;

            if (ptr[0] == '-')
            {
                isNegative = true;
                ptr++;
            }

            for (; ptr < end; ptr++)
            {
                CharType digit = *ptr;
                if (digit < '0' || digit > '9')
                    return false;

                int digitNumeric = digit - '0';
                if (result > INT_MAX / 10)
                    return false;

                result = result * 10;
                if (result > INT_MAX - digitNumeric)
                    return false;

                result += digitNumeric;
            }

            if (isNegative)
            {
                outResult = -result;
            }
            else
            {
                outResult = result;
            }

            return true;
        }
    };
}
}

namespace il2cpp
{
namespace utils
{
    class Il2CppHStringReference
    {
    private:
        Il2CppHString m_String;
        Il2CppHStringHeader m_Header;

    public:
        Il2CppHStringReference(const StringView<Il2CppNativeChar>& str);

        inline operator Il2CppHString() const
        {
            return m_String;
        }
    };
}
}
       



namespace il2cpp
{
namespace utils
{
    class RegisterRuntimeInitializeAndCleanup
    {
    public:
        typedef void (*CallbackFunction) ();
        RegisterRuntimeInitializeAndCleanup(CallbackFunction Initialize, CallbackFunction Cleanup, int order = 0);

        static void ExecuteInitializations();
        static void ExecuteCleanup();
    };
}
}

       



namespace il2cpp
{
namespace vm
{
    class Atomic
    {
    public:
        static int32_t Add(volatile int32_t* location1, int32_t value);
        static int64_t Add64(volatile int64_t* location1, int64_t value);
        static int32_t Increment(volatile int32_t* value);
        static int64_t Increment64(volatile int64_t* value);
        static int32_t Decrement(volatile int32_t* value);
        static int64_t Decrement64(volatile int64_t* value);
        static int32_t CompareExchange(volatile int32_t* dest, int32_t exchange, int32_t comparand);
        static int64_t CompareExchange64(volatile int64_t* dest, int64_t exchange, int64_t comparand);
        static void* CompareExchangePointer(void* volatile* dest, void* exchange, void* comparand);
        static int32_t Exchange(volatile int32_t* dest, int32_t exchange);
        static int64_t Exchange64(volatile int64_t* dest, int64_t exchange);
        static void* ExchangePointer(void* volatile* dest, void* exchange);
        static int64_t Read64(volatile int64_t* addr);
        static void FullMemoryBarrier();

        static inline uint32_t Add(volatile uint32_t* location1, uint32_t value)
        {
            return static_cast<uint32_t>(Add((volatile int32_t*)location1, (int32_t)value));
        }

        template<typename T>
        static inline T* CompareExchangePointer(T* volatile* dest, T* newValue, T* oldValue)
        {
            return static_cast<T*>(CompareExchangePointer((void*volatile*)dest, newValue, oldValue));
        }

        template<typename T>
        static inline T* ExchangePointer(T* volatile* dest, T* newValue)
        {
            return static_cast<T*>(ExchangePointer((void*volatile*)dest, newValue));
        }

        static inline uint64_t Read64(volatile uint64_t* addr)
        {
            return static_cast<uint64_t>(Read64((volatile int64_t*)addr));
        }

        template<typename T>
        static inline T* ReadPointer(T* volatile* pointer)
        {



            return reinterpret_cast<T*>(Add64(reinterpret_cast<volatile int64_t*>(pointer), 0));

        }

        static inline uint32_t Increment(volatile uint32_t* value)
        {
            return static_cast<uint32_t>(Increment(reinterpret_cast<volatile int32_t*>(value)));
        }

        static inline uint64_t Increment64(volatile uint64_t* value)
        {
            return static_cast<uint64_t>(Increment64(reinterpret_cast<volatile int64_t*>(value)));
        }

        static inline uint32_t Decrement(volatile uint32_t* value)
        {
            return static_cast<uint32_t>(Decrement(reinterpret_cast<volatile int32_t*>(value)));
        }

        static inline uint64_t Decrement64(volatile uint64_t* value)
        {
            return static_cast<uint64_t>(Decrement64(reinterpret_cast<volatile int64_t*>(value)));
        }

        static inline uint32_t CompareExchange(volatile uint32_t* value, uint32_t newValue, uint32_t oldValue)
        {
            return static_cast<uint32_t>(CompareExchange(reinterpret_cast<volatile int32_t*>(value), newValue, oldValue));
        }

        static inline uint64_t CompareExchange64(volatile uint64_t* value, uint64_t newValue, uint64_t oldValue)
        {
            return static_cast<uint64_t>(CompareExchange64(reinterpret_cast<volatile int64_t*>(value), newValue, oldValue));
        }

        static inline uint32_t Exchange(volatile uint32_t* value, uint32_t newValue)
        {
            return static_cast<uint32_t>(Exchange(reinterpret_cast<volatile int32_t*>(value), newValue));
        }

        static inline uint64_t Exchange64(volatile uint64_t* value, uint64_t newValue)
        {
            return static_cast<uint64_t>(Exchange64(reinterpret_cast<volatile int64_t*>(value), newValue));
        }
    };
}
}
       






namespace il2cpp
{
namespace vm
{
    class ClassInlines
    {
    public:

        static inline uint8_t HasParentUnsafe(const Il2CppClass* klass, const Il2CppClass* parent) { return klass->typeHierarchyDepth >= parent->typeHierarchyDepth && klass->typeHierarchy[parent->typeHierarchyDepth - 1] == parent; }



        static inline __attribute__ ((always_inline)) uint8_t InitFromCodegen(Il2CppClass *klass)
        {
            if (klass->initialized_and_no_error)
                return true;
            return InitFromCodegenSlow(klass);
        }

        static __attribute__ ((noinline)) uint8_t InitFromCodegenSlow(Il2CppClass *klass);


        static inline __attribute__ ((always_inline)) const VirtualInvokeData& GetInterfaceInvokeDataFromVTable(Il2CppObject* obj, const Il2CppClass* itf, Il2CppMethodSlot slot)
        {
            const Il2CppClass* klass = obj->klass;
            (void(0));
            (void(0));

            for (uint16_t i = 0; i < klass->interface_offsets_count; i++)
            {
                if (klass->interfaceOffsets[i].interfaceType == itf)
                {
                    int32_t offset = klass->interfaceOffsets[i].offset;
                    (void(0));
                    (void(0));
                    return klass->vtable[offset + slot];
                }
            }

            return GetInterfaceInvokeDataFromVTableSlowPath(obj, itf, slot);
        }

        static inline __attribute__ ((always_inline)) const VirtualInvokeData* GetInterfaceInvokeDataFromVTable(const Il2CppClass* klass, const Il2CppClass* itf, Il2CppMethodSlot slot)
        {
            (void(0));
            (void(0));

            for (uint16_t i = 0; i < klass->interface_offsets_count; i++)
            {
                if (klass->interfaceOffsets[i].interfaceType == itf)
                {
                    int32_t offset = klass->interfaceOffsets[i].offset;
                    (void(0));
                    (void(0));
                    return &klass->vtable[offset + slot];
                }
            }

            return GetInterfaceInvokeDataFromVTableSlowPath(klass, itf, slot);
        }


        static __attribute__ ((noinline)) const VirtualInvokeData& GetInterfaceInvokeDataFromVTableSlowPath(Il2CppObject* obj, const Il2CppClass* itf, Il2CppMethodSlot slot);
        static __attribute__ ((noinline)) const VirtualInvokeData* GetInterfaceInvokeDataFromVTableSlowPath(const Il2CppClass* klass, const Il2CppClass* itf, Il2CppMethodSlot slot);
    };
}
}
       

struct Il2CppThread;

namespace il2cpp
{
namespace vm
{
    class ScopedThreadAttacher
    {
    public:
        ScopedThreadAttacher();
        ~ScopedThreadAttacher();

    private:
        Il2CppThread* m_AttachedThread;
    };
}
}
       




struct Il2CppString;

namespace il2cpp
{
namespace vm
{
    class String
    {
    public:

        static Il2CppString* Empty();
        static int32_t GetLength(Il2CppString* str);
        static int32_t GetHash(Il2CppString* str);
        static Il2CppString* New(const char* str);
        static Il2CppString* NewWrapper(const char* str);
        static Il2CppString* NewLen(const char* str, uint32_t length);
        static Il2CppString* NewSize(int32_t len);
        static Il2CppString* NewUtf16(const Il2CppChar *text, int32_t len);
        static Il2CppString* NewUtf16(const utils::StringView<Il2CppChar>& text);

    public:
        static void InitializeEmptyString(Il2CppClass* stringClass);
        static void CleanupEmptyString();
        static Il2CppString* Intern(Il2CppString* str);
        static Il2CppString* IsInterned(Il2CppString* str);
    };
}
}



void il2cpp_codegen_profiler_method_enter(const RuntimeMethod* method);

void il2cpp_codegen_profiler_method_exit(const RuntimeMethod* method);



struct ProfilerMethodSentry
{
    ProfilerMethodSentry(const RuntimeMethod* method)

        : m_method(method)

    {

        il2cpp_codegen_profiler_method_enter(m_method);

    }

    ~ProfilerMethodSentry()
    {

        il2cpp_codegen_profiler_method_exit(m_method);

    }

private:
    const RuntimeMethod* m_method;
};

void il2cpp_codegen_stacktrace_push_frame(Il2CppStackFrameInfo& frame);

void il2cpp_codegen_stacktrace_pop_frame();

struct StackTraceSentry
{
    StackTraceSentry(const RuntimeMethod* method) : m_method(method)
    {
        Il2CppStackFrameInfo frame_info;

        frame_info.method = method;

        il2cpp_codegen_stacktrace_push_frame(frame_info);
    }

    ~StackTraceSentry()
    {
        il2cpp_codegen_stacktrace_pop_frame();
    }

private:
    const RuntimeMethod* m_method;
};



template<typename T>
struct Il2CppFakeBox : RuntimeObject
{
    T m_Value;

    Il2CppFakeBox(RuntimeClass* boxedType, T* value)
    {
        klass = boxedType;
        monitor = (MonitorData*)UINTPTR_MAX;
        m_Value = *value;
    }
};

inline uint8_t il2cpp_codegen_is_fake_boxed_object(RuntimeObject* object)
{
    return object->monitor == (MonitorData*)UINTPTR_MAX;
}





void il2cpp_codegen_register(const Il2CppCodeRegistration* const codeRegistration, const Il2CppMetadataRegistration* const metadataRegistration, const Il2CppCodeGenOptions* const codeGenOptions);



void* il2cpp_codegen_get_thread_static_data(RuntimeClass* klass);

String_t* il2cpp_codegen_string_new_wrapper(const char* str);

String_t* il2cpp_codegen_string_new_utf16(const il2cpp::utils::StringView<Il2CppChar>& str);

Type_t* il2cpp_codegen_type_get_object(const RuntimeType* type);

__attribute__ ((noreturn)) void il2cpp_codegen_raise_exception(Exception_t *ex, MethodInfo* lastManagedFrame = NULL);

__attribute__ ((noreturn)) void il2cpp_codegen_raise_exception(il2cpp_hresult_t hresult, uint8_t defaultToCOMException);

void il2cpp_codegen_raise_execution_engine_exception_if_method_is_not_found(const RuntimeMethod* method);

void il2cpp_codegen_raise_execution_engine_exception(const RuntimeMethod* method);

__attribute__ ((noreturn)) void il2cpp_codegen_raise_out_of_memory_exception();

__attribute__ ((noreturn)) void il2cpp_codegen_raise_null_reference_exception();

__attribute__ ((noreturn)) void il2cpp_codegen_raise_divide_by_zero_exception();

Exception_t* il2cpp_codegen_get_argument_exception(const char* param, const char* msg);

Exception_t* il2cpp_codegen_get_argument_null_exception(const char* param);

Exception_t* il2cpp_codegen_get_overflow_exception();

Exception_t* il2cpp_codegen_get_not_supported_exception(const char* msg);

Exception_t* il2cpp_codegen_get_array_type_mismatch_exception();

Exception_t* il2cpp_codegen_get_invalid_cast_exception(const char* msg);

Exception_t* il2cpp_codegen_get_invalid_operation_exception(const char* msg);

Exception_t* il2cpp_codegen_get_marshal_directive_exception(const char* msg);

Exception_t* il2cpp_codegen_get_missing_method_exception(const char* msg);

Exception_t* il2cpp_codegen_get_maximum_nested_generics_exception();

Exception_t* il2cpp_codegen_get_index_out_of_range_exception();

Exception_t* il2cpp_codegen_get_exception(il2cpp_hresult_t hresult, uint8_t defaultToCOMException);

inline RuntimeClass* il2cpp_codegen_object_class(RuntimeObject* obj)
{
    return obj->klass;
}



RuntimeObject* IsInst(RuntimeObject *obj, RuntimeClass* targetType);

inline RuntimeObject* IsInstSealed(RuntimeObject *obj, RuntimeClass* targetType)
{




    if (!obj)
        return NULL;


    return (obj->klass == targetType ? obj : NULL);
}

inline RuntimeObject* IsInstClass(RuntimeObject *obj, RuntimeClass* targetType)
{



    if (!obj)
        return NULL;


    return il2cpp::vm::ClassInlines::HasParentUnsafe(obj->klass, targetType) ? obj : NULL;
}



__attribute__ ((noreturn)) void RaiseInvalidCastException(RuntimeObject* obj, RuntimeClass* targetType);

inline RuntimeObject* Castclass(RuntimeObject *obj, RuntimeClass* targetType)
{
    if (!obj)
        return NULL;

    RuntimeObject* result = IsInst(obj, targetType);
    if (result)
        return result;

    RaiseInvalidCastException(obj, targetType);
    return NULL;
}

inline RuntimeObject* CastclassSealed(RuntimeObject *obj, RuntimeClass* targetType)
{
    if (!obj)
        return NULL;

    RuntimeObject* result = IsInstSealed(obj, targetType);
    if (result)
        return result;

    RaiseInvalidCastException(obj, targetType);
    return NULL;
}

inline RuntimeObject* CastclassClass(RuntimeObject *obj, RuntimeClass* targetType)
{
    if (!obj)
        return NULL;

    RuntimeObject* result = IsInstClass(obj, targetType);
    if (result)
        return result;

    RaiseInvalidCastException(obj, targetType);
    return NULL;
}

inline void NullCheck(void* this_ptr)
{
    if (this_ptr != NULL)
        return;

    il2cpp_codegen_raise_null_reference_exception();
}



RuntimeObject* Box(RuntimeClass* type, void* data);



void* Unbox_internal(Il2CppObject* obj);

inline void* UnBox(RuntimeObject* obj)
{
    NullCheck(obj);
    return Unbox_internal(obj);
}

inline void* UnBox(RuntimeObject* obj, RuntimeClass* expectedBoxedClass)
{
    NullCheck(obj);

    if (obj->klass->element_class == expectedBoxedClass->element_class)
        return Unbox_internal(obj);

    RaiseInvalidCastException(obj, expectedBoxedClass);
    return NULL;
}

void UnBoxNullable_internal(RuntimeObject* obj, RuntimeClass* expectedBoxedClass, void* storage);

inline void UnBoxNullable(RuntimeObject* obj, RuntimeClass* expectedBoxedClass, void* storage)
{


    if (obj != NULL)
    {
        if (obj->klass->element_class != expectedBoxedClass->element_class)
            RaiseInvalidCastException(obj, expectedBoxedClass);
    }

    UnBoxNullable_internal(obj, expectedBoxedClass, storage);
}

int32_t il2cpp_codgen_class_get_instance_size(RuntimeClass* klass);

inline uint32_t il2cpp_codegen_sizeof(RuntimeClass* klass)
{
    if (!klass->valuetype)
    {
        return sizeof(void*);
    }

    return il2cpp_codgen_class_get_instance_size(klass) - sizeof(RuntimeObject);
}

inline uint8_t il2cpp_codegen_method_is_virtual(RuntimeMethod* method)
{
    return method->slot != kInvalidIl2CppMethodSlot;
}

inline uint8_t il2cpp_codegen_object_is_of_sealed_type(RuntimeObject* obj)
{
    (void(0));
    return (obj->klass->flags & 0x00000100) != 0;
}

uint8_t il2cpp_codegen_method_is_generic_instance(RuntimeMethod* method);

RuntimeClass* il2cpp_codegen_method_get_declaring_type(RuntimeMethod* method);

uint8_t il2cpp_codegen_method_is_interface_method(RuntimeMethod* method);

inline uint16_t il2cpp_codegen_method_get_slot(RuntimeMethod* method)
{
    return method->slot;
}

inline __attribute__ ((always_inline)) const VirtualInvokeData& il2cpp_codegen_get_virtual_invoke_data(Il2CppMethodSlot slot, const RuntimeObject* obj)
{
    Assert(slot != kInvalidIl2CppMethodSlot && "il2cpp_codegen_get_virtual_invoke_data got called on a non-virtual method");
    return obj->klass->vtable[slot];
}

inline __attribute__ ((always_inline)) const VirtualInvokeData& il2cpp_codegen_get_interface_invoke_data(Il2CppMethodSlot slot, RuntimeObject* obj, const RuntimeClass* declaringInterface)
{
    Assert(slot != kInvalidIl2CppMethodSlot && "il2cpp_codegen_get_interface_invoke_data got called on a non-virtual method");
    return il2cpp::vm::ClassInlines::GetInterfaceInvokeDataFromVTable(obj, declaringInterface, slot);
}

const RuntimeMethod* il2cpp_codegen_get_generic_virtual_method_internal(const RuntimeMethod* methodDefinition, const RuntimeMethod* inflatedMethod);

inline __attribute__ ((always_inline)) const RuntimeMethod* il2cpp_codegen_get_generic_virtual_method(const RuntimeMethod* method, const RuntimeObject* obj)
{
    uint16_t slot = method->slot;
    const RuntimeMethod* methodDefinition = obj->klass->vtable[slot].method;
    return il2cpp_codegen_get_generic_virtual_method_internal(methodDefinition, method);
}

inline __attribute__ ((always_inline)) void il2cpp_codegen_get_generic_virtual_invoke_data(const RuntimeMethod* method, const RuntimeObject* obj, VirtualInvokeData* invokeData)
{
    const RuntimeMethod* targetRuntimeMethod = il2cpp_codegen_get_generic_virtual_method(method, obj);




    invokeData->methodPtr = targetRuntimeMethod->methodPointer;
    invokeData->method = targetRuntimeMethod;
}

inline __attribute__ ((always_inline)) const RuntimeMethod* il2cpp_codegen_get_generic_interface_method(const RuntimeMethod* method, RuntimeObject* obj)
{
    const RuntimeMethod* methodDefinition = il2cpp::vm::ClassInlines::GetInterfaceInvokeDataFromVTable(obj, method->klass, method->slot).method;
    return il2cpp_codegen_get_generic_virtual_method_internal(methodDefinition, method);
}

inline __attribute__ ((always_inline)) void il2cpp_codegen_get_generic_interface_invoke_data(const RuntimeMethod* method, RuntimeObject* obj, VirtualInvokeData* invokeData)
{
    const RuntimeMethod* targetRuntimeMethod = il2cpp_codegen_get_generic_interface_method(method, obj);





    invokeData->methodPtr = targetRuntimeMethod->methodPointer;
    invokeData->method = targetRuntimeMethod;
}

inline RuntimeClass* InitializedTypeInfo(RuntimeClass* klass)
{
    il2cpp::vm::ClassInlines::InitFromCodegen(klass);
    return klass;
}

RuntimeClass* il2cpp_codegen_class_from_type_internal(const RuntimeType* type);

inline RuntimeClass* il2cpp_codegen_class_from_type(const RuntimeType *type)
{
    return InitializedTypeInfo(il2cpp_codegen_class_from_type_internal(type));
}

inline void* InterlockedExchangeImplRef(void** location, void* value)
{
    return il2cpp::icalls::mscorlib::System::Threading::Interlocked::ExchangePointer(location, value);
}

template<typename T>
inline T InterlockedCompareExchangeImpl(T* location, T value, T comparand)
{
    return (T)il2cpp::icalls::mscorlib::System::Threading::Interlocked::CompareExchange_T((void**)location, value, comparand);
}

template<typename T>
inline T InterlockedExchangeImpl(T* location, T value)
{
    return (T)InterlockedExchangeImplRef((void**)location, value);
}

void il2cpp_codegen_memory_barrier();

inline void ArrayGetGenericValueImpl(RuntimeArray* thisPtr, int32_t pos, void* value)
{
    memcpy(value, ((uint8_t*)thisPtr) + sizeof(RuntimeArray) + pos * thisPtr->klass->element_size, thisPtr->klass->element_size);
}

inline void ArraySetGenericValueImpl(RuntimeArray * thisPtr, int32_t pos, void* value)
{
    memcpy(((uint8_t*)thisPtr) + sizeof(RuntimeArray) + pos * thisPtr->klass->element_size, value, thisPtr->klass->element_size);
    Il2CppCodeGenWriteBarrier((void**)(((uint8_t*)thisPtr) + sizeof(RuntimeArray) + pos * thisPtr->klass->element_size), value);
}

RuntimeArray* SZArrayNew(RuntimeClass* arrayType, uint32_t length);

RuntimeArray* GenArrayNew(RuntimeClass* arrayType, il2cpp_array_size_t* dimensions);
uint8_t il2cpp_codegen_class_is_assignable_from(RuntimeClass *klass, RuntimeClass *oklass);

RuntimeObject* il2cpp_codegen_object_new(RuntimeClass *klass);

Il2CppMethodPointer il2cpp_codegen_resolve_icall(const char* name);

Il2CppMethodPointer il2cpp_codegen_resolve(const PInvokeArguments& pinvokeArgs);

template<typename FunctionPointerType, size_t dynamicLibraryLength, size_t entryPointLength>
inline FunctionPointerType il2cpp_codegen_resolve_pinvoke(const Il2CppNativeChar(&nativeDynamicLibrary)[dynamicLibraryLength], const char(&entryPoint)[entryPointLength],
    Il2CppCallConvention callingConvention, Il2CppCharSet charSet, int parameterSize, uint8_t isNoMangle)
{
    const PInvokeArguments pinvokeArgs =
    {
        il2cpp::utils::StringView<Il2CppNativeChar>(nativeDynamicLibrary),
        il2cpp::utils::StringView<char>(entryPoint),
        callingConvention,
        charSet,
        parameterSize,
        isNoMangle
    };

    return reinterpret_cast<FunctionPointerType>(il2cpp_codegen_resolve(pinvokeArgs));
}

void* il2cpp_codegen_marshal_allocate(size_t size);

template<typename T>
inline T* il2cpp_codegen_marshal_allocate_array(size_t length)
{
    return static_cast<T*>(il2cpp_codegen_marshal_allocate((il2cpp_array_size_t)(sizeof(T) * length)));
}

template<typename T>
inline T* il2cpp_codegen_marshal_allocate()
{
    return static_cast<T*>(il2cpp_codegen_marshal_allocate(sizeof(T)));
}

char* il2cpp_codegen_marshal_string(String_t* string);

void il2cpp_codegen_marshal_string_fixed(String_t* string, char* buffer, int numberOfCharacters);

Il2CppChar* il2cpp_codegen_marshal_wstring(String_t* string);

void il2cpp_codegen_marshal_wstring_fixed(String_t* string, Il2CppChar* buffer, int numberOfCharacters);

Il2CppChar* il2cpp_codegen_marshal_bstring(String_t* string);

String_t* il2cpp_codegen_marshal_string_result(const char* value);

String_t* il2cpp_codegen_marshal_wstring_result(const Il2CppChar* value);

String_t* il2cpp_codegen_marshal_bstring_result(const Il2CppChar* value);

void il2cpp_codegen_marshal_free_bstring(Il2CppChar* value);

char* il2cpp_codegen_marshal_empty_string_builder(StringBuilder_t* stringBuilder);

char* il2cpp_codegen_marshal_string_builder(StringBuilder_t* stringBuilder);

Il2CppChar* il2cpp_codegen_marshal_empty_wstring_builder(StringBuilder_t* stringBuilder);

Il2CppChar* il2cpp_codegen_marshal_wstring_builder(StringBuilder_t* stringBuilder);

void il2cpp_codegen_marshal_string_builder_result(StringBuilder_t* stringBuilder, char* buffer);

void il2cpp_codegen_marshal_wstring_builder_result(StringBuilder_t* stringBuilder, Il2CppChar* buffer);

Il2CppHString il2cpp_codegen_create_hstring(String_t* str);

String_t* il2cpp_codegen_marshal_hstring_result(Il2CppHString hstring);

void il2cpp_codegen_marshal_free_hstring(Il2CppHString hstring);

void il2cpp_codegen_marshal_type_to_native(Type_t* type, Il2CppWindowsRuntimeTypeName& nativeType);

const Il2CppType* il2cpp_codegen_marshal_type_from_native_internal(Il2CppWindowsRuntimeTypeName& nativeType);

inline Type_t* il2cpp_codegen_marshal_type_from_native(Il2CppWindowsRuntimeTypeName& nativeType)
{
    const Il2CppType* type = il2cpp_codegen_marshal_type_from_native_internal(nativeType);
    if (type == NULL)
        return NULL;

    return il2cpp_codegen_type_get_object(type);
}

void il2cpp_codegen_delete_native_type(Il2CppWindowsRuntimeTypeName& nativeType);

void il2cpp_codegen_marshal_free(void* ptr);

Il2CppMethodPointer il2cpp_codegen_marshal_delegate(MulticastDelegate_t* d);

Il2CppDelegate* il2cpp_codegen_marshal_function_ptr_to_delegate_internal(void* functionPtr, Il2CppClass* delegateType);

template<typename T>
inline T* il2cpp_codegen_marshal_function_ptr_to_delegate(Il2CppMethodPointer functionPtr, RuntimeClass* delegateType)
{
    return (T*)il2cpp_codegen_marshal_function_ptr_to_delegate_internal(reinterpret_cast<void*>(functionPtr), delegateType);
}

void il2cpp_codegen_marshal_store_last_error();

template<typename R, typename S>
inline R il2cpp_codegen_cast_struct(S* s)
{
    static_assert(sizeof(S) == sizeof(R), "Types with different sizes passed to il2cpp_codegen_cast_struct");
    R r;
    il2cpp_codegen_memcpy(&r, s, sizeof(R));
    return r;
}
inline void DivideByZeroCheck(int64_t denominator)
{
    if (denominator != 0)
        return;

    il2cpp_codegen_raise_divide_by_zero_exception();
}

inline void il2cpp_codegen_initobj(void* value, size_t size)
{
    memset(value, 0, size);
}

uint8_t MethodIsStatic(const RuntimeMethod* method);

uint8_t MethodHasParameters(const RuntimeMethod* method);

void il2cpp_codegen_runtime_class_init(RuntimeClass* klass);
inline void ArrayElementTypeCheck(RuntimeArray* array, void* value)
{

    if (value != NULL && IsInst((RuntimeObject*)value, array->klass->element_class) == NULL)
        il2cpp_codegen_raise_exception(il2cpp_codegen_get_array_type_mismatch_exception());

}

inline const RuntimeMethod* GetVirtualMethodInfo(RuntimeObject* pThis, Il2CppMethodSlot slot)
{
    if (!pThis)
        il2cpp_codegen_raise_null_reference_exception();

    return pThis->klass->vtable[slot].method;
}

inline const RuntimeMethod* GetInterfaceMethodInfo(RuntimeObject* pThis, Il2CppMethodSlot slot, RuntimeClass* declaringInterface)
{
    if (!pThis)
        il2cpp_codegen_raise_null_reference_exception();

    return il2cpp::vm::ClassInlines::GetInterfaceInvokeDataFromVTable(pThis, declaringInterface, slot).method;
}

void il2cpp_codegen_initialize_method(uint32_t index);

uint8_t il2cpp_codegen_class_is_value_type(RuntimeClass* type);

inline uint8_t il2cpp_codegen_type_implements_virtual_method(RuntimeClass* type, const RuntimeMethod* method)
{
    (void(0));
    return method->klass == type;
}

MethodBase_t* il2cpp_codegen_get_method_object_internal(const RuntimeMethod* method, RuntimeClass* refclass);

const RuntimeMethod* il2cpp_codegen_get_generic_method_definition(const RuntimeMethod* method);

inline MethodBase_t* il2cpp_codegen_get_method_object(const RuntimeMethod* method)
{
    if (method->is_inflated)
        method = il2cpp_codegen_get_generic_method_definition(method);
    return il2cpp_codegen_get_method_object_internal(method, method->klass);
}

RuntimeString* il2cpp_codegen_type_append_assembly_name_if_necessary(RuntimeString* typeName, const char* assemblyName);

inline Type_t* il2cpp_codegen_get_type(Il2CppMethodPointer getTypeFunction, String_t* typeName, const char* assemblyName)
{
    typedef Type_t* (*getTypeFuncType)(String_t*, const RuntimeMethod*);
    RuntimeString* assemblyQualifiedTypeName = il2cpp_codegen_type_append_assembly_name_if_necessary((RuntimeString*)typeName, assemblyName);


    Type_t* type = ((getTypeFuncType)getTypeFunction)((String_t*)assemblyQualifiedTypeName, NULL);
    if (type == NULL)
        return ((getTypeFuncType)getTypeFunction)(typeName, NULL);
    return type;
}

inline Type_t* il2cpp_codegen_get_type(Il2CppMethodPointer getTypeFunction, String_t* typeName, uint8_t throwOnError, const char* assemblyName)
{
    typedef Type_t* (*getTypeFuncType)(String_t*, uint8_t, const RuntimeMethod*);
    RuntimeString* assemblyQualifiedTypeName = il2cpp_codegen_type_append_assembly_name_if_necessary((RuntimeString*)typeName, assemblyName);


    Type_t* type = ((getTypeFuncType)getTypeFunction)((String_t*)assemblyQualifiedTypeName, throwOnError, NULL);
    if (type == NULL)
        return ((getTypeFuncType)getTypeFunction)(typeName, throwOnError, NULL);
    return type;
}

inline Type_t* il2cpp_codegen_get_type(Il2CppMethodPointer getTypeFunction, String_t* typeName, uint8_t throwOnError, uint8_t ignoreCase, const char* assemblyName)
{
    typedef Type_t* (*getTypeFuncType)(String_t*, uint8_t, uint8_t, const RuntimeMethod*);
    RuntimeString* assemblyQualifiedTypeName = il2cpp_codegen_type_append_assembly_name_if_necessary((RuntimeString*)typeName, assemblyName);


    Type_t* type = ((getTypeFuncType)getTypeFunction)((String_t*)assemblyQualifiedTypeName, throwOnError, ignoreCase, NULL);
    if (type == NULL)
        return ((getTypeFuncType)getTypeFunction)(typeName, throwOnError, ignoreCase, NULL);
    return type;
}

Assembly_t* il2cpp_codegen_get_executing_assembly(const RuntimeMethod* method);



inline void* il2cpp_codegen_atomic_compare_exchange_pointer(void* volatile* dest, void* exchange, void* comparand)
{
    return il2cpp::vm::Atomic::CompareExchangePointer(dest, exchange, comparand);
}

template<typename T>
inline T* il2cpp_codegen_atomic_compare_exchange_pointer(T* volatile* dest, T* newValue, T* oldValue)
{
    return il2cpp::vm::Atomic::CompareExchangePointer(dest, newValue, oldValue);
}



void il2cpp_codegen_com_marshal_variant(RuntimeObject* obj, Il2CppVariant* variant);

RuntimeObject* il2cpp_codegen_com_marshal_variant_result(const Il2CppVariant* variant);

void il2cpp_codegen_com_destroy_variant(Il2CppVariant* variant);

Il2CppSafeArray* il2cpp_codegen_com_marshal_safe_array(Il2CppChar type, RuntimeArray* managedArray);

RuntimeArray* il2cpp_codegen_com_marshal_safe_array_result(Il2CppChar variantType, RuntimeClass* type, Il2CppSafeArray* safeArray);

Il2CppSafeArray* il2cpp_codegen_com_marshal_safe_array_bstring(RuntimeArray* managedArray);

RuntimeArray* il2cpp_codegen_com_marshal_safe_array_bstring_result(RuntimeClass* type, Il2CppSafeArray* safeArray);

void il2cpp_codegen_com_destroy_safe_array(Il2CppSafeArray* safeArray);

void il2cpp_codegen_com_create_instance(const Il2CppGuid& clsid, Il2CppIUnknown** identity);

void il2cpp_codegen_com_register_rcw(Il2CppComObject* rcw);

RuntimeObject* il2cpp_codegen_com_get_or_create_rcw_from_iunknown_internal(Il2CppIUnknown* unknown, RuntimeClass* fallbackClass);

template<typename T>
inline T* il2cpp_codegen_com_get_or_create_rcw_from_iunknown(Il2CppIUnknown* unknown, RuntimeClass* fallbackClass)
{
    return static_cast<T*>(il2cpp_codegen_com_get_or_create_rcw_from_iunknown_internal(unknown, fallbackClass));
}

RuntimeObject* il2cpp_codegen_com_get_or_create_rcw_from_iinspectable_internal(Il2CppIInspectable* unknown, RuntimeClass* fallbackClass);

template<typename T>
inline T* il2cpp_codegen_com_get_or_create_rcw_from_iinspectable(Il2CppIInspectable* unknown, RuntimeClass* fallbackClass)
{
    return static_cast<T*>(il2cpp_codegen_com_get_or_create_rcw_from_iinspectable_internal(unknown, fallbackClass));
}

RuntimeObject* il2cpp_codegen_com_get_or_create_rcw_for_sealed_class_internal(Il2CppIUnknown* unknown, RuntimeClass* objectClass);

template<typename T>
inline T* il2cpp_codegen_com_get_or_create_rcw_for_sealed_class(Il2CppIUnknown* unknown, RuntimeClass* objectClass)
{
    return static_cast<T*>(il2cpp_codegen_com_get_or_create_rcw_for_sealed_class_internal(unknown, objectClass));
}

Il2CppIUnknown* il2cpp_codegen_com_query_interface_internal(Il2CppComObject* rcw, const Il2CppGuid& guid);
Il2CppIUnknown* il2cpp_codegen_com_query_interface_no_throw_internal(Il2CppComObject* rcw, const Il2CppGuid& guid);
void il2cpp_codegen_com_cache_queried_interface(Il2CppComObject* rcw, const Il2CppGuid& guid, Il2CppIUnknown* queriedInterface);

template<typename T>
inline T* il2cpp_codegen_com_query_interface(Il2CppComObject* rcw)
{
    return static_cast<T*>(il2cpp_codegen_com_query_interface_internal(rcw, T::IID));
}

template<typename T>
inline T* il2cpp_codegen_com_query_interface_no_throw(Il2CppComObject* rcw)
{
    return static_cast<T*>(il2cpp_codegen_com_query_interface_no_throw_internal(rcw, T::IID));
}

void il2cpp_codegen_il2cpp_com_object_cleanup(Il2CppComObject* rcw);

Il2CppIUnknown* il2cpp_codegen_com_get_or_create_ccw_internal(RuntimeObject* obj, const Il2CppGuid& iid);

template<typename InterfaceType>
inline InterfaceType* il2cpp_codegen_com_get_or_create_ccw(RuntimeObject* obj)
{
    return static_cast<InterfaceType*>(il2cpp_codegen_com_get_or_create_ccw_internal(obj, InterfaceType::IID));
}

inline intptr_t il2cpp_codegen_com_get_iunknown_for_object(RuntimeObject* obj)
{
    return reinterpret_cast<intptr_t>(il2cpp_codegen_com_get_or_create_ccw_internal(obj, Il2CppIUnknown::IID));
}

Il2CppObject* il2cpp_codegen_com_unpack_ccw(Il2CppIUnknown* obj);

inline void il2cpp_codegen_com_raise_exception_if_failed(il2cpp_hresult_t hr, uint8_t defaultToCOMException)
{

    if ((((il2cpp_hresult_t)(hr)) < 0))
        il2cpp_codegen_raise_exception(hr, defaultToCOMException);
}

inline RuntimeException* il2cpp_codegen_com_get_exception(il2cpp_hresult_t hr, uint8_t defaultToCOMException)
{
    return (RuntimeException*)il2cpp_codegen_get_exception(hr, defaultToCOMException);
}

il2cpp_hresult_t il2cpp_codegen_com_handle_invalid_iproperty_conversion(const char* fromType, const char* toType);

il2cpp_hresult_t il2cpp_codegen_com_handle_invalid_iproperty_conversion(RuntimeObject* value, const char* fromType, const char* toType);

il2cpp_hresult_t il2cpp_codegen_com_handle_invalid_ipropertyarray_conversion(const char* fromArrayType, const char* fromElementType, const char* toElementType, il2cpp_array_size_t index);

il2cpp_hresult_t il2cpp_codegen_com_handle_invalid_ipropertyarray_conversion(RuntimeObject* value, const char* fromArrayType, const char* fromElementType, const char* toElementType, il2cpp_array_size_t index);

void il2cpp_codegen_store_exception_info(RuntimeException* ex, String_t* exceptionString);

Il2CppIActivationFactory* il2cpp_codegen_windows_runtime_get_activation_factory(const il2cpp::utils::StringView<Il2CppNativeChar>& runtimeClassName);



Il2CppAsyncResult* il2cpp_codegen_delegate_begin_invoke(RuntimeDelegate* delegate, void** params, RuntimeDelegate* asyncCallback, RuntimeObject* state);

RuntimeObject* il2cpp_codegen_delegate_end_invoke(Il2CppAsyncResult* asyncResult, void **out_args);


inline uint8_t il2cpp_codegen_delegate_has_invoker(Il2CppDelegate* delegate)
{
    return delegate->invoke_impl != NULL;
}



inline const Il2CppGenericInst* il2cpp_codegen_get_generic_class_inst(RuntimeClass* genericClass)
{
    (void(0));
    return genericClass->generic_class->context.class_inst;
}

RuntimeClass* il2cpp_codegen_inflate_generic_class(RuntimeClass* genericClassDefinition, const Il2CppGenericInst* genericInst);

inline void* il2cpp_codegen_static_fields_for(RuntimeClass* klass)
{
    return klass->static_fields;
}

inline Il2CppMethodPointer il2cpp_codegen_get_method_pointer(const RuntimeMethod* method)
{
    return method->methodPointer;
}

inline const RuntimeType* il2cpp_codegen_method_return_type(const RuntimeMethod* method)
{
    return method->return_type;
}

inline int il2cpp_codegen_method_parameter_count(const RuntimeMethod* method)
{
    return method->parameters_count;
}

inline uint8_t il2cpp_codegen_is_import_or_windows_runtime(const RuntimeObject *object)
{
    return object->klass->is_import_or_windows_runtime;
}

inline intptr_t il2cpp_codegen_get_com_interface_for_object(Il2CppObject* object, Type_t* type)
{
    return il2cpp::icalls::mscorlib::System::Runtime::InteropServices::Marshal::GetCCW(object, reinterpret_cast<Il2CppReflectionType*>(type));
}

__attribute__ ((noreturn)) void il2cpp_codegen_raise_profile_exception(const RuntimeMethod* method);

const char* il2cpp_codegen_get_field_data(RuntimeField* field);
