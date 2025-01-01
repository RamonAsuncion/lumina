#ifndef _PPM_IMAGE_H_
#define _PPM_IMAGE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define GET_PIXEL(_image, _x, _y) \
  ((_y) * (_image)->width * 3 + (_x) * 3)

#define IMAGE_SIZE(_image) ((_image)->width * (_image)->height * 3)

struct ppm_image {
  int width;
  int height;
  int max_value;
  unsigned char *data;
};

struct ppm_image create_ppm_image(int width, int height, int max_value)
{
  struct ppm_image image;
  image.width = width;
  image.height = height;
  image.max_value = max_value;

  image.data = (unsigned char*)malloc(IMAGE_SIZE(&image) * sizeof(unsigned char*));
  if (image.data == NULL) {
    fprintf(stderr, "memory allocation");
    exit(1);
  }

  /**
   * default: set all pixels to max value.
   */
  for (int i = 0; i < IMAGE_SIZE(&image); ++i) {
    image.data[i] = max_value;
  }

  return image;
}

struct ppm_image *read_ppm(char *filename)
{
  int r = 0;

  /**
   * open ppm image
   */
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("error opening file");
    return NULL;
  }

  struct ppm_image *image = (struct ppm_image*)malloc(sizeof(struct ppm_image));

  /*
   * read header
   */
  char magic_number[3];
  fscanf(fp, "%2s", magic_number);
  if (strncmp(magic_number, "P3", 2) != 0) {
    fprintf(stderr, "invalid ppm format\n");
    fclose(fp);
    return NULL;
  }
  r = fscanf(fp, "%d %d\n%d\n", &image->width, &image->height, &image->max_value);
  if (r != 3) {
    fprintf(stderr, "no dimensions\n");
    fclose(fp);
    free(image);
    return NULL;
  }

  image->data = (unsigned char*)malloc(IMAGE_SIZE(image));
  if (!image->data) {
    fprintf(stderr, "memory allocation");
    fclose(fp);
    free(image);
    return NULL;
  }

  /**
   * read image
   */
  for (int i = 0; i < IMAGE_SIZE(image); ++i) {
    if (fscanf(fp, "%hhu", &image->data[i]) != 1) {
      fprintf(stderr, "can't read image data");
      fclose(fp);
      free(image->data);
      free(image);
      return NULL;
    }
  }

  fclose(fp);

  return image;
}

int write_ppm(char *filename, struct ppm_image *image)
{
  FILE *fp = fopen(filename, "wb");
  if (fp == NULL) {
    perror("error opening file");
    return 1;
  }

  fprintf(fp, "P3\n%d %d\n%d\n", image->width, image->height, image->max_value);

  // todo: https://netpbm.sourceforge.net/doc/ppm.html
  // no line longer than 70

  int line_length = 0;

  for (int y = 0; y < image->height; ++y) {
    for (int x = 0; x < image->width; ++x) {
      int idx = GET_PIXEL(image, x, y);
      fprintf(fp, "%-3hhu %-3hhu %-3hhu",
               image->data[idx], image->data[idx + 1], image->data[idx + 2]);

      line_length++;

      if (line_length < image->width) {
        fprintf(fp, " ");
      } else {
        fprintf(fp, "\n");
        line_length = 0;
      }
    }
  }

  fclose(fp);

  return 0;
}

/*
 * Color Inversion
 */
struct ppm_image negative_effect_ppm_image(struct ppm_image *image)
{
  struct ppm_image new_image = create_ppm_image(image->width, image->height, image->max_value);

  for (int y = 0; y < image->height; ++y) {
    for (int x = 0; x < image->width; ++x) {
      int idx = GET_PIXEL(image, x, y);
      new_image.data[idx] = image->max_value - image->data[idx];
      new_image.data[idx + 1] = image->max_value - image->data[idx + 1];
      new_image.data[idx + 2] = image->max_value - image->data[idx + 2];
    }
  }

  return new_image;
}

/**
 * Gray scale
 * Reference: https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
 */
struct ppm_image gray_scale_effect_ppm_image(struct ppm_image *image)
{
  struct ppm_image new_image = create_ppm_image(image->width, image->height, image->max_value);

  for (int y = 0; y < image->height; ++y) {
    for (int x = 0; x < image->width; ++x) {
      int idx = GET_PIXEL(image, x, y);

      unsigned char gray = (unsigned char)(
        0.299 * image->data[idx] +
        0.587 * image->data[idx + 1] +
        0.144 * image->data[idx + 2]
       );

      new_image.data[idx] = gray;
      new_image.data[idx + 1] = gray;
      new_image.data[idx + 2] = gray;
    }
  }

  return new_image;
}


void move_ppm_image(struct ppm_image *dest, struct ppm_image *src)
{
  dest->width = src->width;
  dest->height = src->height;
  dest->max_value = src->max_value;
  dest->data = src->data;
  /**
    * no accidental free
    */
  src->data = NULL;
}

void print_ppm_image(struct ppm_image *image)
{
  printf("ppm_image-data: ");
  for (int y = 0; y < image->height; ++y) {
    for (int x = 0; x < image->width; ++x) {
      int idx = GET_PIXEL(image, x, y);
      printf("(%hhu, %hhu, %hhu) ", image->data[idx],
             image->data[idx + 1], image->data[idx + 2]);
    }
  }
  printf("\n");
}

void free_ppm_image(struct ppm_image *image)
{
  free(image->data);
  image->data = NULL;
}

#endif /* _PPM_IMAGE_H_ */
