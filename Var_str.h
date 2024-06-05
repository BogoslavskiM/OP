#ifndef _VAR_STR_H_
#define _VAR_STR_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

int max_d(int x, int y);

int min_d(int x, int y);

typedef struct MapCell
{
    int x;
    int is_free;
    int can_draw;
    struct MapCell *next_cell;
    struct MapCell *previous_cell;

} MapCell;

typedef struct MapLine
{
    int y;
    struct MapLine *next_line;
    struct MapLine *previous_line;
    struct MapCell *head_cell;
    struct MapCell *tail_cell;
    int len;
} MapLine;

typedef struct Map
{
    struct MapLine *head_line;
    struct MapLine *tail_line;

    int heigh;
    int len;

    int flight_size;

} Map;

MapCell *MapCell_new(int xx);

MapLine *MapLine_new(int len, int curr_y);

Map *Map_new(int h, int l);

MapCell *get_item(MapLine *line, int ind);

MapLine *get_line(Map *map, int ind);

MapCell *get_cell(Map *map, int x, int y);

void draw_point(MapCell *cell);

void kill_space_around_point(Map *map, int x_pos, int y_pos);

void gen_line(Map *map, int is_left);

void gen_point(Map *map);

void gen_map(Map *map, int f_s);

void show_map(Map *map);

#endif