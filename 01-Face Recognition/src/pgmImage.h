//
// Created by jamie on 4/21/20.
//
#include "ffnn.h"
#ifndef FFNNFACERECOGNITION_PGMIMAGE_H
#define FFNNFACERECOGNITION_PGMIMAGE_H

/*** User accessible macros ***/

#define ROWS(img)  ((img)->rows)
#define COLS(img)  ((img)->cols)
#define NAME(img)   ((img)->name)
#define TARGET_HIGH 0.9
#define TARGET_LOW 0.1

/*** User accessible functions ***/

typedef struct {
    char *name;
    int rows, cols;
    int *data;
} IMAGE;

typedef struct {
    int n;
    IMAGE **list;
} IMAGELIST;

IMAGE *img_open(char *filename);

IMAGE *img_creat(char *name, int nr, int nc);

void img_setPixel(IMAGE *img, int r, int c, int val);

int img_getPixel(IMAGE *img, int r, int c);

int img_write(IMAGE *img, char *filename);

void img_free(IMAGE *img);

void imgl_load_images_from_textfile(IMAGELIST *il, char *filename);

IMAGELIST *imgl_alloc();

void imgl_add(IMAGELIST *il, IMAGE *img);

void imgl_free(IMAGELIST *il);

void load_input_with_image(IMAGE *img, ffnn *net);

void load_20_face_target(IMAGE *img, ffnn *net);

void load_sunglasses_target(IMAGE *img, ffnn *net);

void load_pose_target(IMAGE *img, ffnn *net);
#endif //FFNNFACERECOGNITION_PGMIMAGE_H
