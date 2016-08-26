#ifdef _WIN32

#	ifdef _DEBUG
#	    pragma comment(lib, "odb-mysql-vc12-d.lib")
#	    pragma comment(lib, "odb-vc12-d.lib")
#else
#	    pragma comment(lib, "odb-mysql-vc12.lib")
#	    pragma comment(lib, "odb-vc12.lib")
#endif

#	ifdef _DEBUG
#		pragma comment(lib, "ACEd.lib")
#	else
#		pragma comment(lib, "ACE.lib")
#	endif
#endif