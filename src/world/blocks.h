#ifndef BLOCKS_H
#define BLOCKS_H

enum BlockType {
    /* 00 */ GRASS,
    /* 01 */ DIRT,
    /* 02 */ SAND,
    /* 03 */ STONE,
    /* 04 */ SNOW,
    /* 05 */ FLOWER1,
    /* 06 */ FLOWER2,
    /* 07 */ DESERT_BUSH,
    /* 08 */ GRASS_BUSH,
    /* 09 */ WATER_SURFACE,
    /* 10 */ WATER,
    /* 11 */ LAVA_SURFACE,
    /* 12 */ LAVA,
    /* 13 */ EMPTY,
};

class Block {
    public:
     BlockType type;
     Block() : type(EMPTY) {}
     Block(BlockType type) : type(type) {}
     void set_active(bool b) {
       if (b)
         type = GRASS;
       else
         type = EMPTY;
     }
     bool is_active() { return type != EMPTY; }
   };

#endif