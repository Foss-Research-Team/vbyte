#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void encode_vbyte(uint8_t * list, size_t list_size, FILE * out)
{
	size_t element = 0, output_size = 0, i = 0;

	uint8_t byte_chunk = 0, prev = 0, next_byte = 0, output_byte = 0;
	
	while ( i < list_size )
	{
		element = list[i];

		byte_chunk = element & 0b01111111; // 7 ones

		next_byte = (element >>= 7) & 0b01111111;

		while ( next_byte > 0 )
		{
			output_byte = byte_chunk | (0b1 << 7 );

			fputc(output_byte,out);

			byte_chunk = next_byte;

			next_byte = (element >>= 7) & 0b01111111;
		}	
	
		fputc(byte_chunk,out);

		i++;
	}	
}


void decode_byte(uint8_t * input,size_t * list,size_t list_size)
{
	size_t element = 0, shift = 0;

	uint8_t byte = 0;

	while (*input != 0x0)
	{
		while ( *input &(0b1<<7) == (0b1 << 7) )
		{
			element |= ((*input) << shift);

			shift += 7;

			input++;
		}

		*list++ = element; shift = 0; element = 0;
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
	
	FILE * in = NULL, * out = NULL;

	uint8_t * output = NULL; size_t output_size = 0;

	uint8_t * input_list = NULL; size_t input_size = 0;	

	if ( ( in = fopen(argv[1],"rb+") ) == NULL )			
	{
		fprintf(stderr,"Error: Failed to open file %s\n",argv[1]);

		exit(1);
	}

	fseek(in,0L,SEEK_END);

	input_size = ftell(in);

	rewind(in);
		
	if ( ( out = fopen(output_name,"wb+") ) == NULL )			
	{
		fprintf(stderr,"Error: Failed to open file %s\n",output_name);

		exit(1);
	}

	input_list = (uint8_t *)calloc(input_size,sizeof(size_t));
	
	if ( fread(input_list,1,input_size,in) < input_size )
	{
		fprintf(stderr,"Error: Failed to read FILE in into input_list\n");

		exit(1);
	}	
	
	encode_vbyte(input_list,input_size,out);

	rewind(in);
	
	free(input_list);

	fseek(out,0L,SEEK_END);
	
	output_size = ftell(out);

	rewind(out);

	output = (uint8_t *)calloc(output_size,sizeof(uint8_t));
	
	if ( fread(output,1,output_size,out) < output_size )
	{
		fprintf(stderr,"Error: Failed to read encoded file into output array\n");
		exit(1);
	}
#if 0	
	output_list = (size_t *)calloc(input_size,sizeof(size_t));
		
	decode_byte(output,output_list,input_size);		
#endif
	rewind(out);

	if ( fclose(in) == EOF )
	{
		fprintf(stderr,"Error: Failed to close %s\n",argv[1]);
	}		
	
	if ( fclose(out) == EOF )
	{
		fprintf(stderr,"Error: Failed to close %s\n",output_name);
	}		


	free(output);

	return 0;
}
