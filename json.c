#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "json.h"

jsonOffset = 0;

void jsonArrayInit( jsonArray *arr ) {
	arr->indexes = malloc( sizeof(jsonArray) );
	arr->values = malloc( sizeof(jsonArray) );
	arr->length = 0;
	arr->type = 0;
}

void jsonArrayInsert( jsonArray *arr, void *index, void *value ) {
	void ** indexes = realloc(arr->indexes, sizeof(jsonArray) * (arr->length+2) );
	if(indexes) {
		arr->indexes = indexes;
	}

	void ** values = realloc(arr->values, sizeof(jsonArray) * (arr->length+2) );
	if(values) {
		arr->values = values;
	}

	arr->indexes[arr->length] = index;
	arr->values[arr->length] = value;
	arr->length++;
}

void jsonArrayFree( jsonArray *arr ) {
	for( int i = 0; i < arr->length; i++ ) {
		jsonArray *l = arr->indexes[i];

		if( arr->type != 3 ) {
			free(l->values[0]);
			free(l->values);
			free(l->indexes);
			free(l);
		}

		jsonArray *v = arr->values[i];
		if( v->type == 2 || v->type == 3 ) {
			jsonArrayFree( v );	
		} else {
			if( v->type == 1 ) {
				free( v->values[0] );
			}
			free(v->indexes);
			free(v->values);
			free(v);
		}
	}

	free(arr->indexes);
	free(arr->values);
	free(arr);
}

void jsonSkipSpace() {
	while( ( jsonStr[jsonOffset] == ' ' || jsonStr[jsonOffset] == '\n'  || jsonStr[jsonOffset] == '\t' ) && jsonOffset < jsonLength ) ++jsonOffset;
}

void *parseStr() {
	char * buf = malloc(sizeof(char *) * 2048);
	int i = 0;

	while( !(jsonStr[jsonOffset] == '"' && jsonStr[jsonOffset-1] != '\\') && jsonOffset < jsonLength ) {
	
		if(jsonStr[jsonOffset] == '\\') {
		   switch(jsonStr[++jsonOffset]) {
				case 'a': buf[i++] = '\a'; break;
				case 'b': buf[i++] = '\b'; break;
				case 'f': buf[i++] = '\f'; break;
				case 'n': buf[i++] = '\n'; break;
				case 'r': buf[i++] = '\r'; break;
				case 't': buf[i++] = '\t'; break;
				case 'v': buf[i++] = '\v'; break;
				case '\'': buf[i++] = '\''; break;
				case '\"': buf[i++] = '\"'; break;
				default:
					buf[i++] = '\\';
					buf[i++] = jsonStr[jsonOffset];
			}
		} else {
			buf[i++] = jsonStr[jsonOffset++];  
		} 		
	}
	buf[i++] = '\0';

	jsonOffset++;
	jsonSkipSpace();

	jsonArray *arr = malloc( sizeof(jsonArray ) );
	jsonArrayInit( arr );
	arr->type = 1;
	jsonArrayInsert( arr, "0", buf );

	//printf("Str is %s\n", buf );
	return (void *)arr;
}

void *parseObject() {
	jsonArray *arr = malloc( sizeof(jsonArray *) );
	jsonArrayInit( arr );
	arr->type = 2;
	int i = 0;
	while( jsonOffset < jsonLength ) {

		void *index = parser();
		jsonSkipSpace();

		if( jsonStr[jsonOffset++] != ':' ) {
			printf("Error expect : character \n" );
			exit(0);
		}

		void *val = parser();
		jsonArrayInsert( arr, index, val );
		i++;
		jsonSkipSpace();
		if( jsonStr[jsonOffset] == ',' ) {
			jsonOffset++;
			continue;
		} else if( jsonStr[jsonOffset] == '}' ) {
			jsonOffset++;
			return (void *)arr;
		} else {
			printf("Erorr expect , or } character \n");
			exit(0);
		}
	}
}

void *parseArray() {
	int i = 0;
	jsonArray *arr = malloc( sizeof(jsonArray *)  );
	jsonArrayInit( arr );
	arr->type = 3;
	while( jsonOffset < jsonLength ) {

		jsonArrayInsert( arr, (void *)i, parser() );
		i++;
		jsonSkipSpace();
		if( jsonStr[jsonOffset] == ',' ) {
			jsonOffset++;
			continue;
		} else if( jsonStr[jsonOffset] == ']' ) {
			jsonOffset++;
			return (void *)arr;
		} else {
			printf("Erorr expect , or ] character \n");
			exit(0);
		}
	}
}

