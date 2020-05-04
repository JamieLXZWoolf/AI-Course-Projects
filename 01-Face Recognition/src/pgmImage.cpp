//
// Created by jamie on 4/21/20.
//
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "pgmImage.h"

char *img_basename(
    char *filename)
{
  char *newImage, *part;
  int len, dex;

  len = strlen(filename);  dex = len - 1;
  while (dex > -1) {
    if (filename[dex] == '/') {
      break;
    } else {
      dex--;
    }
  }
  dex++;
  part = &(filename[dex]);
  len = strlen(part);
  newImage = (char *) malloc ((unsigned) ((len + 1) * sizeof (char)));
  strcpy(newImage, part);
  return(newImage);
}

IMAGE *img_alloc()
{
  IMAGE *newImage;

  newImage = (IMAGE *) malloc (sizeof (IMAGE));
  if (newImage == NULL) {
    printf("IMGALLOC: Couldn't allocate image structure\n");
    return (NULL);
  }
  newImage->rows = 0;
  newImage->cols = 0;
  newImage->name = NULL;
  newImage->data = NULL;
  return (newImage);
}

IMAGE *img_creat(
    char *name,
    int nr, int nc)
{
  int i, j;
  IMAGE *newImage;

  newImage = img_alloc();
  newImage->data = (int *) malloc ((unsigned) (nr * nc * sizeof(int)));
  newImage->name = img_basename(name);
  newImage->rows = nr;
  newImage->cols = nc;
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++) {
        img_setPixel(newImage, i, j, 0);
    }
  }
  return (newImage);
}

void img_free(
    IMAGE *img)
{
  if (img->data) free ((char *) img->data);
  if (img->name) free ((char *) img->name);
  free ((char *) img);
}

void img_setPixel(
    IMAGE *img,
    int r, int c, int val)
{
  int nc;

  nc = img->cols;
  img->data[(r * nc) + c] = val;
}

int img_getPixel(
    IMAGE *img,
    int r, int c)
{
  int nc;

  nc = img->cols;
  return (img->data[(r * nc) + c]);
}

IMAGE *img_open(
    char *filename)
{
  IMAGE *newImage;
  FILE *pgm;
  char line[512], intbuf[100], ch;
  int type, nc, nr, maxval, i, j, k, found;

  newImage = img_alloc();
  if ((pgm = fopen(filename, "r")) == NULL) {
    printf("IMGOPEN: Couldn't open '%s'\n", filename);
    return(NULL);
  }

  newImage->name = img_basename(filename);

  /*** Scan pnm type information, expecting P5 ***/
  fgets(line, 511, pgm);
  sscanf(line, "P%d", &type);
  if (type != 5 && type != 2) {
    printf("IMGOPEN: Only handles pgm files (type P5 or P2)\n");
    fclose(pgm);
    return(NULL);
  }

  /*** Get dimensions of pgm ***/
  fgets(line, 511, pgm);
  sscanf(line, "%d %d", &nc, &nr);
  newImage->rows = nr;
  newImage->cols = nc;

  /*** Get maxval ***/
  fgets(line, 511, pgm);
  sscanf(line, "%d", &maxval);
  if (maxval > 255) {
    printf("IMGOPEN: Only handles pgm files of 8 bits or less\n");
    fclose(pgm);
    return(NULL);
  }

  newImage->data = (int *) malloc ((unsigned) (nr * nc * sizeof(int)));
  if (newImage->data == NULL) {
    printf("IMGOPEN: Couldn't allocate space for image data\n");
    fclose(pgm);
    return(NULL);
  }

  if (type == 5) {

    for (i = 0; i < nr; i++) {
      for (j = 0; j < nc; j++) {
          img_setPixel(newImage, i, j, fgetc(pgm));
      }
    }

  } else if (type == 2) {

    for (i = 0; i < nr; i++) {
      for (j = 0; j < nc; j++) {

        k = 0;  found = 0;
        while (!found) {
          ch = (char) fgetc(pgm);
          if (ch >= '0' && ch <= '9') {
            intbuf[k] = ch;  k++;
          } else {
            if (k != 0) {
              intbuf[k] = '\0';
              found = 1;
            }
          }
        }
          img_setPixel(newImage, i, j, atoi(intbuf));

      }
    }

  } else {
    printf("IMGOPEN: Fatal impossible error\n");
    fclose(pgm);
    return (NULL);
  }

  fclose(pgm);
  return (newImage);
}

