#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "ai.h"
#include "helper.h"

extern int timeout;

bool isOutside(agent a, field f)
{
    if(a.position_x >= f.size_x || a.position_y >= f.size_y || a.position_x < 0 || a.position_y < 0)
        return true;
    else
        return false;
}

bool isCrashed(agent a, field f)
{
    for(int i=0; i<f.obstacle_count; i++) {
        if(a.position_x == f.o[i].obstacle_x && a.position_y == f.o[i].obstacle_y)
            return true;
    }

    return false;
}

int get_number_of_states(state_properties sp)
{
	int state_count = 1;
	for(int i=0; i<sp.count; i++)
		state_count*=sp.states[i];

    return state_count;
}

int drop_point_direction(agent a, field f)
{
	int posx = a.position_x;
	int posy = a.position_y;

	int dp_x = f.dp.drop_point_x;
	int dp_y = f.dp.drop_point_y;

	int drop_point_dir = 0;
	if(posx == dp_x && posy == dp_y)
	{
		drop_point_dir = 0;
	}
	if(posx > dp_x && posy == dp_y)
	{
		drop_point_dir = 1;
	}
	if(posx > dp_x && posy > dp_y)
	{
		drop_point_dir = 2;
	}
	if(posx == dp_x && posy > dp_y)
	{
		drop_point_dir = 3;
	}
	if(posx < dp_x && posy > dp_y)
	{
		drop_point_dir = 4;
	}
	if(posx < dp_x && posy == dp_y)
	{
		drop_point_dir = 5;
	}
	if(posx < dp_x && posy < dp_y)
	{
		drop_point_dir = 6;
	}
	if(posx == dp_x && posy < dp_y)
	{
		drop_point_dir = 7;
	}
	if(posx > dp_x && posy < dp_y)
	{
		drop_point_dir = 8;
	}

	return drop_point_dir;
}

int item_point_direction(agent a, field f)
{
	int posx = a.position_x;
	int posy = a.position_y;

	int ip_x = f.it.item_x;
	int ip_y = f.it.item_y;

	int item_point_dir = 0;
	if(posx == ip_x && posy == ip_y)
	{
		item_point_dir = 0;
	}
	if(posx > ip_x && posy == ip_y)
	{
		item_point_dir = 1;
	}
	if(posx > ip_x && posy > ip_y)
	{
		item_point_dir = 2;
	}
	if(posx == ip_x && posy > ip_y)
	{
		item_point_dir = 3;
	}
	if(posx < ip_x && posy > ip_y)
	{
		item_point_dir = 4;
	}
	if(posx < ip_x && posy == ip_y)
	{
		item_point_dir = 5;
	}
	if(posx < ip_x && posy < ip_y)
	{
		item_point_dir = 6;
	}
	if(posx == ip_x && posy < ip_y)
	{
		item_point_dir = 7;
	}
	if(posx > ip_x && posy < ip_y)
	{
		item_point_dir = 8;
	}

	return item_point_dir;
}

