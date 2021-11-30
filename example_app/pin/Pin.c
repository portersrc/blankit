#define  _GNU_SOURCE
#include  <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <time.h>
//#define _POSIX_C_SOURCE >= 199309L

#define  shared_pin

#ifdef shared_pin
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define KEY  1008
#define SHMSZ 64
#define THRESH 1
#define SHM_PERM  0666 //(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
#endif

//#define SET_PRIORITY

#ifdef SET_PRIORITY
#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/resource.h>

int old_priority = 0;

#define MAX_PRIORITY -20

#define MIN_PRIORITY 19
#endif

//#define writefile

//#define  Migrate

#ifdef Migrate
FILE *Mp = NULL;
#endif

int  filled = 0;
cpu_set_t was_mask;

int shmid;
key_t key = KEY;
char *shm = NULL;
int shmFlags = IPC_CREAT | SHM_PERM;
int proc = 0, c = 0;
unsigned Pinned = 0;
char pinInitialized = 0;
//#ifdef writefile
    FILE *fp = NULL;
    FILE *tp = NULL;
    double tstart = 0.0;
unsigned long Beacon_gettime(unsigned long *nsecs)
{
     struct timespec tv = {0};
     FILE *fp = NULL;
    if(clock_gettime(CLOCK_REALTIME,&tv) != 0){
        fp = fopen("trialresults.txt","a+");
        fprintf(fp,"Che timer error\n");
        fclose(fp);
        return 0;
     }
    *nsecs = (unsigned long)(tv.tv_nsec);
     return ((unsigned long) tv.tv_sec);
}

double mcmc_gettime()
{
        struct timespec tv = {0};
        FILE *fp = NULL;
        if(clock_gettime(CLOCK_REALTIME,&tv) != 0){
                 fp = fopen("trialresults.txt","a+");
                 fprintf(fp,"Che timer error\n");
                 fclose(fp);
                 return 0;
        }
        return (((double) tv.tv_sec) +(double)(tv.tv_nsec/1.0e9));
}
//#endif


void Pin(double RD);

void PinInit_aux()
{
    printf("PinInit_aux()\n");
}

void PinInit()
{
    printf("PinInit()\n");
    PinInit_aux();
    if ((shmid = shmget(key, SHMSZ, shmFlags)) < 0) {//same as 0666 | IPC_CREAT
        perror("shmget");
        return ;
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return ;
    }

    if(sched_getaffinity(0,sizeof(was_mask),&was_mask)){
        perror("sched_getaffinity");
        printf("Error: sched_getaffinity(0,sizeof(was_mask),&was_mask)");
    }
#ifdef writefile
    fp = fopen("filled.txt","a+");
    if(fp){
        fprintf(fp,"1\n");
        fclose(fp);
    }else{
        printf("\n Could not open file filled.txt");
    }
#endif
#ifdef SET_PRIORITY
    //printf("max priority = %d",sched_get_priority_min(SCHED_OTHER));
    old_priority =  getpriority(PRIO_PROCESS, 0);
    if(old_priority == -1){printf("\n Getpriority failed ");}
    //printf("\n old priority = %d",old_priority);
#endif
   pinInitialized =1;

}


void Pin(double RD)
{
    printf("Hello Pin here\n");
    return;
    if(pinInitialized == 0){//433.milc was seg faulting because Pin() was called 
    //before  PinInit(). This is a WA for it.
	//PinInit();
    }
    cpu_set_t new_mask;
    c = sched_getcpu();
    if(c != -1){
        proc = c;
    }else{
        printf("\n Could not pin ");
        return ;
    }
    if(__sync_fetch_and_add(&shm[proc],1) >= THRESH){
	__sync_fetch_and_sub(&shm[proc],1);
	return;
    }
/*#ifdef SET_PRIORITY
    //printf("max priority = %d",sched_get_priority_min(SCHED_OTHER));
    old_priority =  getpriority(PRIO_PROCESS, 0);
    if(old_priority == -1){printf("\n Getpriority failed ");}
    //printf("\n old priority = %d",old_priority);
#endif*/

    CPU_ZERO(&new_mask);
    CPU_SET(proc, &new_mask);

     if(sched_setaffinity(0,sizeof(new_mask),&new_mask)){
    	__sync_fetch_and_sub(&shm[proc],1);
            perror("sched_setaffinity");
            printf("Error: sched_setaffinity(0,sizeof(new_mask),&new_mask)");
        }else{
            Pinned = 1;
#ifdef writefile
	     if(Pinned){
		fp = fopen("pin.txt","a+");
		if(fp){
	/*	unsigned long secs = 0, nsecs = 0;
		secs = Beacon_gettime(&nsecs);
		fprintf(fp,"Pin %ld %ld \n",secs,nsecs);
        */
		double ttime = mcmc_gettime();
		fprintf(fp,"Pin %lf\n",ttime);
		fclose(fp);
		}else{
		    printf("\n Could not open file pin.txt");
		}

		tstart = mcmc_gettime();
	     }
#endif
        }

#ifdef SET_PRIORITY
     if( setpriority(PRIO_PROCESS, 0, MAX_PRIORITY) == -1){
         printf("\n Setpriority in Pin failed");
     }
#endif
}

void UnPin()
{

    printf("Unpin Here\n");
    return;

    if(Pinned == 0)
        return;
#ifdef Migrate
    //cpu_set_t new_mask;
    c = sched_getcpu();
    if(c != -1){
        if (proc != c){
            Mp = fopen("Migrate.txt","a+");
            if(Mp){
                fprintf(Mp,"1");
                fclose(Mp);
            }else{
                printf("\n Could not open Migrate.txt");
            }
        }
    }
    return;
#endif

     if(shm != NULL){
#ifdef SET_PRIORITY
     if( setpriority(PRIO_PROCESS, 0, old_priority) == -1){
         printf("\n Setpriority in UnPin failed");
     }
#endif

         if(sched_setaffinity(0,sizeof(was_mask),&was_mask)){
                perror("sched_setaffinity");
                fprintf(stderr,"Error: sched_setaffinity(0,sizeof(was_mask),&was_mask)");
         }else{
            Pinned = 0;
#ifdef writefile
 	    tstart = mcmc_gettime() - tstart;
	    tp = fopen("time.txt","a+");
    	    if(tp){
        	fprintf(tp,"%lf\n",tstart);
       		 fclose(tp);
	    }else{
		printf("\n Could not open file time.txt");
	    }

 	   fp = fopen("pin.txt","a+");
	   if(fp){
 	        /* unsigned long secs = 0, nsecs = 0;
	         secs = Beacon_gettime(&nsecs);
	         fprintf(fp,"UnPin %ld %ld \n",secs,nsecs);
		*/
		double ttime = mcmc_gettime();
		fprintf(fp,"UnPin %lf\n",ttime);
       		 fclose(fp);
	    }else{
		printf("\n Could not open file pin.txt");
	    }
#endif
        }
       __sync_fetch_and_sub(&shm[proc],1);
     }
}


void PinFree()
{
        if(shm != NULL){
#ifdef writefile_free
   fp = fopen("pin.txt","a+");
  fprintf(fp,"PinFree\n");	
   fclose(fp);
#endif
	    if(Pinned == 1)
	      __sync_fetch_and_sub(&shm[proc],1);
            shmdt(shm);
            shm = NULL;
            shmctl(key, IPC_RMID, NULL);
        }
}
