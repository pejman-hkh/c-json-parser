#ifndef H_JSON
#define H_JSON 1

typedef struct jsonArray
{
	void **indexes; 
	void **values;
	int length;
	int type;

} jsonArray;

char *jsonStr;
int jsonOffset;

void jsonArrayInit( jsonArray *arr );

void jsonArrayInsert( jsonArray *arr, void *index, void *value );

void jsonArrayFree( jsonArray *arr );

void *parser();

void *parseStr();

void *parseObject();

void *parseArray();

void *parser();

void *jsonArrayGet( jsonArray * arr, void *index );

void dump1( jsonArray * arr );

void * jsonParser( char * str );

#define JSON(json, str) jsonArray *json = jsonParser( str )
#define JSONGET(json, index) jsonArrayGet( json, index )
#define JSONFREE(json) jsonArrayFree(json)
#endif