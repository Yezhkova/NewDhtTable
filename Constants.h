//
// Created by odal on 9/7/23.
//

#pragma once

const size_t SWARM_SIZE = 100000;
const size_t ITER_NUMBER = 2000;
const size_t CLOSEST_NODES_CAPACITY = 24;
const size_t CLOSEST_NODES_NUMBER = 1;
const size_t MAX_FIND_COUNTER = 4000;
const size_t MAX_REQURSION_COUNTER = 200;

// CONSTs are handled by the compiler, #DEFINEs are handled by the pre-processor.
// The big advantage of const over #define is type checking.
// #defines can’t be type checked, so this can cause problems when trying to determine the data type.
