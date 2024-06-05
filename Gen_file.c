#include "Var_str.c"

Map *gen_map_call()
{
    int len, heigh, size;

    printf("Введите размер поля\n x y:");
    scanf("%d %d", &len, &heigh);

    Map *map = Map_new(heigh, len);

    printf("Введите размер летательного аппарата:\n size:");

    scanf("%d", &size);

    gen_map(map, size);

    save_map(map);
    show_map(map);

    return map;
}

// just some tests
int main()
{
    Map *my_map = gen_map_call();

    Map *my_read_map = read_map();

    show_map(my_read_map);
}