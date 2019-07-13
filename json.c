#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "json.h"

json_offset = 0;

void json_array_init( json_array *arr ) {
	arr->indexes = malloc( sizeof(json_array) );
	arr->values = malloc( sizeof(json_array) );
	arr->length = 0;
	arr->type = 0;
}

void json_array_insert( json_array *arr, void *index, void *value ) {
	void ** indexes = realloc(arr->indexes, sizeof(json_array) * (arr->length+2) );
	if(indexes) {
		arr->indexes = indexes;
	}

	void ** values = realloc(arr->values, sizeof(json_array) * (arr->length+2) );
	if(values) {
		arr->values = values;
	}

	arr->indexes[arr->length] = index;
	arr->values[arr->length] = value;
	arr->length++;
}

void json_array_free( json_array *arr ) {
	for( int i = 0; i < arr->length; i++ ) {
		json_array *l = arr->indexes[i];

		if( arr->type != 3 ) {
			free(l->values[0]);
			free(l->values);
			free(l->indexes);
			free(l);
		}

		json_array *v = arr->values[i];
		if( v->type == 2 || v->type == 3 ) {
			json_array_free( v );	
		} else {
			if( v->type == 1 || v->type == 6 ) {
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

void json_skip_space() {
	while( ( json_string[json_offset] == ' ' || json_string[json_offset] == '\n'  || json_string[json_offset] == '\t' ) && json_offset < json_length ) ++json_offset;
}

void *json_parse_str() {
	//printf("%d\n", sizeof(char *) );
	int bufSize = sizeof(char) * 100;
	char * buf = malloc(bufSize);
	int i = 0;

	while( !(json_string[json_offset] == '"' && json_string[json_offset-1] != '\\') && json_offset < json_length ) {
		if( i > bufSize ) {
			bufSize = bufSize * 2;
			buf = realloc( buf, bufSize );
		}

		if(json_string[json_offset] == '\\') {
		   switch(json_string[++json_offset]) {
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
					buf[i++] = json_string[json_offset];
			}
		} else {
			buf[i++] = json_string[json_offset++];  
		} 		
	}
	buf[i++] = '\0';

	json_offset++;
	json_skip_space();

	json_array *arr = malloc( sizeof(json_array ) );
	json_array_init( arr );
	arr->type = 1;
	json_array_insert( arr, "0", buf );

	//printf("Str is %s\n", buf );
	return (void *)arr;
}

void *json_parse_object() {
	json_array *arr = malloc( sizeof(json_array *) );
	json_array_init( arr );
	arr->type = 2;
	if( json_string[json_offset] == '}' ) {
		return arr;
	}	
	int i = 0;
	while( json_offset < json_length ) {

		void *index = parser();
		json_skip_space();

		if( json_string[json_offset++] != ':' ) {
			printf("Error expect : character \n" );
			exit(0);
		}

		void *val = parser();
		json_array_insert( arr, index, val );
		i++;
		json_skip_space();
		if( json_string[json_offset] == ',' ) {
			json_offset++;
			continue;
		} else if( json_string[json_offset] == '}' ) {
			json_offset++;
			return (void *)arr;
		} else {
			printf("Erorr expect , or } character \n");
			exit(0);
		}
	}
}

void *json_parse_array() {
	int i = 0;
	json_array *arr = malloc( sizeof(json_array *)  );
	json_array_init( arr );
	arr->type = 3;
	if( json_string[json_offset] == ']' ) {
		return arr;
	}

	while( json_offset < json_length ) {

		json_array_insert( arr, (void *)i, parser() );
		i++;
		json_skip_space();
		if( json_string[json_offset] == ',' ) {
			json_offset++;
			continue;
		} else if( json_string[json_offset] == ']' ) {
			json_offset++;
			return (void *)arr;
		} else {
			printf("Erorr expect , or ] character \n");
			exit(0);
		}
	}
}

void *json_parse_number() {

	char c;
	int isDbl = 0;
	
	char *expStr = malloc(sizeof(char *));
	int i = 0;
	char *val = malloc(sizeof(char *));
	while( 1 ) {
		c = json_string[ json_offset++ ];

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
		c = json_string[ json_offset++ ];
		if( c == '-' ){ ++json_offset; expStr[i++] = '-';}
		while( 1 ) {
			c = json_string[ json_offset++ ];
			if( c >= '0' && c <= '9' )
				expStr[i++]= c;
			else
				break;
		}
		expStr[i] = '\0';

		exp = strtol(expStr, &ptr, 10);
	}
	free(expStr);
	--json_offset;

	json_array *arr = malloc( sizeof(json_array *) );
	json_array_init( arr );
	arr->type = 4;

	void *ret;
	if( isDbl ) {
		arr->type = 6;
		double *d = malloc(sizeof(double *));
		sscanf(val, "%lf", d);
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
	free(val);
	json_array_insert( arr, "0", ret );
	return (void *)arr;
}

void json_get_str( char *strp, int end ) {

	int i = 0;
	while( json_offset < end ) {
		strp[i++] = json_string[json_offset++];
	}
	strp[i] = '\0';
}

void *json_parse_another() {
	json_array *arr = malloc( sizeof(json_array *) );
	json_array_init( arr );
	arr->type = 5;

	void *ret;
	char buf[6];
	switch( json_string[json_offset] ) {
		case 't' :
			json_get_str( buf, json_offset + 4 );
			if( strcmp( buf, "true" ) == 0 )
				ret = 1;
			break;	
		case 'f' :
			json_get_str( buf, json_offset + 5 );
			if( strcmp( buf, "false" ) == 0 )
				ret = 0;
			
			break;			  
		case 'n':
			json_get_str( buf, json_offset + 4 );
			if( strcmp( buf, "null" ) == 0 )
				ret = "";

			break;
		default:
			printf("Error\n");
			exit(0);
	}

	json_array_insert( arr, "0", ret );

	return (void *)arr;
}

void *parser() {
	json_skip_space();

	switch( json_string[json_offset] ) {
		case '{' :
			json_offset++;
			return json_parse_object();
			break;		
		case '[' :
			json_offset++;
			return json_parse_array();
			break;
		case '"' :
			json_offset++;
			return json_parse_str();
			break;
		case ']' :
			return json_parse_array();
			break;	
		case '}' :
			return json_parse_object();
			break;	
		default :

			if( ( json_string[json_offset] <= '9' && json_string[json_offset] >= '0' ) || json_string[json_offset] == '-' )
				return json_parse_number();
			else
				return json_parse_another();			
	}

}

void *json_array_get( json_array * arr, void *index ) {
	for( int i = 0; i < arr->length; i++ ) {
		json_array *l = arr->indexes[i];
		json_array * v = arr->values[i];
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

void dump1( json_array * arr ) {

	for( int i = 0; i < arr->length; i++ ) {
		json_array *l = arr->indexes[i];
		if( arr->type == 3 ) {
			printf("\n[%d : ", l);
		} else {
			printf("\n[%s : ", (char *)l->values[0]);
		}

		json_array *v = arr->values[i];
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
			} else if( v->type == 6 ) {
				double *a = (double *)v->values[0];
				printf("%lf", *a );
			} else
				printf("%s", (char *)v->values[0] );
			
			printf("]");
			

		} else {
			dump1( v );
			printf("]");
		}
	}
}


void * json_parser( char * str ) {
	json_string = str;
	json_length = strlen( str );
	return parser();
}