#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "header.h"

// Free memory of a color type matrix
void free_colormatrix(int rows, color ***a)
{
	for (int i = 0; i < rows; i++)
		free((*a)[i]);
	free(*a);
	*a = NULL;
}

// Dinamically allocate a matrix of type color with n rows and m columns
// and return a pointer to that matrix
color **alloc_matrix(int n, int m)
{
	color **matrix = (color **)malloc(n * sizeof(color *));
	if (!matrix) {
		fprintf(stderr, "malloc() failed\n");
		return NULL;
	}
	for (int i = 0; i < n; i++) {
		matrix[i] = (color *)malloc(m * sizeof(color));
		if (!matrix[i]) {
			for (int j = 0; j < i; j++)
				free(matrix[j]);
			fprintf(stderr, "malloc() failed\n");
			return NULL;
		}
	}
	return matrix;
}

// jump over comments in a file
void read_comments(FILE *image)
{
	char ch = '\n';
	do {
		while (ch != '\n')
			fscanf(image, "%c", &ch);
		fscanf(image, "%c", &ch);
	} while (ch == '#');
	fseek(image, -1, SEEK_CUR);
}

// Find type of an PBM image and return it as an int
int typeof_file(FILE *image)
{
	char magic_number[10];
	read_comments(image);
	fgets(magic_number, 10, image);
	return magic_number[1] - '0';
}

// Load a grayfile into system
color **load_grayfile(char *filename, data *data, FILE *image, int cursor)
{
	color **matrix;
	matrix = alloc_matrix(ROWS, COLS);
	fseek(image, cursor + 1, SEEK_SET);
	// if the file is type 2 then the file is not binary
	if (TYPE == 2) {
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++)
				fscanf(image, "%hhu", &matrix[i][j].gray);
		fclose(image);

	// if the file is type 5 then the file is binary
	} else if (TYPE == 5) {
		//go back where the file will be reopened
		fclose(image);
		image = fopen(filename, "rb");
		fseek(image, cursor + 1, SEEK_SET);

		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++)
				fread(&matrix[i][j].gray, sizeof(unsigned char), 1, image);
		fclose(image);
	}
	printf("Loaded %s\n", filename);
	return matrix;
}

// Load a colored file into system
color **load_colorfile(char *filename, data *data, FILE *image, int cursor)
{
	color **matrix;
	matrix = alloc_matrix(ROWS, COLS);
	fseek(image, cursor + 1, SEEK_SET);
	// if the file is type 3 then the file is not binary
	if (TYPE == 3) {
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++) {
				fscanf(image, "%le", &matrix[i][j].red);
				fscanf(image, "%le", &matrix[i][j].green);
				fscanf(image, "%le", &matrix[i][j].blue);
			}
		fclose(image);
	// if the file is type 6 then the file is binary
	} else if (TYPE == 6) {
		//gp back where the file will be reopened
		fclose(image);
		image = fopen(filename, "rb");
		fseek(image, cursor + 1, SEEK_SET);

		unsigned char r, g, b;
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++) {
				fread(&r, sizeof(unsigned char), 1, image);
				fread(&g, sizeof(unsigned char), 1, image);
				fread(&b, sizeof(unsigned char), 1, image);

				matrix[i][j].red = (double)r;
				matrix[i][j].green = (double)g;
				matrix[i][j].blue = (double)b;
			}
		fclose(image);
	}
	printf("Loaded %s\n", filename);
	return matrix;
}

void load_image(color ***matrix, data *data, char *filename)
{
	filename = strtok(NULL, " ");
	// free the old image if one exists
	if (*matrix)
		free_colormatrix(ROWS, matrix);
	FILE *image = fopen(filename, "rt");
	if (!image) {
		printf("Failed to load %s\n", filename);
		return;
	}
	read_comments(image);
	TYPE = typeof_file(image);
	read_comments(image);
	fscanf(image, "%d", &COLS);
	read_comments(image);
	fscanf(image, "%d", &ROWS);
	read_comments(image);
	fscanf(image, "%d", &MAXVALUE);
	read_comments(image);
	int cursor = ftell(image);
	if (TYPE == 2 || TYPE == 5) {
		*matrix = load_grayfile(filename, data, image, cursor);
		ISCOLORED = 0;
	} else if (TYPE == 3 || TYPE == 6) {
		*matrix = load_colorfile(filename, data, image, cursor);
		ISCOLORED = 1;
	}
	// initialize selection to the entire image
	if (*matrix) {
		X1 = 0;
		Y1 = 0;
		X2 = ROWS;
		Y2 = COLS;
	}
}

