#include <stdio.h>
#include <stdlib.h>

#include "lzfx.h"

/* Program to demonstrate file compression. Don't use it in the real world! */

int main(int argc, char **argv) {
  if (argc == 3) {
    /* open input */
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
      printf("Error: %s\n", argv[1]);
      return 1;
    }

    /* get size */
    fseek(f, 0, SEEK_END);
    int inputsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    /* read */
    char *input = malloc(inputsize);
    fread(input, inputsize, 1, f);
    fclose(f);

    /* compress */
    int   outputsize = inputsize + 1; /* buffer overflow */
    char *output     = malloc(outputsize);
    lzfx_compress(input, inputsize, output, &outputsize);

    /* open output */
    f = fopen(argv[2], "wb");
    if (!f) {
      printf("Error: %s\n", argv[1]);
      return 1;
    }

    /* write */
    fwrite(output, outputsize, 1, f);
    fclose(f);

    return 0;
  } else {
    printf("Compresses a file.\n\nUsage: lzfx-raw input output\n");
    return 1;
  }
}
