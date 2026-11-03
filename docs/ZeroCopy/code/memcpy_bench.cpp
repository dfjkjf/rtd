
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using Clock = std::chrono::steady_clock;
volatile std::uint64_t g_sink = 0;

struct Result {
    std::size_t bytes;
    std::size_t iterations;
    double elapsed_ms;
    double avg_us;
    double throughput_MBps;
};

Result benchmark(std::size_t bytes, bool dynamic_alloc) {
    // 目标总复制量约 512 MiB，限制迭代次数以控制运行时间。
    constexpr std::size_t target_bytes = 512ULL * 1024 * 1024;
    const std::size_t iterations = std::clamp(
        target_bytes / bytes, std::size_t{100},
        std::size_t{2000000});

    // 源缓冲区在计时前分配并初始化。
    std::vector<std::byte> src(bytes);
    for (std::size_t i = 0; i < bytes; ++i) {
        src[i] = static_cast<std::byte>((i * 131U + 17U) & 0xFFU);
    }

    std::vector<std::byte> dst;
    if (!dynamic_alloc) {
        dst.resize(bytes);
    }

    // 预热：降低首次执行和冷启动带来的干扰。
    if (!dynamic_alloc) {
        for (int i = 0; i < 10; ++i) {
            std::memcpy(dst.data(), src.data(), bytes);
        }
    }

    std::uint64_t checksum = 0;
    const auto start = Clock::now();

    for (std::size_t i = 0; i < iterations; ++i) {
        if (dynamic_alloc) {
            // 每轮分配、复制、读取结果、释放。
            auto* ptr = static_cast<std::byte*>(std::malloc(bytes));
            if (ptr == nullptr) {
                std::cerr << "malloc failed\n";
                std::exit(EXIT_FAILURE);
            }

            std::memcpy(ptr, src.data(), bytes);

            checksum += std::to_integer<unsigned char>(ptr[i % bytes]);
            checksum += std::to_integer<unsigned char>(ptr[bytes - 1]);

            std::free(ptr);
        } else {
            std::memcpy(dst.data(), src.data(), bytes);

            checksum +=
                std::to_integer<unsigned char>(dst[i % bytes]);
            checksum +=
                std::to_integer<unsigned char>(dst[bytes - 1]);
        }
    }

    const auto end = Clock::now();
    g_sink = g_sink ^ checksum;

    const double elapsed_ms =
        std::chrono::duration<double, std::milli>(end - start).count();

    const double total_bytes =
        static_cast<double>(bytes) * iterations;

    return {
        bytes,
        iterations,
        elapsed_ms,
        elapsed_ms * 1000.0 / iterations,
        total_bytes / (elapsed_ms / 1000.0) / 1e6
    };
}

int main() {
    const std::vector<std::size_t> sizes = {
        64,
        1ULL * 1024,
        64ULL * 1024,
        1ULL * 1024 * 1024,
        6ULL * 1024 * 1024
    };

    std::cout << "mode,size_bytes,iterations,elapsed_ms,"
                 "avg_us,throughput_MBps\n";
    std::cout << std::fixed << std::setprecision(3);

    for (auto bytes : sizes) {
        for (bool dynamic_alloc : {false, true}) {
            const auto r = benchmark(bytes, dynamic_alloc);

            std::cout
                << (dynamic_alloc ? "malloc_copy_free" : "preallocated")
                << ',' << r.bytes
                << ',' << r.iterations
                << ',' << r.elapsed_ms
                << ',' << r.avg_us
                << ',' << r.throughput_MBps
                << '\n';
        }
    }

    // 使结果对程序可观察，避免校验数据完全未被使用。
    std::cerr << "checksum=" << g_sink << '\n';
    return 0;
}

