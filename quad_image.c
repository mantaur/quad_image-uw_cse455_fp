#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "quad_image.h"

int leaf(quad q)
{
    return q.qs == 0;
}

square make_square(float w, float h, float d)
{
    square s;
    s.w = w; s.h = h; s.d = d;
    return s;
}

// Check if q is leaf node and return it's value, or...
// recursively search for leaf node containing point p in channel c.
// Current implementation is O(4log(n)) which could be made faster with a switched divide
// and conquer on a per coordinate basis (kinda like: p.x >= q.x? p.y >= q.y : ...)
float q_get_pixel(quad* q, int c, float h, float w)
{
    assert(in_q(w, h, *q)); // Can't find a point recursively if it ain't inside this quad's bounds
    if(!leaf(*q)){
        if      (in_q(w, h, q->qs[0])) return q_get_pixel(&q->qs[0], c, h, w);
        else if (in_q(w, h, q->qs[1])) return q_get_pixel(&q->qs[1], c, h, w);
        else if (in_q(w, h, q->qs[2])) return q_get_pixel(&q->qs[2], c, h, w);
        else                           return q_get_pixel(&q->qs[3], c, h, w);
    } else {
        return q->data[c];
    }
}

// Recursively dive into quad, stopping when quad's square "radius" is smaller than
// size wide(/high) and set that quad's data at channel c to value.
// Also return leaf's value or nodes average of children's values.
void q_set_pixel(quad* q, int c, float h, float w, float value, float size)
{
    assert(in_q(w, h, *q));
    if(q->s.d <= size){
        q->data[c] = value;
    } else {
        if      (in_q(w, h, q->qs[0])) q_set_pixel(&q->qs[0], c, h, w, value, size);
        else if (in_q(w, h, q->qs[1])) q_set_pixel(&q->qs[1], c, h, w, value, size);
        else if (in_q(w, h, q->qs[2])) q_set_pixel(&q->qs[2], c, h, w, value, size);
        else if (in_q(w, h, q->qs[3])) q_set_pixel(&q->qs[3], c, h, w, value, size);
        q->data[c] = (q->qs[0].data[c] + q->qs[1].data[c] + q->qs[2].data[c] + q->qs[3].data[c]) / 4;
    }
}

// Check if point p is inside of quad q's bounds
int in_q(float w, float h, quad q)
{
    return (fabsf(w - q.s.w) <= q.s.d/2.0F) && (fabsf(h - q.s.h) <= q.s.d/2.0F);
}

int square_overlap(square s1, square s2)
{
    float combined_radius = (s1.d/2.0F) + (s2.d/2.0F);
    return (fabsf(s1.w - s2.w) <= combined_radius) && (fabsf(s1.h - s2.h) <= combined_radius);
}

// If top left and bottom right corners of in are inside out, then in is fully
// contained inside out
int square_contained(square in, square out)
{
    int top_l = (fabsf((in.w-(in.d/2.0F)) - out.w) <= (out.d/2.0F)) && (fabsf((in.h-(in.d/2.0F)) - out.h) <= (out.d/2.0F));
    int bot_r = (fabsf((in.w+(in.d/2.0F)) - out.w) <= (out.d/2.0F)) && (fabsf((in.h+(in.d/2.0F)) - out.h) <= (out.d/2.0F));
    return top_l && bot_r;
}

// Allocate quad with c channels for data but no children
quad make_empty_quad(square s, int c)
{
    quad q;
    q.qs = 0;
    q.s = s;
    q.data = calloc(c, sizeof(float));
    return q;
}

q_img make_quad_image(int c, int h, int w)
{
    q_img out;
    out.c = c;
    out.h = h;
    out.w = w;
    out.q = malloc(sizeof(quad));
    float d = ((float)h) / 2.0F;
    *(out.q) = make_empty_quad(make_square(d, d, (float)h), c);
    return out;
}

