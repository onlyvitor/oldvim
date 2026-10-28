/* Feature-test macro: must be defined before any system header. */
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fileio.h"

static char *dup_string(const char *s)
{
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);

    if (copy == NULL)
    {
        return NULL;
    }

    memcpy(copy, s, len);

    return copy;
}

static char *make_temp_path(const char *path)
{
    static const char suffix[] = ".tmp.XXXXXX";
    size_t len = strlen(path);

    char *tmp = malloc(len + sizeof(suffix));

    if (tmp == NULL)
    {
        return NULL;
    }

    memcpy(tmp, path, len);
    memcpy(tmp + len, suffix, sizeof(suffix));

    return tmp;
}

static int write_all(int fd, const char *data, size_t len)
{
    size_t written = 0;

    while (written < len)
    {
        ssize_t rc = write(fd, data + written, len - written);

        if (rc < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            return -1;
        }

        written += (size_t)rc;
    }

    return 0;
}

static void fsync_parent_dir(const char *path)
{
    char *copy = dup_string(path);

    if (copy == NULL)
    {
        return;
    }

    char *slash = strrchr(copy, '/');
    const char *dir;

    if (slash == NULL)
    {
        dir = ".";
    }
    else if (slash == copy)
    {
        slash[1] = '\0';
        dir = copy;
    }
    else
    {
        *slash = '\0';
        dir = copy;
    }

    int fd = open(dir, O_RDONLY);

    if (fd >= 0)
    {
        /* Best effort: the rename already happened. */
        fsync(fd);
        close(fd);
    }

    free(copy);
}

EditorFile *open_file(const char *filename)
{
    struct stat st;
    bool exist = false;

    if (stat(filename, &st) == 0)
    {
        exist = true;
    }
    else if (errno != ENOENT)
    {
        fprintf(stderr, "error: cannot stat %s: %s\n", filename, strerror(errno));
        return NULL;
    }

    char *content = NULL;
    size_t content_len = 0;

    if (exist)
    {
        FILE *fp = fopen(filename, "rb");

        if (fp == NULL)
        {
            fprintf(stderr, "error: cannot open %s: %s\n", filename, strerror(errno));
            return NULL;
        }

        if (fseek(fp, 0, SEEK_END) != 0)
        {
            perror("error: cannot seek to the end of the file");
            fclose(fp);
            return NULL;
        }

        long file_end = ftell(fp);

        if (file_end < 0)
        {
            perror("error: cannot get the file size");
            fclose(fp);
            return NULL;
        }

        if (fseek(fp, 0, SEEK_SET) != 0)
        {
            perror("error: cannot seek to the start of the file");
            fclose(fp);
            return NULL;
        }

        size_t file_size = (size_t)file_end;

        content = malloc(file_size + 1);

        if (content == NULL)
        {
            fprintf(stderr, "error: cannot allocate memory for the file content\n");
            fclose(fp);
            return NULL;
        }

        size_t read_bytes = fread(content, 1, file_size, fp);

        if (read_bytes != file_size)
        {
            if (ferror(fp))
            {
                perror("error: cannot read the file");
            }
            else
            {
                fprintf(stderr, "error: unexpected end of file\n");
            }

            free(content);
            fclose(fp);
            return NULL;
        }

        if (fclose(fp) != 0)
        {
            perror("error: cannot close the file");
            free(content);
            return NULL;
        }

        content_len = read_bytes;

        if (fwrite(content, 1, read_bytes, stdout) != read_bytes)
        {
            perror("error: cannot write the content to the stdout");
            free(content);
            return NULL;
        }
    }
    else
    {
        content = malloc(1);

        if (content == NULL)
        {
            fprintf(stderr, "error: cannot allocate memory for the file content\n");
            return NULL;
        }
    }

    content[content_len] = '\0';

    EditorFile *ef = malloc(sizeof *ef);

    if (ef == NULL)
    {
        fprintf(stderr, "error: cannot allocate memory for the editor file\n");
        free(content);
        return NULL;
    }

    ef->path = dup_string(filename);

    if (ef->path == NULL)
    {
        fprintf(stderr, "error: cannot allocate memory for the file path\n");
        free(content);
        free(ef);
        return NULL;
    }

    ef->data = content;
    ef->len = content_len;
    ef->cap = content_len + 1;
    ef->mode = exist ? (st.st_mode & 07777) : 0644;
    ef->exist = exist;

    return ef;
}

int append_bytes(EditorFile *ef, const char *s, size_t n)
{
    if (n == 0)
    {
        return 0;
    }

    if (n > SIZE_MAX - ef->len - 1)
    {
        fprintf(stderr, "error: the file content would be too large\n");
        return -1;
    }

    size_t needed = ef->len + n + 1;

    if (needed > ef->cap)
    {
        size_t cap = ef->cap > 0 ? ef->cap : 64;

        while (cap < needed)
        {
            if (cap > SIZE_MAX / 2)
            {
                cap = needed;
                break;
            }

            cap *= 2;
        }

        char *data = realloc(ef->data, cap);

        if (data == NULL)
        {
            fprintf(stderr, "error: cannot grow the file content buffer\n");
            return -1;
        }

        ef->data = data;
        ef->cap = cap;
    }

    memcpy(ef->data + ef->len, s, n);
    ef->len += n;
    ef->data[ef->len] = '\0';

    return 0;
}

int save_file(EditorFile *ef)
{
    char *tmp_path = make_temp_path(ef->path);

    if (tmp_path == NULL)
    {
        fprintf(stderr, "error: cannot allocate memory for the temporary path\n");
        return -1;
    }

    int fd = mkstemp(tmp_path);

    if (fd < 0)
    {
        fprintf(stderr, "error: cannot create %s: %s\n", tmp_path, strerror(errno));
        free(tmp_path);
        return -1;
    }

    bool ok = true;

    if (write_all(fd, ef->data, ef->len) != 0)
    {
        perror("error: cannot write the temporary file");
        ok = false;
    }

    if (ok && fchmod(fd, ef->mode) != 0)
    {
        fprintf(stderr, "error: cannot set the file permissions: %s\n", strerror(errno));
        ok = false;
    }

    if (ok && fsync(fd) != 0)
    {
        perror("error: cannot sync the temporary file");
        ok = false;
    }

    if (close(fd) != 0)
    {
        perror("error: cannot close the temporary file");
        ok = false;
    }

    if (ok && rename(tmp_path, ef->path) != 0)
    {
        fprintf(stderr, "error: cannot replace %s: %s\n", ef->path, strerror(errno));
        ok = false;
    }

    if (!ok)
    {
        /* Keep the temporary file so the edited content is not lost. */
        fprintf(stderr, "error: the edited content was kept in %s\n", tmp_path);
        free(tmp_path);
        return -1;
    }

    fsync_parent_dir(ef->path);

    free(tmp_path);

    ef->exist = true;

    return 0;
}

void close_file(EditorFile *ef)
{
    if (ef == NULL)
    {
        return;
    }

    free(ef->path);
    free(ef->data);
    free(ef);
}
