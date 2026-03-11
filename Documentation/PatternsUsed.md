# Patterns Used in the Server

This document lists implementation patterns currently used in the server codebase, with source references.

## 1. Message-driven dispatch (receive loop + switch)
- Pattern: Route each incoming message by message type in one central switch.
- Why used: Simple protocol handling with clear control flow.

## 2. Handler-per-command separation
- Pattern: Keep one method per protocol command.
- Why used: Better readability and smaller, focused methods.

## 3. Pollable state machine
- Pattern: Explicit server state transitions (Idle, Computing, Done).
- Why used: Client can poll status independently of computation duration.

## 4. Non-blocking mock compute with background thread
- Pattern: Start detached worker that completes after 5 seconds.
- Why used: Main loop stays responsive to status polls and other requests.

## 5. Fail-fast parse + rollback on error
- Pattern: Validate/parse aggressively, throw on invalid input, catch and rollback.
- Why used: Keeps error handling centralized and state consistent.

## 6. Chunked payload receive
- Pattern: Assemble full payload from initial message bytes plus follow-up socket reads.
- Why used: Supports payloads larger than one socket buffer.

## 7. Config persistence abstraction module
- Pattern: Move config read/write into separate module.
- Why used: Reduces responsibilities inside ServerApp and improves structure.

## 8. Centralized logging helper
- Pattern: One helper handles all log write formatting and cfg.log gating.
- Why used: Avoids duplicated log guard and formatting logic.

## Bibliography (Web Sources)

1. cppreference.com:https://en.cppreference.com/w/cpp/thread/thread
2. cppreference.com: std::this_thread::sleep_for https://en.cppreference.com/w/cpp/thread/sleep_for
3. cppreference.com: std::atomic. https://en.cppreference.com/w/cpp/atomic/atomic
4. C++ Core Guidelines - Concurrency https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-concurrency
5. Wikipedia: https://en.wikipedia.org/wiki/Reactor_pattern
6. Beej's Guide to Network Programming. https://beej.us/guide/bgnet/
7. Refactoring.Guru https://refactoring.guru/design-patterns/state
8. Wikipedia: Single-responsibility principle. https://en.wikipedia.org/wiki/Single-responsibility_principle
