/*
 * Compiler test if workaround for conflicting Cortex-M4
 * macros is needed (issue #1).
 */


/*
 * Macros defined by Cortex-M4 files, which may conflict with
 * identifiers used internally by compilers.
 */
#define __I
#define __O


#include <array>
#include <algorithm>

int main()
{
    std::array<int, 3> a{ 1, 2, 3 };
    std::for_each(a.begin(), a.end(), [](auto x) { (void) x; });

    return 0;
}
