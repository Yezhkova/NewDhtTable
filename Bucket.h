#pragma once

#include <vector>
#include <cassert>

#include "Utils.h"
#include "Constants.h"

using ClosestNodes = std::vector<const NodeKey*>;

struct Bucket
{
    struct NodeInfo
    {
        const NodeKey*  m_ptr;
        int             m_nodeIndex;
        
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar( m_nodeIndex );
        }
    };
    
    std::vector<NodeInfo> m_nodes;
    
    Bucket() { m_nodes.reserve( CLOSEST_NODES_CAPACITY ); }

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( m_nodes );
    }
    
    bool empty() const { return m_nodes.empty(); }
    
    std::vector<NodeInfo>& bucketNodes() { return m_nodes; }
    
    bool tryToAdd( const NodeKey& candidateKey, int nodeIndex, std::vector<const NodeKey*>& closestNodes )
    {
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( it->m_ptr->m_key == candidateKey.m_key )
            {
                assert(0);
            }
        }
        
        if ( m_nodes.size() >= CLOSEST_NODES_CAPACITY )
        {
            //TODO
            for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
            {
                closestNodes.emplace_back( it->m_ptr );
                if ( closestNodes.size() >= CLOSEST_NODES_NUMBER )
                {
                    return false;
                }
            }
            return false;
        }
        
        m_nodes.emplace_back( NodeInfo{&candidateKey, nodeIndex} );
        return true;
    }

    bool justFindNode( const NodeKey& searchedKey, bool& isFull )
    {
        isFull = m_nodes.size() >= CLOSEST_NODES_CAPACITY;
        
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( it->m_ptr->m_key == searchedKey.m_key )
            {
                return true;
            }
        }
        return false;
    }

    bool findNode( const NodeKey& searchedKey )
    {
        //LOG( "bucket2 this: " << this );
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( it->m_ptr->m_key == searchedKey.m_key )
            {
                return true;
            }
        }
        return false;
    }

    bool tryToAddNodeKey( const NodeKey& requesterNodeKey, int index )
    {
        if ( m_nodes.size() >= CLOSEST_NODES_CAPACITY )
        {
            return false;
        }

        for( auto& nodeInfo : m_nodes )
        {
            if ( nodeInfo.m_ptr->m_key == requesterNodeKey.m_key )
            {
                return false;
            }
        }

        m_nodes.push_back( NodeInfo{ &requesterNodeKey, index } );
        return true;
    }

    bool findNodeKey( const NodeKey& searchedNodeKey, ClosestNodes& closestNodes )
    {
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( it->m_ptr->m_key == searchedNodeKey.m_key )
            {
                return true;
            }
        }

        //
        // Add candidates to 'closestNodes'
        //
        auto initialVectorSize = closestNodes.size();
        
        //TODO: random
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( closestNodes.size() >= initialVectorSize + CLOSEST_NODES_NUMBER )
            {
                return false;
            }
            
            closestNodes.push_back( it->m_ptr );
        }
        
        return false;
    }
};

