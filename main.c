/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <syslog.h>
#include <timer.h>
#include <pwm.h>
#include <plic.h>
#include <sysctl.h>
#include <fpioa.h>
#include "pwm_play_audio.h"
#include <unistd.h>
#include <stdlib.h>
#include "include/opus.h"

/* #include "test_wav.h" */
/* #include "test_8bit_wav.h" */
/* #include "test_16bit_wav.h" */
/* #include "test_24bit_wav.h" */
/* #include "test_16bit_mono_wav.h" */
/* #include "test_welcome.h" */

#define TIMER_NOR   0
#define TIMER_CHN   1
#define TIMER_PWM   0
#define TIMER_PWM_CHN 0

OpusDecoder *opus;

#define FRAMESIZE 5760*2
#define BUFFERSIZE (16384)

unsigned short framebuff[5760*2];
signed short buff[BUFFERSIZE];
volatile unsigned int bp=0;
extern unsigned char test_opus[];

static int timer_callback(void *ctx)
{
  double duty1 = ((float)buff[bp])*(1.f/65536.f)+0.5f;
  double duty2 = ((float)buff[bp+1])*(1.f/65536.f)+0.5f;
  bp+=2;
  bp&=(BUFFERSIZE-1);
  pwm_set_frequency(0, 0, 44100, duty1);
  pwm_set_frequency(0, 1, 44100, duty2);
  //  printf(".");
  return 0;
}

int main(void)
{
  //  printf("%d\n",sysctl_clock_get_freq(SYSCTL_CLOCK_TIMER0 + 0));
    printf("PWM OPUS demo format test\n");
    printf("Music: PeriTune\n");
    printf("Copyright 2001-2011 Xiph.Org, Skype Limited, Octasic,\n\
                    Jean-Marc Valin, Timothy B. Terriberry,\n\
                    CSIRO, Gregory Maxwell, Mark Borgerding,\n\
                    Erik de Castro Lopo, Gombe\n");

    /* Init FPIOA pin mapping for PWM*/
    fpioa_set_function(22, FUNC_TIMER0_TOGGLE1);
    fpioa_set_function(23, FUNC_TIMER0_TOGGLE2);
    /* Init Platform-Level Interrupt Controller(PLIC) */
    plic_init();
    /* Enable global interrupt for machine mode of RISC-V */
    sysctl_enable_irq();

    timer_init(1);
    pwm_init(0);//    pwm_play_init(TIMER_NOR, TIMER_PWM);
    timer_set_interval(1,3,22676);
    timer_irq_register(1, 3, 0, 1, timer_callback,NULL);
    timer_set_enable(1, 3, 1);
    /* Enable PWM */
    pwm_set_enable(0, 0, 1);
    pwm_set_enable(0, 1, 1);

    {
      int err;
      opus= opus_decoder_create(48000,2,&err);
      if(err){
	printf("opus error:%d\n",err);
      }
    }
    /* while(1){ */
    /*   printf("%d\n",bp); */
    /* } */

    int frames;
    uint8_t *ptr=test_opus;
    int pr=0;
    while(1){
//      usleep(15000);      
      int nlen=(ptr[0]<<24)|(ptr[1]<<16)|(ptr[2]<<8)|(ptr[3]<<0);
      ptr+=8;
      frames=opus_decode(opus,ptr,nlen,framebuff,FRAMESIZE,0);
      if(frames<0){
	printf("decode faild:%d,len=%d\n",frames,nlen);
      }else{
	//	printf("success(%d)\n",frames);
      }
      ptr+=nlen;
      if((pr+frames*2)>(BUFFERSIZE-1)){
	while((pr<bp||bp<((pr+frames*2)&(BUFFERSIZE-1))));
      }else{
	while((pr<bp&&bp<((pr+frames*2)&(BUFFERSIZE-1))));
      }
	//	printf("%d-%d(%d\n",bp,pr,frames);
      for(int i=0;i<frames*2;i++){
	/* if(framebuff[i]){ */
	  /*	  	  printf("%d\n",framebuff[i]); */
	/* } */
	if(bp==((pr+i)&(BUFFERSIZE-1))){
	  //	  printf("!\n");
	  while(bp==((pr+i)&(BUFFERSIZE-1)));
	}
	buff[(pr+i)&(BUFFERSIZE-1)]=framebuff[i];
      }
      pr +=frames*2;
      
      //        pwm_play_wav(TIMER_NOR, TIMER_CHN, TIMER_PWM, TIMER_PWM_CHN, test_wav, 0);
      //        pwm_play_wav(TIMER_NOR, TIMER_CHN, TIMER_PWM, TIMER_PWM_CHN, test_8bit_wav, 0);
//        pwm_play_wav(TIMER_NOR, TIMER_CHN, TIMER_PWM, TIMER_PWM_CHN, test_16bit_mono_wav, 0);
//        pwm_play_wav(TIMER_NOR, TIMER_CHN, TIMER_PWM, TIMER_PWM_CHN, test_16bit_wav, 0);
//        pwm_play_wav(TIMER_NOR, TIMER_CHN, TIMER_PWM, TIMER_PWM_CHN, test_24bit_wav, 0);
//        pwm_play_wav(TIMER_NOR, TIMER_CHN, TIMER_PWM, TIMER_PWM_CHN, test_welcome_wav, 0);
    }
}
