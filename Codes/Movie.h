#ifndef _Movie_h_
#define _Movie_h_

#include "SystemInclude.h"
#include "Type.h"
#include "ContainerBase.h"

class Movie: public ContainerBase
{
public:
    typedef Iterator<const char *>::MyIter      iterator;
    typedef ConstIterator<const char *>::MyIter const_iterator;

    Movie(MovieId id, const char *path);

    iterator Begin();
    iterator End();

private:
    MovieId id;
};

#endif