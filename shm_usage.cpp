/*************************************************
*
* Copyright (c) 2006-2015 Chicony, Inc.
* All rights reserved.
*
**************************************************
* File name: audio_shm_usage.c
* Author:     jimmy_wu@chicony.com
* Version:    Initial Draft
* Date:       2019/06/20
* Description: 
*
*
*
* History:
*
*    1. Date:        2019/06/20
*       Author:        jimmy_wu@chicony.com
*        Modification:  Created file
*
*************************************************/

#include <unistd.h>
#include "shm_usage.h"

#define SHM_SIZE 600000;
//struct timeval timestamp;

int shm_audio_write_downlink(u_int8_t* addr, u_int8_t *write_addr,  int frame_size)
{
    unsigned int SHMSZ = SHM_SIZE;
    unsigned int read_audio_frame_index = *((unsigned int *)(addr + SHMSZ - 4));
    unsigned int write_audio_frame_index = *((unsigned int *)(addr + SHMSZ - 8));
    //unsigned long int one_frame_timestamp;
    unsigned int frame_cnt = *((unsigned int *)(addr + SHMSZ - 12));
    
     // avoid write over read
    if(read_audio_frame_index > write_audio_frame_index)
    {
            while (read_audio_frame_index - write_audio_frame_index <= 3000/*0*/) 
            {
                usleep(1000);
                read_audio_frame_index = *((unsigned int *) (addr + SHMSZ - 4));
            }
    }


    if (write_audio_frame_index + frame_size >= SHMSZ) 
    {   
        
        while (read_audio_frame_index <= 3000/*0*/) 
        {
            usleep(1000);
            read_audio_frame_index = *((unsigned int *)(addr + SHMSZ - 4));
        }
        
        write_audio_frame_index = 0;
        frame_cnt++;
        //printf("%s-- audio write reset %d  ~~~~~~~~~~~~~~~~~~~~~~~~~\n", __FUNCTION__,frame_cnt);
    }
    memcpy(addr + write_audio_frame_index, write_addr, frame_size);
    write_audio_frame_index += frame_size;
    *((unsigned int *)(addr + SHMSZ - 8)) = write_audio_frame_index;
    *((unsigned int *)(addr + SHMSZ - 12)) = frame_cnt;
    if(frame_size == 1599)
    {
        *((unsigned int *)(addr + SHMSZ - 8)) = 610000;
    }
    return 0;
}
