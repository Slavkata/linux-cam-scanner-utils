import ctypes
import os

libqr = ctypes.CDLL("./libqr.so")

libqr.read_scanner_word.argtypes = (ctypes.c_char_p, ctypes.c_int)
libqr.read_scanner_word.restype = ctypes.c_char_p


def use_qr():
    """
    Read QR code from the scanner and print it
    """
    try:
        print(
            "QR code reading: ",
            libqr.read_scanner_word(os.environ.get("QR_PATH").encode(), 1000).decode(
                "utf-8"
            ),
        )
    except Exception as e:
        print(f"Error: {e}")
        print("Please make sure the scanner is connected and the path is correct.")


if __name__ == "__main__":
    use_qr()
