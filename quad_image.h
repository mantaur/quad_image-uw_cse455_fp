#ifndef QUAD_IMAGE_H
#define QUAD_IMAGE_H
#include "..\image.h"

#ifdef __cplusplus
extern "C" {
#endif

#define Q_D 1.0F

typedef struct{
    float w,h,d;        // A square has a center (x,y), and "diameter" d
} square;


// Backbone of quadtree
typedef struct quad quad;
struct quad{
    quad* qs;       // "four" sub quad nodes, index clockwise as NW, NE, SW, SE
    square s;
    float* data;    // data (for every channel) if leaf node
};

// Top structure for image
typedef struct{
    int c,h,w;
    quad* q;
} q_img;

// Basic functionality
int leaf(quad q);
square make_square(float w, float h, float d);
float q_get_pixel(quad* q, int c, float h, float w);
void q_set_pixel(quad* q, int c, float h, float w, float value, float size);
int in_q(float w, float h, quad q);
int square_overlap(square s1, square s2);
int square_contained(square in, square out);
quad make_empty_quad(square s, int c);
q_img make_quad_image(int c, int h, int w);
q_img allocate_quad_image(int c, int h, int w);
int allocate_quad_image_helper(quad* q, int c);
q_img im_to_qim(image im);
image qim_to_im(q_img qim);
void free_quad(quad q);
void print_quad(quad q);
void print_quad_tree(quad q);
long long quad_leaf_count_est(quad q);
long long quad_leaf_count(quad q);
long long q_img_size(q_img q);
int quad_depth(quad q);
square* allocate_squares(int n, int* w, int* h, int* d);

// Image compression
int im_interesting_regions(image im, square* squares);
q_img compress_im(image im, long long budget, int iters, float ratio);
q_img compress_im_manual(image im, square* squares, int nr_squares, long long budget, int iters, float ratio);
int compress_quad(quad* q);
int qs_outside_squares(quad* q, square* squares, int nr_squares, quad** qs, int qs_len, int* nr_quads);

#ifdef __cplusplus
}
#endif
#endif
