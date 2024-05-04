#include "structs.h"
#include "pds.h"
#include <string.h>

int matcher(void *rec, void *key){
	struct Shop * temp = rec;
	char * k = key;
	if(strcmp(k,temp->name)) return 1;
    return 0;
}