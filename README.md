# c-json-parser
C json parser

# Sample Code
```c
char *str1 = fileGetContent("test.txt");

JSON(json, str1);

dump1( json );

json_array *object = JSONGET( json, "object");

JSONFREE(json);
free( str1 );
```

# Sample dump output
```
[boolean : true]
[color : #82b92c]
[null : null]
[number : 123]
[object : 
[a : b]
[c : d]
[e : f]]
[string : HelloWorld]
[array : 
[0 : 1]
[1 : 2]
[2 : 3]]
[boolean1 : true]
[color1 : #82b92c]
[null1 : null]
[number1 : 123]
[object1 : 
[a : b]
[c : d]
[e : f]]
[string1 : HelloWorld]
[array1 : 
[0 : 1]
[1 : 2]
[2 : 3]]
```