q_img allocate_quad_image(int c, int h, int w)
{
    assert(h > 0);
    assert(h == w); // Only working with square images so far
    assert(!(w & (w - 1))); // Only work with images sizes equal to powers of two, bithack from: http://www.graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
    q_img qim = make_quad_image(c, h, w);
    int n = allocate_quad_image_helper(qim.q, c); //, h);
    printf("total allocate_quad_image_helper calls: %d\n", n);
    return qim;
}

// As long as radius of q isn't 0.5 (1 wide), recursively allocate children and setup bounds
int allocate_quad_image_helper(quad* q, int c)
{
    if (q->s.d <= Q_D) return 1;
    int calls = 0;

    q->qs = malloc(4 * sizeof(quad));

    float d = q->s.d / (2.0F * Q_D);
    float mov = d / 2.0F;
    q->qs[0] = make_empty_quad(make_square(q->s.w - mov, q->s.h - mov, d), c);
    q->qs[1] = make_empty_quad(make_square(q->s.w + mov, q->s.h - mov, d), c);
    q->qs[2] = make_empty_quad(make_square(q->s.w - mov, q->s.h + mov, d), c);
    q->qs[3] = make_empty_quad(make_square(q->s.w + mov, q->s.h + mov, d), c);

    calls += allocate_quad_image_helper(&q->qs[0], c);
    calls += allocate_quad_image_helper(&q->qs[1], c);
    calls += allocate_quad_image_helper(&q->qs[2], c);
    calls += allocate_quad_image_helper(&q->qs[3], c);

    return calls;
}

q_img im_to_qim(image im)
{
    printf("Converting image to quad image\n");
    q_img qim = allocate_quad_image(im.c, im.h, im.w);
    printf("Allocated quad tree for quad image\n");
    int c,h,w;
    for(c = 0; c < im.c; c++){
        for(h = 0; h < im.h; h++){
            for(w = 0; w < im.w; w++){
                q_set_pixel(qim.q, c, h, w, get_pixel(im, c, h, w), 1);
            }
        }
        printf("channel %d done copying\n", c);
    }
    // Some prints to see if everything looks good
    printf("Diagnostic print, everything should look pretty good here...\n");
    print_quad(*qim.q);
    if(im.w >= 2)   print_quad(qim.q->qs[0]);
    if(im.w >= 4)   print_quad(qim.q->qs[0].qs[0]);
    if(im.w >= 8)   print_quad(qim.q->qs[0].qs[0].qs[0]);
    if(im.w >= 16)  print_quad(qim.q->qs[0].qs[0].qs[0].qs[0]);
    if(im.w >= 32)  print_quad(qim.q->qs[0].qs[0].qs[0].qs[0].qs[0]);
    if(im.w >= 64)  print_quad(qim.q->qs[0].qs[0].qs[0].qs[0].qs[0].qs[0]);
    if(im.w >= 128) print_quad(qim.q->qs[0].qs[0].qs[0].qs[0].qs[0].qs[0].qs[0]);
    if(im.w >= 256) print_quad(qim.q->qs[0].qs[0].qs[0].qs[0].qs[0].qs[0].qs[0].qs[0]);
    if(im.w >= 512) print_quad(qim.q->qs[0].qs[0].qs[0].qs[0].qs[0].qs[0].qs[0].qs[0].qs[0]);
    return qim;
}

// Generate and return an image type "object" given a quad image.
// Can be optimized with recursive function that fills in im using pointer to it and diving into
// the quad tree.
image qim_to_im(q_img qim)
{
    image im = make_image(qim.c, qim.h, qim.w);
    int c,h,w;
    for(c = 0; c < im.c; c++){
        for(h = 0; h < im.h; h++){
            for(w = 0; w < im.w; w++){
                set_pixel(im, c, h, w, q_get_pixel(qim.q, c, h, w));
            }
        }
    }
    return im;
}

