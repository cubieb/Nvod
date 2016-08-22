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
class PlayerInterfaceFactory
{
public:
    typedef std::string I;
    typedef std::function<PlayerInterface*()> F;

    typedef CreatorRepository<I, F>         CreatorRepositoryType;
    typedef CreatorRepositoryRegistor<I, F> RegistorType;
    friend class ACE_Singleton<PlayerInterfaceFactory, ACE_Recursive_Thread_Mutex>;

    ~PlayerInterfaceFactory();
    F GetCreator(const char *name);
    static PlayerInterfaceFactory& GetInstance();

private:
    PlayerInterfaceFactory();
};

/**********************class PlayerInterfaceFactory**********************/
static PlayerInterfaceFactory::RegistorType p0001("TmssPlayer", TmssPlayer::CreateInstance);
PlayerInterfaceFactory::PlayerInterfaceFactory() 
{}

PlayerInterfaceFactory::~PlayerInterfaceFactory() 
{}

PlayerInterfaceFactory::F PlayerInterfaceFactory::GetCreator(const char *name)
{
    CreatorRepositoryType& instance = CreatorRepositoryType::GetInstance();
    return instance.GetCreator(name);
}

PlayerInterfaceFactory& PlayerInterfaceFactory::GetInstance()
{
    typedef ACE_Singleton<PlayerInterfaceFactory, ACE_Recursive_Thread_Mutex> TheSingleton;
    return *TheSingleton::instance();
}

/**********************Global Function**********************/
PlayerInterface* CreatePlayerInterface(const char *name)
{
    PlayerInterfaceFactory::F Create = PlayerInterfaceFactory::GetInstance().GetCreator("TmssPlayer");
    return Create();
}

#pragma warning(pop)