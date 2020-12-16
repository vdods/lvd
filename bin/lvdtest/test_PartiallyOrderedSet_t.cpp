// 2020.08.25 - Copyright Victor Dods - Licensed under Apache 2.0

#include <fstream>
#include "lvd/PartiallyOrderedSet_t.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include <random>

namespace lvd {

// TEMP HACK
template <typename T_>
std::ostream &operator << (std::ostream &out, std::set<T_> const &s) {
    out << '{';
    for (auto const &x : s) {
        out << x << ',';
    }
    return out << '}';
}

// TEMP HACK
template <typename T_>
std::ostream &operator << (std::ostream &out, std::vector<T_> const &v) {
    out << '[';
    for (auto const &x : v) {
        out << x << ',';
    }
    return out << ']';
}

PartialOrder uint32_divides (uint32_t a, uint32_t b) {
    if (a == b)
        return PartialOrder::EQUAL;

    if (a == 0) // We know that b != 0.  Any nonzero integer b divides 0 (a).
        return PartialOrder::GREATER_THAN;

    if (b == 0) // Any nonzero integer a divides 0 (b).
        return PartialOrder::LESS_THAN;

    if (a%b == 0) // This means b divides a, noting that a != b.
        return PartialOrder::GREATER_THAN;

    if (b%a == 0) // This means a divides b, noting that a != b.
        return PartialOrder::LESS_THAN;

    return PartialOrder::INCOMPARABLE;
}

// The hot bits indicate which elements are in the set.
PartialOrder uint32_as_subset (uint32_t a, uint32_t b) {
    if (a == b)
        return PartialOrder::EQUAL;

    if (a == 0) // 0 is always a subset of everything.
        return PartialOrder::LESS_THAN;

    if (b == 0) // 0 is always a subset of everything.
        return PartialOrder::GREATER_THAN;

    if ((a&b) == a) // If masking b by a produces a, then a is a subset of b.
        return PartialOrder::LESS_THAN;

    if ((a&b) == b) // If masking a by b produces b, then b is a subset of a.
        return PartialOrder::GREATER_THAN;

    return PartialOrder::INCOMPARABLE;
}

template <typename T_>
void check_node_against_poset (T_ const &node, PartiallyOrderedSet_t<T_> const &poset) {
    // TODO: Should descendants_of and ancestors_of work for non-elements?  Answer is that
    // for a DAG, it's only well-defined for elements of the graph nodes, but not for non-elements.
    // For a poset, it is well-defined.

    bool is_element_of_graph_nodes = poset.contains(node);

    if (is_element_of_graph_nodes) {
        // Test descendants_of and ancestors_of
        auto descendants = poset.descendants_of(node);
        auto ancestors = poset.ancestors_of(node);
        for (auto const &descendant : descendants) {
            assert(poset.rel_eval()(node, node) == PartialOrder::EQUAL);
            assert(poset.rel_eval()(descendant, descendant) == PartialOrder::EQUAL);
            assert(poset.rel_eval()(node, descendant) == PartialOrder::LESS_THAN);
            assert(poset.rel_eval()(descendant, node) == PartialOrder::GREATER_THAN);
        }
        for (auto const &ancestor : ancestors) {
            assert(poset.rel_eval()(node, node) == PartialOrder::EQUAL);
            assert(poset.rel_eval()(ancestor, ancestor) == PartialOrder::EQUAL);
            assert(poset.rel_eval()(node, ancestor) == PartialOrder::GREATER_THAN);
            assert(poset.rel_eval()(ancestor, node) == PartialOrder::LESS_THAN);
        }

        // Test nondescendants_of and nonascestors_of
        auto nondescendants = poset.nondescendants_of(node);
        auto nonancestors = poset.nonancestors_of(node);
        for (auto const &nondescendant : nondescendants) {
            assert(poset.rel_eval()(node, node) == PartialOrder::EQUAL);
            assert(poset.rel_eval()(nondescendant, nondescendant) == PartialOrder::EQUAL);
            assert(poset.rel_eval()(node, nondescendant) != PartialOrder::LESS_THAN);
            assert(poset.rel_eval()(nondescendant, node) != PartialOrder::GREATER_THAN);
        }
        for (auto const &nonancestor : nonancestors) {
            assert(poset.rel_eval()(node, node) == PartialOrder::EQUAL);
            assert(poset.rel_eval()(nonancestor, nonancestor) == PartialOrder::EQUAL);
            assert(poset.rel_eval()(node, nonancestor) != PartialOrder::GREATER_THAN);
            assert(poset.rel_eval()(nonancestor, node) != PartialOrder::LESS_THAN);
        }
    }

    // Test the definition criteria of greatest_lower_bound_of and least_upper_bound_of
    auto glb = poset.greatest_lower_bound_of(node);
    auto lub = poset.least_upper_bound_of(node);
    for (auto const &p : glb) {
        auto r = poset.rel_eval()(p, node);
        assert(r == PartialOrder::LESS_THAN || r == PartialOrder::EQUAL);
        for (auto const &c : poset.children_of(p)) {
            r = poset.rel_eval()(node, c);
            assert(r == PartialOrder::LESS_THAN || r == PartialOrder::INCOMPARABLE);
        }
    }
    for (auto const &c : lub) {
        auto r = poset.rel_eval()(node, c);
        assert(r == PartialOrder::LESS_THAN || r == PartialOrder::EQUAL);
        for (auto const &p : poset.parents_of(c)) {
            r = poset.rel_eval()(p, node);
            assert(r == PartialOrder::LESS_THAN || r == PartialOrder::INCOMPARABLE);
        }
    }
}

template <typename T_>
void verify_poset (PartiallyOrderedSet_t<T_> const &poset) {
    // Run some tests on the poset
    for (auto const &it : poset.child_map()) {
        auto const &member_node = it.first;
        check_node_against_poset(member_node, poset);
    }
}

LVD_TEST_BEGIN(330__PartiallyOrderedSet_t__00)
    std::default_random_engine engine(42000042);
    std::uniform_int_distribution<uint32_t> uniform_dist(0, 0xFF);

