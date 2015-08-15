/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_THREAD_WAITER_H__
#define __VSID_THREAD_WAITER_H__

#include <mutex>
#include <condition_variable>

namespace Vsid
{
    class ProtocolModelDb;
}

namespace VsidCommon
{

/**
 * Class that can be used to block a thread from spinning indefinitely.
 * When there is notthing to do, it will wait to be notified
 */
class ThreadWaiter
{
public:
    ThreadWaiter() {}
    ThreadWaiter(ThreadWaiter const&) {}
    ThreadWaiter& operator=(ThreadWaiter const&) { return *this; }

    void wait()
    {
        std::unique_lock<std::mutex> lk(mx);
        cv.wait_for(lk, std::chrono::milliseconds(10));
    }

    void notify()
    {
        cv.notify_one();
    }

    std::mutex mx;
    std::condition_variable cv;
};


} // end namespace

#endif // END HEADER GUARD