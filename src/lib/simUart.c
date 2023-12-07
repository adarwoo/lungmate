/**
 *@ingroup lib
 *@defgroup sim PC Simulation
 *@{
 *@file
 *****************************************************************************
 * Simulates the uart API.
 * This code can be compiled by the Cygwin gcc compiler, or any gcc compiler
 * under Linux, or the Mingw32 or MSVisualC++ compiler under Windows.
 * @author software@arreckx.com
 *****************************************************************************
 */
#include <stdio.h>

// Reuse high level methods from the actual uart file
#include "uart.c"

/*
 *  Uart emulation
 */

void uartInit( uartShutdownCallback_t callback )
{
}

void uartTransmit( const uint8_t c )
{
    putc(c, stdout);
    fflush(stdout);
}

#ifdef __CYGWIN__
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

/**
 * Unix version (Cygwin friendly) of the getch function
 *
 * @return charater read from input buffer.
 */
int uartGetChar()
{
  struct termios oldt,newt;
  int ch;

  // Set the terminal mode
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );

  // Read 1 character - blocking
  ch = getchar();

  // Reset the console setup
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

  return ch;
}

/**
 * Unix version (Cygwin friendly) of the kbhit function
 *
 * @return true if a key (serial char) is ready in the buffer
 */
bool uartHasChar()
{
   fd_set stdinfd;
   struct termios oldt,newt;
   int retsize;

   // Prepare the select
   FD_ZERO( &stdinfd );
   FD_SET( STDIN_FILENO, &stdinfd );
   struct timeval tv;
   tv.tv_sec = 0;
   tv.tv_usec = 0;

   // Prepare to change stdin mode. Read 1 char at a time and no echo
   tcgetattr( STDIN_FILENO, &oldt );
   newt = oldt;
   newt.c_lflag &= ~( ICANON | ECHO );
   tcsetattr( STDIN_FILENO, TCSANOW, &newt );

   // Check if 1 char would be ready
   retsize=select( 1, &stdinfd, NULL, NULL, &tv );

   // Reset the console back
   tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

   return retsize>0;
}

#else
#include <windows.h>

/**
 * This function will simulate a getch() function for Windows Console
 *  platform.
 * Note that this does not work for special keys like F1-F12, arrow keys,
 *  even the ESC key does not seem to work with this function.
 * @return charater read from input buffer.
 */
int uartGetChar()
{
    DWORD mode;
    TCHAR theChar = 0;
    DWORD count;

    // Get a handle to stdin
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);

    // Save the current input mode...
    GetConsoleMode(hStdIn,&mode);

    // Clear the mode, turn off ENABLE_ECHO_INPUT and ENABLE_LINE_INPUT
    //  so that the output will not be echoed and will not pause until the end of
    //  a line for input.
    SetConsoleMode(hStdIn,0);

    // Read in 1 char from the input buffer.
    if ( ! ReadConsole(hStdIn,&theChar,sizeof(TCHAR),&count,NULL) )
    {
       DWORD err = GetLastError();
       printf("Error %ld\n",err);
       exit(0);
    }

    // Restore the current input mode.
    SetConsoleMode(hStdIn, mode);

    return theChar;
}

bool uartHasChar()
{
   bool retval = false;

   // Number read
   DWORD numberOfEvents=0;

   // Get a handle to stdin
   HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);

   if ( GetNumberOfConsoleInputEvents(hStdIn, &numberOfEvents ) )
   {
      if ( numberOfEvents > 0 )
      {
         retval = true;
      }
   }

   return retval;
}

#endif // def __CYGWIN__

