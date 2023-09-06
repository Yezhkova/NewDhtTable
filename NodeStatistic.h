#pragma once
#include <iostream>

struct NodeStatistic
{
    uint64_t    m_requestNumber;
    bool        m_isfound;
    
    void resetCounters()
    {
        m_requestNumber = 0;
        m_isfound = false;
    }
};

