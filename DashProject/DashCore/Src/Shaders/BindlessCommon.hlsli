#if PLATFORM_SUPPORTS_BINDLESS
	// SM6.6 path
    #if (RAYCALLABLESHADER || RAYHITGROUPSHADER || RAYGENSHADER || RAYMISSSHADER)
        // Bindless index can be divergent in hit, miss and callable shaders
        #define GetResourceFromHeap(Type, Index) ResourceDescriptorHeap[NonUniformResourceIndex(Index)]
        #define GetSamplerFromHeap(Type, Index) SamplerDescriptorHeap[NonUniformResourceIndex(Index)]
    #else	
        #define GetResourceFromHeap(Type, Index) ResourceDescriptorHeap[Index]
        #define GetSamplerFromHeap(Type, Index) SamplerDescriptorHeap[Index]
    #endif
#endif	

#define UB_CB_PREFIXED_MEMBER_ACCESS(Prefix, MemberName) Prefix##_##MemberName

#if PLATFORM_SUPPORTS_BINDLESS
    #define BINDLESS_SRV(Type, Name) \
            typedef Type SafeType##_##Name; \
            SafeType##_##Name GetBindlessResource##_##Name() { return GetResourceFromHeap(SafeType##_##Name, UB_CB_PREFIXED_MEMBER_ACCESS(BindlessSRV_, Name)); } \
            static const SafeType##_##Name Name = GetBindlessResource##_##Name();

    #define BINDLESS_UAV(Type, Name) \
            typedef Type SafeType##_##Name; \
            SafeType##_##Name GetBindlessResource##_##Name() { return GetResourceFromHeap(SafeType##_##Name, UB_CB_PREFIXED_MEMBER_ACCESS(BindlessUAV_, Name)); } \
            static const SafeType##_##Name Name = GetBindlessResource##_##Name();

    #define BINDLESS_SAMPLER(Type, Name) \
            Type GetBindlessSampler##_##Name() { return GetSamplerFromHeap(Type, UB_CB_PREFIXED_MEMBER_ACCESS(BindlessSampler_, Name)); } \
            static const Type Name = GetBindlessSampler##_##Name();
#else
    #define BINDLESS_SRV(Type, Name) Type Name
    #define BINDLESS_UAV(Type, Name) Type Name
    #define BINDLESS_SAMPLER(Type, Name) Type Name
#endif
