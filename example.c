/*
 *       Filename:  example.c
 *         Author:  Øystein Schønning-Johansen (oystein), oysteijo@gmail.com
 */
#include "progress.h"
#include <stdlib.h>
#include <unistd.h>
int main( int argc, char *argv[] )
{
    int maxvalue = 100;
    if(  argc > 1 )
        maxvalue = atoi( argv[1]);

    for ( int i = 0; i <= maxvalue; i++ ){
        progress_ascii( i, maxvalue, "(%4d/%4d) ", i, maxvalue);
        usleep(50000);
    }

    return 0;
}

