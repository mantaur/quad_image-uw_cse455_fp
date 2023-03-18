#include <stdlib.h>
#include "./fp/load_quad_image.h"
#include "./fp/quad_image.h"


int same_quad_helper(quad q1, quad q2, int c)
{
    if((q1.s.w != q2.s.w) || (q1.s.h != q2.s.h) || (q1.s.d != q2.s.d)) {
        printf("square\n");
        return 0;
    }
    for(int i = 0; i < c; i++) if(q1.data[i] != q2.data[i]) {
        printf("data\n");
        printf("q1.data[%d]=%f\n", i, q1.data[i]);
        printf("q2.data[%d]=%f\n", i, q2.data[i]);
        return 0;
    }
    if(  leaf(q1)  && (!leaf(q2))) {
        printf("leaf1\n");
        return 0;
    }
    if((!leaf(q1)) &&   leaf(q2) ) {
        printf("leaf2\n");
        return 0;
    }
    if(q1.qs != 0) for(int i = 0; i < 4; i++) if(!same_quad_helper(q1.qs[i], q2.qs[i], c)) {
        printf("children\n");
        return 0;
    }
    return 1;
}
int same_quad(quad q1, quad q2, int c)
{
    int equal = same_quad_helper(q1, q2, c);
    if(!equal){
        print_quad(q1);
        print_quad(q2);
    }
    return equal;
}
void make_fp_quad(const char* fname)
{
    printf("Making fp quad\n");
    int c = 1;
    // const char* fname = "data/fp/test/q_2.quad";
    quad q = make_empty_quad(make_square(1, 1, 2*Q_D), c);
    q.data[0] = 0.25;
    q.qs = malloc(4 * sizeof(quad));
    q.qs[0] = make_empty_quad(make_square(0.5, 0.5, Q_D), c);
    q.qs[1] = make_empty_quad(make_square(1.5, 0.5, Q_D), c);
    q.qs[2] = make_empty_quad(make_square(0.5, 1.5, Q_D), c);
    q.qs[3] = make_empty_quad(make_square(1.5, 1.5, Q_D), c);
    q.qs[0].data[0] = 0.1;
    q.qs[1].data[0] = 0.2;
    q.qs[2].data[0] = 0.3;
    q.qs[3].data[0] = 0.4;
    printf("about to save quad\n");
    save_quad_binary(q, fname, c);

    printf("testing if saving and loading is right\n");
    quad q_loaded = load_quad_binary(fname, c);
    TEST(same_quad(q, q_loaded, c));
}
void make_fp_quad_large(const char* fname)
{
    printf("Making fp large quad\n");
    int channels = 1;
    // const char* fname = "data/fp/test/q_4.quad";
    quad q = make_empty_quad(make_square(4, 4, 8*Q_D), channels);
    q.data[0] = 0.25;

    q.qs = malloc(4 * sizeof(quad));
    q.qs[0] = make_empty_quad(make_square(2.0, 2.0, 4*Q_D), channels);
    q.qs[1] = make_empty_quad(make_square(6.0, 2.0, 4*Q_D), channels);
    q.qs[2] = make_empty_quad(make_square(2.0, 6.0, 4*Q_D), channels);
    q.qs[3] = make_empty_quad(make_square(6.0, 6.0, 4*Q_D), channels);
    q.qs[0].data[0] = 0.1;
    q.qs[1].data[0] = 0.2;
    q.qs[2].data[0] = 0.3;
    q.qs[3].data[0] = 0.4;

    for(int i = 0; i < 4; i++){
        q.qs[i].qs = malloc(4 *sizeof(quad));
        float m = q.qs[0].s.d / 4.0F;
        q.qs[i].qs[0] = make_empty_quad(make_square(q.qs[i].s.w - m, q.qs[i].s.h - m, q.qs[i].s.d/2.0F), channels);
        q.qs[i].qs[1] = make_empty_quad(make_square(q.qs[i].s.w + m, q.qs[i].s.h - m, q.qs[i].s.d/2.0F), channels);
        q.qs[i].qs[2] = make_empty_quad(make_square(q.qs[i].s.w - m, q.qs[i].s.h + m, q.qs[i].s.d/2.0F), channels);
        q.qs[i].qs[3] = make_empty_quad(make_square(q.qs[i].s.w + m, q.qs[i].s.h + m, q.qs[i].s.d/2.0F), channels);
        q.qs[i].qs[0].data[0] = q.qs[i].data[0];
        q.qs[i].qs[1].data[0] = q.qs[i].data[0];
        q.qs[i].qs[2].data[0] = q.qs[i].data[0];
        q.qs[i].qs[3].data[0] = q.qs[i].data[0];

        for(int j = 0; j < 4; j++){
            q.qs[i].qs[j].qs = malloc(4 *sizeof(quad));
            float m = q.qs[i].qs[0].s.d / 4.0F;
            q.qs[i].qs[j].qs[0] = make_empty_quad(make_square(q.qs[i].s.w - m, q.qs[i].s.h - m, q.qs[i].qs[j].s.d/2.0F), channels);
            q.qs[i].qs[j].qs[1] = make_empty_quad(make_square(q.qs[i].s.w + m, q.qs[i].s.h - m, q.qs[i].qs[j].s.d/2.0F), channels);
            q.qs[i].qs[j].qs[2] = make_empty_quad(make_square(q.qs[i].s.w - m, q.qs[i].s.h + m, q.qs[i].qs[j].s.d/2.0F), channels);
            q.qs[i].qs[j].qs[3] = make_empty_quad(make_square(q.qs[i].s.w + m, q.qs[i].s.h + m, q.qs[i].qs[j].s.d/2.0F), channels);
            q.qs[i].qs[j].qs[0].data[0] = q.qs[i].qs[j].data[0];
            q.qs[i].qs[j].qs[1].data[0] = q.qs[i].qs[j].data[0];
            q.qs[i].qs[j].qs[2].data[0] = q.qs[i].qs[j].data[0];
            q.qs[i].qs[j].qs[3].data[0] = q.qs[i].qs[j].data[0];
        }
    }

    printf("about to save large quad\n");
    save_quad_binary(q, fname, channels);

    printf("testing if saving and loading of large quad is right\n");
    quad q_loaded = load_quad_binary(fname, channels);
    TEST(same_quad(q, q_loaded, channels));
}
void test_in_q()
{
    int c = 1;
    quad q = load_quad_binary("data/fp/test/q_2.quad", c);
    TEST(in_q(0, 0, q));
    TEST(in_q(2, 0, q));
    TEST(in_q(0, 2, q));
    TEST(in_q(2, 2, q));
    TEST(in_q(1.5, 0.5, q));
    TEST(!in_q(2.5, 1, q));
    TEST(!in_q(-0.5, 1, q));
    free_quad(q);
}
void test_q_set_pixel()
{
    int c = 1;
    quad q = load_quad_binary("data/fp/test/q_2.quad", c);
    quad q_copy;
    copy_quad(q, &q_copy, c);
    q_set_pixel(&q, c-1, 1.0, 1, 0.69, 2*Q_D);
    q_copy.data[0] = 0.69F;
    TEST(same_quad(q, q_copy, c));
    q_set_pixel(&q, c-1, 0.5F, 0.5F, 1.0F, Q_D);
    q_set_pixel(&q, c-1, 1.5F, 1.5F, 0.0F, Q_D);
    q_copy.qs[0].data[0] = 1.0F;
    q_copy.qs[3].data[0] = 0.0F;
    q_copy.data[0] = (1.0F + 0.2F + 0.3F + 0.0F) / 4.0F;
    TEST(same_quad(q, q_copy, c));
    free_quad(q);
    free_quad(q_copy);
}
void test_q_get_pixel()
{
    int c = 1;
    quad q = load_quad_binary("data/fp/test/q_2.quad", c);
    TEST(q_get_pixel(&q, 0, 0.5, 0.5) == 0.1F);
    TEST(q_get_pixel(&q, 0, 0.5, 1.5) == 0.2F);
    TEST(q_get_pixel(&q, 0, 1.5, 0.5) == 0.3F);
    TEST(q_get_pixel(&q, 0, 1.5, 1.5) == 0.4F);
    q.qs[0].data[0] = 1.0F;
    q.qs[1].data[0] = 0.69F;
    q.qs[2].data[0] = 0.0F;
    q.qs[3].data[0] = -0.42F;
    TEST(q_get_pixel(&q, 0, 0.5, 0.5) == 1.0F);
    TEST(q_get_pixel(&q, 0, 0.5, 1.5) == 0.69F);
    TEST(q_get_pixel(&q, 0, 1.5, 0.5) == 0.0F);
    TEST(q_get_pixel(&q, 0, 1.5, 1.5) == -0.42F);
    free_quad(q);
}
void test_square_overlap()
{
    square s1 = make_square(0, 0, 1);
    square s2 = make_square(1, 1, 1);
    TEST(square_overlap(s1, s2));
    s2 = make_square(1.5, 1, 1);
    TEST(!square_overlap(s1, s2));
}
void test_square_contained()
{
    square in = make_square(0, 0, 2);
    square out = make_square(1, 0, 4);
    TEST(square_contained(in, out));
    square not_in = make_square(-1, -2, 1);
    TEST(!square_contained(not_in, out));
    square larger_than_out = make_square(1, 0, 6);
    TEST(!square_contained(larger_than_out, out));
}
void test_quad_leaf_count()
{
    quad q = load_quad_binary("data/fp/test/dog_16.quad", 3);
    int count = 16*16;
    TEST(quad_leaf_count(q) == count);
    compress_quad(&q);
    count /= 4;
    TEST(quad_leaf_count(q) == count);
    compress_quad(&q);
    count /= 4;
    TEST(quad_leaf_count(q) == count);
    compress_quad(&q);
    count /= 4;
    TEST(quad_leaf_count(q) == count);
    free_quad(q);
}
void test_compress_quad()
{
    quad q = load_quad_binary("data/fp/test/q_8.quad", 1);
    compress_quad(&q);
    TEST(!leaf(q));
    TEST(quad_leaf_count(q) == 16);
    free_quad(q);
}
void test_qs_outside_squares()
{
    int c = 1;
    quad q = load_quad_binary("data/fp/test/q_8.quad", c);
    square interesting[] = {make_square(1.5, 1.5, 1)};
    int nr_squares = 1;
    int qs_len = 16;
    quad** qs = malloc(qs_len * sizeof(quad*));
    int temp = 1337;
    int* nr_quads = &temp;
    *nr_quads = 0;
    int n = qs_outside_squares(&q, interesting, nr_squares, qs, qs_len, nr_quads);
    // This graphic clarifies why three quads are found: https://www.geogebra.org/calculator/zwyjsspf
    TEST((n==*nr_quads) && (n==3));
    TEST((qs[0]->s.w == 6) && (qs[0]->s.h == 2) && (qs[0]->s.d == 4));
    TEST((qs[1]->s.w == 2) && (qs[1]->s.h == 6) && (qs[1]->s.d == 4));
    TEST((qs[2]->s.w == 6) && (qs[2]->s.h == 6) && (qs[2]->s.d == 4));
    free_quad(q);
}
void test_dog_16_quad()
{
    // image im = load_image("temp/dog_16.jpg");
    // q_img qim = im_to_qim(im);
    // save_quad_binary(*(im_to_qim(load_image("temp/dog_16.jpg")).q), "data/fp/test/dog_16.quad", 3);
    int c = 3;
    quad q = load_quad_binary("data/fp/test/dog_16.quad", c);
    square interesting[] = {make_square(1.5, 1.5, 1)};
    int nr_squares = 1;
    quad** qs = malloc(64 * sizeof(quad));
    int qs_len = 64;
    int temp = 1337;
    int* nr_quads = &temp;
    *nr_quads = 0;
    int n = qs_outside_squares(&q, interesting, nr_squares, qs, qs_len, nr_quads);
    // This graphic clarifies why 6 quads are found: https://www.geogebra.org/calculator/zwyjsspf
    TEST((n==*nr_quads) && (n==6));

    interesting[0] = make_square(8, 8, 2);
    nr_squares = 1;
    qs = malloc(64 * sizeof(quad));
    qs_len = 64;
    temp = 1337;
    nr_quads = &temp;
    *nr_quads = 0;
    n = qs_outside_squares(&q, interesting, nr_squares, qs, qs_len, nr_quads);
    TEST((n==*nr_quads) && (n==24));
    TEST((qs[0]->s.w==2) && (qs[0]->s.h==2) && (qs[0]->s.d==4));
    TEST((qs[4]->s.w==7) && (qs[4]->s.h==5) && (qs[4]->s.d==2));
    TEST((qs[22]->s.w==10) && (qs[22]->s.h==14) && (qs[22]->s.d==4));
    free_quad(q);
    free(qs);
}
void test_dog_16()
{
    q_img qim = im_to_qim(load_image("temp/dog_16.jpg"));\
    square interesting[] = {make_square(8, 8, 2)};
    int nr_squares = 1;
    int qs_len = 64;
    quad** qs = malloc(qs_len * sizeof(quad*));
    int temp = 1337;
    int* nr_quads = &temp;
    *nr_quads = 0;
    printf("has %lld leafs\n", quad_leaf_count(*(qim.q)));
    int n = qs_outside_squares(qim.q, interesting, nr_squares, qs, qs_len, nr_quads);
    printf("found %d quads in dog_16.jpg\n", n);
    for(int i = 0; i < n; i++) compress_quad(qs[i]);

    *nr_quads = 0;
    n = qs_outside_squares(qim.q, interesting, nr_squares, qs, qs_len, nr_quads);
    printf("found %d quads in dog_16.jpg\n", n);
    for(int i = 0; i < n; i++) compress_quad(qs[i]);
    printf("compressed has %lld leafs\n", quad_leaf_count(*(qim.q)));

    image im_comp = qim_to_im(qim);
    save_image(im_comp, "temp/dog_16_comp2");

    free_quad(*(qim.q));
    free(qs);
    free_image(im_comp);
}
void test_dog_512()
{
    image im = load_image("temp/dog_512.jpg");
    q_img qim = im_to_qim(im);
    square interesting[] = {make_square(256, 256, 64)};
    int nr_squares = 1;
    int qs_len = 1000;
    quad** qs = malloc(qs_len * sizeof(quad*));
    int temp = 1337;
    int* nr_quads = &temp;
    *nr_quads = 0;
    int n = qs_outside_squares(qim.q, interesting, nr_squares, qs, qs_len, nr_quads);
    printf("found %d quads in dog_512.jpg\n", n);
    for(int i = 0; i < n; i++) compress_quad(qs[i]);
    printf("compressed has %lld leafs\n", quad_leaf_count(*(qim.q)));

    *nr_quads = 0;
    n = qs_outside_squares(qim.q, interesting, nr_squares, qs, qs_len, nr_quads);
    printf("found %d quads in dog_512.jpg\n", n);
    for(int i = 0; i < n; i++) compress_quad(qs[i]);
    printf("compressed has %lld leafs\n", quad_leaf_count(*(qim.q)));

    image im_comp = qim_to_im(qim);
    save_image(im_comp, "temp/dog_512_comp2");

    free_image(im);
    free_image(im_comp);
    free_quad(*(qim.q));
    free(qs);
}
void test_fp()
{
    // const char* fname = "data/fp/test/q_8.quad";
    // make_fp_quad(fname);
    // make_fp_quad_large(fname);

    test_in_q();
    test_q_set_pixel();
    test_q_get_pixel();
    test_square_overlap();
    test_square_contained();
    test_quad_leaf_count();
    test_compress_quad();
    test_qs_outside_squares();

    // test_dog_16_quad();
    // test_dog_16();
    test_dog_512();
    printf("%d tests, %d passed, %d failed\n", tests_total, tests_total-tests_fail, tests_fail);
}

void run_tests()
{
    test_structure();
    test_cornerness();
    printf("%d tests, %d passed, %d failed\n", tests_total, tests_total-tests_fail, tests_fail);
}

