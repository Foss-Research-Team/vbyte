#include <stdio.h>
#include <stdlib.h>

FILE * encode_vbyte(FILE * in)
{
	fseek(in,0L,SEEK_END);
	
	const size_t in_size = ftell(fp);	

	unsigned char * input_arr = (unsigned char)calloc(in_size,sizeof(unsigned char));

	unsigned char * output_arr = (unsigned char)calloc(in_size,sizeof(unsigned char));

	unsigned char * input_arr_p = input_arr;
	
	unsigned char * output_arr_p = output_arr;

	if ( fread(input_arr, 1, in_size,in) <= 0 )
	{
		fprintf(stderr,"Error: Failed to read file into dynamically allocated array.");
	}	
	
	unsigned char previous = 0, delta = 0;
	
	size_t i = 0;
	
	while ( i < in_size )
	{
		delta = input_arr_p[i] - previous;
		while ( delta >= (0b1 << 8 ) )
		{
			*output_arr_p++ = (0b1 << 8) + ( delta & (0b1111111) );

			delta >>= 7;	

			delta &= 0b00000001;

			*output_arr_p = delta;
			previous = input_arr_p[i];	
		}					
		i++;
	}
	
	// Writing to FILE * out

	
}
