#ifndef _MemberHelper_h_
#define _MemberHelper_h_

#include <memory>
#include <list>    /* std::list */
#include <vector>  /* std::list */
#include <algorithm> /* remove_if */

/* Entity */
#include "EntityFunctional.h"

using std::list;
using std::vector;
using std::set;
using std::shared_ptr;
using std::weak_ptr;

#define NullSharedPtr(ElementType) shared_ptr<ElementType>()
#define NullWeakPtr(ElementType) weak_ptr<ElementType>()

template<typename T>
weak_ptr<T> GetWeakPtr(shared_ptr<T> ptr)
{
    return ptr;
}

class MemberHelper
{
public:
    /******************** member is value ********************/
    /* BindMember */
    template<typename M, typename ThisPtr>
    static void BindMember(shared_ptr<M>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        r->Bind(GetWeakPtr(thisPtr));
        thisPtr->Bind(GetWeakPtr(r));
    }

    template<typename M, typename ThisPtr>
    static void BindMember(shared_ptr<M>& l, weak_ptr<M>& r, ThisPtr)
    {
        l = r.lock();
    }

    template<typename M, typename ThisPtr>
    static void BindMember(weak_ptr<M>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        //call void BindMember(shared_ptr<M>&, shared_ptr<M>& r, ThisPtr thisPtr) indirectly
        r->Bind(thisPtr);
    }

    template<typename M, typename ThisPtr>
    static void BindMember(weak_ptr<M>& l, weak_ptr<M>& r, ThisPtr)
    {
        l = r;
    }

    /* UnbindMember */
    template<typename M, typename ThisPtr>
    static void UnbindMember(shared_ptr<M>& l, shared_ptr<M>&, ThisPtr thisPtr)
    {
        l->Unbind(GetWeakPtr(thisPtr));
        thisPtr->Unbind(GetWeakPtr(l));
    }

    template<typename M, typename ThisPtr>
    static void UnbindMember(shared_ptr<M>& l, weak_ptr<M>&, ThisPtr)
    {
        l.reset();
    }

    template<typename M, typename ThisPtr>
    static void UnbindMember(weak_ptr<M>& l, shared_ptr<M>&, ThisPtr thisPtr)
    {
        //call void UnbindMember(shared_ptr<M>& l, shared_ptr<M>&, ThisPtr thisPtr) indirectly
        l.lock()->Unbind(thisPtr);
    }

    template<typename M, typename ThisPtr>
    static void UnbindMember(weak_ptr<M>& l, weak_ptr<M>&, ThisPtr)
    {
        l.reset();
    }

    /* SetMember */
    template<typename M, typename ThisPtr>
    static void SetMember(shared_ptr<M>& l, M r, ThisPtr /* not used */)
    {
        if (l == nullptr)
        {
            l = make_shared<M>(r);
        }
        else
        {
            *l = r;
        }
    }

    template<typename M, typename ThisPtr>
    static void SetMember(shared_ptr<M>& l, shared_ptr<M> r, ThisPtr thisPtr)
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

    template<typename M, typename ThisPtr>
    static void SetMember(weak_ptr<M>& l, weak_ptr<M> r, ThisPtr thisPtr)
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

    /******************** member is list ********************/
    /* BindMember */
    template <typename M, typename ThisPtr>
    static void BindMember(list<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        r->Bind(GetWeakPtr(thisPtr));
        thisPtr->Bind(GetWeakPtr(r));
    }

    template <typename M, typename ThisPtr>
    static void BindMember(list<shared_ptr<M>>& l, weak_ptr<M>& r, ThisPtr thisPtr)
    {
        l.push_back(r.lock());
    }

    template <typename M, typename ThisPtr>
    static void BindMember(list<weak_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        //call void BindMember(C<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr) indirectly
        r->Bind(thisPtr);
    }

    template <typename M, typename ThisPtr>
    static void BindMember(list<weak_ptr<M>>& l, weak_ptr<M>& r, ThisPtr thisPtr)
    {
        l.push_back(r);
    }

    /* UnbindMember */
    template <typename M, typename ThisPtr>
    static void UnbindMember(list<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        r->Unbind(GetWeakPtr(thisPtr));
        thisPtr->Unbind(GetWeakPtr(r));
    }

    template <typename M, typename ThisPtr>
    static void UnbindMember(list<shared_ptr<M>>& l, weak_ptr<M>& r, ThisPtr)
    {
        auto cmp = [r](shared_ptr<M> iter)->bool
        {
            return (r.lock()->GetId() == iter->GetId());
        };
        l.erase(remove_if(l.begin(), l.end(), cmp));
    }

    template <typename M, typename ThisPtr>
    static void UnbindMember(list<weak_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        //call void UnbindMember(C<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr) indirectly
        r->Unbind(thisPtr);
    }

    template <typename M, typename ThisPtr>
    static void UnbindMember(list<weak_ptr<M>>& l, weak_ptr<M>& r, ThisPtr)
    {
        auto cmp = [r](weak_ptr<M> iter)->bool
        {
            return (r.lock()->GetId() == iter.lock()->GetId());
        };
        l.erase(remove_if(l.begin(), l.end(), cmp));
    }

