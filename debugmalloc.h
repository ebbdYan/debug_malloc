
#ifndef DEBUGMALLOC_H__
#define DEBUGMALLOC_H__

#ifdef malloc
#undef malloc
#endif 
#define malloc(size) MALLOC_DEBUG(size, __FILE__,__LINE__,__FUNCTION__)

#ifdef free
#undef free
#endif 
#define free(p) FREE_DEBUG(p, __FILE__,__LINE__,__FUNCTION__)

#ifdef __cplusplus
extern "C" {
#endif 
void* MALLOC_DEBUG (int size, const char *szFile, int line, const char *szFunction);

void FREE_DEBUG (void *p, const char *szFile, int line, const char *szFunction);

void MALLOC_DUMP();
#ifdef __cplusplus
}
#endif 

#endif 