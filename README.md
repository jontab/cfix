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
