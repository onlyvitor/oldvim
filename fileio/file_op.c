#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"

EditorFile *open_file(const char *filename)
{
    FILE *fp = fopen(filename, "r+");

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
    char *content = (char *)malloc(file_size + 1);

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

    content[read_bytes] = '\0';

    if (fwrite(content, 1, read_bytes, stdout) != read_bytes)
    {
        perror("error: cannot write the content to the stdout");
        free(content);
        fclose(fp);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        perror("error: cannot seek to the end of the file");
        free(content);
        fclose(fp);
        return NULL;
    }

    EditorFile *ef = {filename, content, sizeof(fp), sizeof(fp), "r+"};

    return ef;
}