int obstacle_pattern(agent a, field f, char map[MAPSIZEY][MAPSIZEX])
{
	int obstacle_placement = 0;

	int posx = a.position_x;
	int posy = a.position_y;


	if(LEFTDIR != '#' && TOPDIR != '#' && RIGHTDIR != '#' && DOWNDIR != '#')
	{
		obstacle_placement = 0;
	}


	if(LEFTDIR == '#' && TOPDIR != '#' && RIGHTDIR != '#' && DOWNDIR != '#')
	{
		obstacle_placement = 1;
	}
	if(LEFTDIR != '#' && TOPDIR == '#' && RIGHTDIR != '#' && DOWNDIR != '#')
	{
		obstacle_placement = 2;
	}
	if(LEFTDIR != '#' && TOPDIR != '#' && RIGHTDIR == '#' && DOWNDIR != '#')
	{
		obstacle_placement = 3;
	}
	if(LEFTDIR != '#' && TOPDIR != '#' && RIGHTDIR != '#' && DOWNDIR == '#')
	{
		obstacle_placement = 4;
	}


	if(LEFTDIR == '#' && TOPDIR == '#' && RIGHTDIR != '#' && DOWNDIR != '#')
	{
		obstacle_placement = 5;
	}
	if(LEFTDIR == '#' && TOPDIR != '#' && RIGHTDIR == '#' && DOWNDIR != '#')
	{
		obstacle_placement = 6;
	}
	if(LEFTDIR == '#' && TOPDIR != '#' && RIGHTDIR != '#' && DOWNDIR == '#')
	{
		obstacle_placement = 7;
	}
	if(LEFTDIR != '#' && TOPDIR == '#' && RIGHTDIR == '#' && DOWNDIR != '#')
	{
		obstacle_placement = 8;
	}
	if(LEFTDIR != '#' && TOPDIR == '#' && RIGHTDIR != '#' && DOWNDIR == '#')
	{
		obstacle_placement = 9;
	}
	if(LEFTDIR != '#' && TOPDIR != '#' && RIGHTDIR == '#' && DOWNDIR == '#')
	{
		obstacle_placement = 10;
	}


	if(LEFTDIR == '#' && TOPDIR == '#' && RIGHTDIR == '#' && DOWNDIR != '#')
	{
		obstacle_placement = 11;
	}
	if(LEFTDIR == '#' && TOPDIR == '#' && RIGHTDIR != '#' && DOWNDIR == '#')
	{
		obstacle_placement = 12;
	}
	if(LEFTDIR == '#' && TOPDIR != '#' && RIGHTDIR == '#' && DOWNDIR == '#')
	{
		obstacle_placement = 13;
	}
	if(LEFTDIR != '#' && TOPDIR == '#' && RIGHTDIR == '#' && DOWNDIR == '#')
	{
		obstacle_placement = 14;
	}

	if(LEFTDIR == '#' && TOPDIR == '#' && RIGHTDIR == '#' && DOWNDIR == '#')
	{
		printf("WRONG OBSTACLE PLACEMENT\n");
		exit(1);
	}

	return obstacle_placement;
}

status return_status(float reward, bool done){
	status s;

	s.reward = reward;
	s.done = done;

	return s;
}

// TODO: better way to calculate state in loop
int get_state(environment e)
{
	int state = 0;

	int item_dir = item_point_direction(e.a,e.f);
	int dp_dir = drop_point_direction(e.a,e.f);
	// int obs_sit = 0;
	// obstacle_pattern(a, f, map);

	state  = item_dir * 9 * 2;
	state += dp_dir * 2;
	if(e.a.item_in_car == true)
		state+=1;

	return state;
}

int get_state_debug(environment e)
{
	int state = 0;

	int item_dir = item_point_direction(e.a,e.f);
	int dp_dir = drop_point_direction(e.a,e.f);
	// int obs_sit = 0;
	// obstacle_pattern(a, f, map);

	printf("item_dir => %d || drop_point_dir => %d\n",item_dir, dp_dir);
	state  = item_dir * 9 * 2;
	state += dp_dir * 2;
	if(e.a.item_in_car == true)
		state+=1;

	return state;
}

char action_string[6][20] = {"GO DOWN","GO UP","GO LEFT","GO RIGHT","PICK UP","DROP OFF"};
status make_action(environment *e, int action)
{
	int posx = e->a.position_x;
	int posy = e->a.position_y;

	int item_pickup_x = e->f.it.item_x;
	int item_pickup_y = e->f.it.item_y;

	int item_dropoff_x = e->f.dp.drop_point_x;;
	int item_dropoff_y = e->f.dp.drop_point_y;;

	int size_x = e->f.size_x;
	int size_y = e->f.size_y;

	switch (action)
	{
		case 0:	// GO DOWN
			if(posy == size_y - 1) {
				return return_status(-10, false);
			} else {
				e->a.position_x = posx;
				e->a.position_y = posy + 1;
				if(e->a.item_in_car == true) {
					e->f.it.item_x = item_pickup_x;
					e->f.it.item_y = item_pickup_y + 1;
				}
				return return_status(-1, false);
			}
			break;
		case 1:	// GO UP
			if(posy == 0) {
				return return_status(-10, false);
			} else {
				e->a.position_x = posx;
				e->a.position_y = posy - 1;
				if(e->a.item_in_car == true) {
					e->f.it.item_x = item_pickup_x;
					e->f.it.item_y = item_pickup_y - 1;
				}
				return return_status(-1, false);
			}
			break;
		case 2:	// GO LEFT
			if(posx == 0) {
				return return_status(-10, false);
			} else {
				e->a.position_x = posx - 1;
				e->a.position_y = posy;
				if(e->a.item_in_car == true) {
					e->f.it.item_x = item_pickup_x - 1;
					e->f.it.item_y = item_pickup_y;
				}
				return return_status(-1, false);
			}
			break;
		case 3:	// GO RIGHT
			if(posx == size_x - 1) {
				return return_status(-10, false);
			} else {
				e->a.position_x = posx + 1;
				e->a.position_y = posy;
				if(e->a.item_in_car == true) {
					e->f.it.item_x = item_pickup_x + 1;
					e->f.it.item_y = item_pickup_y;
				}
				return return_status(-1, false);
			}
			break;
		case 4:	// PICK UP ITEM
			if(e->a.item_in_car == true) {
				return return_status(-10, false);
			} else if(item_pickup_x != posx || item_pickup_y != posy) {
				return return_status(-10, false);
			} else if(item_pickup_x == posx && item_pickup_y == posy && e->a.item_in_car == false){
				e->a.item_in_car = true;
				return return_status(1, false);
			}
			break;
		case 5:	// DROP OFF ITEM
			if(e->a.item_in_car == false) {
				return return_status(-10, false);
			} else if(item_dropoff_x != posx || item_dropoff_y != posy) {
				e->f.it.item_x = posx;
				e->f.it.item_y = posy;
				e->a.item_in_car = false;
				return return_status(-10, false);
			} else if(item_dropoff_x == posx && item_dropoff_y == posy && e->a.item_in_car == true){
				e->f.it.item_x = posx;
				e->f.it.item_y = posy;
				return return_status(20, true);
			}
			break;
	}
	return return_status(-420, true);
}

