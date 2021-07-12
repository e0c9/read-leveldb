// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Slice is a simple structure containing a pointer into some external
// storage and a size.  The user of a Slice must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
// 维护一个执政指向外部存储 以及 维护一个size大小
// 使用者需要保证外部存储被释放时不再使用它
//
// Multiple threads can invoke const methods on a Slice without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Slice must use
// external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>

#include "leveldb/export.h"

namespace leveldb {

// 通过指针避免拷贝数据，骚气啊，需要外部用户自己去释放内存。
class LEVELDB_EXPORT Slice {
 public:
  // Create an empty slice.
  Slice() : data_(""), size_(0) {}

  // Create a slice that refers to d[0,n-1].
  Slice(const char* d, size_t n) : data_(d), size_(n) {}

  // Create a slice that refers to the contents of "s"
  Slice(const std::string& s) : data_(s.data()), size_(s.size()) {}

  // Create a slice that refers to s[0,strlen(s)-1]
  Slice(const char* s) : data_(s), size_(strlen(s)) {}

  // Intentionally copyable.
  Slice(const Slice&) = default;
  Slice& operator=(const Slice&) = default;

  // Return a pointer to the beginning of the referenced data
  const char* data() const { return data_; }

  // Return the length (in bytes) of the referenced data
  size_t size() const { return size_; }

  // Return true iff the length of the referenced data is zero
  bool empty() const { return size_ == 0; }

  // Return the ith byte in the referenced data.
  // REQUIRES: n < size()
  char operator[](size_t n) const {
    assert(n < size());
    return data_[n];
  }

  // Change this slice to refer to an empty array
  void clear() {
    data_ = "";
    size_ = 0;
  }

  // Drop the first "n" bytes from this slice.
  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n; //指针移动快啊
    size_ -= n;
  }

  // Return a string that contains the copy of the referenced data.
  std::string ToString() const { return std::string(data_, size_); }

  // Three-way comparison.  Returns value:
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int compare(const Slice& b) const;

  // Return true if "x" is a prefix of "*this"
  bool starts_with(const Slice& x) const {
    return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
  }

 private:
 // 指向常量char，不可以改变内部内容
  const char* data_;
  size_t size_;
};

inline bool operator==(const Slice& x, const Slice& y) {
  return ((x.size() == y.size()) &&
          (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(const Slice& x, const Slice& y) { return !(x == y); }

inline int Slice::compare(const Slice& b) const {
  const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
  int r = memcmp(data_, b.data_, min_len);
  if (r == 0) {
    if (size_ < b.size_)
      r = -1;
    else if (size_ > b.size_)
      r = +1;
  }
  return r;
}

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_SLICE_H_
