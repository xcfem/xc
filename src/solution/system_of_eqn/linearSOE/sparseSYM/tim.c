/*
 * File:  tim.c
 * ============
 * This function is used to record the time used for the computation.
 */


#include <stdio.h>
#include <sys/times.h>
#include <sys/param.h>
#include "tim.h"


#ifndef HZ
  #define HZ 1.0
#endif



/* Declared as static global variables so as not to be reset every time
 * the "timer" function is called.
 */
static double  cpuold;
static double  cpu[10];


void timer(int tcode)
  {
    struct tms  rettim;  /* struct tms is defined in <sys/times.h> */
    double  cpunew;
    int i;
    static char *tproc[]= {" symbolic     ", 
			    " factorizing  ",
			     " solution     "};
    times(&rettim);             /* "times" defined in <sys/times.h>" */
    cpunew= rettim.tms_utime;  /* utim = user time */

    if(tcode > 0)
      {
	cpu[tcode - 1] = (cpunew-cpuold) / (HZ * 1.0);
	/* HZ is defined in <sys/param.h> */
	printf(" NOTE:  HZ= %d\n", HZ);
      }
    cpuold = cpunew;
    if(tcode == END)
      {
	for(i=0; i < END ;i++)
  	  { printf(" %s   time %f\n", tproc[i], cpu[i] ); }
      }
  }








