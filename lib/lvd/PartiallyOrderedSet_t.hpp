// 2020.12.15 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <cassert>
#include <functional>
#include "lvd/abort.hpp"
#include "lvd/PartialOrder.hpp"
#include <map>
#include <ostream>
#include <set>
#include <string>

namespace lvd {

// A data structure which stores particular elements of a partially ordered set (poset),
// and provides methods which can search for upper and lower bounds on given values which
// don't necessarily have to be explicitly stored in the poset container.
template <typename T_>
class PartiallyOrderedSet_t {
private:

    // Note that actual enforcement of the acyclic property is not yet implemented (and might not be
    // without restrictions on node insertion order).  Enforcement of acyclic property is done by the
    // particular logic of PartiallyOrderedSet_t::insert.
    //
    // Ideally this would implement iterator-based depth-first and breadth-first traversal, which would
    // be a much more efficient replacement for the descendants_of and ancestors_of methods.
    class DirectedAcyclicGraph {
    public:

        using NodeSet = std::set<T_>;
        using EdgeMap = std::map<T_,std::set<T_>>;

        bool is_a_root_node (T_ const &x) const { return m_root_nodes.find(x) != m_root_nodes.end(); }
        bool is_a_leaf_node (T_ const &x) const { return m_leaf_nodes.find(x) != m_leaf_nodes.end(); }

        // Returns the number of nodes in the DAG.
        size_t size () const { return m_child_map.size(); }
        // A root node is defined as a node that is not the target of any edge.
        NodeSet const &root_nodes () const { return m_root_nodes; }
        // A leaf node is defined as a node that is not the source of any edge.
        NodeSet const &leaf_nodes () const { return m_leaf_nodes; }
        // For a given [source] node in the graph, child_map produces the NodeSet of target nodes.
        EdgeMap const &child_map () const { return m_child_map; }
        // Gives the backward edges.  This is information that's equivalent to child_map() but
        // is efficiently indexed to look up based on target node.
        EdgeMap const &parent_map () const { return m_parent_map; }
        // Convenience method which returns the NodeSet of children of the given node.
        // Will throw std::out_of_range if the given node is not present in the graph.
        NodeSet const &children_of (T_ const &node) const { return m_child_map.at(node); }
        // Convenience method which returns the NodeSet of parents of the given node.
        // Will throw std::out_of_range if the given node is not present in the graph.
        NodeSet const &parents_of (T_ const &node) const { return m_parent_map.at(node); }

        NodeSet descendants_of (T_ const &node, bool include_node = false) const {
            NodeSet descendants;
            collect_descendants_of(node, descendants, include_node);
            return descendants;
        }
        NodeSet ancestors_of (T_ const &node, bool include_node = false) const {
            NodeSet ancestors;
            collect_ancestors_of(node, ancestors, include_node);
            return ancestors;
        }
        // This is sort of a dumb function, but whateva.  TODO: Probably move to test code
        NodeSet nondescendants_of (T_ const &node, bool include_node = false) const {
            auto descendants = descendants_of(node, !include_node);
            NodeSet nondescendants;
            for (auto const &it : m_child_map) {
                auto const &node = it.first;
                if (descendants.find(node) == descendants.end())
                    nondescendants.insert(node);
            }
            return nondescendants;
        }
        // This is sort of a dumb function, but whateva.  TODO: Probably move to test code
        NodeSet nonancestors_of (T_ const &node, bool include_node = false) const {
            auto ancestors = ancestors_of(node, !include_node);
            NodeSet nonancestors;
            for (auto const &it : m_parent_map) {
                auto const &node = it.first;
                if (ancestors.find(node) == ancestors.end())
                    nonancestors.insert(node);
            }
            return nonancestors;
        }

