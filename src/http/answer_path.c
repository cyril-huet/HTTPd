#define _XOPEN_SOURCE 700

#include "answer_path.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../config/config.h"
#include "http_structs.h"

#ifndef PATH_MAX
#    define PATH_MAX 4096
#endif

static int contains_parent_directory(const char *path)
{
    size_t index = 0;

    while (path[index] != '\0')
    {
        while (path[index] == '/')
        {
            index++;
        }

        size_t start = index;

        while (path[index] != '\0' && path[index] != '/')
        {
            index++;
        }

        if (index - start == 2 && path[start] == '.' && path[start + 1] == '.')
        {
            return 1;
        }
    }

    return 0;
}

static int build_path(const char *directory, const char *path, char *result)
{
    const char *relative_path = path;

    while (*relative_path == '/')
    {
        relative_path++;
    }

    int written;
    size_t length = strlen(directory);

    if (length > 0 && directory[length - 1] == '/')
    {
        written = snprintf(result, PATH_MAX, "%s%s", directory, relative_path);
    }
    else
    {
        written = snprintf(result, PATH_MAX, "%s/%s", directory, relative_path);
    }

    if (written < 0 || written >= PATH_MAX)
    {
        return -1;
    }

    return 0;
}

static int resolve_path(const char *path, char *result)
{
    if (realpath(path, result) != NULL)
    {
        return 200;
    }

    if (errno == EACCES)
    {
        return 403;
    }

    return 404;
}

static int path_is_inside_root(const char *root, const char *path)
{
    if (strcmp(root, "/") == 0)
    {
        return 1;
    }

    size_t root_length = strlen(root);

    if (strncmp(root, path, root_length) != 0)
    {
        return 0;
    }

    if (path[root_length] == '\0' || path[root_length] == '/')
    {
        return 1;
    }

    return 0;
}

static int resolve_default_file(struct config *config, const char *root,
                                char *path)
{
    const char *default_file = config->servers->default_file;

    if (default_file[0] == '/' || contains_parent_directory(default_file))
    {
        return 403;
    }

    char complete_path[PATH_MAX];

    if (build_path(path, default_file, complete_path) < 0)
    {
        return 404;
    }

    int status = resolve_path(complete_path, path);

    if (status == 200 && path_is_inside_root(root, path) == 0)
    {
        return 403;
    }

    return status;
}

static int add_default_file(struct config *config, const char *root, char *path)
{
    struct stat information;

    if (stat(path, &information) != 0)
    {
        return 404;
    }

    if (S_ISDIR(information.st_mode) == 0)
    {
        return 200;
    }

    return resolve_default_file(config, root, path);
}

static int open_file(struct answer_http *answer, const char *path)
{
    struct stat information;

    if (stat(path, &information) != 0)
    {
        return 404;
    }

    if (S_ISREG(information.st_mode) == 0)
    {
        return 403;
    }

    int file = open(path, O_RDONLY);

    if (file < 0)
    {
        return 403;
    }

    answer->file_fd = file;
    answer->content_length = information.st_size;
    answer->flag_content = 1;

    return 200;
}

static int resolve_request_path(struct request_http *request, const char *root,
                                char *resolved_path)
{
    if (request->path[0] != '/' || contains_parent_directory(request->path))
    {
        return 403;
    }

    char complete_path[PATH_MAX];

    if (build_path(root, request->path, complete_path) < 0)
    {
        return 404;
    }

    int status = resolve_path(complete_path, resolved_path);

    if (status == 200 && path_is_inside_root(root, resolved_path) == 0)
    {
        return 403;
    }

    return status;
}

int find_path(struct request_http *request, struct answer_http *answer,
              struct config *config)
{
    char root[PATH_MAX];

    if (realpath(config->servers->root_dir, root) == NULL)
    {
        return -1;
    }

    char resolved_path[PATH_MAX];
    int status = resolve_request_path(request, root, resolved_path);

    if (status != 200)
    {
        return status;
    }

    status = add_default_file(config, root, resolved_path);

    if (status != 200)
    {
        return status;
    }

    return open_file(answer, resolved_path);
}
