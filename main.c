#include "CommandParser/libcli.h"
#include "graph.h"
#include <stdio.h>
#include <string.h>
#include "CommandParser/libcli.h"
#include "nmcli.h"

extern graph_t * build_first_topo();
graph_t *topo = NULL;

int main(){
    nw_init_cli();
    topo = build_first_topo();
    start_shell();
    return 0;
}
