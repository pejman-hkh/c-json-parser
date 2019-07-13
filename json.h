#ifndef H_JSON
#define H_JSON 1

typedef struct json_array
{
	void **indexes; 
	void **values;
	int length;
	int type;

} json_array;

char *json_string;
int json_offset;
int json_length;

void json_array_init( json_array *arr );

void json_array_insert( json_array *arr, void *index, void *value );

void json_array_free( json_array *arr );

void *json_array_get( json_array *arr, void *index );

void dump1( json_array *arr );

void * json_parser( char *str );

void *parser();

#define JSON(json, str) json_array *json = json_parser( str )
#define JSONGET(json, index) json_array_get( json, index )
#define JSONGET1(object, json, index) json_array *object = json_array_get( json, index )
#define JSONFREE(json) json_array_free(json)
#endif