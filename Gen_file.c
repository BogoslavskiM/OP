#include <stdio.h>
#include <Var_str.h>


int gen_map_call()
{
    int len, heigh, size;

    printf("Введите размер поля\n x y:");
    scanf("%d %d", len, heigh);

    Map* map = Map_new(heigh, len);

    printf("Введите размер летательного аппарата:\n size:");

    gen_map(map, size);
    
    return map;
}