// Given an image, detect "interesting" regions (faces, vehicles, animals etc) and store their
// bounding boxes in squares, finally return the number of found interesting regions.
int im_interesting_regions(image im, square* squares)
{
    squares[0] = make_square(256, 256, 64);
    return 1;
}

// Given an image, corners of interesting regions, a size budget (sizeof(int) bits = 1 unit size),
// and a compression ratio between boring and interesting regions of an image:
// Create and build quad tree of image, fusing boring regions at *ratio* rate more often than
// interesting regions until budget has been achieved or iters iterations or lastly image can't
// be further compressed.
q_img compress_im(image im, long long budget, int iters, float ratio)
{
    q_img qim = im_to_qim(im);
    printf("image size: %lld\n", sizeof(float) * im.c * im.h * im.w);
    printf("quad image size before compression: %lld\n", q_img_size(qim));

    int nr_squares = 100;   // This array may for all intents and purposes be oversized
    square* squares = malloc(nr_squares * sizeof(square));
    nr_squares = im_interesting_regions(im, squares);
    int qs_len = qim.w * 4;
    quad** qs = malloc(qs_len * sizeof(quad*));
    int temp = 1337;    // LEET
    int* nr_quads = &temp;
    *nr_quads = 0;
    int n = 0; int i = 0; int compressible = 1;

    while((q_img_size(qim) > budget) && (i < iters) && (compressible != 0)){
        compressible = 0;
        n = qs_outside_squares(qim.q, squares, nr_squares, qs, qs_len, nr_quads);
        printf("found %d compressible quads\n", n);

        for(n = 0; n < *nr_quads; n++) compressible += compress_quad(qs[n]);
        printf("new compressed has %lld leafs\n", quad_leaf_count(*(qim.q)));
        *nr_quads = 0;
        i++;
    }
    printf("quad image size after compression: %lld\n", q_img_size(qim));
    if(q_img_size(qim) > budget) printf("budget achieved ");
    if(i < iters) printf("iters run ");
    if(compressible == 0) printf("image not compressible further");

    return qim;
}

q_img compress_im_manual(image im, square* squares, int nr_squares, long long budget, int iters, float ratio)
{
    q_img qim = im_to_qim(im);
    printf("image size: %lld\n", sizeof(float) * im.c * im.h * im.w);
    printf("quad image size before compression: %lld\n", q_img_size(qim));

    int qs_len = qim.w * 4;
    quad** qs = malloc(qs_len * sizeof(quad*));
    int temp = 1337;    // LEET
    int* nr_quads = &temp;
    *nr_quads = 0;
    int n = 0; int i = 0; int compressible = 1;

    while((q_img_size(qim) > budget) && (i < iters) && (compressible != 0)){
        compressible = 0;
        n = qs_outside_squares(qim.q, squares, nr_squares, qs, qs_len, nr_quads);
        printf("found %d compressible quads\n", n);

        for(n = 0; n < *nr_quads; n++) compressible += compress_quad(qs[n]);
        printf("new compressed has %lld leafs\n", quad_leaf_count(*(qim.q)));
        *nr_quads = 0;
        i++;
    }
    printf("quad image size after compression: %lld\n", q_img_size(qim));
    if(q_img_size(qim) <= budget) printf("budget achieved ");
    if(i >= iters) printf("iters run ");
    if(compressible == 0) printf("image not compressible further");
    printf("\n");

    return qim;
}

square* allocate_squares(int n, int* w, int* h, int* d)
{
    square* squares = malloc(n * sizeof(square));
    for(int i = 0; i < n; i++) squares[i] = make_square(w[i], h[i], d[i]);
    return squares;
}

// Return 0 if compression isn't possible, otherwise the number of quads that are compressible
int compress_quad(quad* q)
{
    if(leaf(*q)) return 0;
    if(leaf(*(q->qs))){     // children are leafs, so fuse and free up unnecessary quads
        for(int i = 0; i < 4; i++) free_quad(q->qs[i]);
        q->qs = 0;
        return 1;
    } else {                // children are nodes, fuse children deeper in recursively
        int compressible = 0;
        for(int i = 0; i < 4; i++) compressible += compress_quad(&q->qs[i]);
        return compressible;
    }
}

