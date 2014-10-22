#ifndef UTIL_DENSE_HASH_MAP_HPP_
#define UTIL_DENSE_HASH_MAP_HPP_

#include <cassert>
#include "define.hpp"


#ifdef __gcc__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

#ifdef rep
  #undef rep
  #include <google/dense_hash_map>
  #define rep(i, n) reps(i, 0, n)
#else
  #include <google/dense_hash_map>
#endif

#ifdef __gcc__
#pragma GCC diagnostic pop
#endif


namespace util
{

template <class Key, class T,
          class HashFcn = SPARSEHASH_HASH<Key>,   // defined in sparseconfig.h
          class EqualKey = std::equal_to<Key>,
          class Alloc = google::libc_allocator_with_realloc<std::pair<const Key, T>>>
class dense_hash_map : public google::dense_hash_map<Key, T, HashFcn, EqualKey, Alloc>
{
private:
    using Base = google::dense_hash_map<Key, T, HashFcn, EqualKey, Alloc>;

public:
    using key_type             = typename Base::key_type;
    using data_type            = typename Base::data_type;
    using mapped_type          = typename Base::mapped_type;
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

    dense_hash_map() = default;

    explicit dense_hash_map(const key_type& empty_key_val) {
        Base::set_empty_key(empty_key_val);
    }

    data_type& at(const key_type& key) {
        iterator itr = Base::find(key);
        assert(itr != end());
        return itr->second;
    }

    const data_type& at(const key_type& key) const {
        const_iterator itr = Base::find(key);
        assert(itr != end());
        return itr->second;
    }
};

} // end of namespace util

#endif
