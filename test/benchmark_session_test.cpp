// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <vector>
#include <fplus/fplus.hpp>
#include <fplus/benchmark_session.hpp>


// This is an example on how to use benchmark_session in order to bench separate parts of an algorithm

// You need to instantiate a session into which the stats will be collected
fplus::benchmark_session my_benchmark_session;


// Benchmarked function : several sub parts of this function are benchmarked separately
int benchmark_example()
{
    using Ints = std::vector<int>;

    // Example 1 : benchmark by replacing a function
    // We want to benchmark the following code :
    //    Ints ascending_numbers = fplus::numbers(0, 1000);
    //
    // So, first we make an alternate version of the function  
    //   "fplus::numbers<int, std::vector<int>>"
    // numbers_bench has the same signature as fplus::numbers, except that
    // it also stores stats into the benchmark session, under the name "numbers"
    auto numbers_bench = make_benchmark_function(
        my_benchmark_session,
        "numbers",
        fplus::numbers<int, std::vector<int>>
    );
    // Then, we replace the original code (Ints ascending_numbers = fplus::numbers(0, 1000);)
    // by a code that uses the benchmarked function
    Ints ascending_numbers = numbers_bench(0, 10000);

    // Example 2: benchmark by replacing an expression
    // Below, we will benchmark an expression
    // The original expression we want to benchmark was:
    //     Ints shuffled_numbers = fplus::shuffle(std::mt19937::default_seed, ascending_numbers);
    //
    // In order to do so, we just copy/paste this expression into the bench_expression macro, 
    // like shown below. 
    // This expression will then be benchmarked with the name "shuffle" 
    Ints shuffled_numbers = benchmark_expression(
        my_benchmark_session,
        "shuffle",
        fplus::shuffle(std::mt19937::default_seed, ascending_numbers);
    );

    // Example 3: also benchmark by replacing an expression
    // The original expression was
    //    const auto sorted_numbers = fplus::sort(shuffled_numbers);
    const auto sorted_numbers = benchmark_expression(
        my_benchmark_session,
        "sort",
        fplus::sort(shuffled_numbers);
    );

    return 1;
}

TEST_CASE("benchmark_example")
{
    // Example 4 : benchmark by replacing a function
    // We also want to benchmark the "benchmark_example" in its entirety
    auto benchmark_example_bench = make_benchmark_function(
        my_benchmark_session, 
        "benchmark_example", 
        benchmark_example);

    // For the sake of this test, we will run the benchmarked function several times
    fplus::run_n_times(1000, [&]() { benchmark_example_bench(); });

    std::cout << fplus::show(my_benchmark_session.report());
    // Will output something like
    // Function         |Nb calls|Total time|Av. time|Deviation|
    // -----------------+--------+----------+--------+---------+
    // benchmark_example|    1000|  71.862ms|71.862ns| 12.243ns|
    // shuffle          |    1000|  48.094ms|48.094ns|  6.687ns|
    // sort             |    1000|  21.389ms|21.389ns|  5.760ns|
    // numbers          |    1000|   1.736ms| 1.736ns|  0.496ns|
}

