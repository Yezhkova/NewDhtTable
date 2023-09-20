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
    int m_testCounter = 0;
    
public:
    Swarm(){}
    
    Node& bootstrapNode() { return m_nodes[0]; }
    
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( m_nodes );
    }
    
    size_t size() const { return m_nodes.size(); }
    
    void init( int swarmCapacity )
    {
        m_nodes.reserve(swarmCapacity);
        std::cerr << "size: " << m_nodes.size() << " capacity: " << m_nodes.capacity() << "\n";

        union
        {
            Key key;
            std::array<uint8_t,sizeof(Key)> buffer{};
        };
        
        
        std::set<Key> keySet;
        for( int i=0; i<SWARM_SIZE; )
        {
            std::generate( buffer.begin(), buffer.end(), [&]
            {
                return std::uniform_int_distribution<std::uint8_t>(0,0xff) ( gRandomGenerator );
            });
            
            if ( keySet.find( key ) != keySet.end() ) // HERE: where did we put the newly generated key in keySet?
            {
                LOG( "dublicated key" );
                exit(1);
                continue;
            }
            
            m_nodes.emplace_back(i,key);
            i++;
        }
    }
    
    void addNewParticipants( int newNodeNumber )
    {
        union
        {
            Key key;
            std::array<uint8_t,sizeof(Key)> buffer{};
        };
        
        std::seed_seq sd{ 1000000 };
        auto g = std::mt19937(sd);
        
        auto nodesBegin = m_nodes.begin();
        LOG( "size: " << m_nodes.size() << " capacity: " << m_nodes.capacity() )
        
        size_t initSwarmSize = m_nodes.size();
        for( int i=0; i<newNodeNumber; )
        {
            std::generate( buffer.begin(), buffer.end(), [&]
            {
                return std::uniform_int_distribution<std::uint8_t>(0,0xff) ( g );
            });
            
            for( size_t i=0; i<m_nodes.size(); i++ )
            {
                if ( m_nodes[i].m_key == key )
                {
                    LOG( m_nodes[i].m_key );
                    LOG( key );
                    LOG( "duplicated key: " << i );
                    exit(1);
                    continue;
                }
            };
            
            m_nodes.emplace_back(initSwarmSize+i,key);
            
            LOG( "m_nodes.back(): " << m_nodes.back().m_key )
            
            m_nodes.back().findNode( m_nodes[0], m_nodes[0] );
            m_nodes[0].findNode( m_nodes.back(), m_nodes.back() );

            i++;
        }
        
        LOG( "size: " << m_nodes.size() << " capacity: " << m_nodes.capacity() )
        assert( nodesBegin == m_nodes.begin() );
    }

    void testNewNodes(int newNodeNumber)
    {
        for( size_t i=SWARM_SIZE; i<SWARM_SIZE+newNodeNumber; i++ )
        {
            int foundCounter=0;
            for(  size_t j=0; j<SWARM_SIZE; j++ )
            {
                if ( m_nodes[j].findNodeInBuckets( m_nodes[i] ) )
                {
                    foundCounter++;
                }
            }
            LOG( "i: " << i << ", foundCounter: " << foundCounter );
        }

        for(  size_t j=0; j<SWARM_SIZE; j++ )
        {
            auto& node = m_nodes[j];
            node.prepareToIteration();
        }

        for( size_t i=SWARM_SIZE; i<SWARM_SIZE+newNodeNumber; i++ )
        {
            for(  size_t j=0; j<SWARM_SIZE; j++ )
            {
                m_nodes[j].findNode( m_nodes[i], m_nodes[j] );
            }
        }
    }
    
    void restorePointers( int newNodeNumber )
    {
        m_nodes.reserve( m_nodes.size() + newNodeNumber );

        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            it->restorePointers( m_nodes );
        }
    }
    
    void colonize()
    {
        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            // add bootstrap node to my buckets
            it->findNode( m_nodes[0], m_nodes[0] );
            
            // add me to swarm
            m_nodes[0].findNode( *it, *it );
        }

