#ifndef LOAD_QUAD_IMAGE_H
#define LOAD_QUAD_IMAGE_H
#include "quad_image.h"

#ifdef __cplusplus
extern "C" {
#endif


void save_quad_binary(quad q, const char* fname, int c);
void save_quad_binary_helper(quad q, FILE* fp, int c);
quad load_quad_binary(const char* fname, int c);
void load_quad_binary_helper(quad* q, FILE* fp, int c);
void save_qim_binary(q_img qim, const char* fname);
q_img load_qim_binary(const char* fname);
void copy_quad(quad source, quad* dest, int c);

#ifdef __cplusplus
}
#endif
#endif
