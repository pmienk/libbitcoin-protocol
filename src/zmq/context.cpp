/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/protocol/zmq/context.hpp>

#include <mutex>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/zmq/zeromq.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

using namespace bc::system;

context::context(bool started) NOEXCEPT
  : self_(nullptr)
{
    if (started)
        start();
}

context::~context() NOEXCEPT
{
    stop();
}

// Restartable after stop and optionally started on construct.
bool context::start() NOEXCEPT
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std::unique_lock lock(mutex_);
    BC_POP_WARNING()

    if (self_ != nullptr)
        return false;

    self_.store(zmq_ctx_new());
    return self_ != nullptr;
    ///////////////////////////////////////////////////////////////////////////
}

// Signal termination and block until all sockets closed.
bool context::stop() NOEXCEPT
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std::unique_lock lock(mutex_);
    BC_POP_WARNING()

    if (self_ == nullptr)
        return true;

    // This aborts blocking operations but blocks here until all sockets in the
    // context have been closed with zmq_close. Can fail by signal interrupt.
    const auto result = zmq_ctx_term(self_) != zmq_fail;

    self_.store(nullptr);
    return result;
    ///////////////////////////////////////////////////////////////////////////
}

context::operator bool() const NOEXCEPT
{
    return self_ != nullptr;
}

// This may become invalid after return. This call only ensures atomicity.
void* context::self() NOEXCEPT
{
    return self_;
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin
