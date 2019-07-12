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

	dump1( json );

	jsonArray *object = JSONGET( json, "object");

	JSONFREE(json);
	free( str1 );
	//printf("%s\n", JSONGET( object, "a" ) );

	return 0;
}