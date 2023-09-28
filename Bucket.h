#pragma once

#include <vector>
#include <cassert>

#include "Utils.h"
#include "Constants.h"

//using ClosestNodes = std::vector<NodeIndex>;

struct ClosestNodes
{
    NodeIndex   m_nodeIndexes[MAX_FIND_COUNTER+CLOSEST_NODES_NUMBER];
    size_t      m_size = 0;
    
    ClosestNodes() {}
    
    void push_back( NodeIndex i ) { assert(m_size<MAX_FIND_COUNTER+CLOSEST_NODES_NUMBER); m_nodeIndexes[m_size] = i; m_size++; }
    size_t size() const { return m_size; }
    void reserve( size_t ) {}
    
    NodeIndex& operator[](size_t i) { return m_nodeIndexes[i]; }
};

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
    
    bool justFindNode( const NodeKey& searchedKey, bool& isFull ) const
    {
        isFull = m_nodes.size() >= CLOSEST_NODES_CAPACITY;
        
        for( auto& nodeInfo : m_nodes )
        {
            if ( nodeInfo.m_key == searchedKey.m_key )
                return true;
        }
        return false;
    }

    inline bool findNode( const NodeKey& searchedKey ) const
    {
        for( auto& nodeInfo : m_nodes )
        {
            if ( nodeInfo.m_key == searchedKey.m_key )
                return true;
        }
        return false;
    }

    inline void tryToAddNodeInfo( const NodeKey& requesterNodeKey, NodeIndex index )
    {
        if ( m_nodes.size() < CLOSEST_NODES_CAPACITY )
        {
            for( auto& nodeInfo : m_nodes )
            {
                if ( nodeInfo.m_key == requesterNodeKey.m_key )
                    return;
            }
            
            m_nodes.push_back( NodeInfo{ requesterNodeKey.m_key, index } );
        }
    }

    inline bool findNodeKey( const NodeKey& searchedNodeKey ) const
    {
        for( auto& nodeInfo : m_nodes )
        {
            if ( nodeInfo.m_key == searchedNodeKey.m_key )
                return true;
        }
        return false;
    }

    inline void addClosestNodes( const NodeKey& searchedNodeKey, ClosestNodes& closestNodes, size_t& addedClosestNodeCounter ) const
    {
        //TODO: random/ordered
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( addedClosestNodeCounter >= CLOSEST_NODES_NUMBER )
            {
                return;
            }
            
            addedClosestNodeCounter++;
            closestNodes.push_back( it->m_nodeIndex );
        }
    }
};

