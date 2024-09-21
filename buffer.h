#ifndef BUFFER_H
#define BUFFER_H
#include <stdlib.h>
#include <string.h>

typedef struct Buffer
{
    char *content;
    unsigned int size;
    unsigned int position;
} Buffer;

Buffer *Buffer_construct()
{
    Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));
    buffer->content = (char *)malloc(sizeof(char));
    buffer->content[0] = '\0';
    buffer->size = 1;
    buffer->position = 0;
    return buffer;
}

Buffer *Buffer_reset(Buffer *buffer)
{
    buffer->position = 0;
}

char Buffer_read_c(Buffer *buffer)
{
    char res = buffer->content[buffer->position];
    if (buffer->position != buffer->size)
    {
        buffer->position++;
    }else{
        return '\0';
    }
    return res;
}
void Buffer_unread_c(Buffer *buffer)
{
    if (buffer->position == 0)
        return;
    buffer->position--;
}
char Buffer_peek_c(Buffer *buffer)
{
    char res = Buffer_read_c(buffer);
    Buffer_unread_c(buffer);
    return res;
}

void Buffer_write_c(Buffer *buffer, char c)
{
    buffer->size++;
    buffer->content = (char *)realloc(buffer->content, buffer->size);
    buffer->content[buffer->position] = c;
    buffer->position++;
    buffer->content[buffer->position] = '\0';
}
void Buffer_write_s(Buffer *buffer, char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        Buffer_write_c(buffer, str[i]);
    }
}
Buffer *Buffer_s(char *str)
{
    Buffer *buffer = Buffer_construct();
    Buffer_write_s(buffer, str);
    Buffer_reset(buffer);
    return buffer;
}
void Buffer_destruct(Buffer *buffer)
{
    if (buffer->content != NULL)
        free(buffer->content);
    free(buffer);
}

char *Buffer_tostr(Buffer *buffer)
{
    char *result = buffer->content;
    buffer->content = NULL;
    Buffer_destruct(buffer);
    return result;
}

#endif