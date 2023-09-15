#include <iostream>
#include <chrono>
#include <thread>

#include "Swarm.h"


int calcCompleteness( Swarm& swarm )
{
    const size_t THREAD_NUM = 5;
    const size_t NUM = SWARM_SIZE/THREAD_NUM;
    std::thread threads[THREAD_NUM];
    int emptyCounters[THREAD_NUM];

    for( int i=0; i<THREAD_NUM; i++ )
    {
        threads[i] = std::thread( [&swarm,&emptyCounters,i]
        {
            emptyCounters[i] = 0;
            emptyCounters[i] = swarm.testFullCompleteness( i*NUM, i*NUM+NUM );
        });
    }

    for( int i=0; i<THREAD_NUM; i++ )
    {
        threads[i].join();
    }

    int emptyCounter = 0;
    for( int i=0; i<THREAD_NUM; i++ )
    {
        emptyCounter += emptyCounters[i];
    }

    LOG( "emptyCounter: " << emptyCounter );
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

//            auto emptyCounter = swarm.testFullCompleteness();
//            LOG( "emptyCounter: " << emptyCounter );
            calcCompleteness( swarm );
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    LOG( "Execution time: " << durationMs.count()/1000 << "." << durationMs.count()%1000 << " seconds." );

    return 0;
}
