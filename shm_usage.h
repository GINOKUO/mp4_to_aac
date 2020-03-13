/*************************************************
*
* Copyright (c) 2006-2015 Chicony, Inc.
* All rights reserved.
*
**************************************************
* File name: shm_usage.h
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

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

int shm_audio_write_downlink(u_int8_t* shm, u_int8_t* write_addr, int write_len);

