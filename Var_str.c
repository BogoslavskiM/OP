#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

char saving_map_file[] = "/Users/makar/Vs_code/OP/OP/utils_dir/map.txt";

int max_d(int x, int y)
{
    if (x >= y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

int min_d(int x, int y)
{
    return -1 * max_d(-x, -y);
}

typedef struct Coord
{
    int x;
    int y;
    int to_a_step; // направление шага 1...8
    int to_b_step; // направление шага 1...8
    int a_to_b;    // искомая точка 0 или 1
    int path_len;
    struct Coord *next;
} Coord;

typedef struct CoordQueue
{
    struct Coord *head;
    struct Coord *tail;
} CoordQueue;

Coord *get_first_elem(CoordQueue *queue)
{
    if (queue->head->next != NULL)
    {
        Coord *last;
        last = queue->head;
        queue->head = queue->head->next;

        last->next = NULL;

        queue->tail->next = last;

        queue->tail = queue->tail->next;

        return last;
    }
    else if (queue->head != NULL)
        return queue->head;
    return NULL;
}

Coord *pop_elem(CoordQueue *queue)
{
    Coord *curr;

    if (queue->head == NULL)
        return NULL;

    if (queue->head->next == NULL)
        queue->tail = NULL;

    curr = queue->head;
    queue->head = queue->head->next;

    curr->next = NULL;

    return curr;
}

int is_empty(CoordQueue *queue)
{
    if (queue->head == NULL)
        return 1;
    return 0;
}

Coord *get_min_item(CoordQueue *queue)
{

    if (queue->head->next == NULL)
        return queue->head;

    Coord *curr = malloc(sizeof(Coord));
    Coord *curr_min = malloc(sizeof(Coord));

    curr->next = queue->head->next;

    while (curr->next != NULL)
    {
        if (curr_min->path_len > curr->path_len)
        {
            curr_min->path_len = curr->path_len;
            curr_min->x = curr->x;
            curr_min->y = curr->y;
            curr_min->to_a_step = curr->to_a_step;
            curr_min->to_b_step = curr->to_b_step;
            curr->a_to_b = 0;
        }
        curr = curr->next;
    }

    return curr_min;
}

void add_elem_to_que(CoordQueue *queue, Coord *elem)
{
    elem->next = NULL;
    printf("start_adding\n");
    if (queue->head == NULL)
    {
        printf("add_ekem 1\n");
        queue->head = elem;
        queue->tail = elem;
        printf("add_ekem 2\n");
    }
    else
    {
        printf("add_ekem 3\n");
        queue->tail->next = elem;
        printf("add_ekem 4\n");
        queue->tail = queue->tail->next;
        printf("add_ekem 5\n");
    }
    printf("elem_added\n");
}

typedef struct MapCell
{
    int x;
    int y; // using only in way determination
    int is_free;
    int can_draw;

    int is_on_way; // using to save on way_coords and show map with way

    int steps_from_a;   // how many steps we need for come from a to this point
    int steps_from_b;   // how many steps we need for come from b to this point
    int to_a_step_dest; // num from 1 to 8
    int to_b_step_dest; // num from 1 to 8

    int min_a_way_len; // calculating by formula, using in shortest way cearching alg
    int min_b_way_len; // calculating by formula, using in shortest way cearching alg

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

    int a_x;
    int a_y;

    int b_x;
    int b_y;

    int flight_size;

    struct CoordQueue *last_viewed_coords;
    struct CoordQueue *found_coords;

} Map;

MapCell *MapCell_new(int xx)
{
    MapCell *cell = malloc(sizeof(MapCell));
    cell->x = xx;
    cell->is_free = 1;
    cell->can_draw = 1;
    cell->is_on_way = 0;
    return cell;
}

MapLine *MapLine_new(int len, int curr_y)
{
    MapLine *line = malloc(sizeof(MapLine));
    line->len = len;
    line->y = curr_y;

    line->head_cell = MapCell_new(0);
    MapCell *curr_cell = line->head_cell;
    for (int i = 1; i < len; i++)
    {
        curr_cell->next_cell = MapCell_new(i);
        curr_cell->next_cell->previous_cell = curr_cell;
        curr_cell = curr_cell->next_cell;
    }
    line->tail_cell = curr_cell;
    return line;
}

Map *Map_new(int h, int l)
{
    Map *map = malloc(sizeof(Map));
    map->heigh = h;
    map->len = l;

    MapLine *curr_line = MapLine_new(map->len, 0);
    map->head_line = curr_line;

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

MapCell *get_item(MapLine *line, int ind)
{
    MapCell *curr_cell;
    if (ind >= 0 && ind < line->len)
    {
        curr_cell = line->head_cell;
        for (int _ = 0; _ < ind; _++)
            curr_cell = curr_cell->next_cell;
        return curr_cell;
    }
    else if (ind < 0 && ind > (-1) * line->len)
    {
        curr_cell = line->tail_cell;
        for (int _ = 0; _ < (-1) * ind; _++)
            curr_cell = curr_cell->previous_cell;
        return curr_cell;
    }
    return NULL;
}

MapLine *get_line(Map *map, int ind)
{
    MapLine *curr_line;
    if ((ind >= 0) && (ind < map->heigh))
    {
        curr_line = map->head_line;
        for (int _ = 0; _ < ind; _++)
            curr_line = curr_line->next_line;
        return curr_line;
    }
    else if ((ind < 0) && (ind > (-1) * map->heigh))
    {
        curr_line = map->tail_line;
        for (int _ = 0; _ < (-1) * ind; _++)
            curr_line = curr_line->previous_line;
        return curr_line;
    }
    return NULL;
}

MapCell *get_cell(Map *map, int x, int y)
{
    MapCell *cell = get_item(get_line(map, y), x);
    return cell;
}

void draw_point(MapCell *cell)
{
    cell->is_free = 0;
    cell->can_draw = 0;
}

void kill_space_around_point(Map *map, int x_pos, int y_pos)
{
    MapCell *cell;
    for (int i = max_d(x_pos - map->flight_size, 0); i <= min_d(x_pos + map->flight_size, map->len - 1); i++)
    {
        for (int j = max_d(y_pos - map->flight_size, 0); j <= min_d(y_pos + map->flight_size, map->heigh - 1); j++)
        {
            cell = get_cell(map, i, j);
            cell->can_draw = 0;
        }
    }
}

void gen_diag_line(Map *map, int is_left)
{
    int x_start_pos = (int)((rand() % (map->len - (2 * map->flight_size))) + map->flight_size);
    int y_start_pos = (int)((rand() % (map->heigh - (2 * map->flight_size))) + map->flight_size);
    int max_len;

    if (is_left)
        max_len = min_d(x_start_pos, map->heigh - 1 - y_start_pos);
    else
        max_len = min_d(map->len - x_start_pos, map->heigh - y_start_pos) - 1;

    max_len -= map->flight_size;

    if (max_len <= 0)
        return;

    int gen_len = (int)(rand() % max_len);

    if (gen_len == 0)
        gen_len = max_len;

    MapCell *cell;

    for (int i = 0; i < gen_len; i++)
    {
        if (is_left)
            cell = get_cell(map, x_start_pos - i, y_start_pos + i);
        else
            cell = get_cell(map, x_start_pos + i, y_start_pos + i);

        if (cell->can_draw)
            draw_point(cell);
    }
    for (int i = 0; i < gen_len; i++)
    {
        if (is_left)
            kill_space_around_point(map, x_start_pos - i, y_start_pos + i);
        else
            kill_space_around_point(map, x_start_pos + i, y_start_pos + i);
    }
}

void gen_ort_line(Map *map, int vert)
{
    int x_start_pos = (int)((rand() % (map->len - (2 * map->flight_size))) + map->flight_size);
    int y_start_pos = (int)((rand() % (map->heigh - (2 * map->flight_size))) + map->flight_size);
    int max_len;

    if (vert)
        max_len = map->heigh - y_start_pos - 1 - map->flight_size;
    else
        max_len = map->len - x_start_pos - 1 - map->flight_size;

    if (max_len <= 0)
        return;

    int gen_len = (int)(rand() % max_len);

    if (gen_len == 0)
        gen_len = max_len;

    MapCell *cell;

    for (int i = 0; i < gen_len; i++)
    {
        if (vert)
            cell = get_cell(map, x_start_pos, y_start_pos + i);
        else
            cell = get_cell(map, x_start_pos + i, y_start_pos);

        if (cell->can_draw)
            draw_point(cell);
    }
    for (int i = 0; i < gen_len; i++)
    {
        if (vert)
            kill_space_around_point(map, x_start_pos, y_start_pos + i);
        else
            kill_space_around_point(map, x_start_pos + i, y_start_pos);
    }
}

void gen_point(Map *map)
{
    int x_pos = (int)(rand() % (map->len - 2 * map->flight_size)) + map->flight_size;
    int y_pos = (int)(rand() % (map->heigh - 2 * map->flight_size)) + map->flight_size;

    MapCell *cell = get_cell(map, x_pos, y_pos);

    if (cell->can_draw)
    {
        draw_point(cell);
        kill_space_around_point(map, x_pos, y_pos);
    }
}

void gen_map(Map *map, int f_s)
{
    map->flight_size = f_s;
    int obst_type;

    // int max_obst_qw = (int)sqrt((int)((map->len - 2 * map->flight_size) * (map->heigh - 2 * map->flight_size)));
    int max_obst_qw = (int)((map->len * map->heigh));

    srand(time(NULL));
    int obst_qw = (int)(rand() % (max_obst_qw / 2)) + (max_obst_qw / 2);

    int prev_obst = -1;
    for (int i = 0; i <= obst_qw; i++)
    {
        obst_type = (int)(rand() % 5);
        if (obst_type == prev_obst)
            obst_type = (obst_type + 1) % 5;
        prev_obst = obst_type;
        switch (obst_type)
        {
        case 0:
        {
            gen_point(map);
            // break;
        }
        case 1:
        {
            gen_diag_line(map, 0);
            break;
        }
        case 2:
        {
            gen_diag_line(map, 1);
            break;
        }
        case 3:
        {
            gen_ort_line(map, 0);
            break;
        }
        case 4:
        {
            gen_ort_line(map, 1);
            break;
        }
        }
    }
}

void show_map(Map *map)
{
    MapLine *curr_line;
    MapCell *curr_cell;

    curr_line = map->head_line;
    printf("\n\n");
    for (int _ = 0; _ < map->len + 2; _++)
        printf("_");
    printf("\n");
    while (curr_line != NULL)
    {
        printf("|");
        curr_cell = curr_line->head_cell;
        while (curr_cell != NULL)
        {
            if (curr_cell->is_free)
                if (curr_cell->is_on_way)
                    printf("#");
                else
                    printf(" ");
            else
                printf(".");
            curr_cell = curr_cell->next_cell;
        }
        printf("|\n");
        curr_line = curr_line->next_line;
    }

    for (int _ = 0; _ < map->len + 2; _++)
        printf("_");
    printf("\n\n");
}

void save_map(Map *map)
{
    MapLine *curr_line;
    MapCell *curr_cell;

    FILE *f = fopen(saving_map_file, "w");

    curr_line = map->head_line;
    while (curr_line != NULL)
    {
        curr_cell = curr_line->head_cell;
        while (curr_cell != NULL)
        {
            if (curr_cell->is_free)
                fputs(" ", f);
            else
                fputs(".", f);
            curr_cell = curr_cell->next_cell;
        }
        curr_line = curr_line->next_line;
        if (curr_line != NULL)
            fputs("\n", f);
    }
    fclose(f);
}

Map *read_map()
{
    FILE *f = fopen(saving_map_file, "r");

    fseek(f, 0L, SEEK_END);
    long size = ftell(f) + 1;
    fseek(f, 0L, SEEK_SET);

    char *buf = (char *)malloc(sizeof(char) * size);

    fread(buf, 1, size, f);
    fclose(f);

    int h = 1;
    for (int i = 0; i < size; i++)
        if ('\n' == buf[i])
            h++;

    int l = size / (h - 1) - 2;

    Map *my_map = Map_new(h, l);
    MapCell *cell;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < l; j++)
        {
            cell = get_cell(my_map, j, i);
            if (buf[(l + 1) * i + j] == '.')
                cell->is_free = 0;
        }
    }
    free(buf);
    return my_map;
}

// тут написаны просто тесты...
// int main()
// {
//     MapCell *c = MapCell_new(10);
//     printf("%d\n", c->x);

//     MapLine *l = MapLine_new(5, 5);
//     printf("%d %d\n", l->len, l->y);

//     Map *m = Map_new(10, 10);
//     printf("%d %d\n", m->heigh, m->len);

//     MapCell *c2 = get_item(l, 3);
//     printf("%d\n", c2->x);

//     MapLine *l2 = get_line(m, 5);
//     printf("%d %d\n", l2->len, l2->y);

//     MapCell *c3 = get_cell(m, 5, 5);
//     printf("%d\n", c3->x);

//     draw_point(c3);
//     if (c3->is_free || c3->can_draw)
//         printf("test failed\n");
//     else
//         printf("test passed\n");

//     int len, heigh, size;
//     len = 30;
//     heigh = 20;
//     size = 3;

//     printf("creating_map...\n");
//     Map *map = Map_new(heigh, len);

//     printf("generating_obsticals...\n");
//     gen_map(map, size);

//     printf("showing_map...\n");
//     show_map(map);

//     return 1;
// }