// Check if a pixel is inside the image or outside
int check_boundries(int x, int y, int n, int m)
{
	if (x >= 0 && x <= n && y >= 0 && y <= m)
		return 1;
	return 0;
}

// Return the maximum value of two int value
int max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

// Return the minimum value of two int value
int min(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

// Convert a string to an int or retrun junk value if it
// isn't a number
int get_int(char string[])
{
	int ok = 0;
	if (string) {
		int l = strlen(string);
		int n = 0;
		for (int i = 0; i < l; i++) {
			if (string[i] == '-') {
				ok = 1;
			} else {
				int c = string[i] - '0';
				if (c >= 0 && c <= 9)
					n = n * 10 + c;
				else
					return -1e9;
			}
		}
		if (ok)
			n *= -1;
		return n;
	} else {
		return -1e9;
	}
}

void select_pixels(data *data, char *command)
{
	command = strtok(NULL, " ");
	if (strcmp(command, "ALL") == 0) {
		X1 = 0, Y1 = 0, X2 = ROWS, Y2 = COLS;
		printf("Selected ALL\n");
	} else {
		// read every coordinate and check is it is a valid number
		int x1tmp, y1tmp, x2tmp, y2tmp;
		y1tmp = get_int(command);
		if (y1tmp == -1e9) {
			printf("Invalid command\n");
			return;
		}
		command = strtok(NULL, " ");
		x1tmp = get_int(command);
		if (x1tmp == -1e9) {
			printf("Invalid command\n");
			return;
		}
		command = strtok(NULL, " ");
		y2tmp = get_int(command);
		if (y2tmp == -1e9) {
			printf("Invalid command\n");
			return;
		}
		command = strtok(NULL, " ");
		x2tmp = get_int(command);
		if (x2tmp == -1e9) {
			printf("Invalid command\n");
			return;
		}

		// check if all the conditions are met else
		// print a coresponding message
		if (check_boundries(x1tmp, y1tmp, ROWS, COLS) && x1tmp != x2tmp) {
			if (check_boundries(x2tmp, y2tmp, ROWS, COLS) && y1tmp != y2tmp) {
				X1 = min(x1tmp, x2tmp);
				X2 = max(x1tmp, x2tmp);
				Y1 = min(y1tmp, y2tmp);
				Y2 = max(y1tmp, y2tmp);
				printf("Selected %d %d %d %d\n", Y1, X1, Y2, X2);
			} else {
				printf("Invalid set of coordinates\n");
			}
		} else {
			printf("Invalid set of coordinates\n");
		}
	}
}

// Rotate only the selection of an image by a given angle
void rotate_selection(data *data, color **matrix, char *command)
{
	// get angle value
	command = strtok(NULL, " ");
	int angle = atoi(command);

	if (angle % 90 != 0 || angle > 360 || angle < -360) {
		printf("Unsupported rotation angle\n");
	} else if (X2 - X1 != Y2 - Y1) {
		printf("The selection must be square\n");
	} else {
		printf("Rotated %d\n", angle);
		// if angle is equal to -360 we don't need to rotate it
		if (angle != -360) {
			// if the angle is positive we convert it to a negative one
			// without changing the final rotation
			if (angle > 0)
				angle -= 360;
			color **tmp = alloc_matrix(Y2 - Y1, X2 - X1);
			while (angle < 0) {
				for (int i = X1; i < X2; i++)
					for (int j = Y1; j < Y2; j++)
						tmp[j - Y1][i - X1] = matrix[i][j];

				for (int j = 0; j < X2 - X1; j++)
					for (int i = 0; i < (Y2 - Y1) / 2; i++) {
						color aux = tmp[i][j];
						tmp[i][j] = tmp[Y2 - Y1 - i - 1][j];
						tmp[Y2 - Y1 - i - 1][j] = aux;
					}

				for (int i = X1; i < X2; i++)
					for (int j = Y1; j < Y2; j++)
						matrix[i][j] = tmp[i - X1][j - Y1];

				angle += 90;
			}
			free_colormatrix(Y2 - Y1, &tmp);
		}
	}
}

// Rotate the whole image by a given angle
void rotate_image(data *data, color ***matrix, char *command)
{
	// read angle
	command = strtok(NULL, " ");
	int angle = atoi(command);

	color **tmp;
	if (angle % 90 != 0 || angle > 360 || angle < -360) {
		printf("Unsupported rotation angle\n");
	} else {
		printf("Rotated %d\n", angle);
		if (angle != -360) {
			if (angle > 0)
				angle -= 360;
			while (angle < 0) {
				// this time it will allocate a matrix each time a new
				// rotation takes place
				tmp = alloc_matrix(COLS, ROWS);
				for (int i = 0; i < ROWS; i++)
					for (int j = 0; j < COLS; j++)
						tmp[j][i] = (*matrix)[i][j];
				for (int j = 0; j < ROWS; j++)
					for (int i = 0; i < COLS / 2; i++) {
						color aux = tmp[i][j];
						tmp[i][j] = tmp[COLS - i - 1][j];
						tmp[COLS - i - 1][j] = aux;
					}
				free_colormatrix(ROWS, matrix);
				*matrix = tmp;
				X2 = COLS;
				Y2 = ROWS;
				int aux = ROWS;
				ROWS = COLS;
				COLS = aux;
				angle += 90;
			}
		}
	}
}

void rotate(data *data, color ***matrix, char *command)
{
	if (X1 == 0 && Y1 == 0 && X2 == ROWS && Y2 == COLS)
		rotate_image(data, matrix, command);
	else
		rotate_selection(data, *matrix, command);
}

void crop_image(data *data, color ***matrix)
{
	color **tmp = alloc_matrix(X2 - X1, Y2 - Y1);

	for (int i = X1; i < X2; i++)
		for (int j = Y1; j < Y2; j++)
			tmp[i - X1][j - Y1] = (*matrix)[i][j];

	free_colormatrix(ROWS, matrix);
	*matrix = tmp;
	ROWS = X2 - X1;
	COLS = Y2 - Y1;
	X1 = 0, Y1 = 0, X2 = ROWS, Y2 = COLS;

	printf("Image cropped\n");
}

// Clamps a double value between an upper and lower bound
double double_clamp(double x, int low, int upp)
{
	if (x < low)
		return 1. * low;
	else if (x > upp)
		return 1. * upp;
	return x;
}

void save_gray(data *data, color **matrix, char *filename, int new_type)
{
	if (new_type == 2) {
		FILE *image = fopen(filename, "wt");
		fprintf(image, "P2\n");
		fprintf(image, "%d %d\n%d\n", COLS, ROWS, MAXVALUE);

		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLS; j++)
				fprintf(image, "%d ", matrix[i][j].gray);
			fprintf(image, "\n");
		}
		fclose(image);
	} else if (new_type == 5) {
		FILE *image = fopen(filename, "wb");
		fprintf(image, "P5\n");
		fprintf(image, "%d %d\n%d\n", COLS, ROWS, MAXVALUE);

		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++)
				fwrite(&matrix[i][j].gray, sizeof(unsigned char), 1, image);
		fclose(image);
	}
}

