/* progress.c - Øystein Schønning-Johansen 2013 - 2024 */
/* 
 vim: ts=4 sw=4 softtabstop=4 expandtab 
*/
#include "progress.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h> 
#include <math.h> 
#include <time.h> 
#if __WIN32
#include <windows.h>
#define EXTRA_WIDTH 27
#define isatty _isatty
#else /* (Linux?) */
#include <sys/ioctl.h>
#define EXTRA_WIDTH 26
#endif

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

char *partial_blocks[] = {
    "",
    "\u258F", /*  ▏ LEFT ONE EIGHTH BLOCK */
    "\u258E", /*  ▎ LEFT ONE QUARTER BLOCK */
    "\u258D", /*  ▍ LEFT THREE EIGHTHS BLOCK */
    "\u258C", /*  ▌ LEFT HALF BLOCK */
    "\u258B", /*  ▋ LEFT FIVE EIGHTHS BLOCK */
    "\u258A", /*  ▊ LEFT THREE QUARTERS BLOCK */
    "\u2589", /*  ▉ LEFT SEVEN EIGHTHS BLOCK */
    "\u2588", /*  full block  */
};

/* gets the current screen column width */
static unsigned short getcols(int fd)
{
	const unsigned short default_tty = 80;
	unsigned short termwidth = 0;
	const unsigned short default_notty = 0;
	if(!isatty(fd)) {
		return default_notty;
	}
#if __WIN32
    HANDLE console;
    CONSOLE_SCREEN_BUFFER_INFO info;
    /* Create a handle to the console screen. */
    console = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
            0, NULL);
    if (console == INVALID_HANDLE_VALUE)
        return default_tty;

    /* Calculate the size of the console window. */
    if (GetConsoleScreenBufferInfo(console, &info) == 0)
        return default_tty;
    CloseHandle(console);
    termwidth = info.srWindow.Right - info.srWindow.Left + 1;
#else  /* Not Windows */

#if defined(TIOCGSIZE)
	struct ttysize win;
	if(ioctl(fd, TIOCGSIZE, &win) == 0) {
		termwidth = win.ts_cols;
	}
#elif defined(TIOCGWINSZ)
	struct winsize win;
	if(ioctl(fd, TIOCGWINSZ, &win) == 0) {
		termwidth = win.ws_col;
	}
#endif
#endif  /* Not Windows */
	return termwidth == 0 ? default_tty : termwidth;
}

static void write_eta( char *buffer, const char *label, int value  )
{
    if ( value < 3600 ){
        sprintf( buffer, "%s: %02d:%02d (mm:ss)", label, value/60, value % 60);
        return;
    }
    /* FIXME - It can be longer. It will be bad when it's more than 99 hours */
    value /= 60;
    sprintf( buffer, "%s: %02d:%02d (hh:mm)", label, value/60, value % 60);
}

void progress_ascii( int x, int n, const char *fmt, ... )
{
    va_list ap1, ap2;
    int len;

    va_start( ap1, fmt );
    va_copy( ap2, ap1 );
    len = vsnprintf( NULL, 0, fmt, ap1 );
    va_end( ap1 );

    char label[len+1];
    vsprintf( label, fmt, ap2 );
    va_end( ap2 );

    /* eta: --:-- (hh:mm) */
    static time_t start_time;
    char etabuffer[40] = { '\0' };
    if ( x == 0){
        time(&start_time);
    }

	int w = getcols( STDOUT_FILENO ) - EXTRA_WIDTH - len - strlen(etabuffer);
	if( w < 1 ) return;

	// Calculuate the ratio of complete-to-incomplete.
	double ratio = x/(double)n;
	int   c      = ratio * w;
    int partial  = (int) nearbyint( ((ratio*w)-c) * 8);

    time_t now;
    time(&now);
    int elapsed = (int)(now-start_time);
    if ( x == 0){
        sprintf( etabuffer, "eta: --:-- (mm:ss)" );
    } else if ( x == n ){
        write_eta(etabuffer, " in", elapsed );
    } else {
        int eta = elapsed * ( n - x ) / x;
        write_eta(etabuffer, "eta", eta );
    }

    /* The print job itself */
	printf("%s|", label );
	for (int i=0; i<c; i++)
        printf("\x1B[38;2;0;%d;0m\x1B[48;2;100;100;100m\u2588", (int) (100 + (100*((float)i/(float)w))) );
    printf("%s", partial_blocks[partial]);
    printf(KNRM);
    printf("\x1B[38;2;100;100;100m");
    for (int i=c+(!!partial); i<w; i++) printf("\u2588");

	// ANSI Control codes to go back to the
	// previous line and clear it.
    printf(KNRM);
	printf("| %3d%% %s", (int)(ratio*100), etabuffer);
	printf( x == n ? "\n" : "\r");
	fflush( stdout );
}

#include <stdlib.h>
int main( int argc, char *argv[] )
{
    int maxvalue = 100;
    if(  argc > 1 )
        maxvalue = atoi( argv[1]);

    for ( int i = 0; i <= maxvalue; i++ ){
        progress_ascii( i, maxvalue, "(%4d/%4d) ", i, maxvalue);
        usleep(50000);
    }

    return 0;
}

