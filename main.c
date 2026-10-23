#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scan.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r+");

    if (fp == NULL)
    {
        printf("error: file not found");
    }

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *content = (char *)malloc(file_size + 1);

    if (content == NULL)
    {
        printf("error: cant allocate the content to the heap");
        fclose(fp);
        return 1;
    }

    size_t read_bytes = fread(content, 1, file_size, fp);
    content[read_bytes] = '\0';

    printf("%s", content);

    char *buff = scan();

    fwrite(buff, 1, strlen(buff), fp);

    free(content);
    free(buff);
    fclose(fp);
    return 0;
}