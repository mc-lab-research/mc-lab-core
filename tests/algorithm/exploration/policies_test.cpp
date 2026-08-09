#include <mc_lab/algorithm/exploration/fifo_frontier.hpp>
#include <mc_lab/algorithm/exploration/frontier.hpp>
#include <mc_lab/algorithm/exploration/linear_visited_registry.hpp>
#include <mc_lab/algorithm/exploration/visited_registry.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <vector>

namespace exploration = mc_lab::algorithm::exploration;

static_assert(exploration::Frontier<exploration::fifo_frontier<int>, int>);
static_assert(exploration::VisitedRegistry<exploration::linear_visited_registry<int>, int>);

// -----------------------------------------------------------------------------
// Compile-time rejection of incomplete or malformed policies.
// -----------------------------------------------------------------------------

namespace {

struct frontier_missing_empty {
    void push(int) {}
    [[nodiscard]] auto pop() -> int { return 0; }
};

struct frontier_missing_push {
    [[nodiscard]] auto empty() const noexcept -> bool { return true; }
    [[nodiscard]] auto pop() -> int { return 0; }
};

struct frontier_missing_pop {
    [[nodiscard]] auto empty() const noexcept -> bool { return true; }
    void push(int) {}
};

struct frontier_wrong_pop_type {
    [[nodiscard]] auto empty() const noexcept -> bool { return true; }
    void push(int) {}
    // `pop` must return exactly `State` (`int`), not merely something
    // convertible to it.
    [[nodiscard]] auto pop() -> long { return 0; }
};

struct visited_missing_try_insert {};

struct visited_wrong_return_type {
    // `try_insert` must return something contextually convertible to `bool`;
    // `void` is not.
    void try_insert(int) {}
};

}  // namespace

static_assert(!exploration::Frontier<frontier_missing_empty, int>);
static_assert(!exploration::Frontier<frontier_missing_push, int>);
static_assert(!exploration::Frontier<frontier_missing_pop, int>);
static_assert(!exploration::Frontier<frontier_wrong_pop_type, int>);
static_assert(!exploration::VisitedRegistry<visited_missing_try_insert, int>);
static_assert(!exploration::VisitedRegistry<visited_wrong_return_type, int>);

