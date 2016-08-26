cd /D %~dp0%

:: compile TsSvcId.h
odb.exe --std c++11 --database mysql --generate-query --generate-schema --cxx-suffix .cpp --hxx-suffix .h --sql-suffix -odb.sql TsSvcId.h

:: compile NvodService.h
odb.exe --std c++11 --database mysql --generate-query --generate-schema  --generate-session --cxx-suffix .cpp --hxx-suffix .h --sql-suffix -odb.sql NvodService.h

pause