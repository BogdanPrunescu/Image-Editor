#ifndef __IMAGE_EDITOR__
#define __IMAGE_EDITOR__

#include <stdio.h>

// Defines that make the code easy to read
#define MAXVALUE (data->maxvalue)
#define TYPE (data->type)
#define ROWS (data->rows)
#define COLS (data->cols)
#define ISCOLORED (data->iscolored)
#define X1 (data->x1)
#define Y1 (data->y1)
#define X2 (data->x2)
#define Y2 (data->y2)

// Stuct used to store other values needed for commands
typedef struct {
	int maxvalue;
	int type;
	int rows, cols;
	int iscolored;
	int x1, y1, x2, y2;
} data;

// Struct used to store the matrix of pixels
typedef struct {
	unsigned char gray;
	double red, green, blue;
} color;

// Open an image file and store into memory the number of rows, columns,
// maxvalue and the matrix of pixels
void load_image(color ***matrix, data *data, char *filename);

// Select a zone of pixels in which all commands affect the image
// It will select [x1, x2) on the X axis and [y1, y2) on the Y axis
void select_pixels(data *data, char *command);

// Rotate a selection or the entire image by a given angle
void rotate(data *data, color ***matrix, char *command);

// Crop an image to the selection
void crop_image(data *data, color ***matrix);

// Save all the changes made to an image to a new file
void save_image(data *data, color **matrix, char *command);

// apply image kernels to selection if the image is colored
void apply(data *data, color **matrix, char *command);

// deallocate all memory used
void exit_progam(data *data, color ***matrix);

#endif
