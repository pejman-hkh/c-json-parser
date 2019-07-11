#include <stdio.h>
#include <string.h>
#include "json.h"

int main(int argc, char const *argv[])
{
	char *str1 = "{\"peji\":\"123\\n\",\"slm\":{\"aaa\":{\"yyyy\":\"rrrrrrrrr\"}},\"p\":[\"ttt\",\"ddd\"]}";

	JSON(json, str1);

	jsonArray *peji = JSONGET(json, "peji");
	jsonArray *slm = JSONGET(json, "slm");
	jsonArray *aaa = JSONGET(slm, "aaa");
	jsonArray *yyyy = JSONGET(aaa, "yyyy");

	dump1( json );
	printf("%s\n", peji);	
	printf("%s\n", yyyy);	
	JSONFREE(json);
/*	printf("%s\n", JSONGET(slm, "aaa") );
	printf("%s\n", yyyy);
	printf("%s\n", peji);	
*/
	return 0;
}