
#ifndef TIMER_IS_DEF
#define TIMER_IS_DEF

SDL_TimerID timer_set (Uint32 delay, int timer_type, void *param);

int timer_init(void);
void timer_set(Uint32 delay, void *param);
void sdl_push_event (void *param);

#endif