void *parseNumber() {

	char c;
	int isDbl = 0;
	
	char *expStr = malloc(sizeof(char *));
	int i = 0;
	char *val = malloc(sizeof(char *));
	while( 1 ) {
		c = jsonStr[ jsonOffset++ ];

		if( (c == '-') || (c >= '0' && c <= '9') )
			val[i++] = c;
		else if( c == '.' ) {
			val[i++] = c; 
			isDbl = 1;
		}
		else
			break;
	}
	val[i] = '\0';
	char *ptr;
	i = 0;
	long exp = 0;
	if( c == 'E' || c == 'e' ) {
		c = jsonStr[ jsonOffset++ ];
		if( c == '-' ){ ++jsonOffset; expStr[i++] = '-';}
		while( 1 ) {
			c = jsonStr[ jsonOffset++ ];
			if( c >= '0' && c <= '9' )
				expStr[i++]= c;
			else
				break;
		}
		expStr[i] = '\0';

		exp = strtol(expStr, &ptr, 10);
	}
	--jsonOffset;

	jsonArray *arr = malloc( sizeof(jsonArray *) );
	jsonArrayInit( arr );
	arr->type = 4;

	void *ret;
	if( isDbl ) {
		double *d;
		sscanf(val, "%lf", d);
		//printf("%lf\n", d);
		*d = (*d) * pow( 10, exp );
		ret = (void *)d;
		
	}
	else {
		if( exp ) {
			long d = strtol(val, &ptr, 10) * pow( 10, exp );
			ret = (void *)d;
		}
		else {

			char *ptr1;
			int d = strtol(val, &ptr1, 10);
			ret = (void *)d;
		}
	}

	jsonArrayInsert( arr, "0", ret );
	return (void *)arr;
}

void getStr( char *strp, int end ) {

	int i = 0;
	while( jsonOffset < end ) {
		strp[i++] = jsonStr[jsonOffset++];
	}
	strp[i] = '\0';
}

void *parseAnother() {
	jsonArray *arr = malloc( sizeof(jsonArray *) );
	jsonArrayInit( arr );
	arr->type = 5;

	void *ret;
	char buf[6];
	switch( jsonStr[jsonOffset] ) {
		case 't' :
			getStr( buf, jsonOffset + 4 );
			if( strcmp( buf, "true" ) == 0 )
				ret = 1;
			break;	
		case 'f' :
			getStr( buf, jsonOffset + 5 );
			if( strcmp( buf, "false" ) == 0 )
				ret = 0;
			
			break;			  
		case 'n':
			getStr( buf, jsonOffset + 4 );
			if( strcmp( buf, "null" ) == 0 )
				ret = "";

			break;
		default:
			printf("Error\n");
			exit(0);
	}

	jsonArrayInsert( arr, "0", ret );

	return (void *)arr;
}

void *parser() {
	jsonSkipSpace();

	switch( jsonStr[jsonOffset] ) {
		case '{' :
			jsonOffset++;
			return parseObject();
			break;		
		case '[' :
			jsonOffset++;
			return parseArray();
			break;
		case '"' :
			jsonOffset++;
			return parseStr();
			break;
		default :

			if( ( jsonStr[jsonOffset] <= '9' && jsonStr[jsonOffset] >= '0' ) || jsonStr[jsonOffset] == '-' )
				return parseNumber();
			else
				return parseAnother();			
	}

}

void *jsonArrayGet( jsonArray * arr, void *index ) {
	for( int i = 0; i < arr->length; i++ ) {
		jsonArray *l = arr->indexes[i];
		jsonArray * v = arr->values[i];
		if( arr->type == 3 ) {

			if( l == index ) {
				if( v->type == 1 ) {
					return v->values[0];
				} else {
					return v;
				}
			}
		} else {
			if( strcmp((char *)l->values[0], index ) == 0 ) {
				if( v->type != 2 && v->type != 3 ) {
					return v->values[0];
				} else {
					return v;
				}
			}
		}
	}

	return (void *)"";
}

void dump1( jsonArray * arr ) {

	for( int i = 0; i < arr->length; i++ ) {
		jsonArray *l = arr->indexes[i];
		if( arr->type == 3 ) {
			printf("\n[%d : ", l);
		} else {
			printf("\n[%s : ", (char *)l->values[0]);
		}

		jsonArray *v = arr->values[i];
		if( v->type != 2 && v->type != 3 ) {
			if( v->type == 5 ) {
				if( v->values[0] == 0 ) {
					printf("false");
				} else if( v->values[0] == 1 ) {
					printf("true");
				} else {
					printf("null");
				}

			} else if( v->type == 4 ) {
				printf("%ld", (long)v->values[0]);
			}
			else
				printf("%s", (char *)v->values[0] );
			
			printf("]");
			

		} else {
			dump1( v );
			printf("]");
		}
	}
}


void * jsonParser( char * str ) {
	jsonStr = str;
	jsonLength = strlen( str );
	return parser();
}