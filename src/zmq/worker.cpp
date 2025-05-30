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
#include <bitcoin/protocol/zmq/worker.hpp>

#include <future>
#include <mutex>
#include <thread>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/boost.hpp>
#include <bitcoin/protocol/zmq/message.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>
#include <bitcoin/protocol/zmq/zeromq.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

using namespace bc::system;

// Derive from this abstract worker to implement concrete worker.
worker::worker(thread_priority priority) NOEXCEPT
  : priority_(priority),
    stopped_(true)
{
}

worker::~worker() NOEXCEPT
{
    stop();
}

// Restartable after stop and not started on construct.
bool worker::start() NOEXCEPT
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    std::unique_lock lock(mutex_);

    if (stopped_)
    {
        stopped_ = false;

        // Create the worker thread and socket and start polling.
        thread_ = std::make_shared<std::thread>(&worker::work, this);

        // Wait on worker start.
        const auto result = started_.get_future().get();

        // Reset for restartability.
        started_ = std::promise<bool>();
        return result;
    }

    return false;
    ///////////////////////////////////////////////////////////////////////////
}

// Promise is used (vs. join only) to capture stop result code.
// BUGBUG: stop is insufficient to stop a worker that uses relay().
bool worker::stop() NOEXCEPT
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    std::unique_lock lock(mutex_);

    if (!stopped_)
    {
        stopped_ = true;

        // Wait on worker stop.
        const auto result = finished_.get_future().get();

        // Wait for thread to stop.
        thread_->join();

        // Reset for restartability.
        finished_ = std::promise<bool>();
        return result;
    }

    return true;
    ///////////////////////////////////////////////////////////////////////////
}

// Utilities.
//-----------------------------------------------------------------------------

// Call from work to detect an explicit stop.
bool worker::stopped() NOEXCEPT
{
    return stopped_;
}

// Call from work when started (connected/bound) or failed to do so.
bool worker::started(bool result) NOEXCEPT
{
    started_.set_value(result);

    if (result)
        set_priority(priority_);
    else
        finished(true);

    return result;
}

// Call from work when finished working, do not call if started was called.
bool worker::finished(bool result) NOEXCEPT
{
    finished_.set_value(result);
    return result;
}

// TODO: use non-copying private zmq implementation of forward.
// Call from work to forward a message from one socket to another.
bool worker::forward(socket& from, socket& to) NOEXCEPT
{
    message packet;
    return !from.receive(packet) && !to.send(packet);
}

// Call from work to establish a proxy between two sockets.
void worker::relay(socket& left, socket& right) NOEXCEPT
{
    // Blocks until the context is terminated, always returns -1.
    zmq_proxy_steerable(left.self(), right.self(), nullptr, nullptr);

    // Equivalent implementation:
    ////zmq::poller poller;
    ////poller.add(left);
    ////poller.add(right);
    ////
    ////while (!poller.terminated())
    ////{
    ////    const auto signaled = poller.wait();
    ////
    ////    if (signaled.contains(left.id()))
    ////        forward(left, right);
    ////
    ////    if (signaled.contains(right.id()))
    ////        forward(right, left);
    ////}
}

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin
