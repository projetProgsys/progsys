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
struct event{
	void *param; // l'evenement
	struct itimerval time; // le delay avant action en µs
	struct event *next; // l'évemenet suivant
	struct event *previous; // l'évènement précédent
}

static event *first.previous = NULL; // pour le première évènement, le prédécesseur est vide

/**
* Create a new event
*@param first, the first event of the row
*@param new_event, the event to be added
**/
void next_event(event **first, event **new_event){
	if (*first == NULL){
		*first = *new_event;
		(*first)->next = NULL;
	}
	event *tmp = *first;
	while(tmp->next != NULL){
		tmp = tmp -> next;
	}
	(*new_event)-> next = NULL;
	(*new_event)->next = (tmp->next);
	tmp->next = *new_event;
	free(tmp);
}
/**
* Delete the first event of the row
*@param void
**/
void suppr_event(void){
	if (*first == NULL){
		printf("Impossible: empty row");
	}
	else{
		struct event *tmp = *first;
		while (tmp->next != NULL){
			tmp = tmp->next;
		}
		tmp->next = NULL;
		free(tmp);
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
	if (pthred_create (&thread, NULL, demon, NULL) !=0):{
		perror("thread init error");
		exit(EXIT_FAILURE);
	}
		
 	return 0; // Implementation not ready
}

/**
*daemon thread
*@param 
**/
void *demon(void *param){

	struct sigaction s;
	s.sa_handler = sig_handler;
	sigemptyset(&s.sa_mask);
	sigaddset(&s.sa_mask, SIGALRM);
	sigaction(SIGALRM, &s, NULL);

	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask, SIGALRM);
	s.sa_flags = 0;

	while (1):{
		sigsuspend(&mask);
		pthread_self();
	}
}

/**
* the signal handler
*@param sig, the signal
**/
void sig_handler(int sig){
	printf("sdl_push_event(%p) appelée au temps %ld\n", param, get_time ());
}

/**
*Temporizing function
*@param delay, the delay before triggering the event
*@param param, the event
**/
void timer_set (Uint32 delay, void *param)
{
	//sdl_push_event(*param) -> usage
	//TODO
	/*unsigned long delay_µs = (unsigned long) delay * 1000; // delay in µs
	unsigned long trigger = (unsigned long) delay * 1000 + get_time(); // how long before event
	now = (unsigned long)get_time();
	printf("Time before triggering:%ld µs\n", trigger);
	struct event new_event = malloc(sizeof(struct event));
	new_event(NULL, new_event);
	new_event->param = param;
	new_event->time = delay_µs;*/


}

#endif
