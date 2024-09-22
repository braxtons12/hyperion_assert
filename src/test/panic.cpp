#include <hyperion/assert/panic.h>

#include <boost/ut.hpp>

#include <string>

namespace hyperion::_test::assert::panic {

    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace boost::ut;

    static void panic_no_message() {
        HYPERION_PANIC();
    }

    static void panic_with_message() {
        HYPERION_PANIC("With a panic Message!");
    }

    static void panic_with_formatted_message() {
        HYPERION_PANIC("With {} panic Messages!", 42);
    }

    // NOLINTNEXTLINE(cert-err58-cpp, *-avoid-non-const-global-variables)
    static std::string test_str;

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
    static const suite<"hyperion::assert::panic"> panic_tests = [] {
        "no_message_contents"_test = [] {
            hyperion::assert::panic::set_handler(test_handler);
            panic_no_message();
            expect(test_str.find("Panic occurred at") != std::string::npos);
            expect(test_str.find("panic_no_message") != std::string::npos);
        };

        "with_message_contents"_test = [] {
            hyperion::assert::panic::set_handler(test_handler);
            panic_with_message();
            expect(test_str.find("With a panic Message!") != std::string::npos);
            expect(test_str.find("panic_with_message") != std::string::npos);
        };

        "with_formatted_message_contents"_test = [] {
            hyperion::assert::panic::set_handler(test_handler);
            panic_with_formatted_message();
            expect(test_str.find("With 42 panic Messages!") != std::string::npos);
            expect(test_str.find("panic_with_formatted_message") != std::string::npos);
        };

            // ut only supports aborts tests on UNIX-likes for the moment
    #if not HYPERION_PLATFORM_IS_WINDOWS
        "no_message_failure"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] { panic_no_message(); }));
        };

        "with_message_failure"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] { panic_with_message(); }));
        };

        "with_formatted_message_failure"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] { panic_with_formatted_message(); }));
        };
    #endif // not HYPERION_PLATFORM_IS_WINDOWS
    };

} // namespace hyperion::_test::assert::panic
