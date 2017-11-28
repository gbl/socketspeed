all:	socketsend socketslurp socketslurp.exe socketsend.exe

socketsend:	socketsend.c

socketslurp:	socketslurp.c


socketslurp.exe:	socketslurp.c hstrerror.c
	/usr/bin/x86_64-w64-mingw32-gcc-win32 -o socketslurp.exe socketslurp.c hstrerror.c -lws2_32

socketsend.exe:		socketsend.c hstrerror.c
	/usr/bin/x86_64-w64-mingw32-gcc-win32 -o socketsend.exe socketsend.c hstrerror.c -lws2_32
