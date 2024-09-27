#ifndef DEBUG_H
#define DEBUG_H

//#define DEBUG 
#ifdef DEBUG 
#define PRINT printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__)
#else
#define PRINT
#endif 

#endif 


