// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/string_pool.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_functions.h>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <cassert>
#include <algorithm>

namespace libscratchcpp
{

class Thread;

static std::unordered_set<std::unique_ptr<StringPtr>> strings;
static std::multimap<size_t, StringPtr *> freeStrings; // sorted by allocated space

static std::unordered_map<Thread *, std::unordered_set<StringPtr *>> threadStrings;
static Thread *currentThread = nullptr;

extern "C"
{
    /*!
     * Use this instead of dynamically allocating StringPtr.
     * \param[in] internal Whether the string should be deallocated when the current thread is removed (it's only used within the thread).
     * \note The returned string is uninitialized. Use e. g. string_assign_cstring() to initialize it.
     */
    StringPtr *string_pool_new(bool internal)
    {
        if (freeStrings.empty()) {
            auto str = std::make_unique<StringPtr>();
            StringPtr *ptr = str.get();
            assert(strings.find(str) == strings.cend());
            strings.insert(std::move(str));

            if (internal && currentThread)
                threadStrings[currentThread].insert(ptr);

            return ptr;
        }

        // Optimization: pick string with the highest capacity to minimize allocs
        auto last = std::prev(freeStrings.end());
        StringPtr *ptr = last->second;
        freeStrings.erase(last);

        if (internal && currentThread)
            threadStrings[currentThread].insert(ptr);

        return ptr;
    }

    /*! Invalidates the given StringPtr so that it can be used for new strings later. */
    void string_pool_free(StringPtr *str)
    {
        if (currentThread)
            threadStrings[currentThread].erase(str);

        assert(std::find_if(freeStrings.begin(), freeStrings.end(), [str](const std::pair<size_t, StringPtr *> &p) { return p.second == str; }) == freeStrings.end());
        assert(std::find_if(strings.begin(), strings.end(), [str](const std::unique_ptr<StringPtr> &p) { return p.get() == str; }) != strings.end());
        freeStrings.insert(std::pair<size_t, StringPtr *>(str->allocatedSize, str));
    }
}

/* string_pool_p.h */

void string_pool_add_thread(Thread *thread)
{
    // Start capturing allocated strings in the thread
    assert(threadStrings.find(thread) == threadStrings.cend());
    threadStrings[thread] = {};
}

void string_pool_remove_thread(Thread *thread)
{
    // Free all strings in the thread (garbage collection)
    assert(threadStrings.find(thread) != threadStrings.cend());
    auto &strings = threadStrings[thread];

    for (StringPtr *str : strings)
        freeStrings.insert(std::pair<size_t, StringPtr *>(str->allocatedSize, str));

    threadStrings.erase(thread);

    if (currentThread == thread)
        currentThread = nullptr;
}

void string_pool_set_thread(Thread *thread)
{
    currentThread = thread;
}

} // namespace libscratchcpp