namespace {

// -----------------------------------------------------------------------------
// fifo_frontier: FIFO order.
// -----------------------------------------------------------------------------
auto test_fifo_order() -> bool {
    exploration::fifo_frontier<int> frontier;
    frontier.push(1);
    frontier.push(2);
    frontier.push(3);

    if (frontier.pop() != 1 || frontier.pop() != 2 || frontier.pop() != 3) {
        std::fputs("fifo_frontier did not pop in push order\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// fifo_frontier: empty() before push, after full drain, and mid-drain.
// -----------------------------------------------------------------------------
auto test_fifo_empty() -> bool {
    exploration::fifo_frontier<int> frontier;
    if (!frontier.empty()) {
        std::fputs("a freshly constructed fifo_frontier was not empty\n", stderr);
        return false;
    }

    frontier.push(10);
    frontier.push(20);
    if (frontier.empty()) {
        std::fputs("fifo_frontier reported empty with pending states\n", stderr);
        return false;
    }

    (void)frontier.pop();
    if (frontier.empty()) {
        std::fputs("fifo_frontier reported empty with one state still pending\n", stderr);
        return false;
    }

    (void)frontier.pop();
    if (!frontier.empty()) {
        std::fputs("fifo_frontier was not empty after draining every pushed state\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// fifo_frontier: exact reuse sequence after becoming empty.
//
//   push(a); pop(); empty() == true; push(b); empty() == false; pop() == b;
//   empty() == true
// -----------------------------------------------------------------------------
auto test_fifo_reuse_after_empty() -> bool {
    exploration::fifo_frontier<int> frontier;

    frontier.push(1);           // push(a)
    if (frontier.pop() != 1) {  // pop() == a
        std::fputs("fifo_frontier did not return the pushed value 'a'\n", stderr);
        return false;
    }
    if (!frontier.empty()) {  // empty() == true
        std::fputs("fifo_frontier was not empty immediately after draining 'a'\n", stderr);
        return false;
    }

    frontier.push(2);        // push(b)
    if (frontier.empty()) {  // empty() == false
        std::fputs("fifo_frontier reported empty right after pushing 'b'\n", stderr);
        return false;
    }
    if (frontier.pop() != 2) {  // pop() == b
        std::fputs("fifo_frontier did not return the pushed value 'b'\n", stderr);
        return false;
    }
    if (!frontier.empty()) {  // empty() == true
        std::fputs("fifo_frontier was not empty after draining 'b'\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// linear_visited_registry: first insertion of a value is accepted.
// -----------------------------------------------------------------------------
auto test_registry_accepts_new_state() -> bool {
    exploration::linear_visited_registry<int> visited;
    if (!visited.try_insert(1)) {
        std::fputs("linear_visited_registry rejected a genuinely new state\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// linear_visited_registry: a duplicate insertion is rejected.
// -----------------------------------------------------------------------------
auto test_registry_rejects_duplicate() -> bool {
    exploration::linear_visited_registry<int> visited;
    if (!visited.try_insert(1)) {
        std::fputs("linear_visited_registry rejected a genuinely new state\n", stderr);
        return false;
    }
    if (visited.try_insert(1)) {
        std::fputs("linear_visited_registry accepted a duplicate state\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// linear_visited_registry: distinct values remain independently insertable.
// -----------------------------------------------------------------------------
auto test_registry_distinguishes_values() -> bool {
    exploration::linear_visited_registry<int> visited;
    if (!visited.try_insert(1) || !visited.try_insert(2) || !visited.try_insert(3)) {
        std::fputs("linear_visited_registry rejected one of three distinct states\n", stderr);
        return false;
    }
    if (visited.try_insert(2)) {
        std::fputs("linear_visited_registry accepted a duplicate of an interior state\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// linear_visited_registry: states() reflects insertion order, and duplicate
// attempts do not change it.
// -----------------------------------------------------------------------------
auto test_registry_states_preserve_insertion_order() -> bool {
    exploration::linear_visited_registry<int> visited;
    (void)visited.try_insert(3);
    (void)visited.try_insert(1);
    (void)visited.try_insert(1);  // duplicate: must not appear twice or reorder anything
    (void)visited.try_insert(2);

    const std::array<int, 3> expected{3, 1, 2};
    if (!std::ranges::equal(visited.states(), expected)) {
        std::fputs("linear_visited_registry::states() did not preserve insertion order\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// linear_visited_registry: extract() moves the same sequence states() exposed.
// -----------------------------------------------------------------------------
auto test_registry_extract_matches_states() -> bool {
    exploration::linear_visited_registry<int> visited;
    (void)visited.try_insert(5);
    (void)visited.try_insert(7);

    const std::vector<int> extracted = std::move(visited).extract();
    const std::array<int, 2> expected{5, 7};
    if (!std::ranges::equal(extracted, expected)) {
        std::fputs("linear_visited_registry::extract() did not match the recorded sequence\n",
                   stderr);
        return false;
    }
    return true;
}

auto run_tests() -> int {
    if (!test_fifo_order() || !test_fifo_empty() || !test_fifo_reuse_after_empty()
        || !test_registry_accepts_new_state() || !test_registry_rejects_duplicate()
        || !test_registry_distinguishes_values() || !test_registry_states_preserve_insertion_order()
        || !test_registry_extract_matches_states()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

}  // namespace

int main() {
    try {
        return run_tests();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "Unexpected exception: %s\n", error.what());
    } catch (...) {
        std::fputs("Unexpected non-standard exception\n", stderr);
    }
    return EXIT_FAILURE;
}