int img_write(
    IMAGE *img,
    char *filename)
{
  int i, j, nr, nc, k, val;
  FILE *iop;

  nr = img->rows;  nc = img->cols;
  iop = fopen(filename, "w");
  fprintf(iop, "P2\n");
  fprintf(iop, "%d %d\n", nc, nr);
  fprintf(iop, "255\n");

  k = 1;
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++) {
      val = img_getPixel(img, i, j);
      if ((val < 0) || (val > 255)) {
        printf("IMG_WRITE: Found value %d at row %d col %d\n", val, i, j);
        printf("           Setting it to zero\n");
        val = 0;
      }
      if (k % 10) {
        fprintf(iop, "%d ", val);
      } else {
        fprintf(iop, "%d\n", val);
      }
      k++;
    }
  }
  fprintf(iop, "\n");
  fclose(iop);
  return (1);
}

IMAGELIST *imgl_alloc()
{
  IMAGELIST *newImage;

  newImage = (IMAGELIST *) malloc (sizeof (IMAGELIST));
  if (newImage == NULL) {
    printf("IMGL_ALLOC: Couldn't allocate image list\n");
    return(NULL);
  }

  newImage->n = 0;
  newImage->list = NULL;

  return (newImage);
}

void imgl_add(
    IMAGELIST *il,
    IMAGE *img)
{
  int n;

  n = il->n;

  if (n == 0) {
    il->list = (IMAGE **) malloc ((unsigned) (sizeof (IMAGE *)));
  } else {
    il->list = (IMAGE **) realloc ((char *) il->list,
                                   (unsigned) ((n+1) * sizeof (IMAGE *)));
  }

  if (il->list == NULL) {
    printf("IMGL_ADD: Couldn't reallocate image list\n");
  }

  il->list[n] = img;
  il->n = n+1;
}

void imgl_free(
    IMAGELIST *il)
{
  free((char *) il->list);
  free((char *) il);
}

void imgl_munge_name(
    char *buf)
{
  int j;

  j = 0;
  while (buf[j] != '\n') j++;
  buf[j] = '\0';
}

void imgl_load_images_from_textfile(
    IMAGELIST *il,
    char *filename)
{
  IMAGE *iimg;
  FILE *fp;
  char buf[2000];

  if (filename[0] == '\0') {
    printf("IMGL_LOAD_IMAGES_FROM_TEXTFILE: Invalid file '%s'\n", filename);
    return;
  }

  if ((fp = fopen(filename, "r")) == NULL) {
    printf("IMGL_LOAD_IMAGES_FROM_TEXTFILE: Couldn't open '%s'\n", filename);
    return;
  }

  while (fgets(buf, 1999, fp) != NULL) {

    imgl_munge_name(buf);
    printf("Loading '%s'...", buf);  fflush(stdout);
    if ((iimg = img_open(buf)) == 0) {
      printf("Couldn't open '%s'\n", buf);
    } else {
      imgl_add(il, iimg);
      printf("done\n");
    }
    fflush(stdout);
  }

  fclose(fp);
}

void load_input_with_image(
        IMAGE *img,
        ffnn *net)
{
    double *units;
    int nr, nc, imgsize, i, j, k;

    nr = ROWS(img);
    nc = COLS(img);
    imgsize = nr * nc;;
    if (imgsize != net->getInputN()) {
        printf("LOAD_INPUT_WITH_IMAGE: This image has %d pixels,\n", imgsize);
        printf("   but your net has %d input units.  I give up.\n", net->getInputN());
        exit (-1);
    }

    units = net->input_units;
    k = 1;
    for (i = 0; i < nr; i++) {
        for (j = 0; j < nc; j++) {
            units[k] = ((double) img_getPixel(img, i, j)) / 255.0;
            k++;
        }
    }
}

