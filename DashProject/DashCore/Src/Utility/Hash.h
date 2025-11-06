#pragma once

namespace Dash
{
    // SSE CRC32 可用性检查
    #ifdef _MSC_VER
    #include <intrin.h>
    #define ENABLE_SSE_CRC32 1
    #elif defined(__SSE4_2__)
    #include <nmmintrin.h>
    #define ENABLE_SSE_CRC32 1
    #else
    #define ENABLE_SSE_CRC32 0
    #endif

    // FNV-1a 哈希常量
    constexpr size_t FNV_OFFSET_BASIS = sizeof(size_t) == 8 ? 14695981039346656037ULL : 2166136261U;
    constexpr size_t FNV_PRIME = sizeof(size_t) == 8 ? 1099511628211ULL : 16777619U;

    // 对齐辅助函数
    template<typename T>
    inline T* AlignUp(T* ptr, size_t alignment) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        addr = (addr + alignment - 1) & ~(alignment - 1);
        return reinterpret_cast<T*>(addr);
    }

    template<typename T>
    inline T* AlignDown(T* ptr, size_t alignment) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        addr = addr & ~(alignment - 1);
        return reinterpret_cast<T*>(addr);
    }

    // 核心哈希范围函数 - 优化版本
    inline size_t HashRangeOptimized(const uint8* Begin, const uint8* End, size_t Hash) {
#if ENABLE_SSE_CRC32
        // 处理小数据的快速路径
        const size_t Length = End - Begin;
        if (Length <= 16) {
            // 对于小数据，直接处理字节
            for (const uint8* p = Begin; p < End; ++p) {
                Hash = _mm_crc32_u8(static_cast<uint32>(Hash), *p);
            }
            return Hash;
        }

        const uint8* Current = Begin;

        // 阶段1: 处理未对齐的前缀，直到8字节对齐
        while (Current < End && (reinterpret_cast<uintptr_t>(Current) & 7)) {
            Hash = _mm_crc32_u8(static_cast<uint32>(Hash), *Current++);
        }

        // 阶段2: 处理8字节块
        const uint64* Current64 = reinterpret_cast<const uint64*>(Current);
        const uint64* End64 = reinterpret_cast<const uint64*>(AlignDown(End, 8));

        // 展开循环以提高性能
        while (Current64 + 4 <= End64) {
            Hash = _mm_crc32_u64(Hash, Current64[0]);
            Hash = _mm_crc32_u64(Hash, Current64[1]);
            Hash = _mm_crc32_u64(Hash, Current64[2]);
            Hash = _mm_crc32_u64(Hash, Current64[3]);
            Current64 += 4;
        }

        // 处理剩余的8字节块
        while (Current64 < End64) {
            Hash = _mm_crc32_u64(Hash, *Current64++);
        }

        // 阶段3: 处理剩余的字节
        Current = reinterpret_cast<const uint8*>(Current64);
        while (Current < End) {
            Hash = _mm_crc32_u8(static_cast<uint32>(Hash), *Current++);
        }

#else
        // FNV-1a 哈希实现（无SSE）
        const size_t Length = End - Begin;

        // 对于大数据，使用字对齐的处理
        if (Length >= sizeof(size_t)) {
            const uint8* Current = Begin;

            // 处理未对齐的前缀
            while (Current < End && (reinterpret_cast<uintptr_t>(Current) & (sizeof(size_t) - 1))) {
                Hash ^= *Current++;
                Hash *= FNV_PRIME;
            }

            // 处理对齐的字
            const size_t* CurrentWord = reinterpret_cast<const size_t*>(Current);
            const size_t* EndWord = reinterpret_cast<const size_t*>(AlignDown(End, sizeof(size_t)));

            while (CurrentWord < EndWord) {
                Hash ^= *CurrentWord++;
                Hash *= FNV_PRIME;
            }

            // 处理剩余字节
            Current = reinterpret_cast<const uint8*>(CurrentWord);
            while (Current < End) {
                Hash ^= *Current++;
                Hash *= FNV_PRIME;
            }
        }
        else {
            // 小数据直接处理
            for (const uint8* p = Begin; p < End; ++p) {
                Hash ^= *p;
                Hash *= FNV_PRIME;
            }
        }
#endif

        return Hash;
    }

    // 类型特征检测
    template<typename T>
    struct is_trivially_hashable {
        static constexpr bool value =
            std::is_trivially_copyable_v<T> &&
            !std::is_pointer_v<T>;  // 指针需要特殊处理
    };

    template<typename T>
    inline constexpr bool is_trivially_hashable_v = is_trivially_hashable<T>::value;

    // 主哈希函数模板
    template<typename T>
    inline size_t HashState(const T* Data, size_t Count = 1, size_t Hash = FNV_OFFSET_BASIS) {
        if constexpr (is_trivially_hashable_v<T>) {
            // 对于平凡类型，直接哈希内存
            const uint8* Begin = reinterpret_cast<const uint8*>(Data);
            const uint8* End = reinterpret_cast<const uint8*>(Data + Count);
            return HashRangeOptimized(Begin, End, Hash);
        }
        else {
            // 对于非平凡类型，需要自定义哈希
            static_assert(sizeof(T) == 0, "Type is not trivially hashable. Please provide a specialization.");
            return 0;
        }
    }

    // 特化：字符串
    template<>
    inline size_t HashState<char>(const char* Data, size_t Count, size_t Hash) {
        if (Count == 1) {
            // 假设是C字符串
            Count = std::strlen(Data);
        }
        return HashRangeOptimized(
            reinterpret_cast<const uint8*>(Data),
            reinterpret_cast<const uint8*>(Data + Count),
            Hash
        );
    }

    // 便利函数：哈希单个对象
    template<typename T>
    inline size_t HashObject(const T& Object, size_t Hash = FNV_OFFSET_BASIS) {
        if constexpr (is_trivially_hashable_v<T>) {
            return HashState(&Object, 1, Hash);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return HashRangeOptimized(
                reinterpret_cast<const uint8*>(Object.data()),
                reinterpret_cast<const uint8*>(Object.data() + Object.size()),
                Hash
            );
        }
        else {
            static_assert(sizeof(T) == 0, "Type is not hashable. Please provide a specialization.");
            return 0;
        }
    }

    // 组合多个哈希值
    inline size_t HashCombine(size_t Hash1, size_t Hash2) {
        // 使用黄金比例的魔数
        if constexpr (sizeof(size_t) == 8) {
            return Hash1 ^ (Hash2 + 0x9e3779b97f4a7c15ULL + (Hash1 << 6) + (Hash1 >> 2));
        }
        else {
            return Hash1 ^ (Hash2 + 0x9e3779b9U + (Hash1 << 6) + (Hash1 >> 2));
        }
    }

    // 变参模板：哈希多个值
    template<typename T, typename... Args>
    inline size_t HashMultiple(const T& First, const Args&... Rest) {
        size_t Hash = HashObject(First);
        if constexpr (sizeof...(Rest) > 0) {
            return HashCombine(Hash, HashMultiple(Rest...));
        }
        return Hash;
    }
}