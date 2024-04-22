# How to take a picture on Linux:
In this repository you will find the source code for a C lib that can be used in Python or standalone to take a picture using a given video source and a filename to which the image should be stored.
## USAGE:
```bash
    make build-so-cam

    # test using docker
    docker compose build test-cam
    docker compose up -d test-cam

    # test using python
    python3 use_cam.py
```

# How to read from a QR/Barcode scanner on Linux:
In this repository you will find the source code for a C lib that can be used in Python or standalone to make a scanner reading from input when provided the `/dev/input/event<number>` to read from and a timeout for which the program should wait to receive any bytes to read from that stream.
## USAGE:
```bash
    make build-so-qr

    # test using docker
    docker compose build test-qr
    docker compose up -d test-qr

    # test using python
    python3 use_qr.py
```

### Extra rules in order to build both:
```bash
    make build-so-all
```

### There is a setup for testing this in a docker env. This will make images and expect a qr/bar code to be scanned.
