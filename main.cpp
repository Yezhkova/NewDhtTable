#include <iostream>
#include <chrono>

#include "Swarm.h"


int main1() {
    auto startTime = std::chrono::high_resolution_clock::now();

    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::microseconds durationMs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::cout << "Execution time: " << durationMs.count() << " microseconds." << std::endl;

    return 0;
}

int main()
{
    LOG( "init" );
    Swarm swarm;
    swarm.init( SWARM_SIZE );
    LOG( "initialized" );

    swarm.colonize();
    LOG( "colonized" );

    auto startTime = std::chrono::high_resolution_clock::now();

    for( int i=0; i<ITER_NUMBER; i++ )
    {
        //LOG( "\nIteration: " << i )
        swarm.performIteration();
        //swarm.calcStatictic();
        //swarm.testCompleteness();
        if ( (i+1)%100 == 0 )
        {
            LOG( "\nIteration: " << i )
            swarm.calcStatictic();
            swarm.testFullCompleteness();
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    LOG( "Execution time: " << durationMs.count()/1000 << "." << durationMs.count()%1000 << " seconds." );

    return 0;
}