    uint32_t const COLLECTION_SIZE = 16;
    // Test all subsets of the first COLLECTION_SIZE uint32_t values.
    for (uint32_t mask = 0; mask < (uint32_t(1) << COLLECTION_SIZE); ++mask) {
//         std::cout << "checking " << std::hex << mask << '\n';
        PartiallyOrderedSet_t<uint32_t> poset(uint32_divides);
        // Add the nodes whose corresponding bits are set in mask.
        std::vector<uint32_t> added_nodes;
        for (uint32_t node = 0; node < COLLECTION_SIZE; ++node) {
            if (((uint32_t(1) << node) & mask) != 0) {
                poset.insert(node);
                added_nodes.push_back(node);
            }
        }

        if (uniform_dist(engine) == 0) {
            std::ofstream fout(LVD_FMT("uint32-div-poset." << std::setw(4) << std::setfill('0') << std::hex << mask << ".dot"));
            auto title = LVD_FMT(added_nodes);
            poset.print_dot_graph(fout, title);
            // This block ensures that fout is flushed and closed before the rest.
        }

        // Verify
        verify_poset(poset);
        for (uint32_t node = 0; node <= 2*COLLECTION_SIZE; ++node)
            if (!poset.contains(node))
                check_node_against_poset(node, poset);
    }
LVD_TEST_END

LVD_TEST_BEGIN(330__PartiallyOrderedSet_t__01)
    std::default_random_engine engine(42000042);
    std::uniform_int_distribution<uint32_t> uniform_dist(0, 0xFF);

    uint32_t const COLLECTION_SIZE = 16;
    // Test all subsets of the first COLLECTION_SIZE uint32_t values.
    for (uint32_t mask = 0; mask < (uint32_t(1) << COLLECTION_SIZE); ++mask) {
//         std::cout << "checking " << std::hex << mask << '\n';
        PartiallyOrderedSet_t<uint32_t> poset(uint32_as_subset);
        // Add the nodes whose corresponding bits are set in mask.
        std::vector<uint32_t> added_nodes;
        for (uint32_t node = 0; node < COLLECTION_SIZE; ++node) {
            if (((uint32_t(1) << node) & mask) != 0) {
                poset.insert(node);
                added_nodes.push_back(node);
            }
        }

        if (uniform_dist(engine) == 0) {
            std::ofstream fout(LVD_FMT("uint32-subset-poset." << std::setw(4) << std::setfill('0') << std::hex << mask << ".dot"));
            auto title = LVD_FMT(added_nodes);
            poset.print_dot_graph(fout, title);
            // This block ensures that fout is flushed and closed before the rest.
        }

        // Verify
        verify_poset(poset);
        for (uint32_t node = 0; node <= 2*COLLECTION_SIZE; ++node)
            if (!poset.contains(node))
                check_node_against_poset(node, poset);
    }
LVD_TEST_END

} // end namespace lvd
