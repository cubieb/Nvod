#include "SystemInclude.h"

#pragma warning(push)
#pragma warning(disable:702)   //disable warning caused by ACE library.
#pragma warning(disable:4251)  //disable warning caused by ACE library.
#pragma warning(disable:4996)  //disable warning caused by ACE library.
#include "ace/OS.h"
#include "ace/Singleton.h"

#include "TmssPlayer.h"
#include "PlayerInterfaceFactory.h"
using namespace std;

/**********************class PlayerInterfaceFactory**********************/
PlayerInterfaceFactory::PlayerInterfaceFactory() 
{}

PlayerInterfaceFactory::~PlayerInterfaceFactory() 
{}

PlayerInterface* PlayerInterfaceFactory::Create(const char *name)
{
    PlayerPrototypeCache& instance = PlayerPrototypeCache::GetInstance();
    return instance.CreateInstance(name);
}

PlayerInterfaceFactory& PlayerInterfaceFactory::GetInstance()
{
    static PlayerPrototypeCacheRegisterSuite playerReg0001("TmssPlayer", TmssPlayer::CreateInstance);

    typedef ACE_Singleton<PlayerInterfaceFactory, ACE_Recursive_Thread_Mutex> TheSingleton;
    return *TheSingleton::instance();
}

#pragma warning(pop)