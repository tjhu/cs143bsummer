// Producer produces `num_data` amount of numbers and put them into the buffer
// one after another. Consumer consumes those numbers and output their sum.

// Questions:
// 1. Let's say n=1, k1=3, k2=2, k3=2, and both P and C are ready to work at
// t=0. Basically sleep_for or sleep_until
#include "circular_buffer.hpp"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// The number of data we produce/consume
const int64_t num_data = 4;
// Size of one time unit
const auto time_unit_size = std::chrono::milliseconds(1);

bool should_produce(int i, int k1, int k2) {
  const int interval_size = k1 + k2;
  return (i % interval_size) < k1;
}

void producer_fn(Buffer *buffer, int k1, int k2) {
  int t = 0;
  int i = 0;
  while (i < num_data) {
    if (i != 0) {
      std::this_thread::sleep_for(time_unit_size);
    }
    if (should_produce(t, k1, k2)) {
      buffer->put(i + 1);
      i++;
    }
    t++;
  }
}

bool should_consume(int t, int k3) {
  return (t % k3) == 0;
}

void consumer_fn(Buffer *buffer, uint64_t *result, int k3) {
  int t = 0;
  int i = 0;
  while (i < num_data) {
    if (i != 0) {
      std::this_thread::sleep_for(time_unit_size);
    }
    if (should_consume(t, k3)) {
      *result += buffer->get();
      i++;
    }
    t++;
  }
}

// arg1: n, size of the circular array
// arg2: k1, P produces a burst of k1 items, 1 per time unit.
// arg3: k2, Then P waits for k2 time units until the next burst of k1 item.
// arg4: k3, C removes 1 item every k3 time units.
int main(int argc, char **argv) {
  assert(argc == 5);
  int n, k1, k2, k3;
  try {
    n = std::stoi(argv[1]);
    k1 = std::stoi(argv[2]);
    k2 = std::stoi(argv[3]);
    k3 = std::stoi(argv[4]);
  } catch (std::invalid_argument &e) {
    std::cout << "Please provide 4 integers for the arguments" << std::endl;
  }

  Buffer buffer(n);
  uint64_t result = 0;
  std::thread producer_thread(std::bind(producer_fn, &buffer, k1, k2));
  std::thread consumer_thread(std::bind(consumer_fn, &buffer, &result, k3));
  producer_thread.join();
  consumer_thread.join();
  std::cout << "expected: " << (num_data + 1) * (num_data / 2) << std::endl;
  std::cout << "result: " << result << std::endl;
  return 0;
}