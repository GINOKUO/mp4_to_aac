#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include "stream.h"
#define max(a,b) a>b?a:b
#define min(a,b) a>b?b:a
void* file_open(stream_t *stream_s, const char* filename, int mode)
{
   FILE* file = NULL;
   const char* mode_fopen = NULL;
   if ((mode & MODE_READWRITEFILTER) == MODE_READ)
      mode_fopen = "rb";
   else
      if (mode & MODE_EXISTING)
         mode_fopen = "r+b";
      else
         if (mode & MODE_CREATE)
            mode_fopen = "wb";
   if ((filename != NULL) && (mode_fopen != NULL))
      file = fopen(filename, mode_fopen);
   stream_s->opaque = (void*)file;

   return file;
}

int file_read(stream_t *stream_s, void* buf, int size)
{
   FILE* file = (FILE*)stream_s->opaque;
   return fread(buf, 1, size, file);
}

int file_write(stream_t *stream_s, void *buf, int size)
{
   FILE* file = (FILE*)stream_s->opaque;
   return fwrite(buf, 1, size, file);
}

int file_peek(stream_t *stream_s, void* buf, int size)
{
   uint32_t offset = file_tell(stream_s);
   int ret = file_read(stream_s, buf, size);
   file_seek(stream_s, offset, SEEK_SET);
   return ret;
}

uint64_t file_seek(stream_t *stream_s, int64_t offset, int whence)
{
   FILE* file = (FILE*)stream_s->opaque;
   return fseek(file, offset, whence);
}

uint64_t file_tell(stream_t *stream_s)
{
   FILE* file = (FILE*)stream_s->opaque;
   return ftell(file);
}

int file_close(stream_t *stream_s)
{
   FILE* file = (FILE*)stream_s->opaque;
   return fclose(file);
}

void* buffer_open(stream_t *stream_s, BUFFER_t *buffer)
{
   stream_s->opaque = (void *)buffer;

   return buffer;
}

int buffer_read(stream_t *stream_s, void* buf, int size)
{
   BUFFER_t *buffer = (BUFFER_t *)stream_s->opaque;
   memcpy(buf,buffer->buf,size);
   return size;
}

int buffer_write(stream_t *stream_s, void *buf, int size)
{
   BUFFER_t *buffer = (BUFFER_t *)stream_s->opaque;
   memcpy(buffer->buf,buf,size);
   return size;
}

int buffer_peek(stream_t *stream_s, void* buf, int size)
{
   int ret = buffer_read(stream_s, buf, size);
   return ret;
}

uint64_t buffer_seek(stream_t *stream_s, int64_t offset,\
	int whence)
{
   BUFFER_t *buffer = (BUFFER_t *)stream_s->opaque;
   (*buffer).offset = whence + offset;
   memcpy(buffer->buf,buffer->begin_addr + (*buffer).offset,\
	   (*buffer).filesize - (*buffer).offset );
   return 0;
}

uint64_t buffer_tell(stream_t *stream_s)
{
   BUFFER_t *buffer = (BUFFER_t *)stream_s->opaque;
   return (*buffer).offset;
}

int buffer_close(stream_t *stream_s)
{
   BUFFER_t *buffer = (BUFFER_t *)stream_s->opaque;
   free(buffer->buf);
   free(buffer->begin_addr);
   free(buffer);
   return 0;
}

stream_t* create_file_stream()
{
   stream_t* s = (stream_t*)malloc(sizeof(stream_t));
   s->open = file_open;
   s->read = file_read;
   s->write = file_write;
   s->peek = file_peek;
   s->seek = file_seek;
   s->tell = file_tell;
   s->close = file_close;
   return s;
}

void destory_file_stream(stream_t* stream_s)
{
   free(stream_s);
}

stream_t* create_buffer_stream()
{
   stream_t* s = (stream_t*)malloc(sizeof(stream_t));
   s->open = (void* (*)(stream*, const char*, int))buffer_open;
   s->read = buffer_read;
   s->write = buffer_write;
   s->peek = buffer_peek;
   s->seek = buffer_seek;
   s->tell = buffer_tell;
   s->close = buffer_close;
   return s;
}

void destory_buffer_stream(stream_t* stream_s)
{
   free(stream_s);
}

uint16_t Swap16(uint16_t x)
{
   return ((x<<8)|(x>>8));
}

uint32_t Swap32(uint32_t x)
{
   return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
}

uint64_t Swap64(uint64_t x)
{
   uint32_t hi, lo;

   /* Separate into high and low 32-bit values and swap them */
   lo = (uint32_t)(x & 0xFFFFFFFF);
   x >>= 32;
   hi = (uint32_t)(x & 0xFFFFFFFF);
   x = Swap32(lo);
   x <<= 32;
   x |= Swap32(hi);
   return(x);
}

uint16_t read_le16(stream_t *src)
{
   uint16_t value;

   stream_read(src, &value, sizeof(value));
   return(SwapLE16(value));
}

uint16_t read_be16(stream_t *src)
{
   uint16_t value;

   stream_read(src, &value, sizeof(value));
   return(SwapBE16(value));
}

uint32_t read_le32(stream_t *src)
{
   uint32_t value;

   stream_read(src, &value, sizeof(value));
   return(SwapLE32(value));
}

uint32_t read_be32(stream_t *src)
{
   uint32_t value;

   stream_read(src, &value, sizeof(value));
   return(SwapBE32(value));
}

uint64_t read_le64(stream_t *src)
{
   uint64_t value;

   stream_read(src, &value, sizeof(value));
   return(SwapLE64(value));
}

uint64_t read_be64(stream_t *src)
{
   uint64_t value;

   stream_read(src, &value, sizeof(value));
   return(SwapBE64(value));
}

int write_le16(stream_t *dst, uint16_t value)
{
   value = SwapLE16(value);
   return(stream_write(dst, &value, sizeof(value)));
}

int write_be16(stream_t *dst, uint16_t value)
{
   value = SwapBE16(value);
   return(stream_write(dst, &value, sizeof(value)));
}

int write_le32(stream_t *dst, uint32_t value)
{
   value = SwapLE32(value);
   return(stream_write(dst, &value, sizeof(value)));
}

int write_be32(stream_t *dst, uint32_t value)
{
   value = SwapBE32(value);
   return(stream_write(dst, &value, sizeof(value)));
}

int write_le64(stream_t *dst, uint64_t value)
{
   value = SwapLE64(value);
   return(stream_write(dst, &value, sizeof(value)));
}

int write_be64(stream_t *dst, uint64_t value)
{
   value = SwapBE64(value);
   return(stream_write(dst, &value, sizeof(value)));
}
