#include <stdbool.h>

#define return_state(REWARD, STATUS)	\
s.reward = (REWARD); 					\
s.done = (STATUS); 						\
return s

#define LEFTDIR  map[posy  ][posx-1]
#define TOPDIR   map[posy-1][posx  ]
#define RIGHTDIR map[posy  ][posx+1]
#define DOWNDIR  map[posy+1][posx  ]

#define MAPSIZEX 10
#define MAPSIZEY 10

typedef struct Status {
	bool done;
	float reward;
} status;

typedef struct DropPoint {
	int drop_point_x;
	int drop_point_y;
} drop_point;

typedef struct Item {
	int item_type;
	int item_x;
	int item_y;
} item;

typedef struct Obstacle {
	int obstacle_x;
	int obstacle_y;
} obstacle;

typedef struct Agent {
	int position_x;
	int position_y;
	int item_in_car;
	int action_count;
	float **q_table;
} agent;

typedef struct Field {
	int size_x;
	int size_y;
	char **map;
	item it;
	drop_point dp;
	obstacle *o;
    int obstacle_count;
} field;

typedef struct StateProperties {
	int number_of_states;
	int count;
	char (*names)[50];
    int *states;

} state_properties;

typedef struct Environment {
    int agent_count;
	agent a;
	field f;
	int state_count;
	int current_state;
} environment;

float getMax(environment e, int next_state);
int get_state_manual(int item_dir, int dp_dir, int obs_sit, int ic);
obstacle *init_obstacle(int count, int obs[count][2]);
field *init_field(int size_x, int size_y, int item_x, int item_y, int drop_point_x, int drop_point_y, int obstacle_count, obstacle o[obstacle_count]);
agent *init_agent(int x, int y, bool isItemInCar, int action_count, int number_of_states);
state_properties *init_state_properties(int count, char names[count][50], int states[count]);
environment *init_environment(agent *a, field *f, state_properties *sp, int agent_count);
int get_number_of_states(state_properties sp);
void train(environment *e, long iteration);
void test(environment *e);
