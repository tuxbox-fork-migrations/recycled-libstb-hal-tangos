#if HAVE_TRIPLEDRAGON
#include "../libtriple/record_td.h"
#elif HAVE_SPARK_HARDWARE
#include "../libspark/record_lib.h"
#else
#error neither HAVE_TRIPLEDRAGON nor HAVE_SPARK_HARDWARE defined
#endif
