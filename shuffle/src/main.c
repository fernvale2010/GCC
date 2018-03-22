
/*
 * Test implementation of Fisher and Yates' shuffle..
 * 
 * Can be used like in shuffle playlist..
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;

#define ARRSIZE         23
#define STRIKE_OUT      0xff

#define MAX_ARRSIZE     255
#define ACCEPT_ARRSIZE  100
uint8_t input[MAX_ARRSIZE];
uint8_t output[MAX_ARRSIZE];


// Fisher and Yates' shuffle
void shuffle(uint8_t* inarr, uint8_t *outarr, uint8_t size)
{
   int i, j;
   int idx;
   int cnt;
   
   //printf("size = %d\r\n", size);
   
   idx = 0;
   for(i=0; i<size; i++)
   {
      idx = rand();
      idx %= (size - i);
      
      //printf("idx = %d\r\n", idx);

      cnt = 0;
      for(j=0; j<size; j++)
      {
         if (inarr[j] == STRIKE_OUT)
         {
            continue;
         }
         else
         {
            if (cnt == idx)
            {
               if (inarr[j] != STRIKE_OUT)
               {
                  outarr[i] = inarr[j];
                  inarr[j] = STRIKE_OUT;
                  break;
               }
            }
            cnt++;
         }
      }
   }
}


void print_array(uint8_t *arr, int arrsize)
{
   uint8_t *p8;
   int tmpsize;

   p8 = arr;
   tmpsize = arrsize;
   while(tmpsize)
   {
      if (tmpsize >= 8)
      {
         printf("%02d %02d %02d %02d %02d %02d %02d %02d\r\n", 
               p8[0], p8[1], p8[2], p8[3], p8[4], p8[5], p8[6], p8[7]);
         
         p8 += 8;
         tmpsize -= 8;
      }
      else
      {
         // remainder
         switch(tmpsize)
         {
            case 7: printf("%02d ", *p8++);
            case 6: printf("%02d ", *p8++);
            case 5: printf("%02d ", *p8++);
            case 4: printf("%02d ", *p8++);
            case 3: printf("%02d ", *p8++);
            case 2: printf("%02d ", *p8++);
            case 1: printf("%02d\r\n", *p8++);
         }
         
         tmpsize -= tmpsize;
      }
   }
}



int main(int argc, char *argv[])
{
   int i, m, loopcnt;
   int arr_size;
   
   if (argc < 3)
   {
      printf("Usage: %s loopcnt arraysize\r\n", argv[0]);
      return 0;
   }
   
   loopcnt = atoi(argv[1]);
   arr_size = atoi(argv[2]);
   if (arr_size >= ACCEPT_ARRSIZE)
   {
      printf("arraysize must be less than %d\r\n", ACCEPT_ARRSIZE);
      return 0;
   }
   
   srand(time(NULL));
   
   for (m=0; m<loopcnt; m++)
   {
      for (i=0; i<arr_size; i++)
      {
         input[i] = i+1;
      }
      
      memset(output, 0, sizeof(output));
      shuffle(input, output, arr_size);

      printf("output - %d\r\n", m);
      print_array(output, arr_size);

      printf("\r\n");
   }
   
   return 0;
}