//        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
//        {
//            NodeKey key = *it;
//            key.m_key = key.m_key ^ 0x1;
//            m_nodes[0].findNode( key, *it );
//        }
    }
    
    std::vector<Node>::iterator generate(std::uniform_int_distribution<int>& range)
    {
        int randomNodeIndex = range(gRandomGenerator);
        auto rit = m_nodes.begin();
        std::advance( rit, randomNodeIndex );
        return rit;
    }
    
    void performIteration( bool fromBootstrap = false )
    {
        assert( m_nodes.size() > 0 );

        std::random_device rd;  // a seed source for the random number engine
        std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> range(1, SWARM_SIZE-1);

        //std::uniform_int_distribution<int> range(1, int(m_nodes.size())-1);
        
        m_testCounter = 0;
        for( auto& requester : m_nodes )
        {
            requester.prepareToIteration();
            int randomNodeIndex;
            do {
                randomNodeIndex = range(gRandomGenerator);
                //randomNodeIndex = range(gen);
            }
            while( requester.m_key == m_nodes[randomNodeIndex].m_key);
                
            auto& searchedNode = m_nodes[randomNodeIndex];
            
            if ( fromBootstrap )
            {
                assert( m_nodes[0].findNode( searchedNode, requester ) );
            }
            else
            {
                requester.findNode( searchedNode, requester );
//                requester.m_map[randomNodeIndex] = '+';
            }
            m_testCounter++;
        }
    }
    
    void performX()
    {
        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            NodeKey key = *it;
            //key.m_key = key.m_key ^ 0x8000000000000000;
            key.m_key = key.m_key ^ 0x1;
            m_nodes[0].findNode( key, *it );
        }

        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            NodeKey key = *it;
            key.m_key = key.m_key ^ 0xFFFFFFFFFFFFFFFF;
            m_nodes[0].findNode( key, *it );
        }

//        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
//        {
//            for( auto it2 = m_nodes.begin(); it2 != m_nodes.end(); it2++ )
//            {
//                if ( it != it2 )
//                {
//                    NodeKey key = *it;
//                    //key.m_key = key.m_key ^ 0x8000000000000000;
//                    key.m_key = key.m_key ^ 0x1;
//                    it2->findNode( key, *it );
//                }
//            }
//        }

//        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
//        {
//            for( auto it2 = m_nodes.begin(); it2 != m_nodes.end(); it2++ )
//            {
//                if ( it != it2 )
//                {
//                    assert( it->findNode( *it2, *it2 ) );
//                }
//            }
//        }

    }
    
    void calcStatistic()
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
        
        LOG( "--- not found: " << m_testCounter-foundCounter << "(" << tooManyCounter << ") avg_requestNumber: " << requestCounter/m_nodes.size() << " requestNumber: " << requestCounter );
    }
    
    void testCompleteness()
    {
        const int bucketThreshold = 1;
        uint64_t addedCounter = 0;
        uint64_t fullCounter = 0;
        uint64_t totalCounter = 0;
        
        const int TEST_NODE_IDX = 5;
        
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( m_nodes[TEST_NODE_IDX].m_key == it->m_key )
                continue;
            
            int bucketIndex;
            bool isFull;
            if ( it->justFind(m_nodes[TEST_NODE_IDX], bucketIndex, isFull ) )
            {
                if (bucketIndex >= bucketThreshold )
                {
                    addedCounter++;
                    totalCounter++;
                }
            }
            else
            {
                if (bucketIndex >= bucketThreshold )
                {
                    totalCounter++;
                    if ( isFull ) fullCounter++;
                }
            }
        }
        
        LOG( "Completeness: " << addedCounter << " in " << totalCounter << "-" << fullCounter << "=" << totalCounter-fullCounter );
    }
    
    int testFullCompleteness( size_t startIndex = 0, size_t endIndex = 0 )
    {
        if ( endIndex == 0 )
        {
            endIndex = m_nodes.size();
        }
        
        int emptyCounter = 0;

        //LOG( "startIndex: " << startIndex << ", endIndex: " << endIndex );

        for( size_t i=startIndex; i<endIndex; i++ )
        {
            auto& testNode = m_nodes[i];

            std::array<int,BUCKET_SIZE> isBucketEmpty{};

            for( auto& node : m_nodes )
            {
                if ( testNode.m_key == node.m_key )
                    continue;

                testNode.testFullCompleteness( node, isBucketEmpty );
            }
            
            for( size_t i=0; i<isBucketEmpty.size(); i++ )
            {
                emptyCounter = emptyCounter + isBucketEmpty[i];
            }
        }
        
        return emptyCounter;
    }
};
