#ifndef _PlayerInterface_h_
#define _PlayerInterface_h_

#include <utility>    //std::pair

/* Foundation */
#include "Type.h"
#include "ClassFactories.h"

/**********************class PlayerInterface**********************/
class PlayerInterface
{
public:
    PlayerInterface() {};
    virtual ~PlayerInterface() {};    

    virtual PlayerId GetPlayerId() const = 0;
    virtual Milliseconds GetWaitingDuration() = 0;
    virtual std::pair<uchar_t *, size_t> GetCurrentClip() = 0;
};

class ComparePlayerId : public std::unary_function<PlayerInterface, bool>
{
public:
    ComparePlayerId(PlayerId playerId)
        : playerId(playerId)
    {}

    result_type operator()(const argument_type &player)
    {
        return (result_type)(player.GetPlayerId() == playerId);
    }

    result_type operator()(const argument_type *player)
    {
        return this->operator()(*player);
    }

    result_type operator()(const std::shared_ptr<argument_type> player)
    {
        return this->operator()(*player);
    }

    result_type operator()(const std::weak_ptr<argument_type> player)
    {
        return this->operator()(player.lock());
    }

private:
    PlayerId playerId;
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