#pragma once
#include <iostream>
#include <vector>
#include <array>

#include "Bucket.h"
#include "NodeStatistic.h"
#include "Utils.h"

const size_t BUCKET_SIZE = sizeof(Key)*8;

class Node : public NodeKey, public NodeStatistic
{
    std::array<Bucket,BUCKET_SIZE> m_buckets;

public:
    Node(){}

    Node( const Key& key ) { m_key = key; }

    const Key& key() const { return m_key; }

    void initByKey( const Key& key ) { m_key = key; } // why do we need this if this is identical to the constructor? +It is never used
    
    void enterSwarm( Node& bootstrapNode )
    {
        std::vector<const NodeKey*> closestNodes;
        if ( bootstrapNode.tryToAdd( *this, closestNodes ) )
        {
            //TODO
            return;
        }
        
        enterSwarmR( closestNodes );
    }

    void enterSwarmR( std::vector<const NodeKey*>& closestNodes )
    {
        std::vector<const NodeKey*> closestNodes2;
        for( auto it = closestNodes.begin(); it != closestNodes.end(); it++ )
        {
            if ( ((Node*)(*it))->tryToAdd( *this, closestNodes2 ) )
            {
                //TODO
                return;
            }
        }

        enterSwarmR( closestNodes2 );
    }
    
    bool tryToAdd( Node& candidate, std::vector<const NodeKey*>& closestNodes )
    {
        
        int index = calcIndex( candidate );
        
        return m_buckets[index].tryToAdd( candidate, closestNodes );
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
    
    bool tryToFindNode( const NodeKey& searchedNodeKey, bool addMe )
    {
        int index = calcIndex( searchedNodeKey );
        
        std::vector<const NodeKey*> closestNodes;
        if ( m_buckets[index].findNodeKey( searchedNodeKey, closestNodes, addMe ) )
        {
            return true;
        }
        
        m_requestNumber = 0;
        auto result = continueFindNodeR( searchedNodeKey, closestNodes, addMe );
        if ( result )
        {
            m_isfound = true;
        }
        return result;
    }

    bool continueFindNodeR( const NodeKey& searchedNodeKey, std::vector<const NodeKey*>& closestNodes, bool addMe )
    {
        if ( ++m_requestNumber > MAX_FIND_COUNTER )
        {
            return false;
        }
        
        std::vector<const NodeKey*> closestNodes2;
        for( auto it = closestNodes.begin(); it != closestNodes.end(); it++ )
        {
            if ( ((Node*)(*it))->tryToFindNode( searchedNodeKey, addMe ) )
            {
                //TODO
                return true;
            }
        }

        return continueFindNodeR( searchedNodeKey, closestNodes2, addMe );
    }

    void prepareToIteration()
    {
        resetCounters();
    }
};
