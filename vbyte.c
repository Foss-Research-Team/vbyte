#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void encode_vbyte(FILE * in,FILE * out)
{
	fseek(in,0L,SEEK_END);
	
	const size_t in_size = ftell(in);	
	rewind(in);

	unsigned char * input_arr = (unsigned char *)calloc(in_size,sizeof(unsigned char));

	unsigned char * output_arr = (unsigned char *)calloc(in_size,sizeof(unsigned char));

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
		delta = *input_arr_p - previous;
		while ( delta >= (0b1 << 8 ) )
		{
			*output_arr_p++ = (0b1 << 8) + ( delta & (0b1111111) );

			delta >>= 7;	

			delta &= 0b00000001;

			*output_arr_p++ = delta;
			previous = *input_arr_p++;	
		}					
		i++;
	}
	
	// Writing to FILE * out

	i = 0;

	output_arr_p = output_arr;

	while ( i < in_size )
	{
		fputc(*output_arr_p,out);
		
		output_arr_p++;		

		i++;
	}	
	
}

int main(int argc,char*argv[])
{
	if ( argc < 2 )
	{
		fprintf(stderr,"Error: Missing input file\n");
	
	}

	static unsigned char output_name[1024];

	strncat(output_name,argv[1],1024);

	strncat(output_name,".vb",1024-strnlen(argv[1],1024));

	printf("%s\n",output_name);
	
	FILE * in = NULL, *out = NULL;

	if ( ( in  = fopen(argv[1],"rb+") ) == NULL )			
	{
		fprintf(stderr,"Error: Failed to open file %s\n",argv[1]);
	}
		
	if ( ( out  = fopen(output_name,"wb+") ) == NULL )			
	{
		fprintf(stderr,"Error: Failed to open file %s\n",output_name);
	}
	
	encode_vbyte(in,out);

	if ( fclose(in) == EOF )
	{
		fprintf(stderr,"Error: Failed to close %s\n",argv[1]);
	}		
	
	if ( fclose(out) == EOF )
	{
		fprintf(stderr,"Error: Failed to close %s\n",output_name);
	}		

	return 0;
}
