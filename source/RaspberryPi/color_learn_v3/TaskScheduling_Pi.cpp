#include "blob.cpp"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <math.h>
//#include "decision_making.h"

int main()
{
    

    int Ret;
    sigset_t sigset;
    struct sigevent sig;
    timer_t timerone;

	unsigned long Test_Counter = 0;

    // initialise the signal set and add SIGUSR1 as a signal we're interested in
    sigemptyset(&sigset);
    sigaddset(&sigset,SIGUSR1);

    // s = pthread_sigmask(SIG_BLOCK, &sigset, NULL);
    // if (s !=0)
    // 	   handle_error_en(s, "pthread_sigmask");

    // Block the signal we're interested in, this allows us to receive signal information as and when we're ready to
    // process it, as opposed to using sigaction which can only process one signal at a time (if a timer fires while processing
    // another signal handler, typically it is 'missed').
    sigprocmask(SIG_BLOCK,&sigset,NULL);

    // construct a sigevent structure.
    sig.sigev_signo = SIGUSR1;
    sig.sigev_notify = SIGEV_SIGNAL; // see the man pages, but this could be used to specify a handler function to be run in another thread instead of the approach demonstrated here
    sig.sigev_value.sival_int =20; // signal value
    sig.sigev_notify_attributes = NULL;

    //create a new timer.
    Ret = timer_create(CLOCK_REALTIME, &sig, &timerone);

    if (Ret == 0)
    {
        // set the time resolution for the timer created above.

        struct itimerspec in, out;
        in.it_value.tv_sec = 0;
        in.it_value.tv_nsec = (0.5 * powl(10,9)); // start firing after half a second
        in.it_interval.tv_sec = 0;
        in.it_interval.tv_nsec = (65 * powl(10,6));// fire every 100 ms

        Ret = timer_settime(timerone, 0, &in, &out);
    }
    else
    {
        printf("timer_create() failed with %d\n", errno);
        exit(1);
    }
    Ret = 0;

	init();
   // loop until killed.
    //while(1)
    //{
      siginfo_t info;
      sigemptyset(&sigset);
      sigaddset(&sigset,SIGUSR1);

      //You can use a sigtimedwait here (see man page for details) to specify a maximum wait time
      // for interesting events. For example this would allow you to perform 'idle' operations where
      // the system is not busy.

	  //10ms loop
      while(sigwaitinfo(&sigset,&info)>0 && !exitLoop)
      { 
		cout << "hi!" << endl;
		cam();
		////CV function that returns Pos[] and RGB
		////Test Code, Uncomment once integrated with OpenCV
  //      /*-----------------------Start of Test Code-----------------------------------*/
  //      test_counter++;
		//if( (test_counter > 100) && (test_counter < 500)){
		//	rgb[0] = 255; // red
		//	rgb[1] = 0; // red
		//	rgb[2] = 0; // red
		//}
		//else if (test_counter < 1000){
		//	rgb[0] = 0;
		//	rgb[1] = 255; //green
		//	rgb[2] = 0;
		//	pos[2] = 5.0f;
		//}
		//else if(test_counter < 1500){
		//	pos[0] = 5.0f;
		//	pos[2] = 0.0f;
		//}
		//else if(test_counter < 2000){
		//	pos[0] = -5.0f;
		//    pos[2] = 10.0f;
		//}
		//else if(test_counter < 2500){
		//	rgb[0] = rgb[1] =rgb[2] = 0;
		//}
		//else if(test_counter < 3000){
		//	rgb[0] = 255;
		//}
		//else if(test_counter < 3500){
		//	rgb[0] = 0; 
		//}
		///*--------------------------end of test code-------------------------------------*/
  //      //printf("[%d] caught signal %d\n",(int)time(0),info.si_value.sival_int);

  //      camtracking();
		//printf("counter:%d\n",test_counter);
		//printf("rgb:%d,%d,%d pos:%f,%f,%f\n",rgb[0],rgb[1],rgb[2],pos[0],pos[1],pos[2]);
		//printf("state:%d steer:%f drive:%f brake:%f\n", cntlstate, cntlcom[0], cntlcom[1], cntlcom[2] );
		//printf("\n");

		//Serial Communication
      }
   // }

    return 0;
}