// For some root quad q, find at most qs_len quads with childlren outside squares, store found quads in
// quad* qs and return number of found quads. *nr_quads holds the number of found quads currently
int qs_outside_squares(quad* q, square* squares, int nr_squares, quad** qs, int qs_len, int* nr_quads)
{
    if(qs_len == *nr_quads) {
        printf("qs_len == *nr_quads, ie qs filled up!!\n");
        return 0;
    }
    if(leaf(*q)) return 0;
    int no_overlap = 1;
    for(int i = 0; i < nr_squares; i++){
        if(square_overlap(q->s, squares[i])) {
            no_overlap = 0;
            break;
        }
    }
    if(no_overlap) {
        qs[*nr_quads] = q;
        (*nr_quads)++;
        return 1;
    } else {    // Overlap between q and interesting squares
        int quads_in_children = 0;
        for(int i = 0; i < 4; i++) {    // For every child qs[i]...
            int contained = 0;
            for(int j = 0; j < nr_squares; j++){    // If qs[i] contained then so are its children, so don't bother searching it
                if(square_contained(q->qs[i].s, squares[i])){
                    contained = 1;
                    break;
                }
            }
            if (!contained) {                       // qs[i] not contained, so some of its children are worth searching
                quads_in_children += qs_outside_squares(&(q->qs[i]), squares, nr_squares, qs, qs_len, nr_quads);
            }
        }
        return quads_in_children;
    }
}

// Recursively free quad's children's (data and object), then it's own data.
void free_quad(quad q)
{
    if (q.qs != 0) for(int i = 0; i < 4; i++) free_quad(q.qs[i]);
    free(q.qs);
    free(q.data);
}

void print_quad(quad q)
{
    if (!leaf(q)) {
        printf("node (%5.1f,%5.1f)-%5.1f,%2d=%4.2f, childs ((%5.1f,%5.1f),(%5.1f,%5.1f),(%5.1f,%5.1f),(%5.1f,%5.1f))-%5.1f\n",
                            q.s.w, q.s.h, (q.s.d), quad_depth(q), q.data[0],
                            q.qs[0].s.w, q.qs[0].s.h,
                            q.qs[1].s.w, q.qs[1].s.h,
                            q.qs[2].s.w, q.qs[2].s.h,
                            q.qs[3].s.w, q.qs[3].s.h,
                            (q.qs[0].s.d));
        // for(int i = 0; i < 4; i++) print_quad(q.qs[i]);
    }
    else printf("leaf (%5.1f,%5.1f)-%5.1f, 0=%4.2f\n", q.s.w, (q.s.h), (q.s.d), q.data[0]);
}
void print_quad_tree(quad q)
{
    print_quad(q);
    if (!leaf(q)) {
        for(int i = 0; i < 4; i++) print_quad_tree(q.qs[i]);
    }
}

// Assume all leafs in quad tree are of equal size, and that each quad node has exactly 4
// 4 children. Then a quad's size can be calculated as the total number of leaf nodes
// multiplied by the size of each node
long long quad_leaf_count_est(quad q)
{
    if (leaf(q)) return 1;
    return 4*quad_leaf_count_est(q.qs[0]);
}
// True leaf count in quad.
long long quad_leaf_count(quad q)
{
    if (leaf(q)) return 1;
    long long children_leaf_count = 0;
    for(int i = 0; i < 4; i++) children_leaf_count += quad_leaf_count(q.qs[i]);
    return children_leaf_count;
}

long long q_img_size(q_img qim)
{
    return quad_leaf_count(*(qim.q)) * (sizeof(float) * qim.c + sizeof(quad));
}

int quad_depth(quad q)
{
    if(leaf(q)) return 0;
    return 1 + quad_depth(q.qs[0]);
}
