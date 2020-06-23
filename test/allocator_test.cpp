/* MIT License

Copyright (c) 2020 Dheeraj R Reddy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
==============================================================================*/

#include <cstdlib>

#ifdef SINGLE_HEADER
#include "shadesmar.h"
#else
#include "shadesmar/concurrency/lock.h"
#include "shadesmar/memory/allocator.h"
#endif

void basic() {
  auto *memblock = reinterpret_cast<uint8_t *>(malloc(1024 * 1024));

  shm::memory::Allocator alloc(memblock, 1024 * 1024, nullptr);

  auto *x = alloc.alloc(100);
  auto *y = alloc.alloc(250);
  auto *z = alloc.alloc(1000);

  auto xh = alloc.ptr_to_handle(x);
  auto yh = alloc.ptr_to_handle(y);
  auto zh = alloc.ptr_to_handle(z);

  assert(yh - xh > 100);
  assert(zh - yh > 250);

  assert(!alloc.free(z));
  assert(!alloc.free(y));
  assert(alloc.free(x));

  assert(!alloc.free(z));
  assert(alloc.free(y));
  assert(alloc.free(z));

  free(memblock);
}

void size_limit() {
  auto *memblock = reinterpret_cast<uint8_t *>(malloc(100));
  shm::memory::Allocator alloc(memblock, 100, nullptr);

  auto *x = alloc.alloc(50);
  auto *y = alloc.alloc(32);
  auto *z = alloc.alloc(50);

  assert(x != nullptr);
  assert(y != nullptr);
  assert(z == nullptr);

  free(memblock);
}

void perfect_wrap_around() {
  auto *memblock = reinterpret_cast<uint8_t *>(malloc(100));
  shm::memory::Allocator alloc(memblock, 100, nullptr);

  auto *x = alloc.alloc(50);
  auto *y = alloc.alloc(32);
  auto *z = alloc.alloc(50);

  assert(x != nullptr);
  assert(y != nullptr);
  assert(z == nullptr);

  assert(alloc.free(x));
  assert(alloc.free(y));

  z = alloc.alloc(50);
  assert(z != nullptr);

  free(memblock);
}

void wrap_around() {
  auto *memblock = reinterpret_cast<uint8_t *>(malloc(100));
  shm::memory::Allocator alloc(memblock, 100, nullptr);

  auto *x = alloc.alloc(50);
  auto *y = alloc.alloc(32);

  assert(x != nullptr);
  assert(y != nullptr);

  assert(alloc.free(x));

  auto *z = alloc.alloc(40);
  assert(z != nullptr);

  assert(alloc.free(y));
  assert(alloc.free(z));

  free(memblock);
}

void cyclic() {
  auto *memblock = reinterpret_cast<uint8_t *>(malloc(256));
  shm::memory::Allocator alloc(memblock, 256, nullptr);

  auto *it1 = alloc.alloc(40);
  auto *it2 = alloc.alloc(40);

  assert(it1 != nullptr);
  assert(it2 != nullptr);

  int iters = 100;
  while (iters--) {
    auto *it3 = alloc.alloc(40);
    auto *it4 = alloc.alloc(40);

    assert(it3 != nullptr);
    assert(it4 != nullptr);

    assert(alloc.free(it1));
    assert(alloc.free(it2));

    it1 = it3;
    it2 = it4;
  }

  assert(alloc.free(it1));
  assert(alloc.free(it2));

  free(memblock);
}

int main() {
  basic();
  size_limit();
  perfect_wrap_around();
  wrap_around();
  cyclic();
}