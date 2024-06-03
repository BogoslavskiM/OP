#include <stdio.h>
#include <Var_str.h>
#include <Gen_file.h>


int main() {
    Map *map = gen_map_call();

    show_map(map);
}