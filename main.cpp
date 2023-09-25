#include <iostream>
#include <chrono>
#include <thread>

#include "Swarm.h"


int calcCompleteness( Swarm& swarm )
{
    const size_t THREAD_NUM = 12;
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

void makeSwarm( Swarm& swarm, size_t swarmSize )
{
    auto startTime = std::chrono::high_resolution_clock::now();
    
    LOG( "init" );
    swarm.init( swarmSize );
    LOG( "initialized" );
    
    swarm.colonize();
    LOG( "colonized" );
    
//    swarm.performX();
//    swarm.performIteration();
//    swarm.calcStatistic();

    for( int i=0; i<ITER_NUMBER; i++ )
    {
        //LOG( "\nIteration: " << i )
        swarm.performIteration();
        //swarm.calcStatistic();
        //swarm.testCompleteness();
        
        if ( (i+1)%100 == 0 )
        {
            LOG( "\nIteration: " << i )
            swarm.calcStatistic();
            
            //            auto emptyCounter = swarm.testFullCompleteness();
            //            LOG( "emptyCounter: " << emptyCounter );
            calcCompleteness( swarm );
        }
    }

//    swarm.performX();
//    swarm.performIteration();
//    swarm.calcStatistic();
//
//    calcCompleteness( swarm );

    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    LOG( "Make execution time: " << durationMs.count()/1000 << "." << durationMs.count()%1000 << " seconds." );
}

void saveSwarm( Swarm& swarm )
{
    std::ofstream os( "swarm.bin", std::ios::binary );
    cereal::BinaryOutputArchive archive( os );
    archive( swarm );
    LOG("saved");
}

#define NEW_NODE_NUMBER 1

void loadSwarm( Swarm& swarm )
{
    std::ifstream os( "swarm.bin", std::ios::binary );
    cereal::BinaryInputArchive archive( os );
    archive( swarm );
}

//void runTest( Swarm& swarm )
//{
//    auto startTime = std::chrono::high_resolution_clock::now();
//
//    swarm.addNewParticipants(NEW_NODE_NUMBER);
//
//    swarm.testNewNodes(NEW_NODE_NUMBER);
//    swarm.calcStatistic();
//
//    auto endTime = std::chrono::high_resolution_clock::now();
//    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
//    LOG( "Test execution time: " << durationMs.count()/1000 << "." << durationMs.count()%1000 << " seconds." );
//}

int main()
{
    //gHideLog = true;
    
    Swarm swarm;
    
    makeSwarm( swarm, SWARM_SIZE+NEW_NODE_NUMBER*10 );
//    saveSwarm(swarm);
//    loadSwarm(swarm);

//    gHideLog = false;
//    LOG( "-1-: " << swarm.size() );
//    runTest(swarm);
//    LOG( "-2-: " << swarm.size() );

    return 0;
}
