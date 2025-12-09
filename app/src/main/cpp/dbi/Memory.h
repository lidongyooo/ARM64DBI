//
// Created by lidongyooo on 2025/12/7.
//

#ifndef ARM64DBIDEMO_MEMORY_H
#define ARM64DBIDEMO_MEMORY_H
#include "../types/types.h"

#define BLOCK_NUMBER 4096
#define BLOCK_SIZE 1024

struct BlockMeta {
    int index;
    void* code_start;
    int code_size;
    void* block_start; //插桩后的基本块起始地址
    int block_size;
    BlockMeta* slice_block_meta; // 当基本块过大时，拆成多个 BLOCK。
    uint32_t code[BLOCK_SIZE];
};

class Memory {
private:
    static Memory* instance;
    Memory() {
        size_t blocks_meta_size = ALIGN_PAGE_UP(sizeof(BlockMeta) * BLOCK_NUMBER);
        // 申请存放插桩代码的内存。使用预分配内存策略，提高效率。
        first_block_meta = (BlockMeta*)mmap(nullptr, blocks_meta_size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    }

public:
    // 使用无序 map 来索引插桩后的基本块
    std::unordered_map<uint64_t, int> cache_block_meta;
    int curr_block_index = 0;
    BlockMeta* first_block_meta;
    BlockMeta* block_meta_arr[BLOCK_NUMBER] = {nullptr};

    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;

    static Memory* getInstance() {
        if (!instance) {
            instance = new Memory();
        }
        return instance;
    }

    // 获取或创建一个 block_meta
    static BlockMeta* get_or_new_block_meta(int index = 99999999);

    // 创建一个 block_meta
    static BlockMeta* new_block_meta();

    // 设置与获取已插桩的基本块，避免二次插桩同一基本块
    static bool set_cache_block_meta(uint64_t key, int value);
    static BlockMeta* get_cache_block_meta(uint64_t key);
};


#endif //ARM64DBIDEMO_MEMORY_H
