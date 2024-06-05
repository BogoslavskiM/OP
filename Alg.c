#include "Var_str.c"

Coord *define_coord_by_dest(int x, int y, int dest_id)
{
    //  1 2 3
    //  4 0 5
    //  6 7 8

    Coord *coord = malloc(sizeof(Coord));
    coord->x = x;
    coord->y = y;

    switch (dest_id){
        case 1: case 2: case 3:{
            coord->y = y - 1;
            break;
        }
        case 6: case 7: case 8: {
            coord->y = y + 1;
            break;
        }
    }

    switch (dest_id) {
        case 1: case 4: case 6: {
            coord->x = x - 1;
            break;
        }
        case 3: case 5: case 8: {
            coord->x = x + 1;
            break;
        }
    }
    return coord;
}


int check_coord(Map *m, int x, int y)
{
    if (x < 0 || y < 0 || x > m->len - m->flight_size || y > m->heigh - m->flight_size)
    {
        return 0;
    }

    MapCell *cell;
    cell = get_cell(m, x, y);
    for (int i = x; i < x + m->flight_size; i++)
    {
        for (int j = y; j < y + m->flight_size; j++)
        {
            cell = get_cell(m, i, j);
            if (!cell->is_free)
                return 0;
        }
    }
    return 1;
}

int define_dest_id_by_coord_change(int x0, int y0, int x1, int y1)
{
    //  1 2 3
    //  4 0 5
    //  6 7 8

    int dx = x1 - x0;
    int dy = y1 - y0;

    if (dy == -1)
    {
        switch (dx)
        {
        case -1:
            return 1;
        case 0:
            return 2;
        case 1:
            return 3;
        }
    }
    else if (dy == 0)
    {
        switch (dx)
        {
        case -1:
            return 4;
        case 0:
            return 0;
        case 1:
            return 5;
        }
    }
    else if (dy == 1)
    {
        switch (dx)
        {
        case -1:
            return 6;
        case 0:
            return 7;
        case 1:
            return 8;
        }
    }
    printf("not_found dest_id");
    return -1;
}

int init_data_for_calculation(Map *map, int a_x, int a_y, int b_x, int b_y)
{
    if (a_x == b_x && a_y == b_y)
    {
        printf("wrong data");
        return 0;
    }

    if (!(check_coord(map, a_x, a_y) && check_coord(map, b_x, b_y)))
    {
        printf("point on obstical or out of range\n");
        return 0;
    }

    map->a_x = a_x;
    map->a_y = a_y;
    map->b_x = b_x;
    map->b_y = b_y;

    MapCell *c;
    for (int x = 0; x < map->len; x++)
    {
        for (int y = 0; y < map->heigh; y++)
        {
            c = get_cell(map, x, y);
            c->y = y;
            c->min_a_way_len = sqrt((a_x - x) * (a_x - x) + (a_y - y) * (a_y - y));
            c->min_b_way_len = sqrt((b_x - x) * (b_x - x) + (b_y - y) * (b_y - y));
            c->steps_from_a = -1;
            c->steps_from_b = -1;
            c->to_a_step_dest = 0;
            c->to_b_step_dest = 0;
        }
    }
    c = get_cell(map, a_x, a_y);
    c->steps_from_a = 0;

    c = get_cell(map, b_x, b_y);
    c->steps_from_b = 0;

    map->last_viewed_coords = malloc(sizeof(CoordQueue));
    map->found_coords = malloc(sizeof(CoordQueue));
    map->viewed_coords = malloc(sizeof(CoordQueue));
}

Coord *make_coord(MapCell *cell, int a_to_b)
{
    Coord *coord = malloc(sizeof(Coord));
    coord->x = cell->x;
    coord->y = cell->y;
    coord->a_to_b = a_to_b;
    coord->path_len = cell->steps_from_a + cell->steps_from_b;
    coord->to_a_step = cell->to_a_step_dest;
    coord->to_b_step = cell->to_b_step_dest;
    coord->next = NULL;
    return coord;
}

