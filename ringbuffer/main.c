
#include <string.h>
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ringbuffer.h"

#define PROD_BUFSIZE    2048
#define CONS_BUFSIZE    256


u32 rbuffer[256];
ringbuffer_t *pRB;

char *inputfile = "input.log"; // nmea log file

int producer_exit = -1;


void msdelay(u32 msec)
{
   usleep(msec * 1000L);
}

void *producer_task(void *t)
{
   int size = 0;
   int fhnd = -1;
   int total, ftotal;
   int tmp;
   int trycount;
   int fileread;
   ringbuffer_t *p_wrRB = (ringbuffer_t *) t;
   u8 buf[PROD_BUFSIZE];
   int bufsize = PROD_BUFSIZE;

   printf("producer_task started %x\n", (u32)p_wrRB);

   fhnd = open(inputfile, O_RDONLY | O_BINARY);
   if (fhnd < 0)
   {
      printf("Cannot open %s\n", inputfile);
      producer_exit = 1;
      pthread_exit(0);
   }

   producer_exit = 0;
   ftotal = 0;
   fileread = 0;
   while(1)
   {
      size = read(fhnd, buf, bufsize);
      if (size > 0) fileread += size;
      //printf("ftotal = %d, size=%d, fileread=%d\n", ftotal, size, fileread);
      if (size > 0)
      {
         trycount = 0;
         total = 0;
         while(total != size)
         {
            tmp = ringbuffer_write(p_wrRB, &buf[total], size - total);
            if (tmp == 0)
            {
               msdelay(10);
            }
            else
            {
               total += tmp;
            }
         }
         ftotal += total;
      }
      else
      {
         if (++trycount > 5)
            break; // nothing more to read
         else
            msdelay(10);
      }
   }

   close(fhnd);

   msdelay(20); // give consumer chance to run before we quit..
   producer_exit = 1;
   printf("producer_task done\n");
   pthread_exit(0);
   return NULL;
}



// read from ring buffer until CR-LF..
// returns -1 on error, otherwise is length of NMEA sentence
int getSentence(ringbuffer_t *pRB, u8 *buf)
{
   int total;
   int tmp, idx;
   int dollarFound;
   u8 tmpch;

   total = 0;
   dollarFound = 0; // '$' is not found yet
   idx = 0;
   while(1)
   {
      tmp = ringbuffer_read(pRB, &tmpch, 1);  // read 1 byte
      if (tmp > 0)
      {
         if (dollarFound == 0)
         {
            if (tmpch != '$')
            {
               continue;
            }
            else
            {
               dollarFound = 1;
               total += tmp;
               buf[idx++] = tmpch;
            }
         }
         else
         {
            // '$' has been found..
            total += tmp;
            buf[idx] = tmpch;
            if (buf[idx] == '\x0D')
            {
               buf[idx] = 0; // NULL terminate sentence.. next byte should be a '\x0A'..
               break;
            }
            else
            {
               idx++;
            }
         }
      }
      else
      {
         // nothing to read..
         msdelay(10);
         if (producer_exit == 1)
         {
            total = -1;
            break;
         }
      }
   }

   return total;
}



void *consumer_task(void *t)
{
   int tmp;
   ringbuffer_t *p_rdRB = (ringbuffer_t *) t;
   u8 buf[CONS_BUFSIZE];
  
   printf("consumer_task started %x\n", (u32)p_rdRB);
   
   while(1)
   {
      tmp = getSentence(p_rdRB, buf);
      if (tmp > 0)
      {
         printf("%s\n", buf);
      }
      else
      {
         // nothing to read..
         msdelay(10);
         if (producer_exit == 1)
            break;
      }
   }

   printf("consumer_task done\n");
   pthread_exit(0);
   return NULL;
}


int main(int argc, char *argv[])
{
   int num_threads = 2;
   pthread_t thread[2];
   int rc;
   
   srand(time(0));

   pRB = (ringbuffer_t *)rbuffer;
   ringbuffer_init(pRB, sizeof(rbuffer));

   producer_exit = -1;
   printf("Creating producer thread\n");
   rc = pthread_create(&thread[0], NULL, producer_task, (void *)pRB);
   if (rc)
   {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
   }

   printf("Creating consumer thread\n");
   rc = pthread_create(&thread[0], NULL, consumer_task, (void *)pRB);
   if (rc)
   {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
   }

   printf("Main completed\n");
   pthread_join(thread[0], NULL);
   pthread_join(thread[1], NULL);
   //pthread_exit(NULL);
   return 0;
}
