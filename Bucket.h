#pragma once

#include <vector>
#include <cassert>

#include "Utils.h"
#include "Constants.h"

using ClosestNodes = std::vector<NodeIndex>;

class Bucket
{
    // NodeInfo
    struct NodeInfo
    {
        Key         m_key;
        NodeIndex   m_nodeIndex;
        
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar( m_key, m_nodeIndex );
        }
    };

    std::vector<NodeInfo> m_nodes;
    
public:
    
    Bucket() { m_nodes.reserve( CLOSEST_NODES_CAPACITY ); }

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( m_nodes );
    }
    
    bool empty() const { return m_nodes.empty(); }
    
    bool justFindNode( const NodeKey& searchedKey, bool& isFull )
    {
        isFull = m_nodes.size() >= CLOSEST_NODES_CAPACITY;
        
        for( auto& nodeInfo : m_nodes )
        {
            if ( nodeInfo.m_key == searchedKey.m_key )
                return true;
        }
        return false;
    }

    bool findNode( const NodeKey& searchedKey )
    {
        for( auto& nodeInfo : m_nodes )
        {
            if ( nodeInfo.m_key == searchedKey.m_key )
                return true;
        }
        return false;
    }

    bool tryToAddNodeInfo( const NodeKey& requesterNodeKey, NodeIndex index )
    {
        if ( m_nodes.size() >= CLOSEST_NODES_CAPACITY )
        {
            return false;
        }

        for( auto& nodeInfo : m_nodes )
        {
            if ( nodeInfo.m_key == requesterNodeKey.m_key )
                return false;
        }

        m_nodes.push_back( NodeInfo{ requesterNodeKey.m_key, index } );
        return true;
    }

    bool findNodeKeyAndFillClosestNodes( const NodeKey& searchedNodeKey, ClosestNodes& closestNodes )
    {
        for( auto& nodeInfo : m_nodes )
        {
            if ( nodeInfo.m_key == searchedNodeKey.m_key )
                return true;
        }

        //
        // Add candidates to 'closestNodes'
        //
        size_t closestNodeCounter = 0;
        
        //TODO: random
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( closestNodeCounter >= CLOSEST_NODES_NUMBER )
                return false;
            
            closestNodeCounter++;
            closestNodes.push_back( it->m_nodeIndex );
        }
        
        return false;
    }
};

