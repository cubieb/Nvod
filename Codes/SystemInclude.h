 
#ifndef _SystemInclude_h_
#define _SystemInclude_h_

#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

/*** OS header. ***/
#ifdef _WIN32
    /* Disables fopen(), strcpy(), ... security warning on Microsoft compilers.
     * The _CRT_SECURE_NO_WARNINGS must be defined before including any system 
     * header files which may cause the security warning.
     */
#   ifndef _CRT_SECURE_NO_WARNINGS
#       define _CRT_SECURE_NO_WARNINGS 
#   endif

#   ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#       define _WINSOCK_DEPRECATED_NO_WARNINGS
#   endif

	// 包括 SDKDDKVer.h 将定义可用的最高版本的 Windows 平台。
	// 如果要为以前的 Windows 平台生成应用程序，请包括 WinSDKVer.h，并将
	// WIN32_WINNT 宏设置为要支持的平台，然后再包括 SDKDDKVer.h。
#	include <SDKDDKVer.h>

    /* there are two macro "min, max" definition in minwindef.h,
    they will cause troulbe when we call std::min() or std::max() template.
    #ifndef NOMINMAX
    #ifndef max
    #define max(a,b)            (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef min
    #define min(a,b)            (((a) < (b)) ? (a) : (b))
    #endif
    #endif  // NOMINMAX 

    to avoid trouble, we define NOMINMAX.
    */
#   ifndef NOMINMAX
#       define NOMINMAX 
#   endif

#   define  __LITTLE_ENDIAN 1234
#   define  __BIG_ENDIAN    4321
#   define  __PDP_ENDIAN    3412
#   define  __BYTE_ORDER    __LITTLE_ENDIAN

    /* refer to https://msdn.microsoft.com/en-us/library/ms737629(v=vs.85).aspx for socket header file information */
//#   ifndef WIN32_LEAN_AND_MEAN
//#   define WIN32_LEAN_AND_MEAN
//#   endif
//#   include <windows.h>

#   include <Winsock2.h>
#   include <ws2tcpip.h>
#   include <iphlpapi.h>

    /* Debug memory leack. */
#   include "crtdbg.h"
#   ifdef _DEBUG
#       define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#   else
#       define DEBUG_CLIENTBLOCK
#   endif // _DEBUG

#   define __func__ __FUNCTION__
#   include <io.h>      /* int _access(const char *path,  int mode);*/
#   define access(x, y) _access(x, y)
#endif //#ifdef _WIN32

#ifdef __linux
#   include <endian.h>
#   include <unistd.h>
#   include <syslog.h>
#   include <termios.h>
#   include <pthread.h>
#   include <sys/wait.h>   //for wait() function.
#   include <sys/resource.h>
#endif
#include <sys/stat.h>
/* for u_char, u_int ...  on windows platform,  u_char, u_int was defined
 * in <Winsock.h>.
 */
#include <sys/types.h>
#include <fcntl.h>

/*** C++ header. ***/
#if defined(__cplusplus)
#   include <cstdio>
#   include <cstdlib>
#   include <csignal>
#   include <cstddef>
#   include <cassert>
#   include <cstring>
#   include <iostream>
#   include <iomanip>
#   include <sstream>
#   include <fstream>
#   include <iterator>
#   include <list>
#   include <map>
#   include <vector>
#   include <set>
#   include <bitset>
#   include <queue>
#   include <typeinfo>
#   include <functional>
#   include <algorithm>
#   include <memory>
#   include <climits>
#   include <cerrno>
#   include <utility>
#   ifdef _WIN32
#       include <cstdint>
#       include <system_error>
#       include <thread>
#       include <mutex>
#       include <condition_variable>
#       include <chrono>
#   else
#       if GCC_VERSION > 40600
#           include <cstdint>
#           include <system_error>
#           include <thread>
#           include <mutex>
#           include <condition_variable>
#       	include <chrono>
#       else
#           include <stdint.h>
#           define nullptr NULL
#       endif
#   endif
#   define CxxBeginNameSpace(X) namespace X {
#   define CxxEndNameSpace }
#else
#   include <stddef.h>
#   include <signal.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   include <assert.h>
#   include <errno.h>
#   include <limits.h>
#   include <stdint.h>
#   ifdef __GNUC__
#       include <stdbool.h>
#   else
#       define BOOL  bool
#       define TRUE  true
#       define FALSE false
#   endif
#endif  //#if defined(__cplusplus)

#endif /* _STDAFX_H_ */
