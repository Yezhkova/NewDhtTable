#include <iostream>

#include "Swarm.h"

#define SWARM_SIZE 1000

#define ITER_NUMBER 100

int main()
{
    LOG( "init" );
    Swarm swarm;
    swarm.init( SWARM_SIZE );
    LOG( "initialized" );
    
    swarm.colonize();
    LOG( "colonized" );

    for( int i=0; i<ITER_NUMBER; i++ )
    {
        swarm.performIteration();
    }

    return 0;
}
