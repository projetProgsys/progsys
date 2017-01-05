#define _XOPEN_SOURCE 600

#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>

#include "timer.h"

// Return number of elapsed µsec since... a long time ago
//microsecondes
static unsigned long get_time (void)
{
	struct timeval tv;

	gettimeofday (&tv ,NULL);

  // Only count seconds since beginning of 2016 (not jan 1st, 1970)
	tv.tv_sec -= 3600UL * 24 * 365 * 46;

	return tv.tv_sec * 1000000UL + tv.tv_usec;
}

#ifdef PADAWAN

pthread_mutex_t mutex; // permet de bloquer la réception de SIGALRM par les autres threads

/**
* The structure of one event
**/

typedef struct event{
	void *param; // l'evenement
	struct itimerval time; // le delay avant action en µs
	struct event *next; // l'évemenet suivant
	struct event *previous; // l'évènement précédent
} event;

struct event *first = NULL;

/**
* Create a new event
*@param first, the first event of the row
*@param new_event, the event to be added
**/
void next_event(event** first, event** new_event){
	if (*first == NULL){
		*first = *new_event;
		(*first)->next = NULL;
		(*first)->previous = NULL;
	}
	event *tmp = *first;
	while(tmp->next != NULL){
	  tmp = tmp->next;
	}
	(*new_event)-> next = NULL;
	(*new_event)->next = (tmp->next);
	tmp->next = *new_event;
	free(tmp);
}
   
/**
* Delete one event the first event of the row
*@param void
**/
void suppr_event(event* old_event){
	if (first  == NULL){
		perror("Impossible: empty row");
		exit(EXIT_FAILURE);
	}
	else{
		struct event *tmp = old_event;
		while (tmp->next != NULL){
			tmp = tmp->next;
		}
		tmp->next = NULL;
		free(tmp);
	}
}


/**
* the signal handler
*@param sig, the signal
**/
void sig_handler(int sig){
  printf("sdl_push_event(%p) appelée au temps %ld\n", first->param, get_time ());
  //printf("Thread appelé n°%d\n",(int) pthread_self());
}

/**
*daemon thread
*@param 
**/
void *daemon(void *param){

	struct sigaction s;
	s.sa_handler = sig_handler;
	sigemptyset(&s.sa_mask);
	sigaddset(&s.sa_mask, SIGALRM);
	sigaction(SIGALRM, &s, NULL);

	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask, SIGALRM);
	s.sa_flags = 0;

	while (1){
		sigsuspend(&mask);
	}
}

/**
*timer_init returns 1 if timers are fully implemented, 0 otherwise
*@param void
**/
int timer_init (void)
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	pthread_sigmask(SIG_BLOCK, &mask, NULL); // Bloque le signal pour les autres threads

	// Verrou
	pthread_mutex_init(&mutex, NULL);

	// Le thread et creation
	pthread_t thread;
	if (pthread_create(&thread, NULL, &daemon, NULL) !=0){
		perror("thread init error");
		exit(EXIT_FAILURE);
	}
		
 	return 0; // Implementation not ready
}

/**
*Temporizing function
*@param delay, the delay before triggering the event
*@param param, the event
**/
void timer_set (Uint32 delay, void *param)
{
	unsigned long delay_us = (unsigned long) delay * 1000; // delay in µs
	unsigned long delay_s = (unsigned long) delay%1000 * 1000; // delay in s
	unsigned long trigger = (unsigned long) delay * 1000 + get_time(); // how long before event
	
	printf("Time before triggering:%ld µs\n", trigger);

	struct event *new_event = malloc(sizeof(event));
	new_event->param = param;
	new_event->time.it_value.tv_sec = delay_s;
	new_event->time.it_value.tv_usec = delay_us;
	new_event->time.it_interval.tv_sec = 0;
	new_event->time.it_interval.tv_usec = 0;

	setitimer(ITIMER_REAL, &new_event->time, NULL);
}


#endif
