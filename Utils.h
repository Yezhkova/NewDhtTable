#pragma once
#include <cstddef>
#include <cstdint>
#include <map>
#include <iostream>
#include <mutex>

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>


using Key = uint64_t;
//using Key = std::array<uint8_t,20>;

using NodeIndex = uint32_t;

struct NodeKey
{
    Key    m_key;
};

inline std::recursive_mutex gLogMutex;

inline bool gHideLog = false;

// LOG
#ifndef QDBG
    #define LOG(expr) { \
        if ( !gHideLog ) {\
            const std::lock_guard<std::recursive_mutex> autolock( gLogMutex ); \
            std::cout << std::fixed << expr << std::endl << std::flush; \
    }   }
#else
    #define LOG(expr) { \
            std::ostringstream out; \
            out << m_dbgOurPeerName << ": " << expr; \
            ??? \
    }
#endif

inline int equalPrefixLength2( const Key& a, const Key& b )
{
    unsigned char* aBytes = (unsigned char*) &a;
    unsigned char* bBytes = (unsigned char*) &b;

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


inline int equalPrefixLength( const Key& a, const Key& b )
{
    static_assert( sizeof(Key) <= 8 );
    
    Key xValue = a ^ b;
    Key mask = 0xFF00000000000000;

    int len = 0;
    while( (len < sizeof(Key)*8) && (xValue&0xFF00000000000000)==0 )
    {
        xValue = xValue<<8;
        len += 8;
    }

    while( (len < sizeof(Key)*8) && (xValue&0x8000000000000000)==0 )
    {
        xValue = xValue<<1;
        ++len;
    }

    return len;
}



namespace Utils
{

size_t pow2(size_t power);

char hex(int symbol);

}
