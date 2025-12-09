//
// Created by lidongyooo on 2025/12/7.
//

#include "Memory.h"

Memory* Memory::instance = nullptr;

BlockMeta* Memory::new_block_meta() {
    auto self = getInstance();
    auto block_meta = self->first_block_meta + self->curr_block_index;
    block_meta->index = self->curr_block_index;
    self->block_meta_arr[self->curr_block_index++] = block_meta;

    return block_meta;
}

BlockMeta* Memory::get_or_new_block_meta(int index) {
    if (index >= BLOCK_NUMBER) {
        return new_block_meta();
    }

    auto self = getInstance();
    if (self->block_meta_arr[index] == nullptr) {
        return new_block_meta();
    }

    return self->block_meta_arr[index];
}


bool Memory::set_cache_block_meta(uint64_t key, int value) {
    auto self = getInstance();
    self->cache_block_meta[key] = value;
    return true;
}

BlockMeta* Memory::get_cache_block_meta(uint64_t key) {
    auto self = getInstance();
    if (self->cache_block_meta.count(key) == 0) {
        return nullptr;
    }

    int index = self->cache_block_meta[key];
    return get_or_new_block_meta(index);
};