    /* SetMember */
    template <typename M, typename ThisPtr>
    static void SetMember(list<shared_ptr<M>>& l, const list<shared_ptr<M>>& r, ThisPtr thisPtr)
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

    template <typename M, typename ThisPtr>
    static void SetMember(list<weak_ptr<M>>& l, const list<weak_ptr<M>>& r, ThisPtr thisPtr)
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

    /******************** member is vector ********************/
    /* BindMember */
    template <typename M, typename ThisPtr>
    static void BindMember(vector<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        r->Bind(GetWeakPtr(thisPtr));
        thisPtr->Bind(GetWeakPtr(r));
    }

    template <typename M, typename ThisPtr>
    static void BindMember(vector<shared_ptr<M>>& l, weak_ptr<M>& r, ThisPtr thisPtr)
    {
        l.push_back(r.lock());
    }

    template <typename M, typename ThisPtr>
    static void BindMember(vector<weak_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        //call void BindMember(C<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr) indirectly
        r->Bind(thisPtr);
    }

    template <typename M, typename ThisPtr>
    static void BindMember(vector<weak_ptr<M>>& l, weak_ptr<M>& r, ThisPtr thisPtr)
    {
        l.push_back(r);
    }

    /* UnbindMember */
    template <typename M, typename ThisPtr>
    static void UnbindMember(vector<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        r->Unbind(GetWeakPtr(thisPtr));
        thisPtr->Unbind(GetWeakPtr(r));
    }

    template <typename M, typename ThisPtr>
    static void UnbindMember(vector<shared_ptr<M>>& l, weak_ptr<M>& r, ThisPtr)
    {
        auto cmp = [r](shared_ptr<M> iter)->bool
        {
            return (r.lock()->GetId() == iter->GetId());
        };
        l.erase(remove_if(l.begin(), l.end(), cmp));
    }

    template <typename M, typename ThisPtr>
    static void UnbindMember(vector<weak_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        //call void UnbindMember(C<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr) indirectly
        r->Unbind(thisPtr);
    }

    template <typename M, typename ThisPtr>
    static void UnbindMember(vector<weak_ptr<M>>& l, weak_ptr<M>& r, ThisPtr)
    {
        auto cmp = [r](weak_ptr<M> iter)->bool
        {
            return (r.lock()->GetId() == iter.lock()->GetId());
        };
        l.erase(remove_if(l.begin(), l.end(), cmp));
    }

    /* SetMember */
    template <typename M, typename ThisPtr>
    static void SetMember(vector<shared_ptr<M>>& l, const vector<shared_ptr<M>>& r, ThisPtr thisPtr)
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

    template <typename M, typename ThisPtr>
    static void SetMember(vector<weak_ptr<M>>& l, const vector<weak_ptr<M>>& r, ThisPtr thisPtr)
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

    /******************** member is set ********************/
    /* BindMember */
    template <typename M, typename ThisPtr>
    static void BindMember(set<shared_ptr<M>, LessCmpId<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        r->Bind(GetWeakPtr(thisPtr));
        thisPtr->Bind(GetWeakPtr(r));
    }

    template <typename M, typename ThisPtr>
    static void BindMember(set<shared_ptr<M>, LessCmpId<M>>& l, weak_ptr<M>& r, ThisPtr thisPtr)
    {
        l.push_back(r.lock());
    }

    template <typename M, typename ThisPtr>
    static void BindMember(set<weak_ptr<M>, LessCmpId<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        //call void BindMember(C<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr) indirectly
        r->Bind(thisPtr);
    }

    template <typename M, typename ThisPtr>
    static void BindMember(set<weak_ptr<M>, LessCmpId<M>>& l, weak_ptr<M>& r, ThisPtr thisPtr)
    {
        l.insert(r);
    }

    /* UnbindMember */
    template <typename M, typename ThisPtr>
    static void UnbindMember(set<shared_ptr<M>, LessCmpId<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        r->Unbind(GetWeakPtr(thisPtr));
        thisPtr->Unbind(GetWeakPtr(r));
    }

    template <typename M, typename ThisPtr>
    static void UnbindMember(set<shared_ptr<M>, LessCmpId<M>>& l, weak_ptr<M>& r, ThisPtr)
    {
        l.erase(l.find(r.lock()));
    }

    template <typename M, typename ThisPtr>
    static void UnbindMember(set<weak_ptr<M>, LessCmpId<M>>&, shared_ptr<M>& r, ThisPtr thisPtr)
    {
        //call void UnbindMember(C<shared_ptr<M>>&, shared_ptr<M>& r, ThisPtr thisPtr) indirectly
        r->Unbind(thisPtr);
    }

    template <typename M, typename ThisPtr>
    static void UnbindMember(set<weak_ptr<M>, LessCmpId<M>>& l, weak_ptr<M>& r, ThisPtr)
    {
        l.erase(l.find(r));
    }

    /* SetMember */
    template <typename M, typename ThisPtr>
    static void SetMember(set<shared_ptr<M>, LessCmpId<M>>& l, const set<shared_ptr<M>, LessCmpId<M>>& r, ThisPtr thisPtr)
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
};

#endif