#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "header.h"

int main(void)
{
	char command_line[101];
	color **matrix = NULL;
	char *p;
	data values;
	do {
		// read every command using fgets
		fgets(command_line, 100, stdin);
		if (strcmp(command_line, "EXIT") != 0)
			command_line[strlen(command_line) - 1] = '\0';

		// use strtok to iterate through each work in the command
		p = strtok(command_line, " ");

		// LOAD command
		if (strcmp(p, "LOAD") == 0) {
			load_image(&matrix, &values, p);

		// SELECT command
		} else if (strcmp(p, "SELECT") == 0) {
			if (!matrix)
				printf("No image loaded\n");
			else
				select_pixels(&values, p);

		// ROTATE command
		} else if (strcmp(p, "ROTATE") == 0) {
			if (!matrix)
				printf("No image loaded\n");
			else
				rotate(&values, &matrix, p);

		// CROP command
		} else if (strcmp(p, "CROP") == 0) {
			if (!matrix)
				printf("No image loaded\n");
			else
				crop_image(&values, &matrix);

		// APPLY command
		} else if (strcmp(p, "APPLY") == 0) {
			if (!matrix)
				printf("No image loaded\n");
			else
				apply(&values, matrix, p);

		// SAVE command
		} else if (strcmp(p, "SAVE") == 0) {
			if (!matrix)
				printf("No image loaded\n");
			else
				save_image(&values, matrix, p);

		} else if (strcmp(p, "EXIT") != 0) {
			printf("Invalid command\n");
		}
	} while (!p || strcmp(p, "EXIT") != 0);
	exit_progam(&values, &matrix);
	return 0;
}
