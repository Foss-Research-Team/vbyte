#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void encode_byte(FILE * in,FILE * out)
{
	fseek(in,0L,SEEK_END);
	
	const size_t in_size = ftell(in);	
		
	rewind(in);

	unsigned char * input_arr = (unsigned char *)calloc(in_size,sizeof(unsigned char));

	unsigned char * output_arr = (unsigned char *)calloc(in_size,sizeof(unsigned char));

	unsigned char * input_arr_p = input_arr;
	
	unsigned char * output_arr_p = output_arr;

	if ( fread(input_arr,sizeof(unsigned char), in_size,in) <= 0 )
	{
		fprintf(stderr,"Error: Failed to read file into dynamically allocated array.");
	}	
	
	unsigned char previous = 0, delta = 0;
	
	size_t i = 0;
	
	while ( i < in_size )
	{
		delta = *input_arr_p - previous;
		
		while ( delta >= (0b1 << 7 ) )
		{
			*output_arr_p++ = (0b1 << 7) + ( delta & (0b01111111) );
			
			delta >>= 7;	

			delta &= 0b00000001;

		}					
		
		*output_arr_p++ = delta;
		
		previous = *input_arr_p++;
		
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
	
	free(input_arr);

	free(output_arr);	
}

void decode_byte(FILE * in,FILE * out)
{

	fseek(in,0L,SEEK_END);

	const size_t in_size = ftell(in);
	
	static unsigned char * input_arr;

	unsigned char * input_arr_p = input_arr;
	
	input_arr_p = (unsigned char *)calloc(in_size,sizeof(unsigned char));

	unsigned char * output_arr = (unsigned char *)calloc(in_size,sizeof(unsigned char));
	
	unsigned char * output_arr_p = output_arr;
	
	rewind(in);	

	if ( fread(input_arr,sizeof(unsigned char), in_size,in) <= 0 )
	{
		fprintf(stderr,"Error: Failed to read file into dynamically allocated array.");
	}	
	
	unsigned char current = 0;
	
	unsigned char b = 0;

	size_t i = 0;

	size_t shift = 0;

	while ( i < in_size )
	{
		shift = 0;

		b = *input_arr_p++;

		while ( b >= (0b1 << 7) )
		{
			current = current + ( ( b & 0b01111111 ) << shift ); 	

			shift += 7;

			b = *input_arr_p++;
		}			

		current += (b << shift);
		
		*output_arr_p++ = current;

		i++;
	}

	i = 0;

	output_arr_p = output_arr;

	while ( i < in_size )
	{
		fputc(*output_arr_p,out);

		output_arr_p++;

		i++;
	}

	free(input_arr);
	
	free(output_arr);
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
	
	FILE * in = NULL, * out = NULL, * decompressed_output = NULL;

	if ( ( in = fopen(argv[1],"rb+") ) == NULL )			
	{
		fprintf(stderr,"Error: Failed to open file %s\n",argv[1]);
	}
		
	if ( ( out = fopen(output_name,"wb+") ) == NULL )			
	{
		fprintf(stderr,"Error: Failed to open file %s\n",output_name);
	}
	
	if ( ( decompressed_output = fopen("decompressed_output.txt","wb+") ) == NULL )			
	{
		fprintf(stderr,"Error: Failed to open file %s\n","decompressed_output_txt");
	}
	
	encode_byte(in,out);

	if ( fclose(out) == EOF )
	{
		fprintf(stderr,"Error: Failed to close %s\n",output_name);
	}		
	
	if ( ( out = fopen(output_name,"rb+") ) == NULL )			
	{
		fprintf(stderr,"Error: Failed to open file %s\n",output_name);
	}
		

	decode_byte(out,decompressed_output);		

	if ( fclose(in) == EOF )
	{
		fprintf(stderr,"Error: Failed to close %s\n",argv[1]);
	}		
	
	if ( fclose(out) == EOF )
	{
		fprintf(stderr,"Error: Failed to close %s\n",output_name);
	}		

	if ( fclose(decompressed_output) == EOF )
	{
		fprintf(stderr,"Error: Failed to close %s\n","decompressed_output.txt");
	}		
	
	return 0;
}
