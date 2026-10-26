#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scan.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r+");

    if (fp == NULL)
    {
        perror(argv[1]);
        return 1;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        perror("error: cant seek to the end of the file");
        fclose(fp);
        return 1;
    }

    long file_end = ftell(fp);

    if (file_end < 0)
    {
        perror("error: cant get the file size");
        fclose(fp);
        return 1;
    }

    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        perror("error: cant seek to the start of the file");
        fclose(fp);
        return 1;
    }

    size_t file_size = (size_t)file_end;
    char *content = (char *)malloc(file_size + 1);

    if (content == NULL)
    {
        printf("error: cant allocate the content to the heap\n");
        fclose(fp);
        return 1;
    }

    size_t read_bytes = fread(content, 1, file_size, fp);

    if (ferror(fp))
    {
        perror("error: cant read the file");
        free(content);
        fclose(fp);
        return 1;
    }

    content[read_bytes] = '\0';

    if (fwrite(content, 1, read_bytes, stdout) != read_bytes)
    {
        perror("error: cant write the content to the stdout");
        free(content);
        fclose(fp);
        return 1;
    }

    free(content);

    /* depois da leitura o stream precisa ser reposicionado antes de escrever */
    if (fseek(fp, 0, SEEK_END) != 0)
    {
        perror("error: cant seek to the end of the file");
        fclose(fp);
        return 1;
    }

    bool done = false;
    int status = 0;

    while (!done)
    {
        char *buff = scan();

        if (buff == NULL)
        {
            if (ferror(stdin))
            {
                status = 1;
            }
            else
            {
                printf("\n[EOF]\n");
            }

            done = true;
        }
        else
        {
            size_t buff_len = strlen(buff);

            if (fwrite(buff, 1, buff_len, fp) != buff_len)
            {
                perror("error: cant write the line to the file");
                status = 1;
                done = true;
            }

            free(buff);
        }
    }

    if (fclose(fp) != 0)
    {
        perror("error: cant close the file");
        status = 1;
    }

    return status;
}
