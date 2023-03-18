#include <stdlib.h>
#include "load_quad_image.h"

void save_quad_binary(quad q, const char* fname, int c)
{
    FILE* fp = fopen(fname, "wb");
    save_quad_binary_helper(q, fp, c);
    fclose(fp);
}
void save_quad_binary_helper(quad q, FILE* fp, int c)
{
    int is_leaf = leaf(q);
    fwrite(&is_leaf, sizeof(int), 1, fp);
    fwrite(&q.s, sizeof(square), 1, fp);
    fwrite(q.data, sizeof(float), c, fp);
    if(!is_leaf) for(int i = 0; i < 4; i++) save_quad_binary_helper(q.qs[i], fp, c);
}

quad load_quad_binary(const char* fname, int c)
{
    FILE* fp = fopen(fname, "rb");
    quad q = make_empty_quad(make_square(0, 0, 0), 0);
    load_quad_binary_helper(&q, fp, c);
    return q;
}
void load_quad_binary_helper(quad* q, FILE* fp, int c)
{
    *q = make_empty_quad(make_square(0, 0, 0), c);
    int leaf;
    fread(&leaf, sizeof(int), 1, fp);
    fread(&(*q).s, sizeof(square), 1, fp);
    fread((*q).data, sizeof(float), c, fp);
    if(!leaf) {
        q->qs = malloc(4 * sizeof(quad));
        for(int i = 0; i < 4; i++){
            load_quad_binary_helper(&(q->qs[i]), fp, c);
        }
    }
    if(feof(fp)) {
        printf("EOF reached, returning\n");
        return;
    }
}

void save_qim_binary(q_img qim, const char* fname)
{
    FILE* fp = fopen(fname, "wb");
    fwrite(&qim.c, sizeof(int), 1, fp);
    fwrite(&qim.h, sizeof(int), 1, fp);
    fwrite(&qim.w, sizeof(int), 1, fp);
    fwrite(qim.q, sizeof(quad), 1, fp);
    save_quad_binary_helper(*(qim.q), fp, qim.c);
    fclose(fp);
}

q_img load_qim_binary(const char* fname)
{
    int c = 0;
    int h = 0;
    int w = 0;
    FILE* fp = fopen(fname, "rb");
    fread(&c, sizeof(int), 1, fp);
    fread(&h, sizeof(int), 1, fp);
    fread(&w, sizeof(int), 1, fp);
    q_img qim = make_quad_image(c, h, w);
    fread(qim.q, sizeof(quad), 1, fp);
    load_quad_binary_helper(qim.q, fp, c);
    return qim;
}

void copy_quad(quad source, quad* dest, int c)
{
    *dest = make_empty_quad(source.s, c);
    for(int i = 0; i < c; i++) dest->data[i] = source.data[i];
    if(source.qs != 0){
        dest->qs = malloc(4 * sizeof(quad));
        for(int i = 0; i < 4; i++) copy_quad(source.qs[i], &(dest->qs[i]), c);
    }
}