void save_color(data *data, color **matrix, char *filename, int new_type)
{
	if (new_type == 3) {
		FILE *image = fopen(filename, "wt");
		fprintf(image, "P3\n");
		fprintf(image, "%d %d\n%d\n", COLS, ROWS, MAXVALUE);

		unsigned char r, g, b;
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLS; j++) {
				r = double_clamp(round(matrix[i][j].red), 0, MAXVALUE);
				g = double_clamp(round(matrix[i][j].green), 0, MAXVALUE);
				b = double_clamp(round(matrix[i][j].blue), 0, MAXVALUE);

				fprintf(image, "%hhu ", r);
				fprintf(image, "%hhu ", g);
				fprintf(image, "%hhu ", b);
			}
			fprintf(image, "\n");
		}
		fclose(image);

	} else if (new_type == 6) {
		FILE *image = fopen(filename, "wb");
		fprintf(image, "P6\n");
		fprintf(image, "%d %d\n%d\n", COLS, ROWS, MAXVALUE);

		unsigned char r, g, b;
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++) {
				r = double_clamp(round(matrix[i][j].red), 0, MAXVALUE);
				g = double_clamp(round(matrix[i][j].green), 0, MAXVALUE);
				b = double_clamp(round(matrix[i][j].blue), 0, MAXVALUE);

				fwrite(&r, sizeof(unsigned char), 1, image);
				fwrite(&g, sizeof(unsigned char), 1, image);
				fwrite(&b, sizeof(unsigned char), 1, image);
			}
		fclose(image);
	}
}

