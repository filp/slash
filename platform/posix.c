#define _POSIX_SOURCE
#define _BSD_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <slash/platform.h>

char*
sl_realpath(sl_vm_t* vm, char* path)
{
    char *cpath, *gcbuff;
    if(path[0] != '/') {
        gcbuff = sl_alloc_buffer(vm->arena, strlen(vm->cwd) + strlen(path) + 10);
        strcpy(gcbuff, vm->cwd);
        strcat(gcbuff, "/");
        strcat(gcbuff, path);
        path = gcbuff;
    }
    #ifdef PATH_MAX
        cpath = sl_alloc_buffer(vm->arena, PATH_MAX + 1);
        (void)realpath(path, cpath);
        return cpath;
    #else
        cpath = realpath(path, NULL);
        gcbuff = sl_alloc_buffer(vm->arena, strlen(cpath) + 1);
        strcpy(gcbuff, cpath);
        return gcbuff;
    #endif
}

int
sl_file_exists(sl_vm_t* vm, char* path)
{
    struct stat s;
    return !stat(sl_realpath(vm, path), &s);
}

sl_file_type_t
sl_file_type(struct sl_vm* vm, char* path)
{
    struct stat s;
    if(stat(sl_realpath(vm, path), &s)) {
        return SL_FT_NO_EXIST;
    }
    if(S_ISDIR(s.st_mode)) {
        return SL_FT_DIR;
    } else {
        return SL_FT_FILE;
    }
}

int sl_abs_file_exists(char* path)
{
    struct stat s;
    if(path[0] != '/') {
        return 0;
    }
    return !stat(path, &s);
}

int sl_seed()
{
    struct timeval a;
    struct stat s;
    FILE* f;
    int seed;
    if(!stat("/dev/urandom", &s)) {
        f = fopen("/dev/urandom", "rb");
        if(f) {
            int success = fread(&seed, sizeof(int), 1, f);
            fclose(f);
            if(success) {
                return seed;
            }
        }
    }
    gettimeofday(&a, NULL);
    return (int)(a.tv_usec ^ a.tv_sec);
}

#ifndef __APPLE__
char**
sl_environ(struct sl_vm* vm)
{
    return __environ;
    (void)vm;
}
#endif