void draw_state(environment *e, int scale)
{
	int posx = e->a.position_x;
	int posy = e->a.position_y;

	int item_pickup_x = e->f.it.item_x;
	int item_pickup_y = e->f.it.item_y;

	int item_dropoff_x = e->f.dp.drop_point_x;
	int item_dropoff_y = e->f.dp.drop_point_y;

	int state = get_state(*e);
	for(int i=0; i<e->f.size_y; i++) {
		for(int j=0; j<e->f.size_x; j++) {
			e->f.map[i][j] = ' ';
		}
	}

	if(e->a.item_in_car == true)
		e->f.map[posy][posx] = 'L';
	else if(posx == item_pickup_x && posy == item_pickup_y){
		e->f.map[posy][posx] = 'E';
		e->f.map[item_pickup_y][item_pickup_x] = 'E';
	} else {
		e->f.map[posy][posx] = 'I';
		e->f.map[item_pickup_y][item_pickup_x] = '_';
	}
	e->f.map[item_dropoff_y][item_dropoff_x] = '#';
	printf("[item_dropoff_y]=%d || [item_dropoff_x]=%d\n",item_dropoff_y,item_dropoff_x);
	printf("[posy		   ]=%d || [posx		  ]=%d\n",posy,posx);
	printf("[item_pickup_y ]=%d || [item_pickup_x ]=%d\n",item_pickup_y,item_pickup_x);

	for(int i=0; i<(e->f.size_y*2)+1; i++) {
		if(i%2==0) {
			for(int j=0; j<e->f.size_x; j++) {
				if(j==e->f.size_x - 1) {
					printf(".%*c.", (scale+2)*2, ' ');
				} else {
					printf(".%*c", (scale+2)*2, ' ');
				}
			}
			new_line(scale);
		} else {
			for(int j=0; j<e->f.size_x; j++) {
				if(i/2 == posy - 1 && j == posx) {
					printf("%*c%4.2f%*c", scale, ' ', e->a.q_table[state][1], scale, ' ');
				} else if(i/2 == posy + 1 && j == posx) {
					printf("%*c%4.2f%*c", scale, ' ', e->a.q_table[state][0], scale, ' ');
				} else if(i/2 == posy && j == posx - 1) {
					printf("%*c%4.2f%*c", scale, ' ', e->a.q_table[state][2], scale, ' ');
				} else if(i/2 == posy && j == posx + 1) {
					printf("%*c%4.2f%*c", scale, ' ', e->a.q_table[state][3], scale, ' ');
				} else
					printf("%*c%c%*c", scale+2, ' ', e->f.map[i/2][j], scale+2, ' ');
			}
			new_line(scale);
		}
	}
}

