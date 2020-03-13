#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "stdafx.h"
#include "mp4.h"
#include "shm_usage.h"

#define RTP_MAX_PKT_SIZE        1480
#define AUDIO_HEARD_LEN         4
#define PARM S_IRUSR | 0666
#define V_SHM_SIZE 600000;
#define AUDIO_AAC_HEARD_LEN     7

int main()
{
  long int start_index = V_SHM_SIZE;
	/*
    int shm_id;
    uint8_t *shm_av;
    key_t key;

    key = 123456;
    if ((shm_id = shmget(key, start_index, PARM)) < 0) {
        perror("shmget");
        return 0;
    }
    if ((shm_av = (uint8_t *)shmat(shm_id, NULL, 0)) == (uint8_t *) -1) {
        perror("shmat");
        return 1;
    }
	memset(shm_av, 0, start_index);
  *((unsigned int *)(shm_av + start_index - 4)) = 0;
  *((unsigned int *)(shm_av + start_index - 8)) = 0;
  *((unsigned int *)(shm_av + start_index - 12)) = 0;
*/
  //get latest sd card mp4 file
  unsigned long filesize = 0;
  FILE *file;

  char mp4_list[256];
  char mp4_list_path[256];
  char * line = NULL;
  size_t len_date = 0;
  ssize_t read;

  file = fopen("/tmp/sdr_mp4_list.txt", "r");
  if(file == NULL)
  {
    return -1;
  }

  while ((read = getline(&line, &len_date, file)) != -1)
  {
    strncpy(mp4_list, line, sizeof(mp4_list));
    if(read > 0) 
    {        
      if(mp4_list[(read - 1)] == '\n') 
      {
        mp4_list[(read - 1)] = '\0';
      } 
      else 
      {
        mp4_list[read] = '\0';
      }
    }
  }

  snprintf(mp4_list_path, sizeof(mp4_list_path), "/sdcard/%s", mp4_list);
  //printf("%s\n", mp4_list_path);
  fclose(file);
  
   int sampleCount;
   mp4_box_t *root = NULL;
   mp4_box_t *v_stsz = NULL, *v_stco = NULL;
   mp4_box_t *a_stsz = NULL, *a_stco = NULL;
   BUFFER_t *buffer = NULL;

   int v_stsz_sample_count = -1;
   int v_stsz_sample_size[10240];
   int v_stco_entry_count = -1;
   int v_stco_chunk_offset[10240];
   int a_stsz_sample_count = -1;
   int a_stsz_sample_size[10240];
   int a_stco_entry_count =  -1;
   int a_stco_chunk_offset[10240];

   // mp4 parser
       file = fopen(mp4_list_path,"rb");
       fseek(file,0L,SEEK_END);
       filesize = ftell(file);
       fseek(file,0L,SEEK_SET); 
       buffer = (BUFFER_t *)malloc(sizeof(BUFFER_t));
       buffer->begin_addr = (unsigned char *)malloc(filesize);
       buffer->buf = (unsigned char *)malloc(filesize);
       fread(buffer->begin_addr,filesize,1,file);
       memcpy(buffer->buf,buffer->begin_addr,filesize);
       (*buffer).offset = 0;
       (*buffer).filesize = filesize;
       fclose(file);

       stream_t* fd_stream = create_buffer_stream();

       if (buffer_open(fd_stream, buffer) == 0)
       {
          printf("open fail\n");
       }

       root = MP4_BoxGetRootFromBuffer(fd_stream,filesize);

       //********* video box info *********
       v_stsz = MP4_BoxSearchBox(root,ATOM_stsz);
       v_stsz_sample_count =  v_stsz->data.p_stsz->sample_count;
       for(sampleCount = 0; sampleCount < v_stsz_sample_count; sampleCount++)
       {
         v_stsz_sample_size[sampleCount] =  v_stsz->data.p_stsz->entry_size[sampleCount];
       }
       v_stco = MP4_BoxSearchBox(root,ATOM_stco);
      
       v_stco_entry_count =  v_stco->data.p_stco->sample_size;
       for(sampleCount = 0; sampleCount < v_stco_entry_count; sampleCount++) 
       {
         v_stco_chunk_offset[sampleCount] =  v_stco->data.p_stco->entry_size[sampleCount*2];
       }
        
     
       //********* audio box info *********
       a_stsz = MP4_BoxSearchBox(v_stsz->p_next,ATOM_stsz);
       a_stsz_sample_count =  a_stsz->data.p_stsz->sample_count;
       for(sampleCount = 0; sampleCount < a_stsz_sample_count; sampleCount++)
       {
         a_stsz_sample_size[sampleCount] =  a_stsz->data.p_stsz->entry_size[sampleCount];
       }

       a_stco = MP4_BoxSearchBox(v_stco->p_next,ATOM_stco);
       a_stco_entry_count =  a_stco->data.p_stco->sample_size;
       for(sampleCount = 0; sampleCount < a_stco_entry_count; sampleCount++) 
       {
         a_stco_chunk_offset[sampleCount] =  a_stco->data.p_stco->entry_size[sampleCount*2];
       }
     
       MP4_BoxFreeFromBuffer(root);
       buffer_close(fd_stream);
       destory_buffer_stream(fd_stream);
   

   printf("audio parser ready\n");

   int offset = 0;
   int len = 0;
   int a_stco_no = 0;
   int a_stsz_no = 0;
   uint8_t *frame_buff = (uint8_t*)malloc(500000);
   uint8_t cut_frame_buff[1600] = {0};
   uint8_t audio_frame_buff[1000] = {0};
   uint8_t len_buff[16] = {0};

    int profile = 2; // AAC LC
    int freqIdx = 8; //8 標識16000，取特定
    int channelCfg = 2; // 音頻聲道數爲兩個

    fd_stream = create_file_stream();

    if (stream_open(fd_stream, mp4_list_path, MODE_READ) == 0)
    {
        printf("open fail\n");
    }

    file = fopen("/adc/output.aac", "wb");

    if (file == NULL)
    {
        printf("open aac fail\n");
    }
    
    while(1)
    {
      if(offset == a_stco_chunk_offset[a_stco_no])
      {
        len = stream_read(fd_stream, frame_buff, a_stsz_sample_size[a_stsz_no]);
        /*
        audio_frame_buff[0] = 0x00;
        audio_frame_buff[1] = 0x10;
        audio_frame_buff[2] = (a_stsz_sample_size[a_stsz_no] & 0x1FE0) >> 5;
        audio_frame_buff[3] = (a_stsz_sample_size[a_stsz_no] & 0x1F) << 3; 
        */

        // fill in ADTS data
        audio_frame_buff[0] = 0xFF;
        audio_frame_buff[1] = 0xF9;
        audio_frame_buff[2] = (((profile - 1) << 6) + (freqIdx << 2) + (channelCfg >> 2));
        audio_frame_buff[3] = (((channelCfg & 3) << 6) + ((a_stsz_sample_size[a_stsz_no] + 7) >> 11));
        audio_frame_buff[4] = (((a_stsz_sample_size[a_stsz_no] + 7) & 0x7FF) >> 3);
        audio_frame_buff[5] = ((((a_stsz_sample_size[a_stsz_no] + 7) & 7) << 5) + 0x1F);
        audio_frame_buff[6] = 0xFC;

        //memcpy(audio_frame_buff + AUDIO_HEARD_LEN, frame_buff, a_stsz_sample_size[a_stsz_no]);
        memcpy(audio_frame_buff + AUDIO_AAC_HEARD_LEN, frame_buff, a_stsz_sample_size[a_stsz_no]);
        fwrite(audio_frame_buff, a_stsz_sample_size[a_stsz_no] + AUDIO_AAC_HEARD_LEN, 1, file);

        /*
        len_buff[0] = ((a_stsz_sample_size[a_stsz_no] + AUDIO_HEARD_LEN) & 0xF00) >> 8;
        len_buff[1] = (a_stsz_sample_size[a_stsz_no] + AUDIO_HEARD_LEN) & 0xFF;
        shm_audio_write_downlink(shm_av, len_buff, 2);  
        shm_audio_write_downlink(shm_av, audio_frame_buff, a_stsz_sample_size[a_stsz_no] + AUDIO_HEARD_LEN);  
        */
        offset += a_stsz_sample_size[a_stsz_no];
        a_stsz_no++;
        a_stco_no++;
      }   
      else
      {
        len = stream_read(fd_stream, frame_buff, 1);
        offset++;
      }

      if(len == 0)
        break;
     }
    //shm_audio_write_downlink(shm_av, cut_frame_buff, 1599);  

    stream_close(fd_stream);
    destory_file_stream(fd_stream);    
  
   fclose(file);   

   free(frame_buff);
   return 0;
    
}