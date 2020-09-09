#pragma once

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

u16 reverseByteOrder16(u16 v){
    return (v >> 8) | (v << 8);
}

u64 reverseByteOrder64(u64 v){
    return ((v >> 56) & 0xff) | ((v << 56) & 0xff00000000000000) | 
           ((v >> 40) & 0xff00) | ((v << 40) & 0xff000000000000) |
           ((v >> 24) & 0xff0000) | ((v << 24) & 0xff000000) |
           ((v >> 8) & 0xff000000) | ((v << 8) & 0xff0000);
}

