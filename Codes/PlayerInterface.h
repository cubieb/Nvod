#ifndef _PlayerInterface_h_
#define _PlayerInterface_h_

#include <memory>     //shared_ptr

/* Foundation */
#include "ClassFactories.h"

/**********************class PlayerInterface**********************/
class PlayerInterface
{
public:
    PlayerInterface() {};
    virtual ~PlayerInterface() {};    

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() = 0;
};

/**********************class PlayerInterface Factory**********************/
template<typename ... Types>
PlayerInterface* CreatePlayerInterface(const char *playerName, Types ... args)
{
    typedef ClassFactories<PlayerInterface, std::string, Types ...> ClassFactoriesType;
    ClassFactoriesType& instance = ClassFactoriesType::GetInstance();

    return instance.CreateInstance(playerName, args ...);
}

#endif