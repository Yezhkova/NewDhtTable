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
    
    Node& bootstrapNode() { return m_nodes[0]; }

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
            
            if ( keySet.find( key ) != keySet.end() ) // HERE: where did we put the newly generated key in keySet?
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

    std::vector<Node>::iterator generate(std::uniform_int_distribution<int>& range)
    {
        int randomNodeIndex = range(gRandomGenerator);
        auto rit = m_nodes.begin();
        std::advance( rit, randomNodeIndex );
        return rit;
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
        uint64_t tooManyCounter = 0;
        uint64_t requestCounter = 0;

        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            if ( it->m_isFound )
            {
                foundCounter++;
            }
            if ( it->tooManyRequests() )
            {
                tooManyCounter++;
            }
            requestCounter += it->m_requestNumber;
        }
        
        LOG( "--- not found: " << m_nodes.size()-foundCounter-1 << "(" << tooManyCounter << ") avg_requestNumber: " << requestCounter/m_nodes.size() << " requestNumber: " << requestCounter );
    }

    void testCompleteness()
    {
        const int bucketThreashould = 1;
        uint64_t addedCounter = 0;
        uint64_t fullCounter = 0;
        uint64_t totalCounter = 0;

        const int TEST_NODE_IDX = 5001;

        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            if ( m_nodes[TEST_NODE_IDX].m_key == it->m_key )
                continue;

            int backetIndex;
            bool isFull;
            if ( it->justFind( m_nodes[TEST_NODE_IDX], backetIndex, isFull ) )
            {
                if ( backetIndex >= bucketThreashould )
                {
                    addedCounter++;
                    totalCounter++;
                }
            }
            else
            {
                if ( backetIndex >= bucketThreashould )
                {
                    totalCounter++;
                    if ( isFull ) fullCounter++;
                }
            }
        }

        LOG( "Completeness: " << addedCounter << " in " << totalCounter << "-" << fullCounter << "=" << totalCounter-fullCounter );
    }
};
