#include "answer_path.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../config/config.h"
#include "http_structs.h"

int find_path(struct request_http *request, struct answer_http *answer,
              struct config *config)
{
    int flag = 1;
    size_t index = 0;
    while (config->servers->root_dir[index] != '\0')
    {
        if (config->servers->root_dir[index + 1] == '\0'
            && config->servers->root_dir[index] == '/')
        {
            flag = 0;
        }
        index++;
    }
    char *res_path = malloc(strlen(config->servers->root_dir)
                            + strlen(request->path) + flag + 1);
    if (res_path == NULL)
    {
        return -1;
    }
    size_t index_res_path = 0;
    for (size_t i = 0; i < strlen(config->servers->root_dir); i++)
    {
        res_path[index_res_path] = config->servers->root_dir[i];
        index_res_path++;
    }
    if (flag == 1)
    {
        res_path[index_res_path] = '/';
        index_res_path++;
    }
    for (size_t i = 0; i < strlen(request->path); i++)
    {
        res_path[index_res_path] = request->path[i];
        index_res_path++;
    }
    res_path[index_res_path] = '\0';
    struct stat st;
    if (stat(res_path, &st) != 0)
    {
        free(res_path);
        return 404;
    }
    if (S_ISDIR(st.st_mode) != 0)
    {
        size_t len_path = strlen(res_path);
        if (res_path[len_path - 1] != '/')
        {
            char *temps = realloc(res_path, len_path + 2);
            if (temps == NULL)
            {
                free(res_path);
                return -1;
            }
            res_path = temps;
            res_path[len_path] = '/';
            len_path++;
            res_path[len_path] = '\0';
        }
        size_t len_default_file = strlen(config->servers->default_file);
        char *tmp = realloc(res_path, len_path + len_default_file + 1);
        if (tmp == NULL)
        {
            free(res_path);
            return -1;
        }
        res_path = tmp;

        for (size_t i = 0; i < len_default_file; i++)
        {
            res_path[len_path + i] = config->servers->default_file[i];
        }
        res_path[len_path + len_default_file] = '\0';
        if (stat(res_path, &st) != 0)
        {
            free(res_path);
            return 404;
        }
    }
    if (S_ISREG(st.st_mode) == 0)
    {
        free(res_path);
        return 403;
    }
    int fd = open(res_path, O_RDONLY);
    if (fd < 0)
    {
        free(res_path);
        return 403;
    }
    answer->file_fd = fd;
    answer->content_length = st.st_size;
    answer->flag_content = 1;
    free(res_path);
    return 200;
}
