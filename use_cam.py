import ctypes
import os

libcam = ctypes.CDLL("./libcam.so")

libcam.capture_img.argtypes = (ctypes.c_char_p, ctypes.c_char_p)
libcam.capture_img.restype = ctypes.c_int


def use_cam():
    libcam.capture_img("test.jpg".encode(), os.environ.get("CAM_PATH").encode())


if __name__ == "__main__":
    use_cam()
