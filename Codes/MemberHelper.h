#ifndef _MemberHelper_h_
#define _MemberHelper_h_

#include <memory>

#define NullSharedPtr(ElementType) shared_ptr<ElementType>()
#define NullWeakPtr(ElementType) weak_ptr<ElementType>()

template<typename T>
std::weak_ptr<T> GetWeakPtr(std::shared_ptr<T> ptr)
{
    return ptr;
}

/**********************class MemberHelper**********************/
class MemberHelper
{
public:
    /* Bind */
    template<typename MemberType, typename ThisPtr>
    static void BindMember(shared_ptr<MemberType>&, shared_ptr<MemberType>& r, ThisPtr thisPtr)
    {
        r->Bind(GetWeakPtr(thisPtr));
        thisPtr->Bind(GetWeakPtr(r));
    }

    template<typename MemberType, typename ThisPtr>
    static void BindMember(shared_ptr<MemberType>& l, weak_ptr<MemberType>& r, ThisPtr)
    {
        l = r.lock();
    }

    template<typename MemberType, typename ThisPtr>
    static void BindMember(weak_ptr<MemberType>&, shared_ptr<MemberType>& r, ThisPtr thisPtr)
    {
        r->Bind(thisPtr);
    }

    template<typename MemberType, typename ThisPtr>
    static void BindMember(weak_ptr<MemberType>& l, weak_ptr<MemberType>& r, ThisPtr)
    {
        l = r;
    }

    template<typename MemberType, typename ThisPtr>
    static void BindMember(list<shared_ptr<MemberType>>&, shared_ptr<MemberType>& r, ThisPtr thisPtr)
    {
        r->Bind(GetWeakPtr(thisPtr));
        thisPtr->Bind(GetWeakPtr(r));
    }

    template<typename MemberType, typename ThisPtr>
    static void BindMember(list<shared_ptr<MemberType>>& l, weak_ptr<MemberType>& r, ThisPtr)
    {
        l.push_back(r.lock());
    }

    template<typename MemberType, typename ThisPtr>
    static void BindMember(list<weak_ptr<MemberType>>&, shared_ptr<MemberType>& r, ThisPtr thisPtr)
    {
        r->Bind(thisPtr);
    }

    template<typename MemberType, typename ThisPtr>
    static void BindMember(list<weak_ptr<MemberType>>& l, weak_ptr<MemberType>& r, ThisPtr)
    {
        l.push_back(r);
    }

    /* Unbind */
    template<typename MemberType, typename ThisPtr>
    static void UnbindMember(shared_ptr<MemberType>& l, shared_ptr<MemberType>&, ThisPtr thisPtr)
    {
        l->Unbind(GetWeakPtr(thisPtr));
        thisPtr->Unbind(GetWeakPtr(l));
    }

    template<typename MemberType, typename ThisPtr>
    static void UnbindMember(shared_ptr<MemberType>& l, weak_ptr<MemberType>&, ThisPtr)
    {
        l.reset();
    }

    template<typename MemberType, typename ThisPtr>
    static void UnbindMember(weak_ptr<MemberType>& l, shared_ptr<MemberType>&, ThisPtr thisPtr)
    {
        l.lock()->Unbind(thisPtr);
    }

    template<typename MemberType, typename ThisPtr>
    static void UnbindMember(weak_ptr<MemberType>& l, weak_ptr<MemberType>&, ThisPtr)
    {
        l.reset();
    }

    template<typename MemberType, typename ThisPtr>
    static void UnbindMember(list<shared_ptr<MemberType>>&, shared_ptr<MemberType>& r, ThisPtr thisPtr)
    {
        r->Unbind(GetWeakPtr(thisPtr));
        thisPtr->Unbind(GetWeakPtr(r));
    }

    template<typename MemberType, typename ThisPtr>
    static void UnbindMember(list<shared_ptr<MemberType>>& l, weak_ptr<MemberType>& r, ThisPtr)
    {
        auto cmp = [r](shared_ptr<MemberType> iter)->bool
        {
            return (r.lock()->GetId() == iter->GetId());
        };
        l.remove_if(cmp);
    }

    template<typename MemberType, typename ThisPtr>
    static void UnbindMember(list<weak_ptr<MemberType>>&, shared_ptr<MemberType>& r, ThisPtr thisPtr)
    {
        r->Unbind(thisPtr);
    }

    template<typename MemberType, typename ThisPtr>
    static void UnbindMember(list<weak_ptr<MemberType>>& l, weak_ptr<MemberType>& r, ThisPtr)
    {
        auto cmp = [r](weak_ptr<MemberType> iter)->bool
        {
            return (r.lock()->GetId() == iter.lock()->GetId());
        };
        l.remove_if(cmp);
    }

    /* Set */
    template<typename MemberType, typename ThisPtr>
    static void SetMember(shared_ptr<MemberType>& l, MemberType r, ThisPtr /* not used */)
    {
        if (l == nullptr)
        {
            l = make_shared<MemberType>(r);
        }
        else
        {
            *l = r;
        }
    }

    template<typename MemberType, typename ThisPtr>
    static void SetMember(shared_ptr<MemberType>& l, shared_ptr<MemberType> r, ThisPtr thisPtr)
    {
        if (l != nullptr)
        {
            thisPtr->Unbind(l);
        }

        if (r != nullptr)
        {
            thisPtr->Bind(r);
        }
    }

    template<typename MemberType, typename ThisPtr>
    static void SetMember(weak_ptr<MemberType>& l, weak_ptr<MemberType> r, ThisPtr thisPtr)
    {
        if (!l.expired())
        {
            l.lock()->Unbind(thisPtr);
        }

        if (!r.expired())
        {
            r.lock()->Bind(thisPtr);
        }
    }

    template<typename MemberType, typename ThisPtr>
    static void SetMember(list<shared_ptr<MemberType>>& l, const list<shared_ptr<MemberType>>& r, ThisPtr thisPtr)
    {
        for (auto iter = l.begin(); iter != l.end(); ++iter)
        {
            (*iter)->Unbind(GetWeakPtr(thisPtr));
        }
        l.clear();

        for (auto iter = r.cbegin(); iter != r.cend(); ++iter)
        {
            thisPtr->Bind(*iter);
        }
    }

    template<typename MemberType, typename ThisPtr>
    static void SetMember(list<weak_ptr<MemberType>>& l, const list<weak_ptr<MemberType>>& r, ThisPtr thisPtr)
    {
        for (auto iter = l.begin(); iter != l.end(); ++iter)
        {
            iter->lock()->Unbind(GetWeakPtr(thisPtr));
        }
        l.clear();

        for (auto iter = r.cbegin(); iter != r.cend(); ++iter)
        {
            iter->lock()->Bind(thisPtr);
        }
    }
};

#endif