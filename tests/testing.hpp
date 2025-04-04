#ifndef TESTING_HPP
#define TESTING_HPP

#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace testing {

/** Struct representing a test case. */
struct Test {
    std::string desc;
    std::function<void()> func;
};

/** Returns reference to the test registry. */
inline std::vector<Test>& tests() {
    static std::vector<Test> t;
    return t;
}

/** Helper class for registering test cases. */
class Register {
  public:
    Register(const std::string& d, std::function<void()> f) {
        tests().push_back({d, f});
    }
};

/**
 * Runs all registered test cases.
 *
 * @return int 0 if all tests pass, non-zero otherwise.
 */
inline int run_all() {
    int fails = 0;
    for (auto& t : tests()) {
        try {
            t.func();
            std::cout << "[ PASS ] " << t.desc << "\n";
        } catch (const std::exception& e) {
            std::cout << "[ FAIL ] " << t.desc << ": " << e.what() << "\n";
            ++fails;
        } catch (...) {
            std::cout << "[ FAIL ] " << t.desc << ": unknown exception\n";
            ++fails;
        }
    }
    std::cout << "Total tests: " << tests().size() << ", Failures: " << fails
              << "\n";
    return fails;
}

} // namespace testing

// Helper macros for unique identifier generation.
#define CAT_IMPL(a, b) a##b
#define CAT(a, b) CAT_IMPL(a, b)

/** Internal macro to implement TEST_CASE with a unique id. */
#define TEST_CASE_IMPL(id, desc, ...)                                          \
    static void CAT(test_func_, id)();                                         \
    static testing::Register CAT(reg_, id)(desc, CAT(test_func_, id));         \
    static void CAT(test_func_, id)()

/**
 * TEST_CASE macro.
 *
 * Usage:
 *   TEST_CASE("My test case", "[tag]") { ... }
 */
#define TEST_CASE(desc, ...) TEST_CASE_IMPL(__COUNTER__, desc, __VA_ARGS__)

/**
 * REQUIRE macro.
 *
 * Usage:
 *   REQUIRE(expr);
 */
#define REQUIRE(expr)                                                          \
    do {                                                                       \
        if (!(expr)) {                                                         \
            throw std::runtime_error("Requirement failed: " #expr);            \
        }                                                                      \
    } while (0)

/**
 * SECTION macro.
 *
 * Usage:
 *   SECTION("section name") { ... }
 */
#define SECTION(name) for (bool _sec = true; _sec; _sec = false)

#ifdef TESTING_MAIN
#include <iostream>
int main() { return testing::run_all(); }
#endif

#endif
