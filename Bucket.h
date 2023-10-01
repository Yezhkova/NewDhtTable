#pragma once

#include <vector>
#include <cassert>
#include <queue>

#include "Utils.h"
#include "Constants.h"

#ifdef USE_CLOSEST_NODES_SET
    struct NodeInfo
    {
        Key         m_key;
        NodeIndex   m_nodeIndex;
        bool operator<( const NodeInfo& nodeInfo ) const { return m_key < nodeInfo.m_key; }
        bool operator==( const NodeInfo& nodeInfo ) const { return m_key == nodeInfo.m_key; }

        template <class Archive>
        void serialize( Archive & ar )
        {
            ar( m_key, m_nodeIndex );
        }
    };
#endif

    struct ClosestNodes
    {
        NodeIndex   m_nodeIndexes[MAX_FIND_COUNTER+CLOSEST_NODES_NUMBER];
        size_t      m_size = 0;
        
        ClosestNodes() {}
        
        void push_back( NodeIndex i ) { assert(m_size<MAX_FIND_COUNTER+CLOSEST_NODES_NUMBER); m_nodeIndexes[m_size] = i; m_size++; }
        size_t size() const { return m_size; }
        void clear() { m_size = 0; }

        NodeIndex& operator[](size_t i) { return m_nodeIndexes[i]; }
    };

class Bucket
{
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

    inline bool findNodeInBucket( const NodeKey& searchedKey ) const
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
#ifdef USE_PRIORITY
        std::vector<NodeInfo> nodes(m_nodes);
        //std::sort( nodes.begin(), nodes.end() );
        //std::sort( nodes.begin(), nodes.end(), [] (const auto& a, const auto& b ) { return !(a<b);} );
        //std::reverse( nodes.begin(), nodes.end() );
        for( const auto& nodeInfo : nodes )
        {
            if ( addedClosestNodeCounter >= CLOSEST_NODES_NUMBER )
            {
                return;
            }
            
            addedClosestNodeCounter++;
            closestNodes.push_back( nodeInfo.m_nodeIndex );
        }
#else
        for( const auto& nodeInfo : m_nodes )
        {
            if ( addedClosestNodeCounter >= CLOSEST_NODES_NUMBER )
            {
                return;
            }
            
            addedClosestNodeCounter++;
            closestNodes.push_back( nodeInfo.m_nodeIndex );
        }
#endif
    }
};

