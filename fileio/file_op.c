#include "fileio.h"

FILE *open_file(char *filename)
{
    FILE *fp = fopen(filename, "r+");

    if (fp == NULL)
    {
        perror(filename);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        perror("error: cant seek to the end of the file");
        fclose(fp);
        return NULL;
    }

    long file_end = ftell(fp);

    if (file_end < 0)
    {
        perror("error: cant get the file size");
        fclose(fp);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        perror("error: cant seek to the start of the file");
        fclose(fp);
        return NULL;
    }

    size_t file_size = (size_t)file_end;
    char *content = (char *)malloc(file_size + 1);

    if (content == NULL)
    {
        printf("error: cant allocate the content to the heap\n");
        fclose(fp);
        return NULL;
    }

    size_t read_bytes = fread(content, 1, file_size, fp);

    if (ferror(fp))
    {
        perror("error: cant read the file");
        free(content);
        fclose(fp);
        return NULL;
    }

    content[read_bytes] = '\0';

    if (fwrite(content, 1, read_bytes, stdout) != read_bytes)
    {
        perror("error: cant write the content to the stdout");
        free(content);
        fclose(fp);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        perror("error: cant seek to the end of the file");
        fclose(fp);
        return NULL;
    }

    free(content);
    return fp;
}