#include "tOS_Util.h"

void tOS_Error(tOS_String error) {
	printf("###ERROR: %s\n", error);
}

void tOS_Out(tOS_String str) {
	printf("%s\n", str);
}

tOS_Size max(tOS_Size a, tOS_Size b) {
	return a>b ? a : b;
}