void gen_step(Map *m, int x, int y, int a_to_b)
{
    MapCell *go_to_cell;
    MapCell *curr_cell;
    curr_cell = get_cell(m, x, y);

    int dest_id;

    int point_is_changed;

    CoordQueue *queue = malloc(sizeof(queue));

    Coord *coord;

    for (int i = x - 1; i <= x + 1; i++)
    {

        for (int j = y - 1; j <= y + 1; j++)
        {
            if (check_coord(m, i, j))
            {
                point_is_changed = 0;
                go_to_cell = get_cell(m, i, j);
                dest_id = define_dest_id_by_coord_change(i, j, x, y);
                if (a_to_b)
                {
                    if (go_to_cell->steps_from_a == -1 || go_to_cell->steps_from_a > curr_cell->steps_from_a + 1 || go_to_cell->to_a_step_dest == 0)
                    {
                        go_to_cell->steps_from_a = curr_cell->steps_from_a + 1;
                        go_to_cell->to_a_step_dest = dest_id;
                        point_is_changed = 1;
                    }
                }
                else
                {
                    if (go_to_cell->steps_from_b == -1 || go_to_cell->steps_from_b > curr_cell->steps_from_b + 1 || go_to_cell->to_b_step_dest == 0)
                    {
                        go_to_cell->steps_from_b = curr_cell->steps_from_b + 1;
                        go_to_cell->to_b_step_dest = dest_id;
                        point_is_changed = 1;
                    }
                }
                if (point_is_changed)
                {
                    if (go_to_cell->steps_from_a != -1 && go_to_cell->steps_from_b != -1)
                    { // go_to_cell->to_a_step_dest != 0 && go_to_cell->to_b_step_dest != 0 &&
                        coord = make_coord(go_to_cell, a_to_b);
                        add_elem_to_que(m->found_coords, coord);
                    }
                    coord = make_coord(go_to_cell, a_to_b);
                    add_elem_to_que(m->last_viewed_coords, coord);

                    coord = make_coord(go_to_cell, a_to_b);
                    add_elem_to_que(m->viewed_coords, coord);
                }
            }
        }
    }
}

void gen_rout(Map *map)
{
    CoordQueue *queue;
    gen_step(map, map->a_x, map->a_y, 1);

    gen_step(map, map->b_x, map->b_y, 0);

    Coord *coord;
    while (!is_empty(map->last_viewed_coords)) // while (is_empty(map->found_coords)) - такой заголовок цикла, чтобы найти только кратчайший 
    { 
        coord = pop_elem(map->last_viewed_coords);
        gen_step(map, coord->x, coord->y, coord->a_to_b);
    }
}

void fill_in_way_points(Map* map, Coord* coord, int x0, int y0, int a_to_b){
    int prev_x = 0;
    int prev_y = 0;

    int prev_2_x = 0;
    int prev_2_y = 0;

    while (coord->x != x0 || coord->y != y0){
        if (prev_2_x == coord->x && prev_2_y == coord->y)
            break;
        prev_2_x = prev_x;
        prev_2_y = prev_y;
        prev_x = coord->x;
        prev_y = coord->y;
        MapCell *cell;
        cell = get_cell(map, coord->x, coord->y);
        cell->is_on_way = 1;
        if (a_to_b)
            coord = define_coord_by_dest(coord->x, coord->y, cell->to_a_step_dest);
        else
            coord = define_coord_by_dest(coord->x, coord->y, cell->to_b_step_dest);
    }
}

int fill_in_shortest_way_points(Map* map){
    Coord* coord;
    coord = get_first_elem(map->found_coords);
    printf("x:%d y:%d len:%d\n", coord->x, coord->y, coord->path_len);

    fill_in_way_points(map, coord, map->a_x, map->a_y, 1);
    fill_in_way_points(map, coord, map->b_x, map->b_y, 0);
}



int main()
{
    printf("reading_map...\n\n");
    Map *map = read_map();
    printf("%d %d\n", map->len, map->heigh);

    printf("initing_map_data...\n");
    init_data_for_calculation(map, 0, 0, map->len - 1, map->heigh - 1);

    map->flight_size = 2;

    gen_rout(map);

    fill_in_shortest_way_points(map);

    show_map(map);
}





// void tie_to_tail(const Coord *curr_elem, Coord *curr_continue)
// {

//     Coord *curr;
//     curr = curr_elem;

//     while (curr_elem->next != NULL)
//         curr_elem = curr_elem->next;

//     curr->next = curr_continue;

// }

