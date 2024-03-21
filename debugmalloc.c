//#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <map>

#include <pthread.h>

#define xdebug(x...) do {printf("[debug][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)


#ifdef __cplusplus
extern "C" {
#endif 
#define FILE_LEN 128
#define FUNC_LEN 64

void* MALLOC_DEBUG (int size, const char *szFile, int line, const char *szFunction);

void FREE_DEBUG (void *p, const char *szFile, int line, const char *szFunction);

void MALLOC_DUMP();
#ifdef __cplusplus
}
#endif 

typedef struct MALLOC{
    //std::string file;
    char file[FILE_LEN];
    int line;
    //std::string func;
    char func[FUNC_LEN];
    int size;
    void *pointer;
    struct MALLOC* next;
} MALLOC_INFO;

MALLOC_INFO* head = NULL;

//static std::map<void *, MALLOC_INFO> s_malloc_map;
static pthread_mutex_t s_mutex_malloc_map;

static void crash ()
{
    char *p = NULL;
    *p = 0;
}

// 插入节点函数
void insertNode(MALLOC_INFO* data) {
    MALLOC_INFO* newNode = (MALLOC_INFO*)malloc(sizeof(MALLOC_INFO));
    memset(newNode, 0, sizeof(MALLOC_INFO));
    strncpy(newNode->file, data->file, FILE_LEN-1);
    newNode->line = data->line;
    strncpy(newNode->func, data->func, FUNC_LEN-1);
    newNode->size = data->size;
    newNode->pointer = data->pointer;


    newNode->next = head;
    head = newNode;
}

// 删除节点函数
void deleteNode(void *p) {
    int found = 0;
    MALLOC_INFO *current = head;
    MALLOC_INFO *previous = NULL;

    while (current != NULL) {
        if (current->pointer == p) {
            found = 1;
            if (previous == NULL) {
                head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            current = current->next;
        } else {
            previous = current;
            current = current->next;
        }
    }
    if (!found) {   //链表中未找到节点指针
        xdebug ("error, free pointer[%p] not found\n", p);
        crash ();
    }
    
}

void* MALLOC_DEBUG (int size, const char *szFile, int line, const char *szFunction)
{
    void *p = malloc (size);
    if (p == NULL) {
        printf("malloc fail, file: %s, func: %s, line: %d", szFile, szFunction, line);
        //kill(getpid(), SIGTERM);
        return NULL;
    }
    //memset(p, 0, size);
    MALLOC_INFO info;
    strncpy(info.file, szFile, FILE_LEN-1);
    info.line = line;
    strncpy(info.func, szFunction, FUNC_LEN-1);
    info.size = size;
    info.pointer = p;

    pthread_mutex_lock (& s_mutex_malloc_map);
    // if (s_malloc_map.find (p) != s_malloc_map.cend())
    // {
    //     xdebug ("error, same pointer: [%p]\n", p);
    //     crash ();
    // }
    insertNode(&info);
    //s_malloc_map[p] = info;

    pthread_mutex_unlock (& s_mutex_malloc_map);

    printf ("MLOC_DEBUG p=%p size=%d at [%s %d %s]\n", p, size, szFile, line, szFunction);
    return p;
}

void FREE_DEBUG (void *p, const char *szFile, int line, const char *szFunction)
{

    if (NULL == p)
    {
        return ;
    }

    pthread_mutex_lock (& s_mutex_malloc_map);

    deleteNode(p);

    pthread_mutex_unlock (& s_mutex_malloc_map);

    free (p);

    printf ("FREE_DEBUG p=%p at [%s %d %s]\n", p, szFile, line, szFunction);
    return ;
}

static int dump_info (FILE* fp)
{
    long current_size;
    long file_size_limit = 1024 * 1024; // 1M 文件大小限制
    fprintf (fp, "Dumping...\n");
    while (head != NULL){
        fprintf (fp, "p=%p,info=[file=%s,line=%d,func=%s,size=%d]\n",
            head->pointer, head->file, head->line, head->func, head->size);
        head = head->next;
        
        
        fseek(fp, 0, SEEK_END); // 定位到文件末尾
        current_size = ftell(fp); // 获取当前文件大小
        if (current_size >= file_size_limit) {
            printf("文件大小已达到1M限制\n");
            fclose(fp);
            fp = NULL;
            return 1;
        }
    }
    fprintf (fp, "\nEnd\n");
    return 0;
}

void MALLOC_DUMP()
{
    int ret;
    FILE *fp = fopen ("dump.txt", "a+");

    if (fp)
    {
        ret = dump_info (fp);
        if(!ret)
        {
            fclose (fp);
            fp = NULL;
        }     
    }
}