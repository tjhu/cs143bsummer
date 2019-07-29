#pragma once

#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <vector>

// a buffer that holds an integer
class Buffer {
public:
  typedef int64_t data_t;
  Buffer(int capacity)
      : capacity_(capacity), data_(std::vector<data_t>(capacity)) {}

  // Wail till the buffer is not full then append the data into the buffer
  void put(data_t data) {
    std::unique_lock<std::mutex> lck(mtx_);
    if (is_full()) {
      blocked_cnt += 1;
      cv.wait(lck, std::bind(&Buffer::is_not_full, this));
    }
    size_++;
    assert(size_ <= capacity_);
    data_[(current_ + size_) % capacity_] = data;
    cv.notify_one();
  }

  // Wail till the buffer is not empty then get the first data into the buffer
  data_t get() {
    std::unique_lock<std::mutex> lck(mtx_);
    if (!is_not_empty()) {
      cv.wait(lck, std::bind(&Buffer::is_not_empty, this));
    }
    size_--;
    assert(size_ >= 0);
    current_ = (current_ + 1) % capacity_;
    cv.notify_one();
    return data_[current_];
  }

  inline int size() const { return size_; }

  bool is_not_full() const { return !is_full(); }

  bool is_not_empty() const { return !is_empty(); }

  bool is_empty() const { return size_ == 0; }

  bool is_full() const { return size_ == capacity_; }

  int get_blocked_cnt() const { return blocked_cnt; }

private:
  Buffer();

  std::mutex mtx_;
  std::condition_variable cv;
  const int capacity_;
  volatile int size_ = 0;
  volatile int current_ = 0; // the current position of the consumer
  std::vector<data_t> data_;

  // Counters
  // Number of times `put()` is being blocked
  int blocked_cnt = 0;
};