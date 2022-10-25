#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS 480
#define COLS 640

//Declaring Functions 
void clear(unsigned char image[][COLS]);
void header(int row, int col, unsigned char head[32]);

//Moved variables to global scope to avoid stack overflow.
int		i,j,image_count,dotProd;
unsigned char	image[ROWS][COLS], head[32];
FILE* fp;
double r[9] = {50,50,50,10,100,50,50,50,50};
double a[9] = {0.5,0.5,0.5,0.5,0.5,0.1,1,0.5,0.5};
double m[9] = {1,1,1,1,1,1,1,0.1,10000};
double v[3] = {0,0,1}; // viewing direction 
double L[ROWS][COLS]; //Scence Radiance
double alpha, c, p, q, h[3], n[3], t, Ll, Ls, maximum,x,y;
char image_name[9][50];

int main( int argc, char **argv ){

//Couldnt delcare in global scope
double s[9][3] = { {0,0,1},
					{1 / sqrt(3),1 / sqrt(3),1 / sqrt(3)},
					{1,0,0},
					{0,0,1},
					{0,0,1},
					{0,0,1},
					{0,0,1},
					{0,0,1},
					{0,0,1} };

strcpy(image_name[0], "image_a");
strcpy(image_name[1], "image_b");
strcpy(image_name[2], "image_c");
strcpy(image_name[3], "image_d");
strcpy(image_name[4], "image_e");
strcpy(image_name[5], "image_f");
strcpy(image_name[6], "image_g");
strcpy(image_name[7], "image_h");
strcpy(image_name[8], "image_i");


for (image_count = 0; image_count < 9; image_count++) {
	clear(image);
	c = (v[0] + s[image_count][0]) * (v[0] + s[image_count][0]) + (v[1] + s[image_count][1]) * (v[1] + s[image_count][1]) + (v[2] + s[image_count][2]) * (v[2] + s[image_count][2]); //constant

	h[0] = (v[0] + s[image_count][0]) / sqrt(c);
	h[1] = (v[1] + s[image_count][1]) / sqrt(c);
	h[2] = (v[2] + s[image_count][2]) / sqrt(c);
	//this is the loop for x
	for (i = 0; i < ROWS; i++) {
		//this is the loop for y
		for (j = 0; j < COLS; j++) {

			x = j - (COLS / 2);
			y = (ROWS / 2) - i;

			p = (-x) / sqrt(r[image_count] * r[image_count] - (x * x) - (y * y)); //finding p
			q = (-y) / sqrt(r[image_count] * r[image_count] - (x * x) - (y * y)); //finding q

			n[0] = (-p) / sqrt(1 + p * p + q * q);
			n[1] = (-q) / sqrt(1 + p * p + q * q);
			n[2] = 1 / sqrt(1 + p * p + q * q);
			
			//reset t here
			t = 0.0; //temp value
			for (dotProd = 0; dotProd < 3; dotProd++) {
				t += n[dotProd] * s[image_count][dotProd];
			}

			//transfer t into Ll
			Ll = t;

			if (0 <= Ll) {
				//reset t to 0
				t = 0.0;
				//dot prod
				for (dotProd = 0; dotProd < 3; dotProd++) {
					t += n[dotProd] * h[dotProd];
				}
				alpha = acos(t);
				Ls = exp(-(alpha / m[image_count] * alpha / m[image_count]));
				L[i][j] = (Ll * a[image_count] + Ls * (1 - a[image_count]));
				image[i][j] = 255 * L[i][j];
			}


		}
	}


	header(ROWS, COLS, head);
	if (!(fp = fopen(strcat(image_name[image_count], ".ras"), "wb"))) {
		fprintf(stderr, "error: could not open %s\n", image_name[image_count]);
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for (i = 0; i < ROWS; i++) fwrite(image[i], 1, COLS, fp);
	fclose(fp);
} //Outermost loop
	return 0;
}

//Imported Clear from previous assigments 
void clear(unsigned char image[][COLS])
{
	int	i, j;
	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++) image[i][j] = 0;
}


//Imported header from previous assignments 
void header(int row, int col, unsigned char head[32])
{
	int* p = (int*)head;
	char* ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */

	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch++;
	head[6] = *ch;
	ch++;
	head[5] = *ch;
	ch++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch++;
	head[10] = *ch;
	ch++;
	head[9] = *ch;
	ch++;
	head[8] = *ch;

	ch = (char*)&num;
	head[19] = *ch;
	ch++;
	head[18] = *ch;
	ch++;
	head[17] = *ch;
	ch++;
	head[16] = *ch;


	/* Big-endian for unix */
	/*
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;
*/
}