#ifndef _PlayerInterfaceFactory_h_
#define _PlayerInterfaceFactory_h_

/* Foundation */
#include "Type.h"
#include "CreatorRepository.h"

#include "PlayerInterface.h"

/**********************Global Function**********************/
PlayerInterface* CreatePlayerInterface(const char *name);

#endif