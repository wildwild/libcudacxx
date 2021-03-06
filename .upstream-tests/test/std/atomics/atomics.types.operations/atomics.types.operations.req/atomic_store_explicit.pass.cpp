//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: libcpp-has-no-threads, pre-sm-60
// UNSUPPORTED: windows && pre-sm-70

// <cuda/std/atomic>

// template <class T>
//     void
//     atomic_store_explicit(volatile atomic<T>* obj, T desr, memory_order m);
//
// template <class T>
//     void
//     atomic_store_explicit(atomic<T>* obj, T desr, memory_order m);

#include <cuda/std/atomic>
#include <cuda/std/type_traits>
#include <cuda/std/cassert>

#include "test_macros.h"
#include "atomic_helpers.h"
#include "cuda_space_selector.h"

template <class T, template<typename, typename> typename Selector, cuda::thread_scope>
struct TestFn {
  __host__ __device__
  void operator()() const {
    typedef cuda::std::atomic<T> A;
    Selector<A, constructor_initializer> sel;
    A & t = *sel.construct();
    cuda::std::atomic_store_explicit(&t, T(1), cuda::std::memory_order_seq_cst);
    assert(t == T(1));
    Selector<volatile A, constructor_initializer> vsel;
    volatile A & vt = *vsel.construct();
    cuda::std::atomic_store_explicit(&vt, T(2), cuda::std::memory_order_seq_cst);
    assert(vt == T(2));
  }
};


int main(int, char**)
{
#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 700
    TestEachAtomicType<TestFn, local_memory_selector>()();
#endif
#ifdef __CUDA_ARCH__
    TestEachAtomicType<TestFn, shared_memory_selector>()();
    TestEachAtomicType<TestFn, global_memory_selector>()();
#endif

  return 0;
}
