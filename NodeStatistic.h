#pragma once
#include <iostream>

struct NodeStatistic
{
    uint64_t    m_requestNumber;
    bool        m_isFound;
    uint64_t    m_reqursionNumber;

    bool tooManyRequests() const { return m_requestNumber >= MAX_FIND_COUNTER || m_reqursionNumber > MAX_REQURSION_COUNTER; }

    void resetCounters()
    {
        m_requestNumber = 0;
        m_isFound = false;
        m_reqursionNumber = 0;
    }
};

