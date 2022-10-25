#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS		(int)480
#define COLUMNS		(int)640

void clear( unsigned char image[][COLUMNS] );
void header( int row, int col, unsigned char head[32] );

int				temp_x[ROWS][COLUMNS], temp_y[ROWS][COLUMNS], temp_sgm[ROWS][COLUMNS];
int				i, j, k, threshold[3], max[3], x, y, s; // choose threshold values for each image
FILE* fp;
unsigned char	image[ROWS][COLUMNS], ximage[ROWS][COLUMNS], yimage[ROWS][COLUMNS], head[32], sgm_image[ROWS][COLUMNS], binary_image[ROWS][COLUMNS]; //introduced 2 more arrays
char			filename[6][50], ifilename[50], ch;



int main( int argc, char *argv[] )
{

	strcpy( filename[0], "image1" );
	strcpy( filename[1], "image2" );
	strcpy( filename[2], "image3" );
	header ( ROWS, COLUMNS, head );

	threshold[0] = 34; // choose thresholds values for each image for generate binary images
	threshold[1] = 8; // choose thresholds values for each image for generate binary images
	threshold[2] = 16; // choose thresholds values for each image for generate binary images

	printf( "Maximum of Gx, Gy, SGM\n" );
	for ( k = 0; k < 3; k ++)
	{
		//insert a 0 to the temp arrays or Cleaning the arrays
		for (i = 0; i < ROWS; i++) {
			for (j = 0; j < ROWS; j++) {
				temp_x[i][j] = 0;
				temp_y[i][j] = 0;
				temp_sgm[i][j] = 0;
			}
		}

		//Clean the images
		clear(image);
		clear(sgm_image);
		clear( ximage );
		clear( yimage );

		/* Read in the image */
		strcpy( ifilename, filename[k] );
		if (( fp = fopen( strcat(ifilename, ".raw"), "rb" )) == NULL )
		{
		  fprintf( stderr, "error: couldn't open %s\n", ifilename );
		  exit( 1 );
		}			
		for ( i = 0; i < ROWS ; i++ )
		  if ( fread( image[i], sizeof(char), COLUMNS, fp ) != COLUMNS )
		  {
			fprintf( stderr, "error: couldn't read enough stuff\n" );
			exit( 1 );
		  }
		fclose( fp );

		max[0] = 0; //maximum of Gx
		max[1] = 0; //maximum of Gy
		max[2] = 0; //maximum of SGM

		/* Compute Gx, Gy, SGM, find out the maximum and normalize*/
		// We want to implement the filters
		/*
		*		-1 0 1   -1 -2 -1
		*		-2 0 2    0  0  0
		*		-1 0 1	  1  2  1 
		*/
		
		for (i = 1; i < ROWS - 1; i++) {
			for (j = 1; j < COLUMNS - 1; j++) {

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
			for (j = 1; j < COLUMNS - 1; j++) {
				ximage[i][j] = 255 * ((double)temp_x[i][j] / max[0]);
				yimage[i][j] = 255 * ((double)temp_y[i][j] / max[1]);
				sgm_image[i][j] = 255 * ((double)temp_sgm[i][j] / max[2]);
			}
		}

		strcpy(ifilename, filename[k]);
		if (!(fp = fopen(strcat(ifilename, "-g.ras"), "wb")))
		{
			fprintf(stderr, "error: could not open %s\n", ifilename);
			exit(1);
		}
		fwrite(head, 4, 8, fp);
		for (i = 1; i < ROWS - 1; i++)
			fwrite(image[i], 1, COLUMNS, fp);
		fclose(fp);

		/* Write Gx to a new image*/
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-x.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
		for (i = 1; i < ROWS - 1; i++) {
			fwrite(ximage[i], 1, COLUMNS, fp);
		}
	    fclose( fp );
					
		/* Write Gy to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-y.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
		for (i = 1; i < ROWS - 1; i++) {
			fwrite(yimage[i], 1, COLUMNS, fp);
		}
		fclose( fp );

		// Step 4: Compute squared gradient and normalize to [0, 255] by converting to float
		// Write your code here ... you can reuse 'ximage' to store values for SGM




		/* Write SGM to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-s.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );

		for (i = 0; i < ROWS; i++) {
			fwrite(sgm_image[i], 1, COLUMNS, fp);
		}

		fclose( fp );
		
		/* Compute the binary image */
		// Write your code here ... you can reuse 'ximage' to store values for binary image
		
		for (i = 1; i < ROWS - 1; i++) {
			for (j = 1; j < COLUMNS - 1; j++) {
				if (sgm_image[i][j] <= threshold[k]) {
					binary_image[i][j] = 0;
				}
				else {
					binary_image[i][j] = 255;
				}
			}
		}


		
		/* Write the binary image to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-b.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );

		for (i = 1; i < ROWS - 1; i++) {
			fwrite(binary_image[i], 1, COLUMNS, fp);
		}

		fclose( fp );
		printf( "%d %d %d\n", max[0], max[1], max[2] );

	}

	printf( "Press any key to exit: " );
	gets( &ch );
	return 0;
}

void clear( unsigned char image[][COLUMNS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLUMNS ; j++ ) image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
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