        // Returns true iff the specified element is present in the graph.
        bool contains_node (T_ const &node) const { return m_child_map.find(node) != m_child_map.end(); }
        // Returns true iff the edge having given source and target nodes is present in the graph.
        bool contains_edge (T_ const &source, T_ const &target) const {
            auto it = m_child_map.find(source);
            if (it == m_child_map.end())
                return false;
            return it->second.find(target) != it->second.end();
        }
        // Adds the given node to the graph.  Nothing is done if the node is already present.
        void insert_node (T_ const &node) {
            if (m_child_map.find(node) == m_child_map.end()) {
                assert(m_parent_map.find(node) == m_parent_map.end());
                m_root_nodes.insert(node);
                m_leaf_nodes.insert(node);
                m_child_map.emplace(node, NodeSet{});
                m_parent_map.emplace(node, NodeSet{});
            }
        }
        // Adds the given edge to the graph.  Nothing is done if the edge is already present.
        void insert_edge (T_ const &source, T_ const &target) {
            assert(source != target);
            assert(m_child_map.find(source) != m_child_map.end());
            assert(m_child_map.find(target) != m_child_map.end());
            assert(m_parent_map.find(source) != m_parent_map.end());
            assert(m_parent_map.find(target) != m_parent_map.end());
            m_leaf_nodes.erase(source);
            m_root_nodes.erase(target);
            m_child_map.at(source).insert(target);
            m_parent_map.at(target).insert(source);
        }
        // This will replace `source -> target` with `source -> node -> target`, i.e. will
        // perform a local transitive reduction.
        // TODO/NOTE: This probably doesn't suffice by itself to produce an actually transitively reduced DAG.
        // For example, if edges A->B and A->C already exist, and B->C is added, this doesn't remove
        // A->C.  Or similarly, if A->C and B->C already exist and A->B is added, then this doesn't
        // remove A->C.  But as used by PartiallyOrderedSet_t, it does seem to produce correct results.
        void insert_node_along_edge (T_ const &source, T_ const &node, T_ const &target) {
            assert(source != target);
            assert(source != node);
            assert(target != node);
            assert(m_child_map.find(source) != m_child_map.end());
            assert(m_child_map.find(target) != m_child_map.end());
            assert(m_parent_map.find(source) != m_parent_map.end());
            assert(m_parent_map.find(target) != m_parent_map.end());
            insert_node(node);
            insert_edge(source, node);
            insert_edge(node, target);
            {
                auto &source_child_nodes = m_child_map.at(source);
                source_child_nodes.erase(target);
                assert(source_child_nodes.find(target) == source_child_nodes.end());
            }
            {
                auto &target_parent_nodes = m_parent_map.at(target);
                target_parent_nodes.erase(source);
                assert(target_parent_nodes.find(source) == target_parent_nodes.end());
            }
        }

        // NodeSet descendants is the out-parameter into which the results will be inserted.
        void collect_descendants_of (T_ const &node, NodeSet &descendants, bool include_node = false) const {
            assert(m_child_map.find(node) != m_child_map.end()); // TODO: Support non-exact matches for node
            if (include_node)
                descendants.insert(node);
            for (auto const &child : m_child_map.at(node))
                collect_descendants_of(child, descendants, true);
        }
        // NodeSet ancestors is the out-parameter into which the results will be inserted.
        void collect_ancestors_of (T_ const &node, NodeSet &ancestors, bool include_node = false) const {
            assert(m_parent_map.find(node) != m_parent_map.end()); // TODO: Support non-exact matches for node
            if (include_node)
                ancestors.insert(node);
            for (auto const &parent : m_parent_map.at(node))
                collect_ancestors_of(parent, ancestors, true);
        }

        void print_dot_graph (std::ostream &out, std::string const &title, std::function<void(std::ostream &,T_ const &)> const &print_node) const {
            // Reference: https://en.wikipedia.org/wiki/DOT_(graph_description_language)#Directed_graphs
            out << "digraph {\n    // title\n    labelloc=\"t\";\n    label=\"" << title << "\";\n";
            for (auto const &it : child_map()) {
                auto const &node = it.first;
                out << "    ";
                print_node(out, node);
                out << ";\n";
                for (auto const &child : it.second) {
                    out << "    ";
                    print_node(out, node);
                    out << " -> ";
                    print_node(out, child);
                    out << ";\n";
                }
            }
            out << "}\n";
        }

    private:

        NodeSet m_root_nodes;
        NodeSet m_leaf_nodes;
        EdgeMap m_child_map;
        EdgeMap m_parent_map;
    };

public:

    using NodeSet = typename DirectedAcyclicGraph::NodeSet;
    using EdgeMap = typename DirectedAcyclicGraph::EdgeMap;
    using RelationshipEvaluator = std::function<PartialOrder(T_ const &,T_ const &)>;

