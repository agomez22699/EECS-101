#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS	480
#define COLS	640
#define PI 3.14159265358979323846

#define sqr(x)	((x)*(x))

void clear( unsigned char image[][COLS] );
void header( int row, int col, unsigned char head[32] );

int				i, j, sgmmax;
// localmax: number in the three bucket of voting array corrsponding to three local maxima
// index[3][2]: used for store rho and theta for three local maximas

/*
*                               number of votings  |		rho		|	theta
*		first local maxima:			localmax[0]		 index[0][0]      index[0][1]
*		second local maxima:		localmax[1]      index[1][0]      index[1][1]
*		third local maxima:			localmax[2]      index[2][0]      index[2][1]
*
*		You may let hough_threshold equal the minimum of three local maximas to threshold the voting array
*/

int				dedx, dedy, sgm, localmax[3] = { 0, 0, 0 }, index[3][2] = { 0, 0, 0, 0, 0, 0 }, ma, voting_max[3], temp_x[ROWS][COLS], temp_y[ROWS][COLS], temp_sgm[ROWS][COLS], max[3];
// voting: voting array
int				sgm_threshold, hough_threshold, voting[180][1600];
FILE* fp;
unsigned char	image[ROWS][COLS], simage[ROWS][COLS], sgmimage[ROWS][COLS], bimage[ROWS][COLS],himage[ROWS][COLS], x_image[ROWS][COLS], y_image[ROWS][COLS], temp[ROWS][COLS], head[32];
char			filename[50], ifilename[50];
double           theta, rho;



