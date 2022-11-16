#pragma once

void sig_handler(int signum);
void new_line(int count);
int get_state_manual(int item_dir, int dp_dir, int obs_sit, int ic);
void scanf_wprint(char *beginning, int *input, char *ending);
void scanf_wprint_l(char *beginning, long *input, char *ending);
float get_random(int bot, int top);
