
// do some computation.
#define BLOCK_SIZE (1 << BLOCK_BIT)
#define SET_NUM (1 << SET_INDEX_BIT)

// line struct
typedef struct {
    bool is_valid;
    uint32_t m_tag : TAG_BIT; // used as tag.
    char m_block[ BLOCK_SIZE ]; // used as block.
} line;

// cache struct
typedef struct cache{
    /* members */
    line m_set[ SET_NUM ][ LINE_PER_SET];

    /* functions */

    /* read cache */
    uint32_t (*m_cache_read) (struct cache* cache, uint32_t addr, size_t len);

    /* write cache */
    void (*m_cache_write) (struct cache* cache, uint32_t addr, uint32_t data, size_t len);

    /* nothing else to do. */

} Cache;

// define my cache
Cache M_CACHE;

/* 判断缓存是否命中 */
int find(Cache* cache, uint32_t addr);

/* 底层接口 处理一定对齐的数据 */
unalign* align_read(Cache* cache, uint32_t addr);

/* 辅助函数 */
uint32_t unalign_rw_helper(unalign* addr, size_t len);

/* read len bytes from cache */
uint32_t cache_read(Cache* cache, uint32_t addr, size_t len);

/* write cache */
void cache_write(Cache* cache, uint32_t addr, uint32_t data, size_t len);

/* used to initialize. */
void init_cache();

#undef BLOCK_SIZE
#undef SET_NUM