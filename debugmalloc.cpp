#include <stdio.h>
#include <stdlib.h>

#include <map>

#include <pthread.h>

#define xdebug(x...) do {printf("[debug][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)


#ifdef __cplusplus
extern "C" {
#endif 
void* MALLOC_DEBUG (int size, const char *szFile, int line, const char *szFunction);

void FREE_DEBUG (void *p, const char *szFile, int line, const char *szFunction);

void MALLOC_DUMP();
#ifdef __cplusplus
}
#endif 


typedef struct {
    std::string file;
    int line;
    std::string func;
    int size;
} MALLOC_INFO;

static std::map<void *, MALLOC_INFO> s_malloc_map;
static pthread_mutex_t s_mutex_malloc_map;

static void crash ()
{
    char *p = nullptr;
    *p = 0;
}

void* MALLOC_DEBUG (int size, const char *szFile, int line, const char *szFunction)
{
    void *p = malloc (size);
    
    MALLOC_INFO info;
    info.file = std::string (szFile);
    info.line = line;
    info.func = std::string (szFunction);
    info.size = size;

    pthread_mutex_lock (& s_mutex_malloc_map);
    // if (s_malloc_map.find (p) != s_malloc_map.cend())
    // {
    //     xdebug ("error, same pointer: [%p]\n", p);
    //     crash ();
    // }

    s_malloc_map[p] = info;

    pthread_mutex_unlock (& s_mutex_malloc_map);

    printf ("MLOC_DEBUG p=%p size=%d at [%s %d %s]\n", p, size, szFile, line, szFunction);
    return p;
}

void FREE_DEBUG (void *p, const char *szFile, int line, const char *szFunction)
{

    if (nullptr == p)
    {
        return ;
    }

    pthread_mutex_lock (& s_mutex_malloc_map);

    if (s_malloc_map.find (p) == s_malloc_map.cend())
    {
        xdebug ("error, free pointer[%p] not found\n", p);
        crash ();
    }

    s_malloc_map.erase (p);

    pthread_mutex_unlock (& s_mutex_malloc_map);

    free (p);

    printf ("FREE_DEBUG p=%p at [%s %d %s]\n", p, szFile, line, szFunction);
    return ;
}

static void dump_info (FILE* fp)
{
    fprintf (fp, "Dumping...\n");
    for (auto it = s_malloc_map.cbegin(); it != s_malloc_map.cend(); ++it )
    {
        fprintf (fp, "p=%p,info=[file=%s,line=%d,func=%s,size=%d]\n",
            it->first, it->second.file.c_str(), it->second.line, it->second.func.c_str(), it->second.size);
    }
    fprintf (fp, "\nEnd\n");
}

void MALLOC_DUMP()
{
    FILE *fp = fopen ("dump.txt", "a");
    if (fp)
    {
        dump_info (fp);
        fclose (fp);
        fp = NULL;
    }
}