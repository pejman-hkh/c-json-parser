#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json.h"

jsonOffset = 0;

void jsonArrayInit( jsonArray *arr ) {
	arr->indexes = malloc( sizeof(jsonArray) );
	arr->values = malloc( sizeof(jsonArray) );
	arr->length = 0;
	arr->type = 0;
}

void jsonArrayInsert( jsonArray *arr, void *index, void *value ) {

	void ** indexes = realloc(arr->indexes, sizeof(jsonArray) * (arr->length+1) );
	if(indexes) {
		arr->indexes = indexes;
	}

	void ** values = realloc(arr->values, sizeof(jsonArray) * (arr->length+1) );
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
		}

		jsonArray *v = arr->values[i];
		if( v->type == 1 ) {
			free( v->values[0] );
		} else {
			jsonArrayFree( v );
		}
	}

	free(arr->indexes);
	free(arr->values);
	free(arr);
}

void jsonSkipSpace() {
	while( jsonStr[jsonOffset] == ' ' ) ++jsonOffset;
}

void *parseStr() {
	char * buf = malloc(sizeof(char *));
	int i = 0;

	while( !(jsonStr[jsonOffset] == '"' && jsonStr[jsonOffset-1] != '\\') ) {
	
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

	jsonArray *arr = malloc( sizeof(jsonArray *) );
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
	while( 1 ) {

		void *index = parser();
		jsonSkipSpace();
		jsonOffset++;
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
			break;
		}
	}
}

void *parseArray() {
	int i = 0;
	jsonArray *arr = malloc( sizeof(jsonArray *)  );
	jsonArrayInit( arr );
	arr->type = 3;
	while( 1 ) {

		jsonArrayInsert( arr, i, parser() );
		i++;
		jsonSkipSpace();
		if( jsonStr[jsonOffset] == ',' ) {
			jsonOffset++;
			continue;
		} else if( jsonStr[jsonOffset] == ']' ) {
			jsonOffset++;
			return (void *)arr;
		} else {
			break;
		}
	}
}

void *parser() {
	jsonSkipSpace();
	//printf("%c\n", jsonStr[jsonOffset]);
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
				if( v->type == 1 ) {
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
		if( v->type == 1 ) {
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
	return parser();
}