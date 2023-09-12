#pragma once

#include <vector>
#include <cassert>
#include "Utils.h"
#include "Constants.h"
struct Bucket
{
    std::vector<const NodeKey*> m_nodes;
    
    Bucket() { m_nodes.reserve( CLOSEST_NODES_CAPACITY ); }
    
    bool tryToAdd( const NodeKey& candidateKey, std::vector<const NodeKey*>& closestNodes )
    {
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( (*it)->m_key == candidateKey.m_key )
            {
                assert(0);
            }
        }
        
        if ( m_nodes.size() >= CLOSEST_NODES_CAPACITY )
        {
            //TODO
            for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
            {
                closestNodes.push_back( *it );
                if ( closestNodes.size() >= CLOSEST_NODES_NUMBER )
                {
                    return false;
                }
            }
            return false;
        }
        
        m_nodes.push_back( &candidateKey );
        return true;
    }

    bool justFindNode( const NodeKey& searchedKey, bool& isFull )
    {
        isFull = m_nodes.size() >= CLOSEST_NODES_CAPACITY;
        
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( (*it)->m_key == searchedKey.m_key )
            {
                return true;
            }
        }
        return false;
    }
    
    bool findNodeKey( const NodeKey& searchedKey, std::deque<const NodeKey*>& closestNodes, bool addRequester )
    {
        // test that searchKey is present
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( (*it)->m_key == searchedKey.m_key )
            {
                return true;
            }
        }
        
        auto initialVectorSize = closestNodes.size();
        
        //TODO
        for( auto it = m_nodes.begin(); it != m_nodes.end(); it++ )
        {
            if ( closestNodes.size() < initialVectorSize + CLOSEST_NODES_NUMBER )
            {
                closestNodes.push_back( *it );
            }
            else
            {
                break;
            }
        }

        if ( addRequester && (m_nodes.size() < CLOSEST_NODES_CAPACITY) )
        {
            m_nodes.push_back( &searchedKey );
        }
        
        return false;
    }
};