int main( int argc, char** argv )
{
	clear(simage);
	strcpy ( filename, "image.raw");
	memset ( voting, 0, sizeof(int) * 180 * 400 );
	header(ROWS, COLS, head);

	//Declare Thresholds
	hough_threshold = 325;
	sgm_threshold = 20;
	 
	/* Read in the image */
	if (!( fp = fopen(filename, "rb" ) ))
	{
		fprintf( stderr, "error: couldn't open %s\n", argv[1]);
		exit(1);
	}

	for ( i = 0 ; i < ROWS ; i++ )
		if (!( COLS == fread( image[i], sizeof(char), COLS, fp ) ))
		{
			fprintf( stderr, "error: couldn't read %s\n", argv[1] );
			exit(1);
		}
	fclose(fp);

    /* Compute SGM */
	for (i = 1; i < ROWS - 1; i++) {
		for (j = 1; j < COLS - 1; j++) {

			//Implemented Filter for X
			temp_x[i][j] = abs(-1 * (image[i - 1][j - 1]) - 2 * (image[i][j - 1]) - 1 * (image[i + 1][j - 1]) + 1 * (image[i - 1][j + 1]) + 2 * (image[i + 1][j]) + 1 * (image[i + 1][j + 1]));

			//readjusting x max values
			if (max[0] < temp_x[i][j]) {
				max[0] = temp_x[i][j];
			}

			//Implemented Filter for Y
			temp_y[i][j] = abs(-1 * (image[i - 1][j - 1]) - 2 * (image[i - 1][j]) - 1 * (image[i - 1][j + 1]) + 1 * (image[i + 1][j - 1]) + 2 * (image[i + 1][j]) + 1 * (image[i + 1][j + 1]));

			//readjusting y max values
			if (max[1] < temp_y[i][j]) {
				max[1] = temp_y[i][j];
			}

			//SGM 
			temp_sgm[i][j] = temp_x[i][j] * temp_x[i][j] + temp_y[i][j] * temp_y[i][j];

			//readjusting max for sgm
			if (max[2] < temp_sgm[i][j]) {
				max[2] = temp_sgm[i][j];
			}
		}
	}

	//second for loop for transferring from temp to main image
	for (i = 1; i < ROWS - 1; i++) {
		for (j = 1; j < COLS - 1; j++) {
			x_image[i][j] = 255 * ((double)temp_x[i][j] / max[0]);
			y_image[i][j] = 255 * ((double)temp_y[i][j] / max[1]);
			simage[i][j] = 255 * ((double)temp_sgm[i][j] / max[2]);
		}
	}

	/* Compute the binary SGM image using threshold*/
	for (i = 1; i < ROWS - 1; i++) {
		for (j = 1; j < COLS - 1; j++) {
			if (simage[i][j] > sgm_threshold) {
				bimage[i][j] = 255;
			}
			else {
				bimage[i][j] = 0;
			}
		}
	}

	/* build up voting array */
	//Putting 0s in voting array
	for (i = 1; i < 180; i++) {
		for (j = 1; j < 1600; j++) {
			voting[i][j] = 0;
		}
	}

	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLS; j++) {
			if (bimage[i][j] == 255) {
				for (theta = 0; theta < 180; theta++) {
					rho = i * cos(PI / 180 * theta) - j * sin(PI / 180 * theta);
					voting[(int)theta][(int)rho + 800] += 1;
				}
			}
		}

	}

	/* Find out the three local maxima of voting array */
	// Implement your code here ... notice here that any two of three local maximas CANNOT be too close in (rho, theta) space
	for (i = 170; i > 0; i--) {
		for (j = 0; j < 1600; j++) {
			if (100 < voting[i][j]) {
				printf("[%d, %d] = %d \n", i, j - 800, voting[i][j]);
			}

		}
	}

	/* Save SGM to an image */
	strcpy(filename, "image");
	if (!(fp = fopen(strcat(filename, "-sgm.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for (i = 0; i < ROWS; i++)
		fwrite(simage[i], sizeof(char), COLS, fp);
	fclose(fp);

	/* Save the thresholded SGM to an image */
	strcpy(filename, "image");
	if (!(fp = fopen(strcat(filename, "-binary.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for (i = 0; i < ROWS; i++)
		fwrite(bimage[i], sizeof(char), COLS, fp);
	fclose(fp);

	/* Save original voting array to an image */
	//for (i = 0; i < 180; i++)
	//	for (j = 0; j < 400; j++)
	//	{	
	//		// Implement your code here ...
	//		// simage[i][j] = use voting[i][j] and normalize to [0, 255]
	//	}
	strcpy(filename, "image");
	header(180, 1600, head);
	if (!(fp = fopen(strcat(filename, "-voting_array.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);

	for (i = 0; i < 180; i++)
		fwrite(voting[i], sizeof(char), 1600, fp);
	fclose(fp);

	/* Threshold the voting array */
	for (i = 0; i < 180; i++) {
		for (j = 0; j < 1600; j++)
		{
			if (voting[i][j] > hough_threshold)
			{
				voting[i][j] = 255;
			}
			else {
				voting[i][j] = 0;
			}
		}
	}

	/* Write the thresholded voting array to a new image */
	strcpy(filename, "image");
	header(180, 1600, head);
	if (!(fp = fopen(strcat(filename, "-voting_thresholded_array.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);

	for (i = 0; i < 180; i++)
		fwrite(voting[i], sizeof(char), 1600, fp);
	fclose(fp);

	index[0][0] = 51;
	index[0][1] = -170;
	index[1][0] = 13;
	index[1][1] = 320;
	index[2][0] = 129;
	index[2][1] = -297;
	voting_max[0] = 478;
	voting_max[1] = 382;
	voting_max[2] = 306;

	printf("Hough threshold: %d\n", hough_threshold);
	printf("%d %d %d\n%d %d %d\n%d %d %d\n", index[0][0], index[0][1], voting_max[0],
											index[1][0], index[1][1] , voting_max[1],
											index[2][0], index[2][1], voting_max[2]);

	/* Reconstruct an image from the voting array */
	clear(simage);
	for (i = ROWS; 0 < i; i--) {
		for (j = 0; j < COLS; j++) {
			if (0 == (int)(j * sin(index[0][0] * PI / 180) - i * cos(index[0][0] * PI / 180) + index[0][1])) {
				simage[i][j] = 255;
			}
			if (0 == (int)(j * sin(index[1][0] * PI / 180) - i * cos(index[1][0] * PI / 180) + index[1][1])) {
				simage[i][j] = 255;
			}
			if (0 == (int)(j * sin(index[2][0] * PI / 180) - i * cos(index[2][0] * PI / 180) + index[2][1])) {
				simage[i][j] = 255;
			}
		}
	}

	/* Write the reconstructed figure to an image */
	strcpy(filename, "image");
	header(ROWS, COLS, head);
	if (!(fp = fopen(strcat(filename, "-reconstructed_image.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for (i = 0; i < ROWS; i++)
		fwrite(simage[i], sizeof(char), COLS, fp);
	fclose(fp);
	return 0;
}

	void clear(unsigned char image[][COLS])
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLS ; j++ ) image[i][j] = 0;
}

void header(int row, int col, unsigned char head[32])
{
	int *p = (int *)head;
	char *ch;
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
	ch ++; 
	head[6] = *ch;
	ch ++;
	head[5] = *ch;
	ch ++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch ++; 
	head[10] = *ch;
	ch ++;
	head[9] = *ch;
	ch ++;
	head[8] = *ch;
	
	ch = (char*)&num;
	head[19] = *ch;
	ch ++; 
	head[18] = *ch;
	ch ++;
	head[17] = *ch;
	ch ++;
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

