#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <array>

#include "Bucket.h"
#include "NodeStatistic.h"
#include "Utils.h"

const size_t BUCKET_SIZE = sizeof(Key)*8; // 8 bits per byte

class Node : public NodeKey, public NodeStatistic
{
    int m_index;
    
    std::array<Bucket,BUCKET_SIZE> m_buckets;
    
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

    //void initByKey( const Key& key ) { m_key = key; } // why do we need this if this is identical to the constructor? +It is never used

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
        return equalPrefixLength( *this, candidate );
    }
    
    int equalPrefixLength( const NodeKey& a, const NodeKey& b ) const
    {
        unsigned char* aBytes = (unsigned char*) &a.m_key;
        unsigned char* bBytes = (unsigned char*) &b.m_key;

        int len = 0;
        int j = 0;
        for(; (j < sizeof(Key)) && aBytes[j] == bBytes[j]; ++j)
        {
            len += 8;
        }

        if ( j < sizeof(Key) )
        {
            unsigned char aByte = aBytes[j];
            unsigned char bByte = bBytes[j];
            for( int i = 7; (((aByte >> i)&1) == ((bByte >> i)&1)) && i >= 0; --i)
            {
                ++len;
            }
        }
        return len;
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
            // Alway try to add requester to my 'Buckets'
            //
            int index = calcBucketIndex( requesterNode );
            if ( m_buckets[index].tryToAddNodeInfo( requesterNode, requesterNode.m_index ) )
            {
                //LOG( "addeded: " << this << " to: " << m_index << " bucketIdx: " << index << " key: " << requesterNodeKey.m_key );
            }
        }
        
        int index = calcBucketIndex( searchedNodeKey );
        
        return m_buckets[index].findNodeKeyAndFillClosestNodes( searchedNodeKey, closestNodes );
    }
    
    bool findNode( const NodeKey& searchedNodeKey, const Node& requesterNode )
    {
        ClosestNodes closestNodes;
        closestNodes.reserve( MAX_FIND_COUNTER );
        
        m_isFound = privateFindNode( searchedNodeKey, closestNodes, requesterNode );

        if ( ! m_isFound )
        {
            m_isFound = continueFindNode( searchedNodeKey, closestNodes, requesterNode );
        }
  
        //
        // start find from bootstrap
        //
//        if ( ! m_isFound )
//        {
//            closestNodes.clear();
//            closestNodes.reserve( MAX_FIND_COUNTER );
//            m_buckets[0].findNodeKey( searchedNodeKey, closestNodes );
//            m_isFound = continueFindNode( searchedNodeKey, closestNodes, requesterKey );
//        }
        
        return m_isFound;
    }

    bool continueFindNode( const NodeKey& searchedNodeKey, ClosestNodes& closestNodes, const Node& requesterNode )
    {
        m_requestCounter = 0;
        
        for( size_t i=0; i<closestNodes.size(); i++ )
        {
            if ( ++m_requestCounter > MAX_FIND_COUNTER )
            {
                return false;
            }

            Node* closestNode = (this-m_index) + closestNodes[i];
            
            if ( closestNode->privateFindNode( searchedNodeKey, closestNodes, requesterNode ) )
            {
                return true;
            }
        }

        return false;
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
