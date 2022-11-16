#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "helper.h"

int timeout = 0;

void sig_handler(int signum)
{
	timeout = 1;
}

void new_line(int count)
{
	for(int i=0; i<count; i++)
		printf("\n");
}

// TODO:
// type 0 for normal values
// type 1 for scaled down values(0-1)
float get_random(int bot, int top)
{
	return (float)(rand()%((top*10)-(bot*10)+1)+bot)/10;
}

int get_state_manual(int item_dir, int dp_dir, int obs_sit, int ic)
{
	int state = 0;

	state  = item_dir * 9  * 15 * 2;
	state += dp_dir  * 15 * 2;
	state += obs_sit * 2;
	if(ic == true)
		state+=1;

	return state;
}

void scanf_wprint(char *beginning, int *input, char *ending)
{
	printf("%s",beginning);
	scanf("%d",input);
	printf("%s",ending);
}

void scanf_wprint_l(char *beginning, long *input, char *ending)
{
	printf("%s",beginning);
	scanf("%ld",input);
	printf("%s",ending);
}