void draw_state_test(environment e, int scale)
{
	int posx = e.a.position_x;
	int posy = e.a.position_y;

	int item_pickup_x = e.f.it.item_x;
	int item_pickup_y = e.f.it.item_y;

	int item_dropoff_x = e.f.dp.drop_point_x;
	int item_dropoff_y = e.f.dp.drop_point_y;

	int state = get_state(e);

	for(int i=0; i<e.f.size_y; i++) {
		for(int j=0; j<e.f.size_x; j++) {
			e.f.map[i][j] = ' ';
		}
	}

	if(e.a.item_in_car == true)
		e.f.map[posy][posx] = 'L';
	else if(posx == item_pickup_x && posy == item_pickup_y){
		e.f.map[posy][posx] = 'E';
		e.f.map[item_pickup_y][item_pickup_x] = 'E';
	} else {
		e.f.map[posy][posx] = 'I';
		e.f.map[item_pickup_y][item_pickup_x] = '_';
	}
	e.f.map[item_dropoff_y][item_dropoff_x] = '#';


	for(int i=0; i<(e.f.size_y*2)+1; i++) {
		if(i%2==0) {
			for(int j=0; j<e.f.size_x; j++) {
				if(j==e.f.size_x - 1) {
					printf(".%*c.", (scale+2)*2, ' ');
				} else {
					printf(".%*c", (scale+2)*2, ' ');
				}
			}
			new_line(scale);
		} else {
			for(int j=0; j<e.f.size_x; j++) {
				if(i/2 == posy - 1 && j == posx) {
					printf("%*c%4.2f%*c", scale, ' ', e.a.q_table[state][1], scale, ' ');
				} else if(i/2 == posy + 1 && j == posx) {
					printf("%*c%4.2f%*c", scale, ' ', e.a.q_table[state][0], scale, ' ');
				} else if(i/2 == posy && j == posx - 1) {
					printf("%*c%4.2f%*c", scale, ' ', e.a.q_table[state][2], scale, ' ');
				} else if(i/2 == posy && j == posx + 1) {
					printf("%*c%4.2f%*c", scale, ' ', e.a.q_table[state][3], scale, ' ');
				} else
					printf("%*c%c%*c", scale+2, ' ', e.f.map[i/2][j], scale+2, ' ');
			}
			new_line(scale);
		}
	}
}

obstacle *init_obstacle(int count, int obs[count][2])
{
	obstacle *o = malloc(count * sizeof(obstacle));

	return o;
}

agent *init_agent(int x, int y, bool isItemInCar, int action_count, int number_of_states)
{
	agent *a = malloc(sizeof(agent));
	a->position_x = x;
	a->position_y = y;
	a->item_in_car = isItemInCar;
	a->action_count = action_count;

	a->q_table = calloc(number_of_states, sizeof(float*));
	for(int i=0; i<number_of_states; i++) {
		a->q_table[i] = calloc(a->action_count, sizeof(float));
	}

	printf("END OF AGENT INITIATION\n");

	return a;
}

field *init_field(int size_x, int size_y, int item_x, int item_y, int drop_point_x, int drop_point_y, int obstacle_count, obstacle o[obstacle_count])
{
	field *f = malloc(sizeof(field));
	f->size_x = size_x;
	f->size_y = size_y;
	f->obstacle_count = obstacle_count;

	f->map = calloc(f->size_y, sizeof(char*));
	for(int i=0; i<f->size_y; i++) {
		f->map[i] = calloc(f->size_x, sizeof(char));
	}

	for(int i=0; i<f->size_y; i++) {
		for(int j=0; j<f->size_x; j++) {
			f->map[i][j] = ' ';
		}
	}

	f->it.item_x = item_x;
	f->it.item_y = item_y;

	f->dp.drop_point_x = drop_point_x;
	f->dp.drop_point_y = drop_point_y;

	if(f->obstacle_count != 0) {
		f->o = calloc(f->obstacle_count, sizeof(obstacle));
		for(int i=0; i<f->obstacle_count; i++) {
			f->o->obstacle_x = o[i].obstacle_x;
			f->o->obstacle_y = o[i].obstacle_y;
		}
	}

	printf("END OF FIELD INITIATION\n");
	return f;
}

state_properties *init_state_properties(int count, char names[count][50], int states[count])
{
	state_properties *sp = malloc(sizeof(state_properties));

	sp->count = count;

	sp->names = calloc(count,sizeof(char[50]));
	for(int i=0; i<count; i++) {
		strcat(sp->names[i],names[i]);
	}

	sp->states = calloc(count,sizeof(int));
	for(int i=0; i<count; i++) {
		sp->states[i] = states[i];
	}

	printf("END OF STATE PROPERTIES INITIATION\n");
	return sp;
}

