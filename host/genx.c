#include <stdio.h>
#include "genx.h"
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int generate_x(uint32_t *matrix, uint32_t size, uint32_t q) {
  int urandom_fd = open("/dev/urandom", O_RDONLY);
  if (urandom_fd < 0) {
    perror("Failed to open /dev/urandom");
    return -1;
  }
  uint32_t total_elements = size * size;
  uint32_t i = 0;
  uint32_t max_acceptable = UINT32_MAX - (UINT32_MAX % q);
  while (i < total_elements) {
    uint32_t rand_val;
    if (read(urandom_fd, &rand_val, sizeof(rand_val)) != sizeof(rand_val)) {
      close(urandom_fd);
      return -1;
    }
    if (rand_val < max_acceptable) {
      matrix[i] = rand_val % q;
      i++;
    }
  }
  close(urandom_fd);
  return 0;
}
