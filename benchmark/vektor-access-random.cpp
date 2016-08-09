
#include <nonius/nonius_single.h++>

#include <immu/vektor.hpp>
#include <immu/dvektor.hpp>
#include <vector>
#include <list>
#include <random>
#include <functional>

#if IMMU_BENCHMARK_LIBRRB
extern "C" {
#define restrict __restrict__
#include <rrb.h>
#undef restrict
}
#endif

NONIUS_PARAM("size", std::size_t{1000})

auto make_generator(std::size_t runs)
{
    assert(runs > 0);
    auto engine = std::default_random_engine{42};
    auto dist = std::uniform_int_distribution<std::size_t>{0, runs-1};
    auto r = std::vector<std::size_t>(runs);
    std::generate_n(r.begin(), runs, std::bind(dist, engine));
    return r;
}

NONIUS_BENCHMARK("std::vector", [] (nonius::parameters params)
{
    auto benchmark_size = params.get<std::size_t>("size");

    auto g = make_generator(benchmark_size);
    auto v = std::vector<unsigned>(benchmark_size);
    std::iota(v.begin(), v.end(), 0u);

    return [=] {
        volatile auto r = 0u;
        for (auto i = 0u; i < benchmark_size; ++i)
            r += v[g[i]];
        return r;
    };
})

#if IMMU_BENCHMARK_LIBRRB
NONIUS_BENCHMARK("librrb", [] (nonius::parameters params)
{
    auto benchmark_size = params.get<std::size_t>("size");

    auto v = rrb_create();
    for (auto i = 0u; i < benchmark_size; ++i)
        v = rrb_push(v, reinterpret_cast<void*>(i));
    auto g = make_generator(benchmark_size);

    return [=] {
        volatile auto r = 0u;
        for (auto i = 0u; i < benchmark_size; ++i)
            r += reinterpret_cast<unsigned long>(rrb_nth(v, g[i]));
        return r;
    };
})
#endif

NONIUS_BENCHMARK("immu::vektor", [] (nonius::parameters params)
{
    auto benchmark_size = params.get<std::size_t>("size");

    auto v = immu::vektor<unsigned>{};
    for (auto i = 0u; i < benchmark_size; ++i)
        v = v.push_back(i);
    auto g = make_generator(benchmark_size);

    return [=] {
        volatile auto r = 0u;
        for (auto i = 0u; i < benchmark_size; ++i)
            r += v[g[i]];
        return r;
    };
})

NONIUS_BENCHMARK("immu::dvektor", [] (nonius::parameters params)
{
    auto benchmark_size = params.get<std::size_t>("size");

    auto v = immu::dvektor<unsigned>{};
    for (auto i = 0u; i < benchmark_size; ++i)
        v = v.push_back(i);
    auto g = make_generator(benchmark_size);

    return [=] {
        volatile auto r = 0u;
        for (auto i = 0u; i < benchmark_size; ++i)
            r += v[g[i]];
        return r;
    };
})