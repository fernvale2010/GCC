
#include <stdio.h>
#include <string.h>

#include "ringbuffer.h"

/*
 *
 * Critical region protection, write only changes write index, read only changes read index.
 *
 */

struct sRingBuffer_t
{
   u32   rd;         // index to buffer[] where next byte will be read.
   u32   wr;         // index to buffer[] where next byte will be written.
   u32   size;
   u32   available;
   u8    buffer[1];  // define as array of size 1, but in reality, user can provide a contiguous region of memory..
};

#define RINGBUFFER_OVERHEAD   (((u32)((struct sRingBuffer_t *)(0))->buffer) - ((u32)&(((struct sRingBuffer_t *)(0))->rd)))


/*
 * Initialize ringbuffer instance.
 * IN: inst - pointer to ringbuffer_t structure
 * IN: bufsize - size of actual storage, in bytes..
 */
void ringbuffer_init(ringbuffer_t *inst, u32 bufsize)
{
   //printf("Ringbuffer overhead = %d\n", RINGBUFFER_OVERHEAD);

   memset((char*)inst, 0, sizeof(ringbuffer_t));
   inst->size = bufsize - RINGBUFFER_OVERHEAD;
   inst->available = inst->size;
}


/*
 * Reset ringbuffer. This assumes that the ringbuffer has already been initialized..
 */
void ringbuffer_reset(ringbuffer_t *inst)
{
   inst->wr = inst->rd = 0;
   inst->available = inst->size;
   if (inst->size)
   {
      memset((char*)inst->buffer, 0, inst->size);
   }
}


/*
 * returns 1 if empty, 0 if not
 */
int ringbuffer_isempty(ringbuffer_t *inst)
{
   //printf("**wr=%d, rd=%d, size=%d, available=%d\n", inst->wr, inst->rd, inst->size, inst->available);
   return (inst->available == inst->size) ? 1 : 0;
}


/*
 * returns 1 if full, 0 if not
 */
int ringbuffer_isfull(ringbuffer_t *inst)
{
   //printf("++wr=%d, rd=%d, size=%d, available=%d\n", inst->wr, inst->rd, inst->size, inst->available);
   return (inst->available == 0) ? 1 : 0;
}


/*
 * returns number of bytes written..
 */
int ringbuffer_write(ringbuffer_t *inst, u8 *buff, u32 size)
{
   u32 avail = 0;
   u32 written = 0;
   u32 towrite;
   u32 tmp;

   if (ringbuffer_isfull(inst))
      return 0;

   // compute available space..
   avail = inst->available; //(inst->rd > inst->wr) ? (inst->rd - inst->wr) : ((inst->size + inst->rd) - inst->wr);
   towrite = size > avail ? avail : size;  // actual number of bytes to write.

   written = 0;
   if (inst->rd <= inst->wr)
   {
      // possible wrapping needed..
      tmp = inst->size - inst->wr; // free space to end of buffer.
      if (tmp >= towrite)
      {
         // no wrapping needed..
         memcpy(&(inst->buffer[inst->wr]), buff, towrite);
         written += towrite;
         inst->wr += towrite;
         inst->wr %= inst->size;
         inst->available -= written;
      }
      else
      {
         // wrapping needed..
         memcpy(&(inst->buffer[inst->wr]), buff, tmp);
         memcpy(&(inst->buffer[0]), &buff[tmp], towrite-tmp);
         written = towrite;
         inst->wr = towrite-tmp;
         inst->available -= written;
      }
   }
   else
   {
      // no wrapping needed.
      memcpy(&(inst->buffer[inst->wr]), buff, towrite);
      written += towrite;
      inst->wr += towrite;
      inst->wr %= inst->size;
      inst->available -= written;
   }

   return written;
}




/*
 * returns number of bytes read..
 */
int ringbuffer_read(ringbuffer_t *inst, u8 *buff, u32 size)
{
   u32 avail = 0;
   u32 read = 0;
   u32 toread;
   u32 tmp;

   if (ringbuffer_isempty(inst))
      return 0;

   avail = inst->size - inst->available; // available data for reading.
   toread = size > avail ? avail : size;  // actual number of bytes to read.

   read = 0;
   if (inst->wr <= inst->rd)
   {
      // possible wrapping needed..
      tmp = inst->size - inst->rd; // available data to end of buffer.
      if (tmp >= toread)
      {
         // no wrapping needed..
         memcpy(buff, &(inst->buffer[inst->rd]), toread);
         read += toread;
         inst->rd += toread;
         inst->rd %= inst->size;
         inst->available += read;
      }
      else
      {
         // wrapping needed..
         memcpy(buff, &(inst->buffer[inst->rd]), tmp);
         memcpy(&buff[tmp], &(inst->buffer[0]), toread-tmp);
         read = toread;
         inst->rd = toread-tmp;
         inst->available += read;
      }
   }
   else
   {
      // no wrapping needed.
      memcpy(buff, &(inst->buffer[inst->rd]), toread);
      read += toread;
      inst->rd += toread;
      inst->rd %= inst->size;
      inst->available += read;
   }

   return read;
}












