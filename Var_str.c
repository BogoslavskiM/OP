#include <stdio.h>


int max_d(int x, int y)
{
    if (x >= y) {
        return x;
    }
    else {
        return y;
    }
};


int min_d(int x, int y){
    return -1 * max(-x, -y);
}


typedef struct MapCell 
{
    int x;
    int is_free;
    int can_draw;
    char symbol;
    struct MapCell *next_cell;
    struct MapCell *previous_cell;
    

} MapCell;


MapCell* MapCell_new(int xx) {
    MapCell* cell = malloc(sizeof(MapCell));
    cell->x = xx;
    cell->is_free = 1;
    cell->can_draw = 1;
}


typedef struct MapLine
{
    int y;
    struct MapLine *next_line;
    struct MapLine *previous_line;
    struct Cell *head_cell;
    struct Cell *tail_cell;
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


MapLine* MapLine_new(int len, int curr_y) {
    struct MapLine *line = malloc(sizeof(MapLine));
    line->len = len;
    line->y = curr_y;

    line->head_cell = MapCell_new(0);
    struct MapCell *curr_cell = line->head_cell;
    for (int i = 1; i < len; i++){
        curr_cell->next_cell = MapCell_new(i);
        curr_cell->next_cell->previous_cell = curr_cell;
        curr_cell = curr_cell->next_cell;
    }
    line->tail_cell = curr_cell;
    return line;
}


MapCell* get_item(MapLine* line, int ind){
    if (ind > 0 && ind < line->len){
        struct MapCell *curr_cell = line->head_cell;
        for (int _ = 0; _ < ind; _++){
            curr_cell = curr_cell->next_cell;
        }
        return curr_cell;
    }
    else if (ind < 0 && ind > (-1) * line->len){
        struct MapCell *curr_cell = line->tail_cell;
        for (int _ = 0; _ < (-1)*ind; _++){
            curr_cell = curr_cell->previous_cell;
        }
        return curr_cell;
    }
    return NULL;
}


Map* Map_new(int h, int l)
{
    struct Map *map = malloc(sizeof(Map));
    map->heigh = h;
    map->len = l;
    map->head_line = MapLine_new(map->len, 0);
    struct MapLine *curr_line =map->head_line;
    for (int i = 1; i < map->heigh; i++)
    {
        curr_line->next_line = MapLine_new(map->len, i);
        curr_line->next_line->previous_line = curr_line;
        curr_line = curr_line->next_line;
    }
    map->tail_line = curr_line;
    map->flight_size = 1;

    return map;
}


MapLine* get_line(Map* map, int y_ind)
{
    if (y_ind >= 0 && y_ind < map->heigh){
        struct MapLine *curr_line = map->head_line;
        for (int i = 0; i < y_ind; i++){
            curr_line = curr_line->next_line;
        }
        return curr_line;
    }
    else if (y_ind < 0 && y_ind > (-1) * map->heigh){
        struct MapLine *curr_line = map->head_line;
        for (int i = 0; i < (-1)*y_ind; i++){
            curr_line = curr_line->previous_line;
        }
        return curr_line;
    }
}


MapCell* get_cell(Map* map, int x, int y)
{
    struct MapCell *cell = get_item(get_line(map, y), x);
    return cell;
}


void draw_point(MapCell* cell){
    cell->is_free = 0;
    cell->can_draw = 0;
}


void gen_map(Map* map, int f_s)
{
    map->flight_size = f_s;
    int obst_type;

    int obst_qw = (int)(rand() % 10 + 1);
    for (int i = 0; i <= obst_qw; i++){
        obst_type = (int)(rand() % 3);
        switch (obst_type)
        {
            case 0: {
                gen_point(map);
                break;
            }
            case 1: {
                gen_right_line(map);
                break;
            }
            case 2: {
                gen_left_line(map);
                break;
            }
        }
    }
}


void kill_space_around_point(Map* map, int x_pos, int y_pos){
    struct MapCell *cell = get_cell(map, x_pos, y_pos);
    for (int i = max_d(x_pos - map->flight_size, 0); i <= min_d(x_pos + map->flight_size, map->len - 1); i++){
        for (int j = max_d(y_pos - map->flight_size, 0); j <= min_d(y_pos + map->flight_size, map->heigh - 1); i++){
            cell = get_cell(map, i, j);
            cell->can_draw = 0;
        }
    }
}


void gen_point(Map* map){
    int x_pos = (int)(rand() % map->len);
    int y_pos = (int)(rand() % map->heigh);

    struct MapCell *cell = get_cell(map, x_pos, y_pos);
    
    if (cell->can_draw){
        draw_point(cell);
        kill_space_around_point(map, x_pos, y_pos);
    }
}


void gen_line(Map* map, int is_left)
{
    int x_start_pos = (int)(rand() % map->len);
    int y_start_pos = (int)(rand() % map->heigh);
    int max_len;


    if (is_left)
        max_len = max(x_start_pos, map->heigh - y_start_pos); 
    else
        max_len = max(map->len - x_start_pos, map->heigh - y_start_pos);

    int gen_len = (int)(rand() % max_len);

    struct MapCell *cell;

    int x_pos, y_pos;


    for (int i = 0; i <= gen_len; i++)
    {
        if (is_left)
            cell = get_cell(map, x_start_pos - i, y_start_pos + i);
        else
            cell = get_cell(map, x_start_pos + i, y_start_pos + i);
        
        if (cell->can_draw)
            draw_point(cell);  
    }
    for (int i = 0; i <= gen_len; i++)
    {
         if (is_left)
            cell = get_cell(map, x_start_pos - i, y_start_pos + i);
        else
            cell = get_cell(map, x_start_pos + i, y_start_pos + i);

        kill_space_around_point(map, x_start_pos - i, x_start_pos + i);
    }
}


void gen_line(Map* map)
{
    gen_line(map, 0);
}


void show_map(Map* map){
    MapLine *curr_line = map->head_line;
    MapCell *curr_cell;

    while (curr_line != NULL){
        curr_cell = curr_line->head_cell;

        while (curr_cell != NULL){
            if (curr_cell->is_free)
                printf(' ');
            else 
                printf('.');
        }
    }
}


