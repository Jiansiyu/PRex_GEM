//============================================================================//
// A simple benchmark class                                                   //
//                                                                            //
// From Chao Peng                                                             //
// 02/17/2016                                                                 //
//============================================================================//

#ifndef BENCH_MARK_H
#define BENCH_MARK_H

#include <chrono>

class BenchMark
{
public:
    BenchMark();
    ~BenchMark();
    void Reset();
    unsigned int GetElapsedTime();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
};

#endif
