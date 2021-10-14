#include "waitdielock.hpp"
#include <vector>
#include <thread>

void test1()
{
    // single thread - read only
    printf("Single thread - read only\n");
    WaitDieLock wd;
    for (int ts = 0; ts <= 10; ++ts)
    {
        assert(wd.try_lock_shared(ts));
    }
    wd.trace();
    for (int ts = 10; ts >= 0; --ts)
    {
        wd.unlock_shared(ts);
    }
    wd.trace();
}

void test2()
{
    // single thread - write only
    printf("Single thread - write only\n");
    WaitDieLock wd;
    for (int ts = 0; ts <= 10; ++ts)
    {
        if (ts == 0)
            assert(wd.try_lock(ts));
        if (ts != 0)
            assert(!wd.try_lock(ts));
    }
    wd.trace();
}

void test3()
{
    // single thread - read only reverse
    printf("Single thread - read only reverse\n");
    WaitDieLock wd;
    for (int ts = 10; ts >= 0; --ts)
    {
        assert(wd.try_lock_shared(ts));
    }
    wd.trace();
}

void test4()
{
    // single thread - read upgrade
    printf("Single thread - read upgrade\n");
    WaitDieLock wd;
    for (int ts = 10; ts >= 0; --ts)
    {
        assert(wd.try_lock_shared(ts));
        assert(wd.try_lock_upgrade(ts));
        // wd.trace();
        wd.unlock(ts);
    }
    wd.trace();
}

void run_read(int start_ts, int end_ts, int step_ts, WaitDieLock &wd)
{
    for (int ts = start_ts; ts >= end_ts; ts -= step_ts)
    {
        wd.try_lock_shared(ts);
    }
    // wd.trace();
}

void run_unlock_shared(int start_ts, int end_ts, int step_ts, WaitDieLock &wd)
{
    for (int ts = start_ts; ts >= end_ts; ts -= step_ts)
    {
        wd.unlock_shared(ts);
    }
    // wd.trace();
}

void run_upgrade(int start_ts, int end_ts, int step_ts, WaitDieLock &wd)
{
    for (int ts = start_ts; ts >= end_ts; ts -= step_ts)
    {
        wd.try_lock_upgrade(ts);
    }
    // wd.trace();
}

void run_read_upgrade(int start_ts, int end_ts, int step_ts, WaitDieLock &wd)
{
    for (int ts = start_ts; ts >= end_ts; ts -= step_ts)
    {
        wd.try_lock_shared(ts);
    }
    for (int ts = start_ts; ts >= end_ts; ts -= step_ts)
    {
        wd.try_lock_upgrade(ts);
    }
}

void run_write(int start_ts, int end_ts, int step_ts, WaitDieLock &wd)
{
    for (int ts = start_ts; ts >= end_ts; ts -= step_ts)
    {
        wd.try_lock(ts);
    }
    // wd.trace();
}

void run_unlock(int start_ts, int end_ts, int step_ts, WaitDieLock &wd)
{
    for (int ts = start_ts; ts >= end_ts; ts -= step_ts)
    {
        wd.unlock(ts);
    }
    // wd.trace();
}

void test5()
{
    // two thread - both read
    printf("Double thread - both read\n");

    std::vector<std::thread> threads;
    int num_threads = 2;
    threads.reserve(num_threads);

    WaitDieLock wd;
    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(run_read, 10000 + i, 0, 2, std::ref(wd));
    }
    for (int i = 0; i < num_threads; ++i)
    {
        threads[i].join();
    }
    wd.trace();
}

void test6()
{
    // two threads - read after write
    printf("Double thread - read after write\n");

    std::vector<std::thread> threads;
    int num_threads = 3;
    threads.reserve(num_threads);

    WaitDieLock wd;
    threads.emplace_back(run_write, 10, 10, 1, std::ref(wd));
    threads[0].join();
    wd.trace();
    threads.emplace_back(run_read, 9, 9, 1, std::ref(wd));
    threads.emplace_back(run_unlock, 10, 10, 1, std::ref(wd));
    wd.trace();
    threads[1].join();
    threads[2].join();
    wd.trace();
}

void test7()
{
    // two threads - read upgrade after write
    printf("Double thread - read upgrade after write\n");

    std::vector<std::thread> threads;
    int num_threads = 3;
    threads.reserve(num_threads);

    WaitDieLock wd;
    threads.emplace_back(run_write, 10, 10, 1, std::ref(wd));
    threads[0].join();
    wd.trace();
    threads.emplace_back(run_read_upgrade, 9, 9, 1, std::ref(wd));
    threads.emplace_back(run_unlock, 10, 10, 1, std::ref(wd));
    wd.trace();
    threads[1].join();
    threads[2].join();
    wd.trace();
}

void test8()
{
    // two threads - write after write
    printf("Double thread - write after write\n");

    std::vector<std::thread> threads;
    int num_threads = 3;
    threads.reserve(num_threads);

    WaitDieLock wd;
    threads.emplace_back(run_write, 10, 10, 1, std::ref(wd));
    threads[0].join();
    wd.trace();
    threads.emplace_back(run_write, 9, 9, 1, std::ref(wd));
    threads.emplace_back(run_unlock, 10, 10, 1, std::ref(wd));
    wd.trace();
    threads[1].join();
    threads[2].join();
    wd.trace();
}

void test9()
{
    // two threads - write after reads
    printf("Double thread - write after reads\n");

    std::vector<std::thread> threads;
    int num_threads = 3;
    threads.reserve(num_threads);

    WaitDieLock wd;
    threads.emplace_back(run_read, 10, 9, 1, std::ref(wd));
    threads[0].join();
    wd.trace();
    threads.emplace_back(run_write, 8, 8, 1, std::ref(wd));
    threads.emplace_back(run_unlock_shared, 10, 9, 1, std::ref(wd));
    wd.trace();
    threads[1].join();
    threads[2].join();
    wd.trace();
}

void test10()
{
    // multiple threads - write after several writes and reads
    printf("Multiple threads - write after several writes and reads\n");

    std::vector<std::thread> threads;
    int num_threads = 10;
    threads.reserve(num_threads);

    WaitDieLock wd;
    threads.emplace_back(run_write, 10, 10, 1, std::ref(wd));
    threads[0].join();
    wd.trace();
    threads.emplace_back(run_read, 9, 9, 1, std::ref(wd));
    threads.emplace_back(run_write, 8, 8, 1, std::ref(wd));
    threads.emplace_back(run_read, 7, 7, 1, std::ref(wd));
    threads.emplace_back(run_read, 6, 6, 1, std::ref(wd));
    wd.trace();
    threads.emplace_back(run_unlock, 10, 10, 1, std::ref(wd));
    threads[1].join();
    threads[5].join();
    wd.trace();
    threads.emplace_back(run_unlock_shared, 9, 9, 1, std::ref(wd));
    threads[2].join();
    threads[6].join();
    wd.trace();
    threads.emplace_back(run_unlock, 8, 8, 1, std::ref(wd));
    threads[3].join();
    threads[4].join();
    threads[7].join();
    wd.trace();
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
}