// Producer produces `num_data` amount of numbers and put them into the buffer
// one after another. Consumer consumes those numbers and output their sum.
// I know global variables is everywhere, but for simplicity...

#include "circular_buffer.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

// The even number of data we produce/consume
const int64_t num_data = 1E3;
static_assert(num_data % 2 == 0);
// Size of one time unit
const auto time_unit_size = std::chrono::milliseconds(1);
// Whether the producer finished or not
std::atomic<bool> producer_finished = false;

class Producer {

public:
  Producer(Buffer *buffer, int k1, int k2, int d1 = -1, int d2 = -1)
      : buffer_(buffer), i_(0), k1_(k1), k2_(k2), use_rand_(d1 >= 0),
        rand_engine_(std::random_device()()), k1_dist_(k1, d1),
        k2_dist_(k2, d2) {
    assert((d1 < 0) == (d2 < 0));
    if (use_rand_) {
      next_burst_length_ = k1_dist_(rand_engine_);
    } else {
      next_burst_length_ = k1_;
    }
  }

  // Produce `data` and return the current(on function entry) size of the buffer
  int produce(int data) {
    int size = buffer_->size();
    // Produce
    std::this_thread::sleep_for(time_unit_size);
    buffer_->put(data);

    // After producing every k1 items, we take a nap that's k2 long
    if (++i_ % next_burst_length_ == 0) {
      int sleep_length = k2_;
      if (use_rand_) {
        next_burst_length_ = std::max<int>(1, k1_dist_(rand_engine_));
        sleep_length = std::max<int>(1, k2_dist_(rand_engine_));
      }
      std::this_thread::sleep_for(time_unit_size * sleep_length);
    }
    return size;
  }

private:
  Buffer *buffer_;
  int i_;
  int k1_;
  int k2_;

  // Member variables for random number generation
  const bool use_rand_;
  int next_burst_length_;
  std::mt19937 rand_engine_;
  // TODO: need to confirmation that normal is okay
  std::normal_distribution<> k1_dist_;
  std::normal_distribution<> k2_dist_;
};

class Consumer {
public:
  Consumer(Buffer *buffer, int k3) : buffer_(buffer), k3_(k3) {}

  // Return the next item on the buffer and set `size` to the size of current
  // buffer
  int consume(int *size = nullptr) {
    if (size != nullptr) {
      *size = buffer_->size();
    }
    std::this_thread::sleep_for(time_unit_size * k3_);
    return buffer_->get();
  }

private:
  Buffer *buffer_;
  int k3_;
};

void producer_fn(Producer *p) {
  uint64_t size_sum = 0;
  int max_size = 0;
  for (int i = 0; i < num_data; i++) {
    int tmp = p->produce(i + 1);
    size_sum += tmp;
    max_size = std::max(tmp, max_size);
  }
  std::cout << "producer: ( avg : " << (float)size_sum / num_data
            << " ), ( max: " << max_size << " )" << std::endl;
  producer_finished = true;
}

void consumer_fn(Consumer *c, uint64_t *result) {
  uint64_t size_sum = 0;
  int max_size = 0;
  for (int i = 0; i < num_data; i++) {
    int tmp;
    *result += c->consume(&tmp);
    size_sum += tmp;
    max_size = std::max(tmp, max_size);
  }

  // Spin until the producer finishes
  while (!producer_finished.load()) {
    ; // spin
  }
  std::cout << "consumer: ( avg : " << (float)size_sum / num_data
            << " ), ( max: " << max_size << " )" << std::endl;
}

// arg1: N, size of the circular buffer.
// arg2: k1, P produces a burst of k1 items, 1 per time unit.
// arg3: k2, Then P waits for k2 time units until the next burst of k1 item.
// arg4: k3, C removes 1 item every k3 time units.
// arg5: d1, if it's set, k1 will be draw from a normal(k1, d1) distribution.
// arg6: d2, if it's set, k2 will be draw from a normal(k2, d1) distribution.
// d1 and d2 must be both set or both not set.
int main(int argc, char **argv) {
  assert(argc == 5 || argc == 7);
  int N, k1, k2, k3, d1 = -1, d2 = -1;
  try {
    N = std::stoi(argv[1]);
    k1 = std::stoi(argv[2]);
    k2 = std::stoi(argv[3]);
    k3 = std::stoi(argv[4]);
    if (argc == 7) {
      d1 = std::stoi(argv[5]);
      d2 = std::stoi(argv[6]);
    }
  } catch (std::invalid_argument &e) {
    std::cout << "Please provide 4-6 integers for the arguments" << std::endl;
  }

  Buffer buffer(N);
  uint64_t result = 0;
  Producer p(&buffer, k1, k2, d1, d2);
  std::thread producer_thread(std::bind(producer_fn, &p));
  Consumer c(&buffer, k3);
  std::thread consumer_thread(std::bind(consumer_fn, &c, &result));
  producer_thread.join();
  consumer_thread.join();
  const auto expected = (num_data + 1) * (num_data / 2);
  assert(expected == result);
  return 0;
}