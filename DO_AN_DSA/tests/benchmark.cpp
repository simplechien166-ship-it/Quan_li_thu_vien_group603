#include "../include/CustomHashTable.h"
#include "../include/CustomMinHeap.h"
#include "../include/ReservationQueue.h"
#include "../include/models.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

struct BenchmarkResult {
    double linearSearchMs;
    double hashSearchMs;
    double rangeScanMs;
    double top10Ms;
    double queueMs;
    unsigned long long operationCount;
    double estimatedMemoryMb;
};

template <typename Function>
double measureMilliseconds(Function function) {
    auto start = Clock::now();
    function();
    auto end = Clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

BenchmarkResult runBenchmark(int recordCount) {
    std::vector<Book> records;
    records.reserve(recordCount);
    CustomHashTable<Book> table;

    for (int i = 0; i < recordCount; ++i) {
        std::string id = "B" + std::to_string(i);
        records.emplace_back(id, "Book Title " + std::to_string(i), "Author " + std::to_string(i),
                             2000 + (i % 25), 10, 5, i % 500);
        table.insert(id, records.back());
    }

    unsigned long long operationCount = 0;
    volatile int resultGuard = 0;

    double linearSearchMs = measureMilliseconds([&]() {
        for (int i = 0; i < recordCount; ++i) {
            std::string target = "B" + std::to_string(i);
            for (const auto& book : records) {
                ++operationCount;
                if (book.ma_sach == target) {
                    resultGuard += book.luot_muon;
                    break;
                }
            }
        }
    });

    double hashSearchMs = measureMilliseconds([&]() {
        for (int i = 0; i < recordCount; ++i) {
            Book* book = table.search("B" + std::to_string(i));
            ++operationCount;
            if (book != nullptr) resultGuard += book->luot_muon;
        }
    });

    double rangeScanMs = measureMilliseconds([&]() {
        int matches = 0;
        for (const auto& book : records) {
            ++operationCount;
            if (book.nam_xuat_ban >= 2010 && book.nam_xuat_ban <= 2015) ++matches;
        }
        resultGuard += matches;
    });

    double top10Ms = measureMilliseconds([&]() {
        CustomMinHeap heap;
        for (const auto& book : records) {
            heap.insertOrUpdate(book);
            ++operationCount;
        }
        resultGuard += static_cast<int>(heap.getSortedTop10().size());
    });

    double queueMs = measureMilliseconds([&]() {
        ReservationQueue queue;
        const int queueSize = recordCount < 100 ? recordCount : 100;
        for (int i = 0; i < queueSize; ++i) {
            if (queue.push("R" + std::to_string(i))) ++operationCount;
        }
        std::string readerId;
        while (queue.pop(readerId)) ++operationCount;
        resultGuard += queue.getSize();
    });

    (void)resultGuard;
    double estimatedBytes = static_cast<double>(recordCount) * sizeof(Book);
    return {linearSearchMs, hashSearchMs, rangeScanMs, top10Ms, queueMs,
            operationCount, estimatedBytes / (1024.0 * 1024.0)};
}

int main() {
    const int sizes[] = {1000, 10000, 100000};

    std::cout << "================ BENCHMARK D2 ================\n";
    std::cout << "Baseline: Linear Search | Toi uu: Hash Table / Min-Heap K=10\n";
    std::cout << std::left << std::setw(10) << "N"
              << std::setw(16) << "Linear MC1(ms)"
              << std::setw(16) << "Hash MC1(ms)"
              << std::setw(16) << "Range MC2(ms)"
              << std::setw(16) << "Top10(ms)"
              << std::setw(14) << "Queue(ms)"
              << std::setw(18) << "Operations"
              << "Memory est.(MB)\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    for (int size : sizes) {
        BenchmarkResult result = runBenchmark(size);
        std::cout << std::left << std::setw(10) << size
                  << std::setw(16) << std::fixed << std::setprecision(3) << result.linearSearchMs
                  << std::setw(16) << result.hashSearchMs
                  << std::setw(16) << result.rangeScanMs
                  << std::setw(16) << result.top10Ms
                  << std::setw(14) << result.queueMs
                  << std::setw(18) << result.operationCount
                  << std::setprecision(3) << result.estimatedMemoryMb << "\n";
    }

    std::cout << "\nGhi chu: Memory est. la uoc luong bo nho cua cac Book trong benchmark.\n";
    std::cout << "===============================================\n";
    return 0;
}