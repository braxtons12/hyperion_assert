# hyperion::assert

[![XMake](https://github.com/braxtons12/hyperion_assert/actions/workflows/xmake.yml/badge.svg?event=push)](https://github.com/braxtons12/hyperion_assert/actions/workflows/xmake.yml)
[![CMake](https://github.com/braxtons12/hyperion_assert/actions/workflows/cmake.yml/badge.svg?event=push)](https://github.com/braxtons12/hyperion_assert/actions/workflows/cmake.yml)

hyperion::assert is a C++20 library for all manner of runtime asserts. It includes asserts
for preconditions, postconditions, hard errors, debug asserts, and optimization suggestions,
all with printed printed stacktrace support and user-configurable handling.

### Quick Start

See the [Quick Start Guide](https://braxtons12.github.io/hyperion_assert/quick_start.html)
for how to get started using hyperion::assert.

### Documentation

You can also find the rest of the project documentation [here](https://braxtons12.github.io/hyperion_assert)

### Example

After following the Quick Start Guide, you'll be able to use hyperion::assert in your project.
A basic example of what you can do with hyperion::assert is below:

```cpp
#include <hyperion/assert.h>
```

### Contributing

Feel free to submit issues, pull requests, etc.!<br>
When contributing code, please follow the general style expectations of Hyperion projects:
- Follow the project `.clang-format` (except in judicious cases of templates or requires clauses
        ruining formatting),
- Use trailing returns types,
- Use assign-init (e.g. `auto var = value;`) instead of direct-init (e.g. `auto var{value}`),
- By default, prefer simplicity and correctness over performance
- We try to target Clang/LLVM 15, which doesn't yet support usage of concepts in function
definitions occurring after the declaration (for example, defining a function template outside of
the class it was declared in). In these cases, revert to using `enable_if` instead.

### License

hyperion::assert uses the MIT license.

