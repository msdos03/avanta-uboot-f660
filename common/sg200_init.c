/*
 */

/*
 * env init support
 */
#include <common.h>
#include <command.h>


void sg200_post_init(void)
{
 // mpp to our board
 //* (unsigned long *) 0xf1018000 = 0x11111111;
 //* (unsigned long *) 0xf1018004 = 0x31111111;
 //* (unsigned long *) 0xf1018008 = 0x30000003;
 //* (unsigned long *) 0xf101800C = 0x00000003;
 //* (unsigned long *) 0xf1018010 = 0x00004444;
 //* (unsigned long *) 0xf1018014 = 0x00000000;
 //* (unsigned long *) 0xf1018018 = 0x00050560;
 //* (unsigned long *) 0xf101801C = 0x00000000;
 //* (unsigned long *) 0xf1018020 = 0x00000000;
 //* (unsigned long *) 0xf1018024 = 0x00000000;

 // reset zwave chip: gpio data out for pin 21
 * (unsigned long *) 0xf1018100 = 0xFFFFFFFF;
 
 // led matrix to support link status
 * (unsigned long *) 0xf1018280 = 0x83;
}

void sg200_env_init(void)
{
 setenv("ethprime", "egiga0");
 setenv("ethact", "egiga0");
}

