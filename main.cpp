#include <iostream>

#include "Swarm.h"

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
        LOG( "Iteration: " << i )
        swarm.performIteration();
        swarm.calcStatictic();
    }

    return 0;
}
