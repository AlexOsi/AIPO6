#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>


class ThreadWrapper {
public:
    ThreadWrapper() = default;

    explicit ThreadWrapper(std::thread t)
            : m_t(std::move(t)) {}

    void join() {
        if (m_t.joinable())
            m_t.join();
    }

    ~ThreadWrapper() {
        join();
    }

private:
    std::thread m_t;
};

int partition(std::vector<int> &a, int l, int r) {
    int pivot = a[r];
    int j = l;
    for (int i = l; i < r; ++i)
        if (a[i] <= pivot) {
            std::swap(a[i], a[j]);
            j++;
        }
    std::swap(a[j], a[r]);
    return j;
}

void QuickSort(std::vector<int> &a, int l, int r, int &curCnt, std::atomic_int &countOfThreads) {
    if (l < r) {
        int q = partition(a, l, r);
        ThreadWrapper t;
        if (countOfThreads <= curCnt) {
            ThreadWrapper thread(std::thread(QuickSort,
                                             std::ref(a),
                                             l,
                                             q - 1,
                                             std::ref(curCnt),
                                             std::ref(countOfThreads)));
            countOfThreads++;
        } else {
            QuickSort(a, l, q - 1, curCnt, countOfThreads);
        }
        QuickSort(a, q, r, curCnt, countOfThreads);
        t.join();
        if (countOfThreads > 0)
            countOfThreads--;
    }
}


int main() {
    std::atomic_int countOfThreads = 0;
    auto start = std::chrono::steady_clock::now();
    int amountOfNums;
    std::cin >> amountOfNums;
    int threadsNum = (std::thread::hardware_concurrency() > 1)
            ? static_cast<int>(std::thread::hardware_concurrency()) : 2;

    std::vector<int> nums(amountOfNums);

    std::mt19937 engine(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> uniformIntDistribution(0, 9);

    for (int& num : nums) {
        num = uniformIntDistribution(engine);
    }

    for (int num : nums)
        std::cout << num << " ";
    std::cout << std::endl;

    QuickSort(nums, 0, amountOfNums - 1, threadsNum, countOfThreads);

    for (int i : nums)
        std::cout << i << " ";
    std::cout << std::endl;

    auto end = std::chrono::steady_clock::now();

    std::cout << "Time passed: "
    << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;

}
