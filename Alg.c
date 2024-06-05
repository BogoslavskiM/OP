#include "Var_str.c"

Coord *define_coord_by_dest(int x, int y, int dest_id)
{
    //  1 2 3
    //  4 0 5
    //  6 7 8

    Coord *coord = malloc(sizeof(Coord));
    coord->x = x;
    coord->y = y;

    switch (dest_id)
    {
    case 1:
    case 2:
    case 3:
    {
        coord->y = y - 1;
        break;
    }
    case 6:
    case 7:
    case 8:
    {
        coord->y = y + 1;
        break;
    }
    }

    switch (dest_id)
    {
    case 1:
    case 4:
    case 6:
    {
        coord->x = x - 1;
        break;
    }
    case 3:
    case 5:
    case 8:
    {
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

CoordQueue *sort_destinations(Map *map, int x, int y, int a_to_b)
{
    int dests_ids[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int dest_id = 0; dest_id <= 8; dest_id++)
    {
        Coord *coord;
        coord = define_coord_by_dest(x, y, dest_id);
        if (check_coord(map, coord->x, coord->y))
        {
            dests_ids[dest_id] = dest_id;
        }
        free(coord);
    }

    for (int _ = 0; _ <= 8; _++)
    {
        for (int i = 0; i < 8; i++)
        {
            int tmp;
            MapCell *cell1;
            MapCell *cell2;
            Coord *coord;
            coord = define_coord_by_dest(x, y, dests_ids[i]);
            cell1 = get_cell(map, coord->x, coord->y);
            free(coord);
            coord = define_coord_by_dest(x, y, dests_ids[i + 1]);
            cell2 = get_cell(map, coord->x, coord->y);
            free(coord);
            if (a_to_b)
            {
                if (cell1->min_b_way_len > cell2->min_b_way_len)
                {
                    tmp = dests_ids[i];
                    dests_ids[i] = dests_ids[i + 1];
                    dests_ids[i + 1] = tmp;
                }
            }
            else
            {
                if (cell1->min_a_way_len > cell2->min_a_way_len)
                {
                    tmp = dests_ids[i];
                    dests_ids[i] = dests_ids[i + 1];
                    dests_ids[i + 1] = tmp;
                }
            }
        }
    }
    CoordQueue *queue;
    queue = malloc(sizeof(CoordQueue));
    int dest;
    for (int i = 0; i <= 8; i++)
    {
        dest = dests_ids[i];
        if (dest != 0)
        {
            Coord *coord;
            coord = define_coord_by_dest(x, y, dest);
            printf("130 %d %d %d %d\n", i, dest, coord->x, coord->y);
            add_elem_to_que(queue, coord);
            printf("132 %d %d\n", i, dest);
        }
    }
    return queue;
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
            c->min_a_way_len = ((a_x - x) * (a_x - x) + (a_y - y) * (a_y - y)); // убрал квадраты, чтобы исключить ошибку округления
            c->min_b_way_len = ((b_x - x) * (b_x - x) + (b_y - y) * (b_y - y));
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

    CoordQueue *queue;

    Coord *coord;
    queue = sort_destinations(m, x, y, a_to_b);

    while (!is_empty(queue))
    {
        coord = pop_elem(queue);
        if (check_coord(m, coord->x, coord->y))
        {
            point_is_changed = 0;
            go_to_cell = get_cell(m, coord->x, coord->y);
            dest_id = define_dest_id_by_coord_change(coord->x, coord->y, x, y);
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
                    free(coord);
                    coord = make_coord(go_to_cell, a_to_b);
                    add_elem_to_que(m->found_coords, coord);
                }
                free(coord);
                coord = make_coord(go_to_cell, a_to_b);
                add_elem_to_que(m->last_viewed_coords, coord);
            }
        }
        free(coord);
        
    }    
    free(queue);
}

void gen_rout(Map *map)
{
    gen_step(map, map->a_x, map->a_y, 1);
    gen_step(map, map->b_x, map->b_y, 0);
    Coord *coord;
    while (!is_empty(map->last_viewed_coords)) // while (is_empty(map->found_coords)) - такой заголовок цикла, чтобы найти только кратчайший
    {
        coord = pop_elem(map->last_viewed_coords);
        gen_step(map, coord->x, coord->y, coord->a_to_b);
    }
    free(coord);
}

void fill_in_way_points(Map *map, Coord *coord, int x0, int y0, int a_to_b)
{
    int prev_x = 0;
    int prev_y = 0;

    int prev_2_x = 0;
    int prev_2_y = 0;
    MapCell *cell;

    do
    {
        if (prev_2_x == coord->x && prev_2_y == coord->y)
            break;
        prev_2_x = prev_x;
        prev_2_y = prev_y;
        prev_x = coord->x;
        prev_y = coord->y;

        for (int i = 0; i < map->flight_size; i++)
        {
            for (int j = 0; j < map->flight_size; j++)
            {
                cell = get_cell(map, coord->x + i, coord->y + j);
                cell->is_on_way = 1;
            }
        }

        cell = get_cell(map, coord->x, coord->y);

        if (a_to_b)
            coord = define_coord_by_dest(coord->x, coord->y, cell->to_a_step_dest);
        else
            coord = define_coord_by_dest(coord->x, coord->y, cell->to_b_step_dest);
    } while (coord->x != x0 || coord->y != y0);

    for (int i = 0; i < map->flight_size; i++)
    {
        for (int j = 0; j < map->flight_size; j++)
        {
            cell = get_cell(map, min_d(x0 + i, map->heigh - 1), min_d(y0 + j, map->heigh - 1));
            cell->is_on_way = 1;
        }
    }
}

int fill_in_shortest_way_points(Map *map)
{
    Coord *coord;
    coord = get_first_elem(map->found_coords);
    printf("x:%d y:%d len:%d\n", coord->x, coord->y, coord->path_len);

    fill_in_way_points(map, coord, map->a_x, map->a_y, 1);
    fill_in_way_points(map, coord, map->b_x, map->b_y, 0);
}

int main()
{
    // printf("reading_map...\n\n");
    Map *map = read_map();
    // printf("%d %d\n", map->len, map->heigh);

    // printf("initing_map_data...\n");
    map->flight_size = 4;
    init_data_for_calculation(map, 0, 0, map->len - map->flight_size, map->heigh - map->flight_size);

    gen_rout(map);

    fill_in_shortest_way_points(map);

    show_map(map);
}
