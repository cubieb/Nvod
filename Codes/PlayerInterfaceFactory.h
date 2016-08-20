#ifndef _PlayerInterfaceFactory_h_
#define _PlayerInterfaceFactory_h_

/* Foundation */
#include "Type.h"
#include "PrototypeCache.h"

#include "PlayerInterface.h"

/**********************class PlayerInterfaceFactory**********************/
/*
Example:
    PlayerInterfaceFactory& factory = PlayerInterfaceFactory::GetInstance();
    PlayerInterface *player = factory.Create("TmssPlayer");
    cout << player->GetWaitingDuration() << endl;
*/
class PlayerInterfaceFactory
{
public:
    typedef PtrPrototypeCache<std::string, PlayerInterface> PlayerPrototypeCache;
    typedef PrototypeCacheRegisterSuite<PlayerPrototypeCache> PlayerPrototypeCacheRegisterSuite;
    friend class ACE_Singleton<PlayerInterfaceFactory, ACE_Recursive_Thread_Mutex>;

    ~PlayerInterfaceFactory();
    PlayerInterface* Create(const char *name);
    static PlayerInterfaceFactory& GetInstance();

private:
    PlayerInterfaceFactory();
};

#endif