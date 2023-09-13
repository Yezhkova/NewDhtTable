#pragma once
#include <iostream>
#include <vector>
#include <deque>
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
        closestNodes.reserve(10000);
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
        closestNodes2.reserve(10000);
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
    
    void prepareToIteration()
    {
        resetCounters();
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
    
    // return -1 or backet index
    bool justFind( const NodeKey& searchedNodeKey, int& backetIndex, bool& isFull )
    {
        backetIndex = calcIndex( searchedNodeKey );

        std::vector<const NodeKey*> closestNodes;
        closestNodes.reserve(10000);

        if ( m_buckets[backetIndex].justFindNode( searchedNodeKey, isFull ) )
        {
            return true;
        }
        
        return false;
    }
    
    bool tryToFindNodeR( const NodeKey& searchedNodeKey,  std::deque<const NodeKey*>& closestNodes, bool addMe )
    {
        if ( ++m_reqursionNumber > MAX_REQURSION_COUNTER )
        {
            return false;
        }
        std::deque<const NodeKey*> closestNodes2;
        
        for( auto it = closestNodes.begin(); it != closestNodes.end(); it++ )
        {
            if ( ++m_requestNumber > MAX_FIND_COUNTER )
            {
                return false;
            }

            int index = ((Node*)(*it))->calcIndex( searchedNodeKey );
            
            if ( ((Node*)(*it))->m_buckets[index].findNodeKey( searchedNodeKey, closestNodes2, addMe ) )
            {
                return true;
            }
        }
        
        return tryToFindNodeR( searchedNodeKey, closestNodes2, addMe );
    }

    bool tryToFindNode( const NodeKey& searchedNodeKey, bool addMe )
    {
        int index = calcIndex( searchedNodeKey );
        
        std::deque<const NodeKey*> closestNodes;

//        if ( index > 3 )
//        {
//            if ( m_buckets[index-3].findNodeKey( searchedNodeKey, closestNodes, addMe ) )
//            {
//                m_isFound = true;
//                return true;
//            }
//        }

//        if ( m_buckets[0].findNodeKey( searchedNodeKey, closestNodes, addMe ) )
//        {
//            m_isFound = true;
//            return true;
//        }
            
        if ( m_buckets[index].findNodeKey( searchedNodeKey, closestNodes, addMe ) )
        {
            m_isFound = true;
            return true;
        }
        
        m_requestNumber = 0;
        
        if (1)
        {
            m_isFound = continueFindNode( searchedNodeKey, closestNodes, addMe );
        }
        else
        {
            m_isFound = tryToFindNodeR( searchedNodeKey, closestNodes, addMe );
        }
        
        return m_isFound;
    }

    bool continueFindNode( const NodeKey& searchedNodeKey, std::deque<const NodeKey*>& closestNodes, bool addMe )
    {
        
        for( auto it = closestNodes.begin(); it != closestNodes.end(); it++ )
        {
            if ( ++m_requestNumber > MAX_FIND_COUNTER )
            {
                return false;
            }

            if ( ((Node*)(*it))->privateFindNode( searchedNodeKey, closestNodes, addMe ) )
            {
                return true;
            }
        }

        return false;
    }

    bool privateFindNode( const NodeKey& searchedNodeKey, std::deque<const NodeKey*>& closestNodes, bool addMe )
    {
        int index = calcIndex( searchedNodeKey );
        
        if ( m_buckets[index].findNodeKey( searchedNodeKey, closestNodes, addMe ) )
        {
            return true;
        }

        return false;
    }

};