    template <typename... Args_>
    PartiallyOrderedSet_t (Args_&&... args)
        :   m_rel_eval(std::forward<Args_>(args)...)
    { }

    RelationshipEvaluator const &rel_eval () const { return m_rel_eval; }

    bool is_a_root_node (T_ const &x) const { return m_dag.is_a_root_node(x); }
    bool is_a_leaf_node (T_ const &x) const { return m_dag.is_a_leaf_node(x); }

    // Returns the number of elements in the poset.
    size_t size () const { return m_dag.size(); }
    // A root node is defined as a node that is not the target of any edge.
    NodeSet const &root_nodes () const { return m_dag.root_nodes(); }
    // A leaf node is defined as a node that is not the source of any edge.
    NodeSet const &leaf_nodes () const { return m_dag.leaf_nodes(); }
    // For a given [source] node in the graph, child_map produces the NodeSet of target nodes.
    EdgeMap const &child_map () const { return m_dag.child_map(); }
    // Gives the backward edges.  This is information that's equivalent to child_map() but
    // is efficiently indexed to look up based on target node.
    EdgeMap const &parent_map () const { return m_dag.parent_map(); }
    // Convenience method which returns the NodeSet of children of the given node.
    NodeSet const &children_of (T_ const &node) const { return m_dag.children_of(node); }
    // Convenience method which returns the NodeSet of parents of the given node.
    NodeSet const &parents_of (T_ const &node) const { return m_dag.parents_of(node); }
    // Returns true iff the specified element is in the PartiallyOrderedSet_t.
    bool contains (T_ const &x) const { return m_dag.contains_node(x); }

    NodeSet descendants_of (T_ const &node, bool include_node = false) const { return m_dag.descendants_of(node, include_node); }
    NodeSet ancestors_of (T_ const &node, bool include_node = false) const { return m_dag.ancestors_of(node, include_node); }
    // This is sort of a dumb function, but whateva.  TODO: Probably move to test code
    NodeSet nondescendants_of (T_ const &node, bool include_node = false) const { return m_dag.nondescendants_of(node, include_node); }
    // This is sort of a dumb function, but whateva.  TODO: Probably move to test code
    NodeSet nonancestors_of (T_ const &node, bool include_node = false) const { return m_dag.nonancestors_of(node, include_node); }

    void print_dot_graph (std::ostream &out, std::string const &title, std::function<void(std::ostream &, T_ const &)> const &print_node) const { m_dag.print_dot_graph(out, title, print_node); }

    // NOTE: This is an elegant solution, but is probably not super efficient.
    // Could potentially compute lub from glb in order to improve efficiency.
    void insert (T_ const &x) {
        auto glb = greatest_lower_bound_of(x);
        auto lub = least_upper_bound_of(x);
        for (auto const &b : glb) {
            auto r = m_rel_eval(b, x);
            assert(r == PartialOrder::EQUAL || r == PartialOrder::LESS_THAN);
            for (auto const &c : m_dag.children_of(b)) {
                r = m_rel_eval(x, c);
                assert(r == PartialOrder::LESS_THAN || r == PartialOrder::INCOMPARABLE);
            }
        }
        for (auto const &b : lub) {
            auto r = m_rel_eval(x, b);
            assert(r == PartialOrder::EQUAL || r == PartialOrder::LESS_THAN);
            for (auto const &p : m_dag.parents_of(b)) {
                r = m_rel_eval(p, x);
                assert(r == PartialOrder::LESS_THAN || r == PartialOrder::INCOMPARABLE);
            }
        }

        if (glb.empty()) {
            if (lub.empty()) {
                m_dag.insert_node(x);
            } else {
                m_dag.insert_node(x);
                for (auto const &c : lub)
                    m_dag.insert_edge(x, c);
            }
        } else {
            if (lub.empty()) {
                m_dag.insert_node(x);
                for (auto const &p : glb)
                    m_dag.insert_edge(p, x);
            } else {
                if (glb == lub) {
                    assert(m_dag.contains_node(x));
                    return; // The node is already in the graph, so there's nothing to do.
                }
                for (auto const &p : glb) {
                    for (auto const &c : lub) {
                        assert(m_rel_eval(p, c) != PartialOrder::EQUAL);
                        m_dag.insert_node_along_edge(p, x, c);
                    }
                }
            }
        }
    }

