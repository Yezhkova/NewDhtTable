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

    void initByKey( const Key& key ) { m_key = key; } // why do we need this if this is identical to the constructor? +It is never used

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( m_index, m_key, m_buckets );
    }
    
    void prepareToIteration()
    {
        resetCounters();
    }
    
    bool tryToAdd( Node& candidate, std::vector<const NodeKey*>& closestNodes )
    {
        
        int index = calcIndex( candidate );
        
        return m_buckets[index].tryToAdd( candidate, index, closestNodes );
    }

    int calcIndex( const NodeKey& candidate ) const
    {
        return equalPrefixLength( *this, candidate );
    }
    
    int equalPrefixLength( const NodeKey& a, const NodeKey& b ) const
    {
        unsigned char* aBytes = (unsigned char*) &a;
        unsigned char* bBytes = (unsigned char*) &b;

        int len = 0;
        int j = 0;
        for(; j < sizeof(Key) && aBytes[j] == bBytes[j]; ++j)
        {
            len += 8;
        }

        if ( j < sizeof(Key) )
        {
            unsigned char aByte = aBytes[j];
            unsigned char bByte = bBytes[j];
            for( int i = 7; !(((aByte >> i)&1) ^ ((bByte >> i)&1)) && i >= 0; --i)
            {
                ++len;
            }
        }
        return len;
    }

    // return -1 or backet index
    bool justFind(const NodeKey& searchedNodeKey, int& bucketIndex, bool& isFull )
    {
        bucketIndex = calcIndex(searchedNodeKey );

        if ( m_buckets[bucketIndex].justFindNode(searchedNodeKey, isFull ) )
        {
            return true;
        }
        
        return false;
    }
    
    bool findNodeInBuckets( const NodeKey& searchedNodeKey )
    {
        int index = calcIndex( searchedNodeKey );
        //LOG( " this: " << this << " b_index: " << index << " key: " << searchedNodeKey.m_key )
        return m_buckets[index].findNode(searchedNodeKey);

    }

    bool privateFindNode( const NodeKey& searchedNodeKey, ClosestNodes& closestNodes, const NodeKey& requesterNodeKey )
    {
        // Alway try to add requester in my 'Buckets'
        if ( requesterNodeKey.m_key != m_key )
        {
            int index = calcIndex( requesterNodeKey );
            if ( m_buckets[index].tryToAddNodeKey( requesterNodeKey, ((Node&)requesterNodeKey).m_index ) )
            {
                //LOG( "addeded: " << this << " to: " << m_index << " bucketIdx: " << index << " key: " << requesterNodeKey.m_key );
            }
        }
        
        int index = calcIndex( searchedNodeKey );
        
        return m_buckets[index].findNodeKey( searchedNodeKey, closestNodes );
    }
    
    bool findNode( const NodeKey& searchedNodeKey, const NodeKey& requesterKey )
    {
        ClosestNodes closestNodes;
        closestNodes.reserve( MAX_FIND_COUNTER );
        
        m_isFound = privateFindNode( searchedNodeKey, closestNodes, requesterKey );

        if ( ! m_isFound )
        {
            m_isFound = continueFindNode( searchedNodeKey, closestNodes, requesterKey );
        }
        
//        if ( ! m_isFound )
//        {
//            closestNodes.clear();
//            closestNodes.reserve( MAX_FIND_COUNTER );
//            m_buckets[0].findNodeKey( searchedNodeKey, closestNodes );
//            m_isFound = continueFindNode( searchedNodeKey, closestNodes, requesterKey );
//        }
        
        return m_isFound;
    }

    bool continueFindNode( const NodeKey& searchedNodeKey, ClosestNodes& closestNodes, const NodeKey& requesterKey )
    {
        m_requestNumber = 0;
        
        for( size_t i=0; i<closestNodes.size(); i++ )
        {
            if ( ++m_requestNumber > MAX_FIND_COUNTER )
            {
                return false;
            }

            Node& closestNode = (Node&) *closestNodes[i];
            
            if ( closestNode.privateFindNode( searchedNodeKey, closestNodes, requesterKey ) )
            {
                return true;
            }
        }

        return false;
    }

    void restorePointers( std::vector<Node>& swarmNodes )
    {
        for( int i=0; i<BUCKET_SIZE; i++ )
        {
            std::vector<Bucket::NodeInfo>& bucketNodes = m_buckets[i].bucketNodes();
            for( auto& nodeInfo: bucketNodes )
            {
                nodeInfo.m_ptr = &swarmNodes[nodeInfo.m_nodeIndex];
            }
        }
    }

    void testFullCompleteness( Node& node, std::array<int,BUCKET_SIZE>& isBucketEmpty )
    {
        int index = calcIndex( node );
        assert( index >=0 && index<BUCKET_SIZE );
        
        if ( m_buckets[index].empty() )
        {
            isBucketEmpty[index] = 1;
        }
    }
};
