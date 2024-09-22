/// @file test/assert.cpp
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Unit tests for hyperion::assert.
/// @version 0.1
/// @date 2024-09-21
///
/// MIT License
/// @copyright Copyright (c) 2024 Braxton Salyer <braxtonsalyer@gmail.com>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

#include <hyperion/assert.h>

#include <boost/ut.hpp>

#include <string>

namespace hyperion::_test::assert::assert {

    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace boost::ut;

    // NOLINTNEXTLINE(cert-err58-cpp, *-avoid-non-const-global-variables)
    static std::string test_str;

    // used in the last "complex expression" test
    namespace invoke {
        struct invoker {
            constexpr auto invoke(auto&& func) {
                return std::forward<decltype(func)>(func)();
            }
        };
    } // namespace invoke

    static auto test_handler(const std::string_view panic_message,
                             const hyperion::source_location& location,
                             const hyperion::assert::Backtrace& backtrace) noexcept -> void {
        if(panic_message.empty()) {
            test_str = fmt::format("Panic occurred at {0}:\n\n"
                                   "Backtrace:\n{1}\n",
                                   location,
                                   backtrace);
        }
        else {
            test_str = fmt::format("Panic occurred at {0}:\n\n"
                                   "{1}\n\n"
                                   "Backtrace:\n{2}\n",
                                   location,
                                   panic_message,
                                   backtrace);
        }
    }

    // NOLINTNEXTLINE(cert-err58-cpp)
    static inline const suite<"hyperion::assert::assert"> assert_tests = [] {
        "require_tests"_test = [] {
            "no_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                HYPERION_ASSERT_REQUIRE(value + lambda() == 7);
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Requirement") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
            };

            "with_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                HYPERION_ASSERT_REQUIRE(value + lambda() == 7, "with context message");
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Requirement") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with context message") != std::string::npos);
            };

            "with_formatted_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                HYPERION_ASSERT_REQUIRE(value + lambda() == 7, "with {} context messages", 42);
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Requirement") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with 42 context messages") != std::string::npos);
            };

            // ut only supports aborts tests on UNIX-likes for the moment
#if not HYPERION_PLATFORM_IS_WINDOWS
            "no_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };
                    HYPERION_ASSERT_REQUIRE(value + lambda() == 7);
                }));
            };

            "with_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };
                    HYPERION_ASSERT_REQUIRE(value + lambda() == 7, "with context message");
                }));
            };

            "with_formatted_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };
                    HYPERION_ASSERT_REQUIRE(value + lambda() == 7, "with {} context messages", 42);
                }));
            };
#endif // not HYPERION_PLATFORM_IS_WINDOWS
        };

        "fatal_tests"_test = [] {
            "no_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                HYPERION_ASSERT_FATAL(value + lambda() == 7);
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Fatal") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
            };

            "with_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                HYPERION_ASSERT_FATAL(value + lambda() == 7, "with context message");
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Fatal") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with context message") != std::string::npos);
            };

            "with_formatted_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                HYPERION_ASSERT_FATAL(value + lambda() == 7, "with {} context messages", 42);
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Fatal") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with 42 context messages") != std::string::npos);
            };

            // ut only supports aborts tests on UNIX-likes for the moment
#if not HYPERION_PLATFORM_IS_WINDOWS
            "no_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };
                    HYPERION_ASSERT_FATAL(value + lambda() == 7);
                }));
            };

            "with_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };
                    HYPERION_ASSERT_FATAL(value + lambda() == 7, "with context message");
                }));
            };

            "with_formatted_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };
                    HYPERION_ASSERT_FATAL(value + lambda() == 7, "with {} context messages", 42);
                }));
            };
#endif // not HYPERION_PLATFORM_IS_WINDOWS
        };

#if HYPERION_PLATFORM_MODE_IS_DEBUG

        "debug_tests"_test = [] {
            "no_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                HYPERION_ASSERT_DEBUG(value + lambda() == 7);
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Debug") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
            };

            "with_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                HYPERION_ASSERT_DEBUG(value + lambda() == 7, "with context message");
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Debug") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with context message") != std::string::npos);
            };

            "with_formatted_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                HYPERION_ASSERT_DEBUG(value + lambda() == 7, "with {} context messages", 42);
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Debug") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with 42 context messages") != std::string::npos);
            };

            // ut only supports aborts tests on UNIX-likes for the moment
    #if not HYPERION_PLATFORM_IS_WINDOWS
            "no_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };
                    HYPERION_ASSERT_DEBUG(value + lambda() == 7);
                }));
            };

            "with_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };
                    HYPERION_ASSERT_DEBUG(value + lambda() == 7, "with context message");
                }));
            };

            "with_formatted_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };
                    HYPERION_ASSERT_DEBUG(value + lambda() == 7, "with {} context messages", 42);
                }));
            };
    #endif // not HYPERION_PLATFORM_IS_WINDOWS
        };

