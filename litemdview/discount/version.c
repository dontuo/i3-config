#include "config.h"

#define VERSION "3"
char markdown_version[] = VERSION
#if 4 != 4
		" TAB=4"
#endif
#if USE_AMALLOC
		" DEBUG"
#endif
#if CHECKBOX_AS_INPUT
		" GHC=INPUT"
#else
		" GHC=ENTITY"
#endif
		;
