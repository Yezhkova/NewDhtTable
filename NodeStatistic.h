#pragma once
#include <iostream>

struct NodeStatistic
{
    uint64_t    m_requestCounter;
    bool        m_isFound;

    uint64_t    m_rcvMsgCount = 0;
    
    bool tooManyRequests() const { return m_requestCounter >= MAX_FIND_COUNTER; }

    void resetCounters()
    {
        m_requestCounter = 0;
        m_isFound = false;
        
        m_rcvMsgCount = 0;
    }
};

