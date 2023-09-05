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

    void initByKey( const Key& key ) { m_key = key; }
    
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

    int calcIndex( const NodeKey& candidate )
    {
        return 0;
    }
    
    void tryToFindNode( const NodeKey& searchedNodeKey )
    {
        int index = calcIndex( searchedNodeKey );
        
        std::vector<const NodeKey*> closestNodes;
        if ( m_buckets[index].findNodeKey( searchedNodeKey, closestNodes ) )
        {
            return;
        }
        
        continueFindNodeR( searchedNodeKey, closestNodes );
    }

    void continueFindNodeR( const NodeKey& searchedNodeKey, std::vector<const NodeKey*>& closestNodes )
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

        continueFindNodeR( searchedNodeKey, closestNodes2 );
    }

};
