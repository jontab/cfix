# cfix

## Background

A C parser for the Financial Information eXchange (FIX) protocol.

## Motivation

This project aims to provide a performant FIX protocol parser written in C. While the FIX protocol encompasses session management, heartbeats, and message retransmission, this implementation focuses solely on parsing.

## Key Features

* **Zero-Copy Approach:**
    * Minimizes data copying for high-frequency, low-latency environments.
    * Provides an API to directly pass a character pointer and byte count to `recv`.
    * Modifies the ring buffer in-place to null-terminate strings, eliminating extra copies.
    * **Important:** Message strings point to the ring buffer; ensure the buffer's lifetime exceeds message usage.
* **Magic Ring Buffer:**
    * Implements a ring buffer for efficient stream processing (e.g., packet parsing).
    * Avoids data shifting by wrapping around at the buffer's end.
    * Utilizes `mmap` to create a "magic" ring buffer:
        * Reserves a page of memory.
        * Maps the subsequent page to the same memory region.
        * Enables seamless wrap-around without explicit boundary checks.
* **Performance Focused:** Designed to be decently fast.

## Ring Buffer Explanation

A ring buffer optimizes stream processing by eliminating data shifting. When the "read" pointer reaches the end, it wraps around to the beginning. Similarly, the "write" pointer wraps when it exceeds the buffer's capacity.

## Benchmark Statistics

Although not a representation of a typical scenario, I've made a benchmark that parses the same message over a million times. The results are interesting.

- With BeginString & CheckSum validation enabled.
```
Total time for 10000000 iterations: 2977346 usec
Average time per iteration: 0.297735 usec
Minimum time per iteration: 0 usec
Maximum time per iteration: 2944 usec
```

- With only BeginString validation disabled.
```
Total time for 10000000 iterations: 2899572 usec
Average time per iteration: 0.289957 usec
Minimum time per iteration: 0 usec
Maximum time per iteration: 51 usec
```

- With only CheckSum validation disabled.
```
Total time for 10000000 iterations: 1382040 usec
Average time per iteration: 0.138204 usec
Minimum time per iteration: 0 usec
Maximum time per iteration: 47 usec
```

This shows that there is about a:
- 53.5% increase in performance when disabling CheckSum validation; and about a
- 2.6% increase in performance when disabling BeginString validation.

The drastic increase in the CheckSum scenario is probably due to the fact that I iterate over the entire message again to calculate the measured CheckSum value. Also, the scenario with both of the options enabled rises to around 4,000,000 microseconds when `message_init` is moved within the for-loop. This is because there is a memory allocation when the `field_list` objects inside the list have never been written to & expanded (this is how C++ `std::vector` works as well).