environment *init_environment(agent *a, field *f, state_properties *sp, int agent_count)
{
	environment *e = malloc(sizeof(environment));

	e->agent_count = agent_count;
	e->a = *a;
	e->f = *f;

	e->state_count = sp->number_of_states;

	printf("END OF ENVIRONMENT INITIATION\n");

	return e;
}

int getIndexofMax(environment e)
{
	float argmax = e.a.q_table[e.current_state][0];
	int argmax_index = 0;
	for(int i=1; i<e.a.action_count; i++)
	{
		if(argmax<e.a.q_table[e.current_state][i]) {
			argmax = e.a.q_table[e.current_state][i];
			argmax_index = i;
		}
	}
	return argmax_index;
}

float getMax(environment e, int next_state)
{
	float argmax = e.a.q_table[next_state][0];
	for(int i=1; i<e.a.action_count; i++)
	{
		if(argmax<e.a.q_table[next_state][i]) {
			argmax = e.a.q_table[next_state][i];
		}
	}

	return argmax;
}

void csv_q_table(environment e)
{
	FILE *fp = fopen("q_table_c.csv","w+");
	for(int i=0; i<e.state_count; i++) {
		for(int j=0; j<e.a.action_count; j++) {
			fprintf(fp, "%.2f, ", e.a.q_table[i][j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void draw_state_value(environment e)
{
	int state = get_state(e);
	printf("%d\n",state);
	printf("\t%5.3f\n",e.a.q_table[state][1]);
	printf("%5.3f",e.a.q_table[state][2]);
	printf("\t\t%5.3f",e.a.q_table[state][3]); printf("\t\t%5.3f",e.a.q_table[state][4]); printf("\t\t%5.3f\n",e.a.q_table[state][5]);
	printf("\t%5.3f\n\n",e.a.q_table[state][0]);
}

void train(environment *e, long iteration)
{
	status s;
	float epsilon = 0.5;
	float alpha = 0.1;
	float gamma = 0.6;

	int state = 0;
	int next_state = 0;
	int action = 0;
	int trycount = 0;
	int episode = 0;

	int map_scaler = 1;

	signal(SIGALRM,sig_handler); // Register signal handler

	alarm(4);  // Scheduled alarm after 4 seconds

	float next_state_max = 0;
	float r = 0;

	// int check = 1;
	for(long i=0; i<iteration; i++)
	{
		e->a.position_x = rand()%e->f.size_x;
		e->a.position_y = rand()%e->f.size_y;
		e->f.it.item_x = rand()%e->f.size_x;
		e->f.it.item_y = rand()%e->f.size_y;
		e->f.dp.drop_point_x = rand()%e->f.size_x;
		e->f.dp.drop_point_y = rand()%e->f.size_y;

		while(1) {
			if((e->f.dp.drop_point_x != e->f.it.item_x || e->f.dp.drop_point_y != e->f.it.item_y ) && (e->f.dp.drop_point_x != e->a.position_x || e->f.dp.drop_point_y != e->a.position_y ))
			{
				break;
			} else {
				e->f.dp.drop_point_x = rand()%e->f.size_x;
				e->f.dp.drop_point_y = rand()%e->f.size_y;
			}
		}
		obstacle o[1];
		field *trainf = init_field(e->f.size_x, e->f.size_y, e->f.it.item_x, e->f.it.item_y, e->f.dp.drop_point_x, e->f.dp.drop_point_y, 0, o);
		e->f = *trainf;

		s.done = false;
		printf("\e[1;1H\e[2J");
		printf("episode => %d\n", episode);
		episode++;
		while(!s.done) {
			// printf("e->f.size_x=%d e->f.size_y=%d\ne->f.it.item_x=%d e->f.it.item_y=%d\ne->f.dp.drop_point_x=%d e->f.dp.drop_point_y=%d  \n",e->f.size_x, e->f.size_y, e->f.it.item_x, e->f.it.item_y, e->f.dp.drop_point_x, e->f.dp.drop_point_y);

			state = get_state(*e);
			e->current_state = state;
			r = (float)(rand()%11)/10;
			if(r<epsilon) {
				action = rand()%6;
			} else {
				action = getIndexofMax(*e);
			}
			if(timeout || episode>(iteration)) {
				printf("\e[1;1H\e[2J");
				printf("\n");
	/*
				if(check == 1) {
					epsilon = 0;
					printf("ok\n");
					csv_q_table(*e);
					printf("notok\n");
					check = 0;
				}
	 */
				if(r<epsilon)
					printf("action => %d\t\tRANDOM\n\n",action);
				else
					printf("action => %d\n\n",action);
				printf("REWARD => %.2f\nSTATUS => %d\n\n\n",s.reward,s.done);

				draw_state(e, map_scaler);

				printf("\ns.reward = %.2f\n",s.reward);
				printf("state => %d\n",e->current_state);
				printf("episode => %d\n",episode);
				printf("action => %s\n",action_string[action]);
				printf("value => %.3f\n",e->a.q_table[state][action]);
			}

			s = make_action(e, action);
			next_state = get_state(*e);
			next_state_max = getMax(*e, next_state);
			e->a.q_table[e->current_state][action] = e->a.q_table[e->current_state][action] + alpha * (s.reward + gamma * next_state_max - e->a.q_table[e->current_state][action]);

			if(timeout || episode>iteration) {
				printf("%.2f %.2f %.2f %.2f %.2f %.2f\n",e->a.q_table[e->current_state][0],e->a.q_table[e->current_state][1],e->a.q_table[e->current_state][2],e->a.q_table[e->current_state][3],e->a.q_table[e->current_state][4],e->a.q_table[e->current_state][5]);
				printf("q_table[%5d][%d] = (1 - alpha) * q_table[%d][%d] + alpha * (reward + gamma * next_state_max)\n",e->current_state,action,e->current_state,action);
				printf("        %f	  = (1 - %.2f) *      %f 	  +  %.3f * (  %.2f +  %.2f * %f   )\n",e->a.q_table[e->current_state][action],alpha, e->a.q_table[e->current_state][action], alpha,s.reward, gamma, next_state_max);
				printf("	%f	  = 	%f   * 	  %f	  +	 %f * (  %f )\n",e->a.q_table[e->current_state][action], 1-alpha, e->a.q_table[e->current_state][action], alpha, (s.reward + (gamma * next_state_max)));

				usleep(300000);
				//getchar();
			}
			trycount++;

		}
		printf("trycount => %d\n\n\n",trycount);
		alarm(4);  // Alarm resets after successful iteration
		trycount = 0;
		timeout = 0;

		free(trainf);
	}
	csv_q_table(*e);
}


void test(environment *e)
{
	int map_scaler = 1;

	status s;

	int state = 0;
	int action = 0;
	int test_count = 10, test_type = 0;
	int map_size = 10;

	int episode = 0;

	scanf_wprint("Enter the number of test to run: ", &test_count, "\t(default: 10)\n");
	scanf_wprint("Enter '0' for the random tests, or '1' for manual tests: ", &test_type, "\t(default: 0(RANDOM))\n");
	scanf_wprint("Enter map size: ",&map_size,"\t(default: 10)\n");

	for(int i=0; i<test_count;i++)
	{
		switch (test_type)
		{
			case 0:
				e->a.position_x = rand()%e->f.size_x;
				e->a.position_y = rand()%e->f.size_y;
				e->f.it.item_x = rand()%e->f.size_x;
				e->f.it.item_y = rand()%e->f.size_y;
				e->f.dp.drop_point_x = rand()%e->f.size_x;
				e->f.dp.drop_point_y = rand()%e->f.size_y;

				while(1) {
					if((e->f.dp.drop_point_x != e->f.it.item_x || e->f.dp.drop_point_y != e->f.it.item_y ) && (e->f.dp.drop_point_x != e->a.position_x || e->f.dp.drop_point_y != e->a.position_y ))
					{
						break;
					} else {
						e->f.dp.drop_point_x = rand()%e->f.size_x;
						e->f.dp.drop_point_y = rand()%e->f.size_y;
					}
				}
				break;
			case 1:
				scanf_wprint("Enter the coordinates(x,y) of the item:\n",&e->a.position_x,"");
				scanf_wprint("",&e->a.position_y,"\t(default: (0,9))");
				break;
		}
		obstacle o[1];
		field *trainf = init_field(e->f.size_x, e->f.size_y, e->f.it.item_x, e->f.it.item_y, e->f.dp.drop_point_x, e->f.dp.drop_point_y, 0, o);
		e->f = *trainf;

		s.done = false;

		while(!s.done)
		{
			state = get_state(*e);
			e->current_state = state;

			action = getIndexofMax(*e);

			printf("\e[1;1H\e[2J");
			printf("EPISODE ===> %d\n\n",episode);
			draw_state_test(*e, map_scaler);

			s = make_action(e, action);

			usleep(300000);
		}
		e->a.item_in_car = false;
		episode++;
		free(trainf);
	}
}