void load_20_face_target(IMAGE *img, ffnn *net) {
    int scale;
    char userid[40], head[40], expression[40], eyes[40], photo[40];

    userid[0] = head[0] = expression[0] = eyes[0] = photo[0] = '\0';

    /*** scan in the image features ***/
    sscanf(NAME(img), "%[^_]_%[^_]_%[^_]_%[^_]_%d.%[^_]",
           userid, head, expression, eyes, &scale, photo);

    for (int i = 1; i <= net->getOutputN(); i++) {
        net->target[i] = TARGET_LOW;
    }

    if (!strcmp(userid, "an2i")) {
        net->target[1] = TARGET_HIGH;
    } else if (!strcmp(userid, "at33")) {
        net->target[2] = TARGET_HIGH;
    } else if (!strcmp(userid, "boland")) {
        net->target[3] = TARGET_HIGH;
    } else if (!strcmp(userid, "bpm")) {
        net->target[4] = TARGET_HIGH;
    } else if (!strcmp(userid, "ch4f")) {
        net->target[5] = TARGET_HIGH;
    } else if (!strcmp(userid, "cheyer")) {
        net->target[6] = TARGET_HIGH;
    } else if (!strcmp(userid, "choon")) {
        net->target[7] = TARGET_HIGH;
    } else if (!strcmp(userid, "danieln")) {
        net->target[8] = TARGET_HIGH;
    } else if (!strcmp(userid, "glickman")) {
        net->target[9] = TARGET_HIGH;
    } else if (!strcmp(userid, "karyadi")) {
        net->target[10] = TARGET_HIGH;
    } else if (!strcmp(userid, "kawamura")) {
        net->target[11] = TARGET_HIGH;
    } else if (!strcmp(userid, "kk49")) {
        net->target[12] = TARGET_HIGH;
    } else if (!strcmp(userid, "megak")) {
        net->target[13] = TARGET_HIGH;
    } else if (!strcmp(userid, "mitchell")) {
        net->target[14] = TARGET_HIGH;
    } else if (!strcmp(userid, "night")) {
        net->target[15] = TARGET_HIGH;
    } else if (!strcmp(userid, "phoebe")) {
        net->target[16] = TARGET_HIGH;
    } else if (!strcmp(userid, "saavik")) {
        net->target[17] = TARGET_HIGH;
    } else if (!strcmp(userid, "steffi")) {
        net->target[18] = TARGET_HIGH;
    } else if (!strcmp(userid, "sz24")) {
        net->target[19] = TARGET_HIGH;
    } else if (!strcmp(userid, "tammo")) {
        net->target[20] = TARGET_HIGH;
    }
}

void load_sunglasses_target(IMAGE *img, ffnn *net) {
    int scale;
    char userid[40], head[40], expression[40], eyes[40], photo[40];

    userid[0] = head[0] = expression[0] = eyes[0] = photo[0] = '\0';

    /*** scan in the image features ***/
    sscanf(NAME(img), "%[^_]_%[^_]_%[^_]_%[^_]_%d.%[^_]",
           userid, head, expression, eyes, &scale, photo);

    if (!strcmp(eyes, "sunglasses")) {
        net->target[1] = TARGET_HIGH;
    } else {
        net->target[1] = TARGET_LOW;
    }
}

void load_pose_target(IMAGE *img, ffnn *net) {
    int scale;
    char userid[40], head[40], expression[40], eyes[40], photo[40];

    userid[0] = head[0] = expression[0] = eyes[0] = photo[0] = '\0';

    /*** scan in the image features ***/
    sscanf(NAME(img), "%[^_]_%[^_]_%[^_]_%[^_]_%d.%[^_]",
           userid, head, expression, eyes, &scale, photo);

    for (int i = 1; i <= net->getOutputN(); i++) {
        net->target[i] = TARGET_LOW;
    }

    if (!strcmp(head, "straight")) {
        net->target[1] = TARGET_HIGH;
    }
    else if (!strcmp(head, "up")) {
        net->target[2] = TARGET_HIGH;
    }
    else if (!strcmp(head, "left")) {
        net->target[3] = TARGET_HIGH;
    }
    else if (!strcmp(head, "right")) {
        net->target[4] = TARGET_HIGH;
    }
}
