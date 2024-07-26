#include <thread>
#include <mutex>
#include <condition_variable>

class Barrier
{
private:
private:
    int num_threads, wait_count, instance;
    std::mutex mut;
    std::condition_variable cv;

public:
    Barrier(int num)
        : num_threads(num),
          wait_count(0),
          instance(0) {};

    void wait()
    {
        std::unique_lock<std::mutex> lock(mut);
        int inst = instance;

        if (++wait_count == num_threads)
        {
            wait_count = 0;
            instance++;
            cv.notify_all();
        }
        else
            cv.wait(lock, [this, &inst]()
                    { return instance != inst; });
    }
};