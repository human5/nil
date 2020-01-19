/***************************************************************************
  win32_system.cpp  -  win32 helper funcions
  --------------------------------------------------------------------------
  begin                : ????-??-??
  by                   : ???
  email                : ???

  last changed         : 2004-01-25
  by                   : Christoph Brill (egore)
  email                : egore@gmx.de

  copyright            : (C) 2004 by Christoph Brill (egore)
  email                : egore@gmx.de

 ***************************************************************************/

#ifdef _WIN32
// only when compiled under windows

#include "fluff.h"
#include <stdexcept>

namespace
{
	HANDLE event_done;

	class Init_winsock
	{
	public:
		Init_winsock()
		{
			WORD wVersionRequested;
			WSADATA wsaData;
			int err;
 
			wVersionRequested = MAKEWORD( 1, 1 );
 
			err = WSAStartup( wVersionRequested, &wsaData );
			if ( err != 0 ) {
				throw std::runtime_error("Could not initialize winsock.");
			}
 
			/* Confirm that the WinSock DLL supports 2.0.*/
			/* Note that if the DLL supports versions greater    */
			/* than 2.0 in addition to 2.0, it will still return */
			/* 2.0 in wVersion since that is the version we      */
			/* requested.                                        */
 
			if ( LOBYTE( wsaData.wVersion ) != 1 ||
					HIBYTE( wsaData.wVersion ) != 1 ) {
				/* Tell the user that we couldn't find a usable */
				/* WinSock DLL.                                  */
				WSACleanup( );
				throw std::runtime_error("Winsock 1.1 or greater required.");
			}
		}

		~Init_winsock() {	WSACleanup(); }
	};

	class Init_console
	{
	public:
		Init_console()
		{
			int hCrtIn, hCrtOut, hCrtErr;
			FILE *fIn, *fOut, *fErr;

			AllocConsole();
			
			// Obtain console I/O handles
			hCrtIn	= _open_osfhandle( (long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
			hCrtOut	= _open_osfhandle( (long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
			hCrtErr	= _open_osfhandle( (long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
			
			// Associate handles to standard I/O streams
			fIn			= _fdopen(hCrtIn, "r");
			fOut		= _fdopen(hCrtOut, "w");
			fErr		= _fdopen(hCrtErr, "w");
			*stdin	= *fIn;
			*stdout	= *fOut;
			*stderr	= *fErr;

			// Modify stream buffering
			setvbuf( stdin, NULL, _IONBF, 0 );
			setvbuf( stdout, NULL, _IONBF, 0 );
			setvbuf( stderr, NULL, _IONBF, 0 );

			SetConsoleCtrlHandler( NULL, TRUE );
			SetConsoleCtrlHandler( CtrlHandler, TRUE );
		}

		~Init_console() 
		{ 
			FreeConsole(); 
		}

	private:
		static BOOL __stdcall CtrlHandler( DWORD dwCtrlType )
		{
			request_quit = true;
			WaitForSingleObject(event_done, 5000);
			ExitProcess(0);
			return TRUE;
		}

	};
}

/* own implementation of strsep function */
char *
strsep (char **stringp, const char *delim)
{
  char *begin, *end;

  begin = *stringp;
  if (begin == NULL)
    return NULL;

  /* A frequent case is when the delimiter string contains only one
     character.  Here we don't need to call the expensive `strpbrk'
     function and instead work using `strchr'.  */
  if (delim[0] == '\0' || delim[1] == '\0')
    {
      char ch = delim[0];

      if (ch == '\0')
	end = NULL;
      else
	{
	  if (*begin == ch)
	    end = begin;
	  else
	    end = strchr (begin + 1, ch);
	}
    }
  else
    /* Find the end of the token.  */
    end = strpbrk (begin, delim);

  if (end)
    {
      /* Terminate the token and set *STRINGP past NUL character.  */
      *end++ = '\0';
      *stringp = end;
    }
  else
    /* No more delimiters; this is the last token.  */
    *stringp = NULL;

  return begin;
}



// the main function from nil_main.cpp
int nil_main(int argc,char **argv);

// Main is redirected here, to initialize win32 specific code.
//extern "C" int SDL_main(int argc, char *argv[])
int win32_main(int argc, char *argv[])
{
	event_done = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (event_done == NULL)
		return 1;

	Init_winsock iw;	// Initialize winsock
	Init_console im;	// Bring up a console

	int n = nil_main(argc,argv);

	SetEvent(event_done);

	return n;
}

// only when compiled under windows
#endif



