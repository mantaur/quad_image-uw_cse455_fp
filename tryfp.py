from uwimg import *

im = load_image("temp/dog_512.jpg")

budget = 10000
iters = 5
ratio = 0

# qim = compress_im(im, budget, iters, ratio)
n = 2
int_array = c_int * n
w_list = [256, 512]
w_array = int_array(*w_list)
int_array = c_int * n
h_list = [256, 115]
h_array = int_array(*h_list)
int_array = c_int * n
d_list = [100, 80]
d_array = int_array(*d_list)

squares = allocate_squares(n, w_array, h_array, d_array)
qim = compress_im_manual(im, squares, n, budget, iters, ratio)

im_compressed = qim_to_im(qim)
save_image(im_compressed, "temp/dog_512-" + str(iters) + "-" + str(n) + "squares")

print("DONE")