#endif // HYPERION_PLATFORM_MODE_IS_DEBUG

#if HYPERION_PLATFORM_MODE_IS_DEBUG or not HYPERION_ASSERT_CONTRACT_ASSERTIONS_DEBUG_ONLY

        "precondition_and_postcondition_tests"_test = [] {
            "no_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                [&]() {
                    HYPERION_ASSERT_PRECONDITION(value == 3);

                    value = 4;
                }();
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Pre-condition") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                [&]() {
                    HYPERION_ASSERT_POSTCONDITION(value == 7);

                    value = 2 + lambda();
                }();
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Post-condition") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
            };

            "with_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                [&]() {
                    HYPERION_ASSERT_PRECONDITION(value == 3, "with context message");

                    value = 4;
                }();
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Pre-condition") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with context message") != std::string::npos);
                [&]() {
                    HYPERION_ASSERT_POSTCONDITION(value == 7, "with context message");

                    value = 2 + lambda();
                }();
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Post-condition") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with context message") != std::string::npos);
            };

            "with_formatted_message_contents"_test = [] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };

                hyperion::assert::panic::set_handler(test_handler);
                [&]() {
                    HYPERION_ASSERT_PRECONDITION(value == 3, "with {} context messages", 42);

                    value = 4;
                }();
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Pre-condition") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with 42 context messages") != std::string::npos);
                [&]() {
                    HYPERION_ASSERT_POSTCONDITION(value == 7, "with {} context messages", 42);

                    value = 2 + lambda();
                }();
                expect(test_str.find("Panic occurred at") != std::string::npos);
                expect(test_str.find("Post-condition") != std::string::npos);
                expect(test_str.find("Where:") != std::string::npos);
                expect(test_str.find("Evaluated To:") != std::string::npos);
                expect(test_str.find("with 42 context messages") != std::string::npos);
            };

            // ut only supports aborts tests on UNIX-likes for the moment
    #if not HYPERION_PLATFORM_IS_WINDOWS
            "no_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;

                    [&]() {
                        HYPERION_ASSERT_PRECONDITION(value == 3);

                        value = 4;
                    }();
                }));
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };

                    [&]() {
                        HYPERION_ASSERT_POSTCONDITION(value == 7);

                        value = 2 + lambda();
                    }();
                }));
            };

            "with_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;

                    [&]() {
                        HYPERION_ASSERT_PRECONDITION(value == 3, "with context message");

                        value = 4;
                    }();
                }));
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };

                    [&]() {
                        HYPERION_ASSERT_POSTCONDITION(value == 7, "with context message");

                        value = 2 + lambda();
                    }();
                }));
            };

            "with_formatted_message_failure"_test = [] {
                hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
                expect(aborts([] {
                    auto value = 2;

                    [&]() {
                        HYPERION_ASSERT_PRECONDITION(value == 3, "with {} context messages", 42);

                        value = 4;
                    }();
                }));
                expect(aborts([] {
                    auto value = 2;
                    auto lambda = []() {
                        return 4;
                    };

                    [&]() {
                        HYPERION_ASSERT_POSTCONDITION(value == 7, "with {} context messages", 42);

                        value = 2 + lambda();
                    }();
                }));
            };
    #endif // not HYPERION_PLATFORM_IS_WINDOWS
        };

#endif // HYPERION_PLATFORM_MODE_IS_DEBUG or not HYPERION_ASSERT_CONTRACT_ASSERTIONS_DEBUG_ONLY

        // ut only supports aborts tests on UNIX-likes for the moment
#if not HYPERION_PLATFORM_IS_WINDOWS
        "complex_expression"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] {
                auto value = 2;
                HYPERION_ASSERT_REQUIRE(value + invoke::invoker{}.invoke([]() { return 4; }) == 7);
            }));
        };
#endif // not HYPERION_PLATFORM_IS_WINDOWS
    };

} // namespace hyperion::_test::assert::assert
