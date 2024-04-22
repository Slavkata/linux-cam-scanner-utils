#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define BUFFER_COUNT 1

struct buffer {
  void *start;
  size_t length;
};

int capture_img(const char *filename, const char *VIDEO_DEVICE) {
  int fd;
  struct v4l2_capability cap;
  struct v4l2_format fmt;
  struct v4l2_requestbuffers req;
  struct v4l2_buffer buf;
  struct buffer *buffers;
  int i;

  fd = open(VIDEO_DEVICE, O_RDWR);
  if (fd == -1) {
    perror("Failed to open video device");
    return -1;
  }

  if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
    perror("Failed to query capabilities");
    close(fd);
    return -1;
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf(stderr, "Device does not support video capture\n");
    close(fd);
    return -1;
  }

  memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = 640;
  fmt.fmt.pix.height = 480;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;

  if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
    perror("Failed to set format");
    close(fd);
    return -1;
  }

  memset(&req, 0, sizeof(req));
  req.count = BUFFER_COUNT;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
    perror("Failed to request buffers");
    close(fd);
    return -1;
  }

  buffers = calloc(BUFFER_COUNT, sizeof(*buffers));
  if (!buffers) {
    perror("Failed to allocate memory");
    close(fd);
    return -1;
  }

  for (i = 0; i < BUFFER_COUNT; i++) {
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
      perror("Failed to query buffer");
      close(fd);
      free(buffers);
      return -1;
    }

    buffers[i].length = buf.length;
    buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, buf.m.offset);

    if (buffers[i].start == MAP_FAILED) {
      perror("Failed to mmap buffer");
      close(fd);
      free(buffers);
      return -1;
    }
  }

  for (i = 0; i < BUFFER_COUNT; i++) {
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
      perror("Failed to enqueue buffer");
      close(fd);
      free(buffers);
      return -1;
    }
  }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
    perror("Failed to start streaming");
    close(fd);
    free(buffers);
    return -1;
  }

  if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
    perror("Failed to dequeue buffer");
    close(fd);
    free(buffers);
    return -1;
  }

  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    perror("Failed to open output file");
    close(fd);
    free(buffers);
    return -1;
  }

  fwrite(buffers[buf.index].start, buf.bytesused, 1, fp);
  fclose(fp);

  if (ioctl(fd, VIDIOC_STREAMOFF, &type) == -1) {
    perror("Failed to stop streaming");
  }

  for (i = 0; i < BUFFER_COUNT; i++) {
    munmap(buffers[i].start, buffers[i].length);
  }

  free(buffers);
  close(fd);

  return 0;
}

int main() { return capture_img("image.jpg", "/dev/video0"); }