void save_image(data *data, color **matrix, char *command)
{
	command = strtok(NULL, " ");
	char *filename = command;
	command = strtok(NULL, " ");
	if (!ISCOLORED) {
		if (!command)
			save_gray(data, matrix, filename, 5);
		else if (strcmp(command, "ascii") == 0)
			save_gray(data, matrix, filename, 2);
	} else {
		if (!command)
			save_color(data, matrix, filename, 6);
		else if (strcmp(command, "ascii") == 0)
			save_color(data, matrix, filename, 3);
	}
	printf("Saved %s\n", filename);
}

// Apply a given effect to the image
void apply_filter(data *data, color **mat, double effect[3][3])
{
	// Used two directional vectors to easily iterate through all
	// neighbours of a pixel
	int dx[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
	int dy[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};

	color **tmp = alloc_matrix(ROWS, COLS);
	for (int i = X1; i < X2; i++)
		for (int j = Y1; j < Y2; j++) {
			int ok = 1;
			double r = 0., g = 0., b = 0.;

			for (int d = 0; d < 9 && ok; d++) {
				int x = i + dx[d];
				int y = j + dy[d];

				if (x >= 0 && x < ROWS && y >= 0 && y < COLS) {
					r += effect[x - i + 1][y - j + 1] * mat[x][y].red;
					g +=  effect[x - i + 1][y - j + 1] * mat[x][y].green;
					b += effect[x - i + 1][y - j + 1] * mat[x][y].blue;
				} else {
					ok = 0;
				}
			}
			if (ok) {
				tmp[i][j].red = double_clamp(r, 0, MAXVALUE);
				tmp[i][j].green = double_clamp(g, 0, MAXVALUE);
				tmp[i][j].blue = double_clamp(b, 0, MAXVALUE);
			} else {
				tmp[i][j] = mat[i][j];
			}
		}
	for (int i = X1; i < X2; i++)
		for (int j = Y1; j < Y2; j++)
			mat[i][j] = tmp[i][j];
	free_colormatrix(ROWS, &tmp);
}

void apply(data *data, color **matrix, char *command)
{
	double edge[3][3] = {{-1., -1., -1.},
						{-1., 8., -1.},
						{-1., -1., -1.}};

	double sharpen[3][3] = {{0., -1., 0.},
						{-1., 5., -1.},
						{0., -1., 0.}};

	double boxblur[3][3] = {{0.1111, 0.1111, 0.1111},
						{0.1111, 0.1111, 0.1111},
						{0.1111, 0.1111, 0.1111}};

	double gaussblur[3][3] = {{0.0625, 0.125, 0.0625},
							{0.125, 0.25, 0.125},
							{0.0625, 0.125, 0.0625}};

	command = strtok(NULL, " ");
	char *type = command;
	int ok = 1;
	if (type) {
		if (ISCOLORED) {
			if (strcmp(type, "EDGE") == 0) {
				apply_filter(data, matrix, edge);

			} else if (strcmp(type, "SHARPEN") == 0) {
				apply_filter(data, matrix, sharpen);

			} else if (strcmp(type, "BLUR") == 0) {
				apply_filter(data, matrix, boxblur);

			} else if (strcmp(type, "GAUSSIAN_BLUR") == 0) {
				apply_filter(data, matrix, gaussblur);

			} else {
				printf("APPLY parameter invalid\n");
				ok = 0;
			}
		} else {
			printf("Easy, Charlie Chaplin\n");
			ok = 0;
		}
		if (ok)
			printf("APPLY %s done\n", command);
	} else {
		printf("Invalid command\n");
	}
}

void exit_progam(data *data, color ***matrix)
{
	if (!(*matrix))
		printf("No image loaded\n");
	else
		free_colormatrix(ROWS, matrix);
}
