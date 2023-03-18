import sys, os
from ctypes import *

lib = CDLL(os.path.join(os.path.dirname(__file__), "libuwimg.so"), RTLD_GLOBAL)

def c_array(ctype, values):
    arr = (ctype*len(values))()
    arr[:] = values
    return arr

class SQUARE(Structure):
    _fields_ = [("w", c_float),
                ("h", c_float),
                ("r", c_float)]

class QUAD(Structure):
    pass

QUAD._fields_ = [("qs", POINTER(QUAD)),
                ("s", SQUARE),
                ("data", POINTER(c_float))]

class Q_IMG(Structure):
    _fields_ = [("c", c_int),
                ("h", c_int),
                ("w", c_int),
                ("q", POINTER(QUAD))]

im_to_qim = lib.im_to_qim
im_to_qim.argtypes = [IMAGE]
im_to_qim.restype = Q_IMG

qim_to_im = lib.qim_to_im
qim_to_im.argtypes = [Q_IMG]
qim_to_im.restype = IMAGE

im_interesting_regions = lib.im_interesting_regions
im_interesting_regions.argtypes = [IMAGE, POINTER(SQUARE)]
im_interesting_regions.restype = c_int

compress_im = lib.compress_im
compress_im.argtypes = [IMAGE, c_longlong, c_int, c_float]
compress_im.restype = Q_IMG

compress_quad = lib.compress_quad
compress_quad.argtypes = [POINTER(QUAD)]
compress_quad.restype = None

q_img_size = lib.q_img_size
q_img_size.argtypes = [Q_IMG]
q_img_size.restype = c_longlong

print_quad = lib.print_quad
print_quad.argtypes = [QUAD]
print_quad.restype = None

allocate_squares = lib.allocate_squares
allocate_squares.argtypes = [c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int)]
allocate_squares.restype = POINTER(SQUARE)

compress_im_manual = lib.compress_im_manual
compress_im_manual.argtypes = [IMAGE, POINTER(SQUARE), c_int, c_longlong, c_int, c_float]
compress_im_manual.restype = Q_IMG

