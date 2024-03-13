#include <hyperion/assert/backtrace.h>
#include <hyperion/assert/panic.h>
#include <hyperion/source_location.h>

#include <fmt/format.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <string_view>

namespace hyperion::assert::panic {

    namespace detail {

        namespace {
            HYPERION_ATTRIBUTE_COLD [[noreturn]] auto
            default_handler(const std::string_view panic_message,
                            const hyperion::source_location& location,
                            const Backtrace& backtrace) noexcept -> void {
                if(panic_message.empty()) {
                    fmt::print(stderr,
                               "panic occurred at {0}:\n\n"
                               "Backtrace:\n{1}\n",
                               location,
                               backtrace);
                }
                else {
                    fmt::print(stderr,
                               "panic occurred at {0}:\n\n"
                               "{1}\n\n"
                               "Backtrace:\n{2}\n",
                               location,
                               panic_message,
                               backtrace);
                }
                HYPERION_ASSERT_DEBUG_BREAK();
                std::abort();
            }

            std::atomic<panic::Handler> s_handler // NOLINT(*-avoid-non-const-global-variables)
                = &default_handler;
        } // namespace

    } // namespace detail

    HYPERION_ATTRIBUTE_COLD auto set_handler(Handler handler) noexcept -> void {
        detail::s_handler.store(handler, std::memory_order_release);
    }

    HYPERION_ATTRIBUTE_COLD [[nodiscard]] auto get_handler() noexcept -> Handler {
        return detail::s_handler.load(std::memory_order_acquire);
    }

    HYPERION_ATTRIBUTE_COLD [[nodiscard]] auto default_handler() noexcept -> Handler {
        return &detail::default_handler;
    }

    HYPERION_ATTRIBUTE_COLD auto execute(const hyperion::source_location& location,
                                         const Backtrace& backtrace) noexcept -> void {
        panic::get_handler()("", location, backtrace);
    }

    HYPERION_ATTRIBUTE_COLD auto execute(const hyperion::source_location& location,
                                         const Backtrace& backtrace,
                                         std::string_view message) noexcept -> void {
        panic::get_handler()(message, location, backtrace);
    }
} // namespace hyperion::assert::panic