    // Returns the maximal set of graph nodes B such that for each b in B and each child c of b, b <= x < c.
    // If x < r for each root node r, then this will return the empty set.  In particular, if x is not in
    // the graph, then b < x < c for each b in B.
    NodeSet greatest_lower_bound_of (T_ const &x) const {
        NodeSet retval;
        for (auto const &root_node : m_dag.root_nodes())
            greatest_lower_bound__recursive(x, root_node, retval);
        return retval;
    }
    // Returns the maximal set of graph nodes B such that for each b in B and each parent p of b, p < x <= b.
    // If l < x for each leaf node l, then this will return the empty set.  In particular, if x is not in
    // the graph, then p < x < b for each b in B.
    NodeSet least_upper_bound_of (T_ const &x) const {
        NodeSet retval;
        for (auto const &leaf_node : m_dag.leaf_nodes())
            least_upper_bound__recursive(x, leaf_node, retval);
        return retval;
    }

private:

    bool greatest_lower_bound__recursive (T_ const &x, T_ const &node, NodeSet &retval) const {
        switch (m_rel_eval(node, x)) {
            case PartialOrder::EQUAL:
                retval.insert(node);
                return true;

            case PartialOrder::GREATER_THAN:
            case PartialOrder::INCOMPARABLE:
                return false;

            case PartialOrder::LESS_THAN: {
                // Check the children of node.
                bool inserted = false;
                for (auto const &child : m_dag.children_of(node)) {
                    switch (m_rel_eval(x, child)) {
                        case PartialOrder::LESS_THAN:
                            retval.insert(node);
                            inserted = true;
                            break;

                        case PartialOrder::INCOMPARABLE:
                            break;

                        case PartialOrder::EQUAL:
                        case PartialOrder::GREATER_THAN:
                            // TODO: Maybe preserve the value of m_rel_eval(x, child) and pass it along to be used again.
                            if (greatest_lower_bound__recursive(x, child, retval))
                                inserted = true;
                            break;
                    }
                }
                if (!inserted)
                    retval.insert(node);
                return true;
            }

            default: LVD_ABORT("invalid PartialOrder");
        }
    }
    bool least_upper_bound__recursive (T_ const &x, T_ const &node, NodeSet &retval) const {
        switch (m_rel_eval(node, x)) {
            case PartialOrder::EQUAL:
                retval.insert(node);
                return true;

            case PartialOrder::LESS_THAN:
            case PartialOrder::INCOMPARABLE:
                return false;

            case PartialOrder::GREATER_THAN: {
                // Check the children of node.
                bool inserted = false;
                for (auto const &parent : m_dag.parents_of(node)) {
                    switch (m_rel_eval(x, parent)) {
                        case PartialOrder::GREATER_THAN:
                            retval.insert(node);
                            inserted = true;
                            break;

                        case PartialOrder::INCOMPARABLE:
                            break;

                        case PartialOrder::EQUAL:
                        case PartialOrder::LESS_THAN:
                            // TODO: Maybe preserve the value of m_rel_eval(x, parent) and pass it along to be used again.
                            if (least_upper_bound__recursive(x, parent, retval))
                                inserted = true;
                            break;
                    }
                }
                if (!inserted)
                    retval.insert(node);
                return true;
            }

            default: LVD_ABORT("invalid PartialOrder");
        }
    }

    RelationshipEvaluator m_rel_eval;
    DirectedAcyclicGraph m_dag;
};

template <typename T_>
std::ostream &operator << (std::ostream &out, PartiallyOrderedSet_t<T_> const &poset) {
    out << "poset with " << poset.child_map().size() << " nodes {\n";
    out << "    root nodes:";
    for (auto root_node : poset.root_nodes())
        out << ' ' << root_node;
    out << '\n';
    out << "    leaf nodes:";
    for (auto leaf_node : poset.leaf_nodes())
        out << ' ' << leaf_node;
    out << '\n';
    out << "    forward edges:\n";
    for (auto it : poset.child_map()) {
        for (auto t : it.second)
            out << "        " << it.first << " -> " << t << '\n';
    }
    out << "    backward edges:\n";
    for (auto it : poset.parent_map()) {
        for (auto t : it.second)
            out << "        " << it.first << " -> " << t << '\n';
    }
    return out << "}";
}

} // end namespace lvd
