#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <unistd.h>

#include "ai.h"

int main()
{
	srand(time(NULL));
	obstacle o;
	o.obstacle_x = 0;
	o.obstacle_y = 0;
//	char names[4][50] = {"ItemDir","DropPointDir","ObstaclePattern","ItemOnOff"};
//	int states[4] = { 9, 9, 15, 2 };

	char names[3][50] = {"ItemDir","DropPointDir","ItemOnOff"};
	int states[3] = { 9, 9, 2 };

	state_properties *sp = init_state_properties(3, names, states);

	int number_of_states  = get_number_of_states(*sp);
	sp->number_of_states = number_of_states;

	printf("nos => %d\n",number_of_states);
	agent *a = init_agent(0, 0, false, 6, number_of_states);

	field *f = init_field(10, 10, 0, 9, 9, 0, 0, &o);

	environment *e = init_environment(a, f, sp, 1);

	train(e, 100000);
	test(e);

	return 0;
}
