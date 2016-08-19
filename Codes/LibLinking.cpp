#ifdef _WIN32
#	pragma comment(lib, "odb-mysql-d.lib")
#	pragma comment(lib, "odb-d.lib")
 
#	ifdef _DEBUG
#		pragma comment(lib, "ACEd.lib")
#	else
#		pragma comment(lib, "ACE.lib")
#	endif
#endif