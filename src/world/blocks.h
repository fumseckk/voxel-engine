#ifndef BLOCKS_H
#define BLOCKS_H

enum BlockType {
    AIR = 0,
    GRASS = 1,
    DIRT = 2,
    SAND = 3,
    STONE = 4,
    SNOW = 5,
    FLOWER1 = 6,
    FLOWER2 = 7,
    LOGS = 8,
    LEAVES = 9,
    WATER = 10,
};

struct Block {
    BlockType type = AIR;
};

#endif