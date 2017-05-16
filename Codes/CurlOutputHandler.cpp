#include "CurlOutputHandler.h"

size_t CurlOutputHandler(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    HttpStringHandler *handler = (HttpStringHandler*)userdata;

    return (*handler)(ptr, size, nmemb);
}