#include <stdio.h>
#include <unistd.h>

#include "ppm_image.h"

void print_usuage(char *prog_name)
{
  fprintf(stderr, "Usage: %s -i <image> [-n] [-o <output>]\n", prog_name);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -i <image>   Specify input image file (required)\n");
  fprintf(stderr, "  -n           Apply negative effect\n");
  fprintf(stderr, "  -o <output>  Specify output file name (default: output.ppm)\n");
}

int main(int argc, char **argv)
{
  char *input_file = NULL;
  char *output_file = "output.ppm";
  int apply_negative = 0;

  int opt;
  while ((opt = getopt(argc, argv, "i:ngso:")) != -1) {
    switch(opt) {
      case 'i':
        input_file = optarg;
        break;
      case 'n':
        apply_negative = 1;
        break;
      case 'o':
        output_file = optarg;
        break;
      default:
        print_usuage(argv[0]);
        return 1;
    }
  }

  if (!input_file) {
    print_usuage(argv[0]);
    return 1;
  }

  struct ppm_image *image = read_ppm(input_file);
  if (!image) {
    fprintf(stderr, "failed to load image\n");
    return 1;
  }

  struct ppm_image processed_image;
  move_ppm_image(&processed_image, image);

  if (apply_negative) {
    struct ppm_image negative_effect_image = negative_effect_ppm_image(&processed_image);
    free_ppm_image(&processed_image);
    move_ppm_image(&processed_image, &negative_effect_image);
  }

  if (write_ppm(output_file, &processed_image) != 0) {
    fprintf(stderr, "failed to write output file\n");
    free_ppm_image(&processed_image);
    free(image);
    return 1;
  }

  free_ppm_image(&processed_image);
  free(image);
  return 0;
}
