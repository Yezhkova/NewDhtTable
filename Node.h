#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <array>
#include <set>

#include "Bucket.h"
#include "NodeStatistic.h"
#include "Utils.h"

const size_t BUCKET_SIZE = sizeof(Key)*8; // 8 bits per byte



class Node : public NodeKey, public NodeStatistic
{
    int m_index;
    
    std::array<Bucket,BUCKET_SIZE> m_buckets;

#ifdef USE_CLOSEST_NODES_SET
    struct ClosestNodeInfo
    {
        Key m_key;
        int m_index;
        bool operator<( const ClosestNodeInfo& nodeInfo ) const { return m_key < nodeInfo.m_key; }
        bool operator==( const ClosestNodeInfo& nodeInfo ) const { return m_key == nodeInfo.m_key; }
    };
    std::set<ClosestNodeInfo> m_candidateSet;
    std::set<Key>             m_usedCandidates;
#endif
    
//public:
//    char m_map[SWARM_SIZE];

public:
    Node(){}

    Node( int index, const Key& key )
    {
        m_index = index;
        m_key = key;
//        std::memset( m_map, '-', SWARM_SIZE);
    }

    const Key& key() const { return m_key; }
    int        index() const { return m_index; }

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( m_index, m_key, m_buckets );
    }
    
    void prepareToIteration()
    {
        resetCounters();
    }
    
    int calcBucketIndex( const NodeKey& candidate ) const
    {
        return equalPrefixLength( this->m_key, candidate.m_key );
    }

    bool justFind( const NodeKey& searchedNodeKey, int& bucketIndex, bool& isFull )
    {
        bucketIndex = calcBucketIndex( searchedNodeKey );

        return m_buckets[bucketIndex].justFindNode( searchedNodeKey, isFull );
    }
    
    bool justFindNodeInBuckets( const NodeKey& searchedNodeKey )
    {
        int index = calcBucketIndex( searchedNodeKey );
        //LOG( " this: " << this << " b_index: " << index << " key: " << searchedNodeKey.m_key )
        return m_buckets[index].findNode(searchedNodeKey);
    }

    bool privateFindNode( const NodeKey& searchedNodeKey, ClosestNodes& closestNodes, const Node& requesterNode )
    {
        // skip myself
        if ( requesterNode.m_key != m_key )
        {
            //
            // Always try to add requester to my 'Buckets'
            //
            int index = calcBucketIndex( requesterNode );
            m_buckets[index].tryToAddNodeInfo( requesterNode, requesterNode.m_index );
        }
        
        int index = calcBucketIndex( searchedNodeKey );
        
        if ( m_buckets[index].findNodeKey( searchedNodeKey ) )
        {
            return true;
        }
        
        size_t addedClosestNodeCounter = 0;
        if ( closestNodes.size() < MAX_FIND_COUNTER )
        {
            m_buckets[index].addClosestNodes( searchedNodeKey, closestNodes, addedClosestNodeCounter );
        }
        
        if ( closestNodes.size() < MAX_FIND_COUNTER )
        {
            auto i = index;
            while( addedClosestNodeCounter < CLOSEST_NODES_NUMBER && i > 0 )
            {
                i--;
                m_buckets[i].addClosestNodes( searchedNodeKey, closestNodes, addedClosestNodeCounter );
            }
        }

        if ( closestNodes.size() < MAX_FIND_COUNTER )
        {
            auto i = index;
            while( addedClosestNodeCounter < CLOSEST_NODES_NUMBER && i < m_buckets.size()-1 )
            {
                i++;
                m_buckets[i].addClosestNodes( searchedNodeKey, closestNodes, addedClosestNodeCounter );
            }
        }
        
        return false;
    }
    
    void enterToSwarm( Node& bootstrapNode, bool enterToSwarm = false )
    {
        ClosestNodes closestNodes;

        // query 'bootstrapNode' for closest node list
        bootstrapNode.privateFindNode( *this, closestNodes, *this );

        continueFindNode( *this, closestNodes, *this );
    }
    
    bool findNode( const NodeKey& searchedNodeKey, Node& requesterNode, bool enterToSwarm = false )
    {
        ClosestNodes closestNodes;
        closestNodes.reserve( MAX_FIND_COUNTER );
#ifdef USE_CLOSEST_NODES_SET
        m_candidateSet.clear();
        m_usedCandidates.clear();
#endif

//        if ( ((Node&)searchedNodeKey).m_index == 44 && m_index == 33 )
//        {
//            LOG( "not found?: " << ((Node&)searchedNodeKey).m_index << " by: " << m_index <<  " cl_nd_sz: " << closestNodes.size());
//        }

        
        m_isFound = privateFindNode( searchedNodeKey, closestNodes, requesterNode );

        if ( ! m_isFound )
        {
            if ( enterToSwarm )
            {
                m_isFound = requesterNode.continueFindNode( searchedNodeKey, closestNodes, requesterNode );
            }
            else
            {
                m_isFound = continueFindNode( searchedNodeKey, closestNodes, requesterNode );
            }
        }
        
//        if ( ! m_isFound )
//        {
//            std::set<Key> clNodeKeys;
//            for( auto& clNodeKey: closestNodes )
//            {
//                clNodeKeys.insert( clNodeKey );
//            }
//            LOG( "not found: " << ((Node&)searchedNodeKey).m_index << " by: " << m_index <<  " cl_nd_sz: " << clNodeKeys.size());
//            LOG( "(2) not found: " << ((Node&)searchedNodeKey).m_index << " by: " << m_index <<  " cl_nd_sz: " << clNodeKeys.size());
//        }
        
        return m_isFound;
    }

    bool continueFindNode( const NodeKey& searchedNodeKey, ClosestNodes& closestNodes, Node& requesterNode )
    {
        m_requestCounter = 0;
        
#ifdef USE_CLOSEST_NODES_SET
        while( ++m_requestCounter <= MAX_FIND_COUNTER )
        {
            for( size_t i=0; i<closestNodes.size(); i++ )
            {
                Node& closestNode = *((this-m_index) + closestNodes[i]);
                if ( ! m_usedCandidates.contains( closestNode.m_key ) )
                {
                    m_candidateSet.emplace( ClosestNodeInfo{ closestNode.m_key ^ searchedNodeKey.m_key, closestNode.m_index } );
                }
            }
            closestNodes.reset();
            
            if ( m_candidateSet.empty() )
            {
                return false;
            }

            ClosestNodeInfo info = * m_candidateSet.begin();
            m_candidateSet.erase( m_candidateSet.begin() );
            m_usedCandidates.emplace( info.m_key ^ searchedNodeKey.m_key );
            
            Node& closestNode = *((this-m_index) + info.m_index);

            if ( closestNode.privateFindNode( searchedNodeKey, closestNodes, requesterNode ) )
            {
                //addClosestNodeToBuckets( closestNode );
                return true;
            }
            addClosestNodeToBuckets( closestNode );
        }
#else
        for( size_t i=0; i<closestNodes.size(); i++ )
        {
            if ( ++m_requestCounter > MAX_FIND_COUNTER )
            {
                return false;
            }

            Node& closestNode = *((this-m_index) + closestNodes[i]);

            if ( closestNode.privateFindNode( searchedNodeKey, closestNodes, requesterNode ) )
            {
                //addClosestNodeToBuckets( closestNode );
                return true;
            }
            addClosestNodeToBuckets( closestNode );
        }
#endif
        
        return false;
    }

    void addClosestNodeToBuckets( Node& closestNode )
    {
        int index = calcBucketIndex( closestNode );
        if ( index < sizeof(m_buckets)/sizeof(m_buckets[0]) )
        {
            m_buckets[index].tryToAddNodeInfo( closestNode, closestNode.m_index );
        }
    }
    
    void testBucketCompleteness( Node& node, std::array<int,BUCKET_SIZE>& isBucketEmpty )
    {
        int index = calcBucketIndex( node );
        assert( index >=0 && index<BUCKET_SIZE );
        
        // Calculate only empty buckets, that could be filled
        if ( m_buckets[index].empty() )
        {
            isBucketEmpty[index] = 1;
        }
    }
};
