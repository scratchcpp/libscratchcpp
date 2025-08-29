// SPDX-License-Identifier: Apache-2.0

namespace libscratchcpp
{

class Thread;

void string_pool_add_thread(Thread *thread);
void string_pool_remove_thread(Thread *thread);
void string_pool_clear_thread(Thread *thread);
void string_pool_set_thread(Thread *thread);

} // namespace libscratchcpp
