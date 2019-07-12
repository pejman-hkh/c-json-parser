#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json.h"

char *fileGetContent( char *fname) {
	char * buffer = 0;
	long length;
	FILE * f = fopen (fname, "rb");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = malloc (length);
	  if (buffer)
	  {
	    fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}

	if (buffer)
	{
		return buffer;
	}	
}

int main(int argc, char const *argv[])
{

	char *str1 = fileGetContent("test.txt");

	JSON(json, str1);
	printf("dump :");
	dump1( json );

	printf("\n\n");
	int number = JSONGET( json, "number");
	printf("%d\n", number);

	char *string = JSONGET( json, "string");
	printf("%s\n", string);

	int boolean = JSONGET( json, "boolean");
	printf("%d\n", boolean);

	int boolean1 = JSONGET( json, "boolean1");
	printf("%d\n", boolean1);

	char * null = JSONGET( json, "null");
	printf("%s\n", null);

	JSONFREE(json);
	free( str1 );

	return 0;
}