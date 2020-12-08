
// 计算block size.
#define BLOCK_SIZE (1 << BLOCK_BIT)

// line struct
typedef struct {
    bool is_valid;
    uint32_t tag : TAG_BIT; // used as tag.
    char block[ BLOCK_SIZE ]; // used as block.
} Line;
