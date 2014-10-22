#ifndef UTIL_DENSE_HASH_SET_HPP_
#define UTIL_DENSE_HASH_SET_HPP_

#include "define.hpp"

#ifdef __gcc__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

#ifdef rep
  #undef rep
  #include <google/dense_hash_set>
  #define rep(i, n) reps(i, 0, n)
#else
  #include <google/dense_hash_set>
#endif

#ifdef __gcc__
#pragma GCC diagnostic pop
#endif


namespace util
{

template <class Value,
          class HashFcn = SPARSEHASH_HASH<Value>,   // defined in sparseconfig.h
          class EqualKey = std::equal_to<Value>,
          class Alloc = google::libc_allocator_with_realloc<Value>
>
class dense_hash_set : public google::dense_hash_set<Value, HashFcn, EqualKey, Alloc>
{
private:
    using Base = google::dense_hash_set<Value, HashFcn, EqualKey, Alloc>;

public:
    using key_type             = typename Base::key_type;
    using value_type           = typename Base::value_type;
    using hasher               = typename Base::hasher;
    using key_equal            = typename Base::key_equal;
    using allocator_type       = typename Base::allocator_type;

    using size_type            = typename Base::size_type;
    using difference_type      = typename Base::difference_type;
    using pointer              = typename Base::pointer;
    using const_pointer        = typename Base::const_pointer;
    using reference            = typename Base::reference;
    using const_reference      = typename Base::const_reference;

    using iterator             = typename Base::iterator;
    using const_iterator       = typename Base::const_iterator;
    using local_iterator       = typename Base::local_iterator;
    using const_local_iterator = typename Base::const_local_iterator;

    using Base::end;

    dense_hash_set() = default;

    explicit dense_hash_set(const key_type& empty_key_val) {
        Base::set_empty_key(empty_key_val);
    }
};

} // end of namespace util

#endif
