#include <ctype.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *key_names[] = {
    "Undefined",  "ESC",       "1", "2", "3", "4",           "5",
    "6",          "7",         "8", "9", "0", "-",           "=",
    "Backspace",  "Tab",       "q", "w", "e", "r",           "t",
    "y",          "u",         "i", "o", "p", "[",           "]",
    "Enter",      "Left Ctrl", "a", "s", "d", "f",           "g",
    "h",          "j",         "k", "l", ";", "'",           "`",
    "Left Shift", "\\",        "z", "x", "c", "v",           "b",
    "n",          "m",         ",", ".", "/", "Right Shift", "*",
    "Left Alt",   " "};

const char *up_key_names[] = {
    "_", "_", "!", "@", "#", "$", "%", "^", "&", "*", "(",  ")", "_", "+", "_",
    "_", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",  "{", "}", "_", "_",
    "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "~", "_", "|", "Z",
    "X", "C", "V", "B", "N", "M", "<", ">", "?", "_", "*",  "_", " "};

const char *read_scanner_word(char *filename, int timeout) {
  struct input_event ev;
  int shift = 0;
  int file = open(filename, O_RDONLY);
  if (file < 0) {
    perror("Error opening input event file");
    return NULL;
  }

  int allocated_size = 100;
  char *word;
  word = malloc(allocated_size * sizeof(char));
  word[0] = '\0';
  if (word == NULL) {
    perror("Error allocating memory");
    close(file);
    return NULL;
  }

  struct pollfd fds[1];
  fds[0].fd = file;
  fds[0].events = POLLIN;
  while (poll(fds, 1, timeout) > 0) {
    ssize_t bytes_read = read(file, &ev, sizeof(struct input_event));
    if (bytes_read < sizeof(struct input_event)) {
      perror("Error reading input event");
      close(file);
      free(word);
      return NULL;
    }
    if (ev.type == EV_KEY && ev.value == 1) {
      if (strcmp(key_names[ev.code], "Enter") == 0) {
        return word;
      }
      if (strlen(word) + 1 == allocated_size) {
        allocated_size += 10;
        char *new_word = realloc(word, allocated_size * sizeof(char));
        if (new_word == NULL) {
          perror("Error reallocating memory");
          close(file);
          free(word);
          return NULL;
        }
        word = new_word;
      }
      if (ev.type == EV_KEY && (key_names[ev.code] == "Left Shift" ||
                                key_names[ev.code] == "Right Shift")) {
        shift = 1;
        continue;
      }
      if (shift) {
        strcat(word, up_key_names[ev.code]);
        shift = 0;
      } else {
        strcat(word, key_names[ev.code]);
      }
    }
  }
  free(word);
  close(file);
  return NULL; // Return NULL if "Enter" key is not pressed
}
