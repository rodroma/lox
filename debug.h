//
// Created by rodrigo on 17/1/21.
//

#ifndef LOX_DEBUG_H
#define LOX_DEBUG_H

#include "chunk.h"

void dissasemble_chunk(Chunk*, const char* name);
int dissasemble_instruction(Chunk*, int offset);

#endif //LOX_DEBUG_H
