#pragma once

#include <vector>
#include <random>
#include <set>

#include "Node.h"
#include "Utils.h"

static std::mt19937 gRandomGenerator;

class Swarm
{
    std::vector<Node> m_nodes;
    
public:
    Swarm(){}
    
    void init( int swarmSize )
    {
        m_nodes.reserve(swarmSize);
        
        std::random_device   dev;
        std::seed_seq        seed({dev(), dev(), dev(), dev()});
        std::mt19937         rng(seed);

        union
        {
            Key key;
            std::array<uint8_t,sizeof(Key)> buffer{};
        };
        
        
        std::set<Key> keySet;
        for( int i=0; i<swarmSize; )
        {
            std::generate( buffer.begin(), buffer.end(), [&]
            {
                return std::uniform_int_distribution<std::uint8_t>(0,0xff) ( rng );
            });
            
            if ( keySet.find( key ) != keySet.end() )
            {
                LOG( "dublicated key" );
                exit(1);
                continue;
            }
            
            m_nodes.emplace_back(key);
            i++;
        }
    }
    
    void colonize()
    {
        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            it->enterSwarm( m_nodes.front() );
        }
    }
    
    void performIteration()
    {
        assert( m_nodes.size() > 0 );
        std::uniform_int_distribution<int> range(0, m_nodes.size()-1);
        
        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            it->prepareToIteration();
            
          gen_another:
            int randomNodeIndex = range(gRandomGenerator);
            auto rit = m_nodes.begin();
            std::advance( rit, randomNodeIndex );

            //LOG( " " << it->m_key << " " << rit->m_key )
            if ( it->m_key == rit->m_key )
            {
                goto gen_another;
            }
            it->tryToFindNode( *rit, true );
        }
    }

    void calcStatictic()
    {
        assert( m_nodes.size() > 0 );

        uint64_t foundCounter = 0;
        uint64_t requestCounter = 0;

        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            if ( it->m_isfound )
            {
                foundCounter++;
            }
            requestCounter += it->m_requestNumber;
        }
        
        LOG( "--- not found: " << m_nodes.size()-foundCounter-1 << " avg_requestNumber: " << requestCounter/m_nodes.size() << " requestNumber: " << requestCounter );
    }
};
