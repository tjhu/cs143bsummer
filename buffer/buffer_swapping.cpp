// Producer produces 1 million numbers and put them into the buffer one after
// another. Consumer consumes those numbers and output their sum.
#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

// The number of data we produce/consume
const int64_t num_data = 1E6;

// a buffer that holds an integer
class Buffer {
public:
  // Wail till the buffer is not full then append the data into the buffer
  void put(int data) {
    std::unique_lock<std::mutex> lck(mtx_);
    if (is_full()) {
      cv.wait(lck, std::bind(&Buffer::is_not_full, this));
    } 
    size_++;
    assert(size_ <= MAX_SIZE);
    data_[(current_ + size_) % MAX_SIZE] = data;
    cv.notify_one();
  }

  // Wail till the buffer is not empty then get the first data into the buffer
  int get() {
    std::unique_lock<std::mutex> lck(mtx_);
    if (!is_not_empty()) {
      cv.wait(lck, std::bind(&Buffer::is_not_empty, this));
    }
    size_--;
    assert(size_ >= 0);
    current_ = (current_ + 1) % MAX_SIZE;
    cv.notify_one();
    return data_[current_];
  }

  bool is_not_full() { return !is_full(); }

  bool is_not_empty() { return !is_empty(); }

  bool is_empty() { return size_ == 0; }

  bool is_full() { return size_ == MAX_SIZE; }

private:
  std::mutex mtx_;
  std::condition_variable cv;
  int size_ = 0;
  const static int MAX_SIZE = 2;
  int current_ = 0; // the current position of the consumer
  int data_[MAX_SIZE];
};

void producer_fn(Buffer *buffer) {
  for (int i = 0; i < num_data; i++) {
    buffer->put(i + 1);
  }
}

void consumer_fn(Buffer *buffer, uint64_t *result) {
  for (int i = 0; i < num_data; i++) {
    *result += buffer->get();
  }
}

int main() {
  Buffer buffer;
  uint64_t result = 0;
  std::thread producer_thread(std::bind(producer_fn, &buffer));
  std::thread consumer_thread(std::bind(consumer_fn, &buffer, &result));
  producer_thread.join();
  consumer_thread.join();
  std::cout << "expected: " << (num_data + 1) * (num_data / 2)
            << std::endl;
  std::cout << "result: " << result << std::endl;
  return 0;
}