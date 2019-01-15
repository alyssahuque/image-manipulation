#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	unsigned char R, G, B; /* colors */
} Pixel;

typedef struct
{
	int width;
	int height;
	Pixel *pxl;
} Image;

/*
 * Arguments:
 *   filename: the name of the file to read
 *   img:      a pointer to an Image struct.
 */

void
ReadImage(char *filename, Image *img)
{
    /* putting pixels into the image struct */
    fprintf(stderr, "Reading image \"%s\"\n", filename);
    
    FILE *f_in;
    char magicNum[128];
    int width, height, maxval;


    f_in = fopen(filename, "r");

    fscanf(f_in, "%s\n%d %d\n%d\n", magicNum, &width, &height, &maxval);

    img -> pxl = malloc(sizeof(Pixel) * width * height);

    img -> height = height;
    img -> width = width;    


    fread(img -> pxl, sizeof(Pixel), width * height, f_in);


    fclose(f_in);

    fprintf(stderr, "Done reading image \"%s\"\n", filename);
}

void WriteImage(char *filename, Image *img)
{
    fprintf(stderr, "Writing image \"%s\"\n", filename);

    FILE *f_out = fopen(filename, "w");
    int width;
    int height;


    fprintf(f_out, "P6\n%d %d\n255\n", img -> width, img -> height);
	    

    fwrite(img -> pxl, sizeof(Pixel), img -> width * img -> height, f_out);

    fprintf(stderr, "Done writing image \"%s\"\n", filename);
    fclose(f_out);
}

/* 
 * Arguments:
 *    width:  the width of the image
 *    height: the height of the image
 *    row:    the row of the pixel
 *    column: the column of the pixel
 *
 * Returns: the index of the pixel in the image for (column, row)
 */

int GetPixelIndex(int width, int height, int row, int column)
{
    if (column < 0 || column >= width)
    {
        fprintf(stderr, "You asked for a pixel index for column %d, but the only valid values are between 0 and %d\n", column, width-1);
        exit(EXIT_FAILURE);
    }
    if (row < 0 || row >= height)
    {
        fprintf(stderr, "You asked for a pixel index for row %d, but the only valid values are between 0 and %d\n", row, height-1);
        exit(EXIT_FAILURE);
    }
    return row*width + column;
}

/* 
 * Arguments:
 *   input:    an image.  Take this image and replace the diagonal with
 *             a yellow line.
 *   output:   an image.  This isthe image with the yellow diagonal line
 */


void YellowDiagonal(Image *input, Image *output)
{
    fprintf(stderr, "Executing YellowDiagonal\n");
    int Get, i, j;

    output -> width = input -> width;
    output -> height = input -> height;

    output -> pxl = malloc(sizeof(Pixel) * input -> width * input -> height);
    
    for(i=0; i < input -> height; i++){
	    for(j=0; j < input -> width; j++){
		    Get = GetPixelIndex(input -> width, input -> height, i, j);
		    if (i == j){
			    output -> pxl[Get].R = 255;
			    output -> pxl[Get].G = 255;
			    output -> pxl[Get].B = 0;
		    }
		    else{
			    output -> pxl[Get].R = input -> pxl[Get].R;
			    output -> pxl[Get].G = input -> pxl[Get].G;
			    output -> pxl[Get].B = input -> pxl[Get].B;
		    }
	    }
    }

    fprintf(stderr, "Done executing YellowDiagonal\n");
}

/* 
 * Arguments:
 *   input1:    an image.  This is the "left image".
 *   input2:    an image.  This is the "right image".
 *   output:    an image.  This is the horizontal combination of input1 and 2.
 */

void LeftRightCombine(Image *input1, Image *input2, Image *output)
{
    fprintf(stderr, "Executing LeftRightCombine\n");
    int i, n, m, Geti1, Geto1, Geti2, Geto2;

    output -> width = input1 -> width + input2 -> width;
    output -> height = input1 -> height;

    output -> pxl = malloc(sizeof(Pixel) * output -> width * output -> height);

    for(i=0; i < output -> height; i++){
	    for(n=0; n < input1 -> width; n++){
		    Geti1 = GetPixelIndex(input1 -> width, input1 -> height, i, n);
		    Geto1 = GetPixelIndex(output -> width, output -> height, i, n);
		    output -> pxl[Geto1].R = input1 -> pxl[Geti1].R;
		    output -> pxl[Geto1].G = input1 -> pxl[Geti1].G;
		    output -> pxl[Geto1].B  = input1 -> pxl[Geti1].B;}
	    for(m=0; m < input2 -> width; m++){
		    Geti2 = GetPixelIndex(input2 -> width, input2 -> height, i, m);
		    Geto2 = GetPixelIndex(output -> width, output -> height, i, input1 -> width + m);
		    output -> pxl[Geto2].R = input2 -> pxl[Geti2].R;
		    output -> pxl[Geto2].G = input2 -> pxl[Geti2].G;
		    output -> pxl[Geto2].B = input2 -> pxl[Geti2].B;}
    }

    fprintf(stderr, "Done executing LeftRightCombine\n");
}

void
VerifyInput()
{
    FILE *f_in = fopen("2G_input.pnm", "r");
    if (f_in == NULL)
    {
        fprintf(stderr, "Cannot open 2G_input.pnm for reading.\n");
        exit(EXIT_FAILURE);
    }
    fclose(f_in);

    FILE *f_out = fopen("2G_output.pnm", "w");
    if (f_out == NULL)
    {
        fprintf(stderr, "Cannot open 2G_output.pnm for writing.\n");
        exit(EXIT_FAILURE);
    }
    fclose(f_out);
}

int main(int argc, char *argv[])
{
    VerifyInput();

    Image source_image;
    Image yellow_diagonal;
    Image left_right;

    ReadImage("2G_input.pnm", &source_image);
    WriteImage("output_of_read.pnm", &source_image);

    YellowDiagonal(&source_image, &yellow_diagonal);
    WriteImage("yellow_diagonal.pnm", &yellow_diagonal);


    LeftRightCombine(&source_image, &yellow_diagonal, &left_right);
    WriteImage("2G_output.pnm", &left_right);
    
    free(source_image.pxl);
    free(yellow_diagonal.pxl);
    free(left_right.pxl);

    return 0;
}
