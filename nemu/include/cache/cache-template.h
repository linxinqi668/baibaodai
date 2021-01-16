


/* line struct */
typedef struct {
    bool is_dirty; // used in task2.
    bool is_valid;
    uint32_t m_tag : TAG_BIT; // used as tag.
    char m_block[ BLOCK_SIZE ]; // used as block.
} line;

// cache struct
typedef struct cache_{
    /* data members */
    line m_set[ SET_NUM ][ LINE_PER_SET];

    /* functions */

    /* read cache */
    uint32_t (*m_cache_read) (struct cache_* cache, uint32_t addr, size_t len);

    /* write cache */
    void (*m_cache_write) (struct cache_* cache, uint32_t addr, uint32_t data, size_t len);

} Cache;

// define my cache
Cache M_CACHE;

/* 判断缓存是否命中 */
int find(Cache* cache, uint32_t addr);

/* 底层接口 处理对齐的数据 */
unalign* align_read(Cache* cache, uint32_t addr);

/* read this cache */
uint32_t cache_read(Cache* cache, uint32_t addr, size_t len);

/* write this cache */
void cache_write(Cache* cache, uint32_t addr, uint32_t data, size_t len);

/* used to initialize. */
void init_cache();