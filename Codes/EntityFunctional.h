#ifndef _EntityFunctional_h_
#define _EntityFunctional_h_

#include <memory>     /* shared_ptr, weak_ptr, enable_shared_from_this */
#include <functional> /* std::binary_function */

using std::shared_ptr;
using std::weak_ptr;

template<typename E>
struct LessCmpId : public std::binary_function <E, E, bool>
{
    bool operator()(const E& left, const E& right) const
    {
        return left.GetId() < right.GetId();
    }

    bool operator()(shared_ptr<E> left, shared_ptr<E> right) const
    {
        return operator()(*left, *right);
    }

    bool operator()(weak_ptr<E> left, weak_ptr<E> right) const
    {
        return operator()(*left.lock(), *right.lock());
    }
};

template<typename E>
struct GreaterCmpId : public std::binary_function <E, E, bool>
{
    bool operator()(const E& left, const E& right) const
    {
        return left.GetId() > right.GetId();
    }

    bool operator()(shared_ptr<E> left, shared_ptr<E> right) const
    {
        return operator()(*left, *right);
    }

    bool operator()(weak_ptr<E> left, weak_ptr<E> right) const
    {
        return operator()(*left.lock(), *right.lock());
    }
};

template<typename E>
struct EqualCmpId : public std::binary_function <E, E, bool>
{
    bool operator()(const E& left, const E& right) const
    {
        return left.GetId() == right.GetId();
    }

    bool operator()(shared_ptr<E> left, shared_ptr<E> right) const
    {
        return operator()(*left, *right);
    }

    bool operator()(weak_ptr<E> left, weak_ptr<E> right) const
    {
        return operator()(*left.lock(), *right.lock());
    }
};

#endif
