#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#define MAXFILENAME	100
#define MAXFILESIZE	1048576
	
int main( int argc, char *argv[] )
{
	int fd, fd_out, s;
	int filesize;
	int i;
	char * b1, * b2;
	double sum, corr;
	int counts, ccounts;

	char response_filename[MAXFILENAME];

	if(argc < 2)
	{
		printf("Finds correlation between file and file-response.\nUsage: %s finename\n", argv[0]);
		exit(1);
	}


	struct stat file_stat;
	strncpy( response_filename, argv[1], MAXFILENAME - 10 );
	strncat( response_filename, "-response", 10 );
	fd = open( argv[1], O_RDONLY);
	if( fd < 0 )
	{
		perror("Error opening input file\n");
		exit(1);
	}

	// Get the size of the input file
	if ( fstat( fd, &file_stat) < 0) {
		perror("Unable to get file statistics");
		exit(1);
	}
	filesize = file_stat.st_size;
	if ( filesize > MAXFILESIZE )
	{
		printf("Error: File %s is too large!\nExiting...\n", argv[1]);
		return 10;
	}

	fd_out = open( response_filename, O_RDONLY );
	if( fd_out < 0 )
	{
		perror("Error opening output file\n");
		exit(1);
	}
	b1 = malloc(filesize);
	b2 = malloc(filesize);
	if( b1 == NULL || b2 == NULL )
	{
		printf("Error: cannot allocate memory!\n");
		return 9;
	}
	s = read(fd, b1, filesize);
	if( s != filesize )
	{
		printf("Error: Couldn't read %d bytes from file %s\nExiting...\n", filesize, argv[1]);
		return 11;
	}
	s = read(fd_out, b2, filesize);
	if( s != filesize )
	{
		printf("Error: Couldn't read %d bytes from file %s\nExiting...\n", filesize, response_filename);
		return 12;
	}
	sum = 0;
	counts = 0;
	ccounts = 0;
	for( i = 0; i < filesize; i++ )
	{
		if( b2[i] & 0x1 )
		{
			counts++;
			if(b1[i] & 0x1)
			{
				if(b1[i] & 0x8)
					sum += 1.0;
				else
					sum -= 1.0;
			}
			else
			{
				if(b1[i] & 0x8)
					sum -= 1.0;
				else
					sum += 1.0;
			}
		}
		if( b2[i] & 0x2 )
			ccounts++;
	}
	corr = sum/(double)counts;
		printf( "Correlation %.3f from %d counts, classic counts %d (%d missing) - %s\n", corr, counts, ccounts, filesize - ccounts, argv[1]);

	return 0;
}




