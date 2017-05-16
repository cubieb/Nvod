#ifndef _CurlOutputHandler_h_
#define _CurlOutputHandler_h_

/* Foundation */
#include "SystemInclude.h"

typedef std::function<size_t(void *, size_t, size_t)> HttpStringHandler;

size_t CurlOutputHandler(void *ptr, size_t size, size_t nmemb, void *userdata);

#endif