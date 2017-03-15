#ifndef _MovieRepositoryInterface_h_
#define _MovieRepositoryInterface_h_

#include "SystemInclude.h"

using std::shared_ptr;

class MovieEntity;

class MovieRepositoryInterface
{
protected:
    MovieRepositoryInterface() {};
    virtual ~MovieRepositoryInterface() {}

public:
    static MovieRepositoryInterface& GetInstance();

    virtual bool QueryMovieInfo(shared_ptr<MovieEntity> movie) = 0;
    virtual bool DownloadMovie();
};

#endif