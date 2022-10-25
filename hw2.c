#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define ROWS		100
#define COLUMNS		100

int main( int argc, char **argv )
{

	int			i,j,k;
	FILE			*fp;
	float			u, var, t, a;
	unsigned char	image[ROWS][COLUMNS];
  	char			*ifile, *ofile, ch;
	char			filename[4][50];

	strcpy(filename[0], "image1.raw");
	strcpy(filename[1], "image2.raw");
	strcpy(filename[2], "image3.raw");
	strcpy(filename[3], "image4.raw");

	u = 0;
	var = 0;


	//Assign each image name in filename to ifile here	
	/* example: ifile = filename[k]; k=0,1,2,3; a loop might be needed*/

	t = 100.0;
	a = t * t;


	for (k = 0; k < 4; k++) {

		ifile = filename[k];

		if ((fp = fopen(ifile, "rb")) == NULL)
		{
			fprintf(stderr, "error: couldn't open %s\n", ifile);
			exit(1);
		}

		for (i = 0; i < ROWS; i++)
			if (fread(image[i], 1, COLUMNS, fp) != COLUMNS)
			{
				fprintf(stderr, "error: couldn't read enough stuff\n");
				exit(1);
			}

		fclose(fp);

		//Calculate Mean for each image here
		for (i = 0; i < ROWS; i++) { //for loop for x coordinates
			for (j = 0; j < COLUMNS; j++) { //for loop for y coordinates
				u += image[i][j];
			}
		}

		u = u / a;

		//Calculate Variance for each image here
		for (i = 0; i < ROWS; i++) {
			for (j = 0; j < COLUMNS; j++) {
				var += pow((image[i][j] - u), 2);
			}
		}

		var = var / (a - 1.0);

		//Print mean and variance for each image
		printf("%s: %f %f\n", ifile, u, var);
	}
	printf("Press any key to exit: ");
	gets ( &ch );

	return 0;
}