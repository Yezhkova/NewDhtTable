#pragma once
#include <iostream>

struct NodeStatistic
{
    uint64_t    m_requestNumber;
    bool        m_isFound;

    bool tooManyRequests() const { return m_requestNumber >= MAX_FIND_COUNTER; }

    void resetCounters()
    {
        m_requestNumber = 0;
        m_isFound = false;
    }
};

