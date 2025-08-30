// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/string_pool.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_functions.h>
#include <memory>
#include <unordered_set>
#include <map>
#include <cassert>
#include <algorithm>

namespace libscratchcpp
{

static std::unordered_set<std::unique_ptr<StringPtr>> strings;
static std::multimap<size_t, StringPtr *> freeStrings; // sorted by allocated space

extern "C"
{
    /*!
     * Use this instead of dynamically allocating StringPtr.
     * \note The returned string is uninitialized. Use e. g. string_assign_cstring() to initialize it.
     */
    StringPtr *string_pool_new()
    {
        if (freeStrings.empty()) {
            auto str = std::make_unique<StringPtr>();
            StringPtr *ptr = str.get();
            assert(strings.find(str) == strings.cend());
            strings.insert(std::move(str));

            return ptr;
        }

        // Optimization: pick string with the highest capacity to minimize allocs
        auto last = std::prev(freeStrings.end());
        StringPtr *ptr = last->second;
        freeStrings.erase(last);

        return ptr;
    }

    /*! Invalidates the given StringPtr so that it can be used for new strings later. */
    void string_pool_free(StringPtr *str)
    {
        assert(std::find_if(freeStrings.begin(), freeStrings.end(), [str](const std::pair<size_t, StringPtr *> &p) { return p.second == str; }) == freeStrings.end());
        assert(std::find_if(strings.begin(), strings.end(), [str](const std::unique_ptr<StringPtr> &p) { return p.get() == str; }) != strings.end());
        freeStrings.insert(std::pair<size_t, StringPtr *>(str->allocatedSize, str));
    }
}

} // namespace libscratchcpp
