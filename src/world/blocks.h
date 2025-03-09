#ifndef BLOCKS_H
#define BLOCKS_H

enum BlockType {
    EMPTY,
    GRASS,
    DIRT,
    SAND,
    STONE,
    SNOW,
    WATER_SURFACE,
    WATER,
    LAVA_SURFACE,
    LAVA,
    FLOWER1,
    FLOWER2,
    GRASS_BUSH,
    DESERT_BUSH
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