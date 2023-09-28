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
    
    int m_totalFindCounter = 0;
    
public:
    Swarm(){}
    
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( m_nodes );
    }
    
    Node& bootstrapNode() { return m_nodes[0]; }
    
    size_t size() const { return m_nodes.size(); }
    
    void init( int swarmCapacity )
    {
        assert( SWARM_SIZE <= swarmCapacity );
        m_nodes.reserve(swarmCapacity);

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
    
//    void addNewParticipants( int newNodeNumber )
//    {
//        union
//        {
//            Key key;
//            std::array<uint8_t,sizeof(Key)> buffer{};
//        };
//
//        std::seed_seq sd{ 1000000 };
//        auto g = std::mt19937(sd);
//
//        auto nodesBegin = m_nodes.begin();
//        LOG( "size: " << m_nodes.size() << " capacity: " << m_nodes.capacity() )
//
//        size_t initSwarmSize = m_nodes.size();
//        for( int i=0; i<newNodeNumber; )
//        {
//            std::generate( buffer.begin(), buffer.end(), [&]
//            {
//                return std::uniform_int_distribution<std::uint8_t>(0,0xff) ( g );
//            });
//
//            for( size_t i=0; i<m_nodes.size(); i++ )
//            {
//                if ( m_nodes[i].m_key == key )
//                {
//                    LOG( m_nodes[i].m_key );
//                    LOG( key );
//                    LOG( "duplicated key: " << i );
//                    exit(1);
//                    continue;
//                }
//            };
//
//            m_nodes.emplace_back(initSwarmSize+i,key);
//
//            LOG( "m_nodes.back(): " << m_nodes.back().m_key )
//
//            m_nodes.back().findNode( bootstrapNode(), bootstrapNode() );
//            bootstrapNode().findNode( m_nodes.back(), m_nodes.back() );
//
//            i++;
//        }
//
//        LOG( "size: " << m_nodes.size() << " capacity: " << m_nodes.capacity() )
//        assert( nodesBegin == m_nodes.begin() );
//    }
//
//    void testNewNodes(int newNodeNumber)
//    {
//        for( size_t i=SWARM_SIZE; i<SWARM_SIZE+newNodeNumber; i++ )
//        {
//            int foundCounter=0;
//            for(  size_t j=0; j<SWARM_SIZE; j++ )
//            {
//                if ( m_nodes[j].justFindNodeInBuckets( m_nodes[i] ) )
//                {
//                    foundCounter++;
//                }
//            }
//            LOG( "i: " << i << ", foundCounter: " << foundCounter );
//        }
//
//        for(  size_t j=0; j<SWARM_SIZE; j++ )
//        {
//            auto& node = m_nodes[j];
//            node.prepareToIteration();
//        }
//
//        for( size_t i=SWARM_SIZE; i<SWARM_SIZE+newNodeNumber; i++ )
//        {
//            for(  size_t j=0; j<SWARM_SIZE; j++ )
//            {
//                m_nodes[j].findNode( m_nodes[i], m_nodes[j] );
//            }
//        }
//    }
    
    void colonize()
    {
        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            // add bootstrap node to my buckets
            it->findNode( bootstrapNode(), bootstrapNode() );
            
            // add me to swarm
            bootstrapNode().findNode( *it, *it );
        }
    }
    
    void performIteration()
    {
        assert( m_nodes.size() > 0 );

        std::uniform_int_distribution<> range(1, SWARM_SIZE-1);
        
        m_totalFindCounter = 0;
        
        for( auto& requester : m_nodes )
        {
            if ( requester.index() == 0 )
            {
                continue;
            }
            requester.prepareToIteration();
            
            int searchedNodeIndex;
            do {
                searchedNodeIndex = range(gRandomGenerator);
            }
            while( requester.m_key == m_nodes[searchedNodeIndex].m_key );
                
            auto& searchedNode = m_nodes[searchedNodeIndex];
            
            requester.findNode( searchedNode, requester );
            //assert( bootstrapNode().findNode( searchedNode, requester ) );
            //requester.m_map[randomNodeIndex] = '+';
            
            m_totalFindCounter++;
        }
    }
    
    void performX()
    {
        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            NodeKey key = *it;
            //key.m_key = key.m_key ^ 0x8000000000000000;
            key.m_key = key.m_key ^ 0x1;
            bootstrapNode().findNode( key, *it );
        }

        for( auto it = m_nodes.begin()+1; it != m_nodes.end(); it++ )
        {
            NodeKey key = *it;
            key.m_key = key.m_key ^ 0xFFFFFFFFFFFFFFFF;
            bootstrapNode().findNode( key, *it );
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
        uint64_t totalRequestCounter = 0;
        
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
            totalRequestCounter += it->m_requestCounter;
        }
        
        LOG( "--- not found: " << m_totalFindCounter-foundCounter << "(" << tooManyCounter << ") avg_requestNumber: " << totalRequestCounter/m_nodes.size() << " requestNumber: " << totalRequestCounter );
    }
    
//    void testCompleteness()
//    {
//        // it skips first 'bucketThreshold' buckets, that usually are full
//        const int bucketThreshold = 0;
//
//        uint64_t addedCounter = 0;
//        uint64_t fullCounter = 0;
//        uint64_t totalCounter = 0;
//
//        const int TEST_NODE_IDX = 5;
//
//        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
//        {
//            if ( m_nodes[TEST_NODE_IDX].m_key == it->m_key )
//                continue;
//
//            int bucketIndex;
//            bool isFull;
//            if ( it->justFind(m_nodes[TEST_NODE_IDX], bucketIndex, isFull ) )
//            {
//                if (bucketIndex >= bucketThreshold )
//                {
//                    addedCounter++;
//                    totalCounter++;
//                }
//            }
//            else
//            {
//                if (bucketIndex >= bucketThreshold )
//                {
//                    totalCounter++;
//                    if ( isFull ) fullCounter++;
//                }
//            }
//        }
//
//        LOG( "Completeness: " << addedCounter << " in " << totalCounter << "-" << fullCounter << "=" << totalCounter-fullCounter );
//    }
    
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

            for( auto& candidateNode : m_nodes )
            {
                if (testNode.m_key == candidateNode.m_key )
                    continue;

                testNode.testBucketCompleteness(candidateNode, isBucketEmpty );
            }
            
            for( size_t i=0; i<isBucketEmpty.size(); i++ )
            {
                emptyCounter = emptyCounter + isBucketEmpty[i];
            }
        }
        
        return emptyCounter;
    }
};