// Coord *make_steps_from_point(Map *map, int x, int y, int a_to_b)
// {
//     int dests[8];
//     *dests = sort_destinations(map, x, y, a_to_b);
//     Coord *coord;
//     MapCell *cell;
//     MapCell *go_to_cell;
//     for (int i = 0; i < 8; i++){
//         coord = define_coord_by_dest(x, y, dests[i]);
//         if (check_coord(map, coord)){
//             go_to_cell = get_cell(map, coord->x, coord->y);
//             if (go_to_cell->next_cell != 0 && go_to_cell->prev_step_cell != 0){
//                 coord->path_len = go_to_cell->min_a_way_len + go_to_cell->min_b_way_len;
//                 tie_to_tail(map->head_ready_path_coord, coord);
//             }
//             else {

//             }
//         }
//     }
// }

// void make_route(Map *map)
// {
//     Coord *head_a_coord;
//     Coord *head_b_coord;
//     Coord *curr_a_coord;
//     Coord *curr_b_coord;
//     Coord *tail_a_coord;
//     Coord *tail_b_coord;
//     Coord *c;

//     head_a_coord = make_steps_from_point(map, map->a_x, map->a_y, 1);
//     head_b_coord = make_steps_from_point(map, map->b_x, map->b_y, 0);
//     tail_a_coord = head_a_coord;
//     tail_b_coord = head_b_coord;
//     curr_a_coord = head_a_coord;
//     curr_b_coord = head_b_coord;

//     while (curr_a_coord->next != NULL || curr_b_coord != NULL)
//     {
//         printf("xa ya xb yb: %d %d %d %d\n", curr_a_coord->x, curr_a_coord->y, curr_b_coord->x, curr_b_coord->y);
//         c = make_steps_from_point(map, curr_a_coord->x, curr_a_coord->y, 1);
//         if (c->next != NULL)
//             tie_to_tail(tail_a_coord, c->next);

//         c = make_steps_from_point(map, curr_b_coord->x, curr_b_coord->y, 0);
//         if (c->next != NULL)
//             tie_to_tail(tail_b_coord, c->next);

//         curr_a_coord = curr_a_coord->next;
//         curr_b_coord = curr_b_coord->next;
//         if (map->head_ready_path_coord != NULL)
//             printf("%d %d %d\n", map->head_ready_path_coord->path_len, map->head_ready_path_coord->x, map->head_ready_path_coord->y);
//     }
// }

// int dest;

//     Coord *head_coord;
//     head_coord = malloc(sizeof(Coord));

//     head_coord->x = x;
//     head_coord->y = y;

//     coord = head_coord;

//     MapCell *go_to_cell;
//     MapCell *curr_cell;

//     curr_cell = get_cell(map, x, y);

//     Coord *ready_point;
//     Coord *tail_ready_point;

//     ready_point = malloc(sizeof(Coord));

//     for (int i = 0; i < 8; i++)
//     {
//         dest = dest_ids[i];
//         if (dest != 0)
//         {
//             coord->next = define_coord_by_dest(x, y, dest);
//             go_to_cell = get_cell(map, coord->x, coord->y);
//             printf("x: %d y: %d dest: %d \n", x, y, dest);
//             if ((go_to_cell->prev_step_cell == 0) || (go_to_cell->steps_from_a > curr_cell->steps_from_a + 1))
//             {
//                 coord = coord->next;
//                 if (a_to_b)
//                 {
//                     go_to_cell->prev_step_cell = define_dest_id_by_coord_change(coord->x, coord->y, x, y);
//                     go_to_cell->steps_from_a = curr_cell->steps_from_a + 1;
//                 }
//                 else
//                 {
//                     go_to_cell->next_step_cell = define_dest_id_by_coord_change(coord->x, coord->y, x, y);
//                     go_to_cell->steps_from_b = curr_cell->steps_from_b + 1;
//                 }
//             }
//             printf("prev_step: %d next_step: %d \n", go_to_cell->prev_step_cell, go_to_cell->next_step_cell);
//             if (go_to_cell->next_step_cell != 0 && go_to_cell->prev_step_cell != 0)
//             {
//                 ready_point->x = go_to_cell->x;
//                 ready_point->y = go_to_cell->y;
//                 ready_point->path_len = go_to_cell->steps_from_a + go_to_cell->steps_from_b;
//                 if (map->head_ready_path_coord == NULL)
//                 {
//                     map->head_ready_path_coord = ready_point;
//                     tail_ready_point = map->head_ready_path_coord;
//                 }
//                 else
//                     tie_to_tail(tail_ready_point, ready_point);
//                 printf("added_to_tail: yes\n");
//             }
//             printf("\n");
//         }
//     }
//     coord->next = NULL;

//     return head_coord;