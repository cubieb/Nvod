cd /D %~dp0%

:: compile TsSvcId.h
odb.exe --std c++11 --database mysql --generate-query --generate-schema --cxx-suffix .cpp --hxx-suffix .h --sql-suffix -odb.sql TsSvcId.h

:: compile ReferenceService.h
odb.exe --std c++11 --database mysql --generate-query --generate-schema  --generate-session --cxx-suffix .cpp --hxx-suffix .h --sql-suffix -odb.sql ReferenceService.h

:: compile TimeShiftedService.h
odb.exe --std c++11 --database mysql --generate-query --generate-schema  --generate-session --cxx-suffix .cpp --hxx-suffix .h --sql-suffix -odb.sql TimeShiftedService.h

pause