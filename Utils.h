#pragma once
#include <cstddef>
#include <cstdint>
#include <map>
#include <iostream>
#include <mutex>

using Key = uint64_t;
//using Key = std::array<uint8_t,20>;

struct NodeKey
{
    Key    m_key;
};

inline std::recursive_mutex gLogMutex;

// LOG
#ifndef QDBG
    #define LOG(expr) { \
            const std::lock_guard<std::recursive_mutex> autolock( gLogMutex ); \
            std::cout << std::fixed << expr << std::endl << std::flush; \
        }
#else
    #define LOG(expr) { \
            std::ostringstream out; \
            out << m_dbgOurPeerName << ": " << expr; \
            ??? \
    }
#endif


namespace Utils
{

size_t pow2(size_t power);

char hex(int symbol);

}
