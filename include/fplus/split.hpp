// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "fplus/container_common.hpp"
#include "fplus/generate.hpp"
#include "fplus/search.hpp"
#include "fplus/transform.hpp"
#include "fplus/pairs.hpp"
#include "fplus/numeric.hpp"

namespace fplus
{

// API search type: group_by : (((a, a) -> Bool), [a]) -> [[a]]
// group_by((==), [1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],[4],[5,5]]
// ContainerOut is not deduced to
// SameContNewType(ContainerIn, ContainerIn)
// here, since ContainerIn could be a std::string.
// BinaryPredicate p is a (not neccessarily transitive) connectivity check.
// O(n)
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_by(BinaryPredicate p, const ContainerIn& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    ContainerOut result;
    if (is_empty(xs))
        return result;
    typedef typename ContainerOut::value_type InnerContainerOut;
    *internal::get_back_inserter(result) = InnerContainerOut(1, xs.front());
    for (auto it = ++std::begin(xs); it != std::end(xs); ++it)
    {
        if (p(result.back().back(), *it))
            *internal::get_back_inserter(result.back()) = *it;
        else
            *internal::get_back_inserter(result) = InnerContainerOut(1, *it);
    }
    return result;
}

// API search type: group_on : ((a -> b), [a]) -> [[a]]
// group_on((mod 10), [12,22,34]) == [[12,22],[34]]
// O(n)
template <typename F, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_on(F f, const ContainerIn& xs)
{
    return group_by(is_equal_by(f), xs);
}

// API search type: group : [a] -> [[a]]
// group([1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2],[3],[2,2],[4],[5,5]]
// O(n)
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group(const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerIn::value_type T;
    auto pred = [](const T& x, const T& y) { return x == y; };
    return group_by<decltype(pred), ContainerIn, ContainerOut>(pred, xs);
}

// API search type: group_globally_by : (((a, a) -> Bool), [a]) -> [[a]]
// group_globally_by((==), [1,2,2,2,3,2,2,4,5,5])
// == [[1],[2,2,2,2,2],[3],[4],[5,5]]
// BinaryPredicate p is a transitive equality check.
// O(n^2)
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_globally_by(BinaryPredicate p, const ContainerIn& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerOut::value_type InnerContainerOut;
    ContainerOut result;
    for (const auto& x : xs)
    {
        bool found = false;
        for (auto& ys : result)
        {
            if (p(x, ys.back()))
            {
                *internal::get_back_inserter(ys) = x;
                found = true;
                break;
            }
        }
        if (!found)
        {
            *internal::get_back_inserter(result) = InnerContainerOut(1, x);
        }
    }
    return result;
}

// API search type: group_globally_on : ((a -> b), [a]) -> [[a]]
// group_globally_on((mod 10), [12,34,22]) == [[12,34],[22]]
// O(n^2)
template <typename F, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_globally_on(F f, const ContainerIn& xs)
{
    return group_globally_by(is_equal_by(f), xs);
}

// API search type: group_globally : [a] -> [[a]]
// group_globally([1,2,2,2,3,2,2,4,5,5]) == [[1],[2,2,2,2,2],[3],[4],[5,5]]
// O(n^2)
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut group_globally(const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    typedef typename ContainerIn::value_type T;
    auto pred = [](const T& x, const T& y) { return x == y; };
    return group_globally_by(pred, xs);
}

// API search type: cluster_by : (((a, a) -> Bool), [a]) -> [[a]]
// Groups connected components, stable regarding initial order.
// cluster_by(\x y -> abs (y - x) <= 3), [2,3,6,4,12,11,20,23,8,4])
// == [[2,3,6,4,12,11,8,4],[20,23]]
// BinaryPredicate p is a connectivity check, being
//  - commutative
//  - reflexive
//  - not neccessarily transitive, but can be
// O(n^2), memory complexity also O(n^2)
template <typename BinaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut cluster_by(BinaryPredicate p, const ContainerIn& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");

    typedef std::vector<unsigned char> bools;
    bools zero_filled_row(size_of_cont(xs), 0);

    // adjecency matrix
    typedef std::vector<bools> boolss;
    boolss adj_mat(size_of_cont(xs), zero_filled_row);

    for (const auto& idx_and_val_y : enumerate(xs))
    {
        auto idx_y = idx_and_val_y.first;
        auto val_y = idx_and_val_y.second;
        for (const auto& idx_and_val_x : enumerate(xs))
        {
            auto idx_x = idx_and_val_x.first;
            auto val_x = idx_and_val_x.second;
            if (p(val_y, val_x))
            {
                adj_mat[idx_y][idx_x] = 1;
            }
        }
    }

    bools already_used = zero_filled_row;
    auto is_already_used = [&](std::size_t i) -> bool
    {
        return already_used[i] != 0;
    };

    typedef std::vector<std::size_t> idxs;
    typedef std::vector<idxs> idxss;

    auto bools_to_idxs = [](const bools& activations) -> idxs
    {
        auto unsigned_char_to_bool = [](unsigned char x)
        {
            return x != 0;
        };
        return find_all_idxs_by(unsigned_char_to_bool, activations);
    };

    idxss idx_clusters;
    std::function<void(std::size_t)> process_idx = [&](std::size_t idx) -> void
    {
        auto connected_idxs = bools_to_idxs(adj_mat[idx]);
        auto new_connected_idxs = drop_if(is_already_used, connected_idxs);
        if (is_empty(new_connected_idxs))
        {
            return;
        }
        idx_clusters.back() = append(idx_clusters.back(), new_connected_idxs);
        for (const auto& new_idx : new_connected_idxs)
        {
            already_used[new_idx] = 1;
        }
        for (const auto& new_idx : new_connected_idxs)
        {
            process_idx(new_idx);
        }
    };

    typedef typename ContainerOut::value_type InnerContainerOut;

    for (const auto& idx : all_idxs(xs))
    {
        if (is_already_used(idx))
        {
            continue;
        }
        *internal::get_back_inserter(idx_clusters) = idxs();
        *internal::get_back_inserter(idx_clusters.back()) = idx;
        already_used[idx] = 1;
        process_idx(idx);
    }

    typedef typename ContainerIn::value_type T;

    auto idx_to_val = [&](std::size_t idx) -> T
    {
        return elem_at_idx(idx, xs);
    };

    auto idxs_to_vals = [&](const idxs& val_idxs) -> InnerContainerOut
    {
        return transform_convert<InnerContainerOut>(idx_to_val, sort(val_idxs));
    };

    return transform_convert<ContainerOut>(idxs_to_vals, idx_clusters);
}

// API search type: split_by : ((a -> Bool), Bool, [a]) -> [[a]]
// split_by(is_even, true, [1,3,2,2,5,5,3,6,7,9]) == [[1,3],[],[5,5,3],[7,9]]
// also known as split_when
// O(n)
template <typename UnaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_by
        (UnaryPredicate pred, bool allow_empty, const ContainerIn& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");

    if (allow_empty && is_empty(xs))
    {
        return {{}};
    }

    ContainerOut result;
    auto itOut = internal::get_back_inserter(result);
    auto start = std::begin(xs);

    while (start != std::end(xs))
    {
        const auto stop = std::find_if(start, std::end(xs), pred);
        if (start != stop || allow_empty)
        {
            *itOut = { start, stop };
        }
        if (stop == std::end(xs))
        {
            break;
        }
        start = std::next(stop);
        if (allow_empty && start == std::end(xs))
        {
            *itOut = {};
        }
    }
    return result;
}

// API search type: split_by_keep_separators : ((a -> Bool), [a]) -> [[a]]
// split_by_keep_separators(is_even, true, [1,3,2,2,5,5,3,6,7,9])
// == [[1,3],[2],[2,5,5,3],[6,7,9]]
// O(n)
template <typename UnaryPredicate, typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_by_keep_separators
        (UnaryPredicate pred, const ContainerIn& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, ContainerIn>();
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    ContainerOut result;
    if (is_empty(xs))
        return result;
    auto itOut = internal::get_back_inserter(result);
    auto start = std::begin(xs);
    while (start != std::end(xs))
    {
        const auto stop = std::find_if(std::next(start), std::end(xs), pred);
        *itOut = { start, stop };
        if (stop == std::end(xs))
        {
            break;
        }
        start = stop;
    }
    return result;
}

// API search type: split : (a, [a]) -> [[a]]
// split(0, [1,3,2,0,0,6,0,7,5]) == [[1,3,2],[],[6],[7,5]]
// O(n)
template <typename ContainerIn,
        typename T = typename ContainerIn::value_type,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split(const T& x, bool allow_empty, const ContainerIn& xs)
{
    return split_by(is_equal_to(x), allow_empty, xs);
}

// API search type: split_keep_separators : ((a -> Bool), [a]) -> [[a]]
// split_keep_separators(2, true, [1,3,2,2,5,5,3,2,7,9])
// == [[1,3],[2],[2,5,5,3],[6,7,9]]
// O(n)
template <typename ContainerIn,
        typename T = typename ContainerIn::value_type,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_keep_separators(const T& x, const ContainerIn& xs)
{
    return split_by_keep_separators(is_equal_to(x), xs);
}

// API search type: split_at_idx : (Int, [a]) -> ([a], [a])
// split_at_idx(2, [0,1,2,3,4]) == ([0,1],[2,3,4])
template <typename Container>
std::pair<Container, Container> split_at_idx
        (std::size_t idx, const Container& xs)
{
    assert(idx <= size_of_cont(xs));
    return make_pair(get_range(0, idx, xs),
        get_range(idx, size_of_cont(xs), xs));
}

// API search type: partition : ((a -> Bool), [a]) -> ([a], [a])
// partition(is_even, [0,1,1,3,7,2,3,4]) == ([0,2,4],[1,1,3,7,3])
template <typename UnaryPredicate, typename Container>
std::pair<Container, Container> partition
        (UnaryPredicate pred, const Container& xs)
{
    internal::check_unary_predicate_for_container<UnaryPredicate, Container>();
    Container matching;
    Container notMatching;
    auto itOutMatching = internal::get_back_inserter(matching);
    auto itOutNotMatching = internal::get_back_inserter(notMatching);
    for (const auto& x : xs)
    {
        if (pred(x))
            *itOutMatching = x;
        else
            *itOutNotMatching = x;
    }
    return make_pair(matching, notMatching);
}

// API search type: split_at_idxs : ([Int], [a]) -> [[a]]
// split_at_idxs([2,5], [0,1,2,3,4,5,6,7]) == [[0,1],[2,3,4],[5,6,7]]
// split_at_idxs([2,5,5], [0,1,2,3,4,5,6,7]) == [[0,1],[2,3,4],[],[5,6,7]]
template <typename ContainerIdxs, typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut split_at_idxs(const ContainerIdxs& idxsIn, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    ContainerIdxs idxStartC = {0};
    ContainerIdxs idxEndC = {size_of_cont(xs)};
    std::vector<ContainerIdxs> containerIdxss = {idxStartC, idxsIn, idxEndC};
    auto idxs = concat(containerIdxss);
    auto idxsClean = sort(idxs);
    ContainerOut result;
    internal::prepare_container(result, size_of_cont(idxsClean) + 1);
    auto itOut = internal::get_back_inserter(result);
    auto idxPairs = overlapping_pairs(idxsClean);
    for (const auto& idxPair : idxPairs)
    {
        *itOut = get_range(idxPair.first, idxPair.second, xs);
    }
    return result;
}

// API search type: split_every : (Int, [a]) -> [[a]]
// split_every(3, [0,1,2,3,4,5,6,7]) == [[0,1,2],[3,4,5],[6,7]]
// Also known as chunk.
template <typename ContainerIn,
        typename ContainerOut = std::vector<ContainerIn>>
ContainerOut split_every(std::size_t n, const ContainerIn& xs)
{
    return split_at_idxs(
        generate_range_step<std::vector<std::size_t>, std::size_t>(
            n, size_of_cont(xs), n),
        xs);
}

// API search type: split_by_token : ([a], Bool, [a]) -> [[a]]
// split_by_token(", ", "foo, bar, baz") == ["foo", "bar", "baz"]
template <typename ContainerIn,
        typename ContainerOut = typename std::vector<ContainerIn>>
ContainerOut split_by_token(const ContainerIn& token,
        bool allow_empty, const ContainerIn& xs)
{
    static_assert(std::is_same<ContainerIn,
        typename ContainerOut::value_type>::value,
        "Containers do not match.");
    const auto token_begins =
        find_all_instances_of_token_non_overlapping(token, xs);
    const auto token_ends =
        transform(add_to<std::size_t>(size_of_cont(token)), token_begins);
    assert(is_sorted(interweave(token_begins, token_ends)));

    typedef std::vector<std::size_t> idx_vec;
    const auto ranges = zip(
        fplus::append(idx_vec(1, 0), token_ends),
        fplus::append(token_begins, idx_vec(1, size_of_cont(xs))));

    ContainerOut result;
    auto itOut = internal::get_back_inserter(result);
    for (const auto& range : ranges)
    {
        if (range.first != range.second || allow_empty)
        *itOut = get_range(range.first, range.second, xs);
    }
    return result;
}

// API search type: count_occurrences : [a] -> Map a Int
// count_occurrences([1,2,2,3,2)) == [(1, 1), (2, 3), (3, 1)]
// O(n)
template <typename ContainerIn,
        typename MapOut = typename std::map<
            typename ContainerIn::value_type, std::size_t>>
MapOut count_occurrences(const ContainerIn& xs)
{
    MapOut result;
    for (const auto& x : xs)
    {
        ++result[x];
    }
    return result;
}

// API search type: run_length_encode_by : (((a, a) -> Bool), [a]) -> [(Int, a)]
// run_length_encode_by((==),[1,2,2,2,2,3,3,2)) == [(1,1),(4,2),(2,3),(1,2)]
template <typename BinaryPredicate,
        typename ContainerIn,
        typename T = typename ContainerIn::value_type,
        typename ContainerOut =
            typename std::vector<std::pair<std::size_t, T>>>
ContainerOut run_length_encode_by(BinaryPredicate pred, const ContainerIn& xs)
{
    internal::check_binary_predicate_for_container<BinaryPredicate, ContainerIn>();
    ContainerOut result;
    auto groups = group_by(pred, xs);
    auto group_to_pair = [](const ContainerIn& group )
    {
        return std::make_pair(size_of_cont(group), group.front());
    };
    return transform(group_to_pair, groups);
}

// API search type: run_length_encode : [a] -> [(Int, a)]
// run_length_encode([1,2,2,2,2,3,3,2)) == [(1,1),(4,2),(2,3),(1,2)]
template <typename ContainerIn,
        typename T = typename ContainerIn::value_type,
        typename ContainerOut =
            typename std::vector<std::pair<std::size_t, T>>>
ContainerOut run_length_encode(const ContainerIn& xs)
{
    return run_length_encode_by(is_equal_by(identity<T>), xs);
}

// API search type: run_length_decode : [(Int, a)] -> [a]
// run_length_decode([(1,1),(4,2),(2,3),(1,2)]) == [1,2,2,2,2,3,3,2)
template <typename ContainerIn,
        typename Pair = typename ContainerIn::value_type,
        typename Cnt = typename Pair::first_type,
        typename T = typename Pair::second_type,
        typename ContainerOut = typename std::vector<T>>
ContainerOut run_length_decode(const ContainerIn& pairs)
{
    static_assert(std::is_convertible<Cnt, std::size_t>::value,
        "Count type must be convertible to std::size_t.");
    auto pair_to_vec = apply_to_pair(replicate<T>);
    return concat(transform(pair_to_vec, pairs));
}

// API search type: take_while : ((a -> Bool), [a]) -> [a]
// take_while(is_even, [0,2,4,5,6,7,8]) == [0,2,4]
template <typename Container, typename UnaryPredicate>
Container take_while(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = find_first_idx_by(logical_not(pred), xs);
    return take(just_with_default<std::size_t>(
        size_of_cont(xs), maybeIdx), xs);
}

// API search type: drop_while : ((a -> Bool), [a]) -> [a]
// drop_while(is_even, [0,2,4,5,6,7,8]) == [5,6,7,8]
template <typename Container, typename UnaryPredicate>
Container drop_while(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = find_first_idx_by(logical_not(pred), xs);
    return drop(just_with_default<std::size_t>(
        size_of_cont(xs), maybeIdx), xs);
}

// API search type: span : ((a -> Bool), [a]) -> ([a], [a])
// span, applied to a predicate p and a list xs,
// returns a tuple where first element is longest prefix (possibly empty)
// of xs of elements that satisfy p
// and second element is the remainder of the list.
// span(is_even, [0,2,4,5,6,7,8]) == ([0,2,4], [5,6,7,8])
template <typename Container, typename UnaryPredicate>
std::pair<Container, Container> span(UnaryPredicate pred, const Container& xs)
{
    auto maybeIdx = find_first_idx_by(logical_not(pred), xs);
    return {
        take(just_with_default<std::size_t>(size_of_cont(xs), maybeIdx), xs),
        drop(just_with_default<std::size_t>(size_of_cont(xs), maybeIdx), xs)
    };
}

} // namespace fplus
