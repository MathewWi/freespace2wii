
/*
 * $Logfile: $
 * $Revision: 2.32.2.5 $
 * $Date: 2007-10-28 16:46:36 $
 * $Author: taylor $
 *
 * OS-dependent functions.
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.32.2.4  2007/10/17 21:03:06  taylor
 * change Warning() and Error() to use const format variables (can't remember who said to do this)
 * fix memory error when zero'ing buffers for error messages on non-Windows platforms
 *
 * Revision 2.32.2.3  2007/02/12 00:23:40  taylor
 * get rid of non-standard itoa(), make use of the proper sprintf() instead
 *
 * Revision 2.32.2.2  2006/12/07 18:26:48  taylor
 * fix for new GCC versions (Fedora Core 6)
 *
 * Revision 2.32.2.1  2006/09/08 06:14:44  taylor
 * fix things that strict compiling balked at (from compiling with -ansi and -pedantic)
 *
 * Revision 2.32  2006/05/27 16:39:40  taylor
 * remove non-Windows timeSetEvent() functions
 * make PDWORD and LPDWORD int instead of long (64-bit issue) (thanks Spike)
 * fix 64-bit ptr issue with OpenAL audio streaming code (thanks Spike)
 * add start of device enumberation for OpenAL 1.1 (it just lists available devices now)
 * make sure we open the default OpenAL 1.1 device under Windows
 * comment out blank listener stuff, this may have been crashing under OS X
 * add support for AL_BYTE_OFFSET in OpenAL 1.1 for buffer positioning under Windows
 *
 * Revision 2.31  2006/04/16 05:24:16  taylor
 * probably doesn't need to be world readable, but certainly doesn't need to be world writeable
 * add Assert() to can when we are stupidly trying to allocate 0 or less size bytes in vm_malloc()
 *
 * Revision 2.30  2006/03/15 17:32:13  taylor
 * bump error msg buffer size a little (I'll probably just go dynamic with this since you hopefully won't get this too much anyway)
 *
 * Revision 2.29  2006/03/08 05:22:14  taylor
 * remove asm from MulDiv() since Intel Mac doesn't like it very much, and it doesn't do exactly what it should anyway
 *
 * Revision 2.28  2006/01/31 15:44:55  taylor
 * remove USE_LUA
 *
 * Revision 2.27  2006/01/21 00:11:51  taylor
 * fix for LuaError change, *again*  ;)
 *
 * Revision 2.26  2006/01/13 14:19:34  taylor
 * move SDL autoconf checks up so we get the defines earlier
 * fix some Lua build problems on Linux (enabled by default now, but with configure option)
 * newline fixage
 * comment fixage
 *
 * Revision 2.25  2005/10/27 16:23:03  taylor
 * go back to using abort() over exit() for Assert's and Int3's but close out SDL first this time, makes debugging much easier
 *
 * Revision 2.24  2005/10/17 05:48:18  taylor
 * dynamically allocate object collision pairs
 *
 * Revision 2.23  2005/10/11 08:30:38  taylor
 * fix memory freakage from dynamic spawn weapon types
 *
 * Revision 2.22  2005/09/22 11:21:22  taylor
 * delay for fox OS X, SDL messes up for some reason and though I don't like this, even Apple does it
 *
 * Revision 2.21  2005/09/14 20:38:12  taylor
 * some vm_* fixage for Windows
 *
 * Revision 2.20  2005/09/06 05:32:12  taylor
 * use exit(EXIT_FAILURE) rather than abort() so that all atexit() calls will actually get executed
 *
 * Revision 2.19  2005/09/05 09:38:19  taylor
 * merge of OSX tree
 * a lot of byte swaps were still missing, will hopefully be fully network compatible now
 *
 * Revision 2.18  2005/08/14 21:01:59  Kazan
 * I'm stupid, sorry - fixed release-build-crash
 *
 * Revision 2.17  2005/06/22 15:18:58  taylor
 * make sure that _mkdir() will return an error if the path is invalid
 *
 * Revision 2.16  2005/05/28 19:44:17  taylor
 * haven't got a clue what I was thinking with that crap
 *
 * Revision 2.15  2005/05/24 20:52:10  taylor
 * make sure that we recursively create directories for Linux/OSX
 * replace older recursive mkdir method in cfileextractor with new method
 *
 * Revision 2.14  2005/05/12 17:49:18  taylor
 * use vm_malloc(), vm_free(), vm_realloc(), vm_strdup() rather than system named macros
 *   fixes various problems and is past time to make the switch
 *
 * Revision 2.13  2005/04/21 15:52:24  taylor
 * stupid little comment fix, no big deal
 *
 * Revision 2.12  2005/04/11 05:45:38  taylor
 * _endthread() doesn't take an argument so do support one (Jens Granseuer)
 * debug variable fixes in freespace.cpp (Jens Granseuer)
 * as a safety catch we should Assert() on future usage of things we don't yet support in *nix _splitpath()
 *
 * Revision 2.11  2005/03/27 08:51:25  taylor
 * this is what coding on an empty stomach will get you
 *
 * Revision 2.10  2005/03/27 06:17:54  taylor
 * fill in some stuff for timer and mmio calls, not 100% but it works ok
 *
 * Revision 2.9  2005/03/24 23:22:59  taylor
 * make sure we will get the right error message
 *
 * Revision 2.8  2005/03/10 08:00:18  taylor
 * change min/max to MIN/MAX to fix GCC problems
 * add lab stuff to Makefile
 * build unbreakage for everything that's not MSVC++ 6
 * lots of warning fixes
 * fix OpenGL rendering problem with ship insignias
 * no Warnings() in non-debug mode for Linux (like Windows)
 * some campaign savefile fixage to stop reverting everyones data
 *
 * Revision 2.7  2005/02/04 10:12:33  taylor
 * merge with Linux/OSX tree - p0204
 *
 * Revision 2.6  2005/01/30 18:32:42  taylor
 * merge with Linux/OSX tree - p0130
 * remove extra brace in cfile.cpp
 *
 *
 * $NoKeywords: $
 */

#ifdef SCP_UNIX

#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/time.h>
#include "SDL.h"

#if defined(APPLE_APP)
#include <CoreFoundation/CoreFoundation.h>
#endif

#if defined(HAVE_MALLOC_H)
#include <malloc.h>
#endif

#include "globalincs/pstypes.h"

#include "parse/lua.h"
#include "cmdline/cmdline.h"

bool env_enabled = false;
bool cell_enabled = false;

#define MAX_BUF_SIZE	1024
static char buffer[MAX_BUF_SIZE], buffer_tmp[MAX_BUF_SIZE];


char *strnset( char* string, int fill, size_t count)
{
	char *p = string;

 	for(; *p; p++ ) {
		if( count == 0 )
			break;

 		*p = (char)fill;
		count--;
 	}

	return string;
}

// find the size of a file
int filelength(int fd)
{
	struct stat buf;

	if (fstat (fd, &buf) == -1)
		return -1;

	return buf.st_size;
}

// non-blocking process pause
void Sleep(int mili)
{
#ifdef __APPLE__
	// ewwww, I hate this!!  SDL_Delay() is causing issues for us though and this
	// basically matches Apple examples of the same thing.  Same as SDL_Delay() but
	// we aren't hitting up the system for anything during the process
	uint then = SDL_GetTicks() + mili;

	while (then > SDL_GetTicks());
#else
	SDL_Delay(mili);
#endif
}

#include <memtracer.h>
#include <wiitrace.h>
extern void os_deinit();
// fatal assertion error
void WinAssert(char * text, char *filename, int line)
{
	fprintf(stderr, "ASSERTION FAILED: \"%s\" at %s:%d\n", text, filename, line);

	// this stuff migt be really useful for solving bug reports and user errors. We should output it! 
	mprintf(("ASSERTION: \"%s\" at %s:%d\n", text, strrchr(filename, '/')+1, line ));

	if (Cmdline_nowarn) {
		return;
	}	fflush(stdout);
	fflush(stderr);
	closeProfiler();
	closeMemtrace();

	// we have to call os_deinit() before abort() so we make sure that SDL gets
	// closed out and we don't lose video/input control
	os_deinit();

	abort();
}


extern "C" void wiipause();

// standard warning message
void Warning( const char * filename, int line, const char * format, ... )
{
#ifndef NDEBUG
	va_list args;
	int i;
	int slen = 0;

	memset( buffer, 0, sizeof(buffer) );
	memset( buffer_tmp, 0, sizeof(buffer_tmp) );

	va_start(args, format);
	vsnprintf(buffer_tmp, sizeof(buffer_tmp) - 1, format, args);
	va_end(args);

	slen = strlen(buffer_tmp);

	// strip out the newline char so the output looks better
	for (i = 0; i < slen; i++){
		if (buffer_tmp[i] == (char)0x0a) {
			buffer[i] = ' ';
		} else {
			buffer[i] = buffer_tmp[i];
		}
	}

	// kill off extra white space at end
	if (buffer[slen-1] == (char)0x20) {
		buffer[slen-1] = '\0';
	} else {
		// just being careful
		buffer[slen] = '\0';
	}

	mprintf(("WARNING: \"%s\" at %s:%d\n", buffer, strrchr(filename, '/')+1, line));
#endif
}

#ifdef SCP_WII

extern "C" void printTextureStats();

#include <ogc/system.h>

#define AssertLoc(x,a,b) do { if (!(x)){ WiiAssert(#x,a,b); } } while (0)
#include <memtracer.h>

#endif


// fatal error message
void Error( const char * filename, int line, const char * format, ... )
{
	va_list args;
	int i;
	int slen = 0;

	memset( buffer, 0, sizeof(buffer) );
	memset( buffer_tmp, 0, sizeof(buffer_tmp) );

	va_start(args, format);
	vsnprintf(buffer_tmp, sizeof(buffer_tmp) - 1, format, args);
	va_end(args);

	mprintf(("ERROR: %s\nFile: %s\nLine: %d\n", buffer_tmp, filename, line));

#if defined(APPLE_APP)
	CFStringRef AsMessage;
	char AsText[1024];
	CFOptionFlags result;

	snprintf(AsText, 1024, "Error: %s\n\nFile: %s\nLine %d\n", buffer_tmp, filename, line);
	AsMessage = CFStringCreateWithCString(NULL, AsText, kCFStringEncodingASCII);

	CFUserNotificationDisplayAlert(0, kCFUserNotificationStopAlertLevel, NULL, NULL, NULL, CFSTR("Error!"), AsMessage, CFSTR("Exit"), NULL, NULL, &result);
#else
	slen = strlen(buffer_tmp);

	// strip out the newline char so the output looks better
	for (i = 0; i < slen; i++){
		if (buffer_tmp[i] == (char)0x0a) {
			buffer[i] = ' ';
		} else {
			buffer[i] = buffer_tmp[i];
		}
	}

	// kill off extra white space at end
	if (buffer[slen-1] == (char)0x20) {
		buffer[slen-1] = '\0';
	} else {
		// just being careful
		buffer[slen] = '\0';
	}

	// Order UP!!
	fprintf(stderr, "ERROR: \"%s\" at %s:%d\n", buffer, filename, line);
	
	fflush(stdout);
	fflush(stderr);
	printTextureStats();
	closeProfiler();
	closeMemtrace();
	wiipause();	
	
	u32 *death = (u32 *) 0x1;
	*death = 0xDEADBEEF;
#endif
	exit(EXIT_FAILURE);
}

extern lua_Debug Ade_debug_info;
void LuaError(struct lua_State *L, char *format, ...)
{
	va_list args;
	memset( &buffer, 0, sizeof(buffer) );

	if (format == NULL) {
		// make sure to cap to a sane string size
		snprintf( buffer, sizeof(buffer) - 1, "%s", lua_tostring(L, -1) );
		lua_pop(L, -1);
	} else {
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer) - 1, format, args);
		va_end(args);
	}

	// Order UP!!
	fprintf(stderr, "LUA ERROR: \"%s\"\n", buffer);
	fprintf(stderr, "\n");

	fprintf(stderr, "------------------------------------------------------------------\n");
	fprintf(stderr, "ADE Debug:\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Name:  %s\n",  Ade_debug_info.name);
	fprintf(stderr, "Name of:  %s\n",  Ade_debug_info.namewhat);
	fprintf(stderr, "Function type:  %s\n",  Ade_debug_info.what);
	fprintf(stderr, "Defined on:  %d\n",  Ade_debug_info.linedefined);
	fprintf(stderr, "Upvalues:  %d\n",  Ade_debug_info.nups);
	fprintf(stderr, "\n" );
	fprintf(stderr, "Source:  %s\n",  Ade_debug_info.source);
	fprintf(stderr, "Short source:  %s\n",  Ade_debug_info.short_src);
	fprintf(stderr, "Current line:  %d\n",  Ade_debug_info.currentline);
	
	fprintf(stderr, "------------------------------------------------------------------\n");
	fprintf(stderr, "LUA Stack:\n");
	fprintf(stderr, "\n");
	ade_stackdump(L, buffer);
	fprintf(stderr, "%s\n", buffer);
	fprintf(stderr, "\n");
	
	wiipause();

	exit(EXIT_FAILURE);
}

#ifdef SCP_WII

HMMIO mmioOpen(LPSTR szFilename, LPMMIOINFO lpmmioinfo, DWORD dwOpenFlags)
{
	SDL_RWops *handle = NULL;

	char *mode = "rb";

	if (dwOpenFlags & MMIO_READ)
		mode = "rb";
	else if (dwOpenFlags & MMIO_READWRITE)
		mode = "r+b";
	else if (dwOpenFlags & MMIO_WRITE)
		mode = "wb";

	if ( szFilename != NULL ) {
		Assert( lpmmioinfo == NULL );

		handle = SDL_RWFromFile( szFilename, mode );
	} else if ( lpmmioinfo != NULL ) {
		Assert( szFilename == NULL );

		handle = SDL_RWFromMem( lpmmioinfo->pchBuffer, lpmmioinfo->cchBuffer );
	}

	return handle;
}

long mmioSeek(HMMIO hmmio, long lOffset, int iOrigin)
{
	return (long) SDL_RWseek( hmmio, lOffset, iOrigin );
}

long mmioRead(HMMIO hmmio, HPSTR pch, long cch)
{
	return (long) SDL_RWread( hmmio, pch, 1, cch );
}

MMRESULT mmioClose(HMMIO hmmio, uint wFlags)
{
	if (wFlags != 0)
		STUB_FUNCTION;

	int rc = 0;

	rc = SDL_RWclose( hmmio );

	if (rc)
		return MMIOERR_CANNOTWRITE;

	return 0;
}

#endif


// get a filename minus any leading path
const char *clean_filename(const char *name)
{
	const char *p = name + strlen(name)-1;

	// Move p to point to first letter of EXE filename
	while( (p > name) && (*p != '\\') && (*p != '/') && (*p != ':') )
		p--;

	p++;	

	return p;	
}

// high precision timer
bool QueryPerformanceCounter(LARGE_INTEGER *pcount)
{
	struct timeval timer_now;

	gettimeofday(&timer_now, NULL);

	pcount->QuadPart = (longlong)timer_now.tv_usec;
	
	return 1;
}

#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
int TotalRam = 0;
#endif

int Watch_malloc = 0;
DCF_BOOL(watch_malloc, Watch_malloc )


#ifndef NDEBUG
void windebug_memwatch_init()
{
	//_CrtSetAllocHook(MyAllocHook);
	TotalRam = 0;
}
#endif

// retrieve the current working directory
int _getcwd(char *out_buf, unsigned int len)
{
	if (getcwd(out_buf, len) == NULL) {
		Error(__FILE__, __LINE__, "buffer overflow in getcwd (buf size = %u)", len);
	}

	return 1;
}

#ifdef SCP_WII
#include <sys/dir.h>
#endif

// change directory to specified path
int _chdir(const char *path)
{
	int status = chdir(path);

#ifndef NDEBUG
	int m_error = errno;

	if (status) {
		Warning(__FILE__, __LINE__, "Cannot chdir to %s: %s", path, strerror(m_error));
	}
#endif

#ifdef SCP_WII
	if(!status)
	{
		struct stat st;
		status = stat(path, &st);
		
		if(!status)
		{
			if(!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
			{
				status = -1;
			}			
		}		
	}
#endif

	return status;
}

// make specified directory
int _mkdir(const char *path)
{
	int status = 1;		// if we don't ever call mkdir() to update this then assume we are in error
	char *c, tmp_path[MAX_PATH];

	memset(tmp_path, 0, MAX_PATH);
	strncpy(tmp_path, path, MAX_PATH-1);

	c = &tmp_path[1];

	while (c++) {
		c = strchr(c, '/');

		if (c) {
			*c = '\0';

			status = mkdir(tmp_path, 0755);

#ifndef NDEBUG
			int m_error = errno;

			if (status && (m_error != EEXIST) ) {
				Warning(__FILE__, __LINE__, "Cannot mkdir %s: %s", tmp_path, strerror(m_error));
			}
#endif
			*c = '/';
		}
	}

	return status;
}

void _splitpath (char *path, char *drive, char *dir, char *fname, char *ext)
{
	if ( (path == NULL) || (fname == NULL) )
		return;

	// stop at these in case they ever get used, we need to support them at that point
	Assert( (dir == NULL) && (ext == NULL) );

	/* fs2 only uses fname */
	if (fname != NULL) {
		const char *ls = strrchr(path, '/');

		if (ls != NULL) {
			ls++;		// move past '/'
		} else {
			ls = path;
		}

		const char *lp = strrchr(path, '.');

		if (lp == NULL) {
			lp = ls + strlen(ls);	// move to the end
		}

		int dist = lp-ls;

		if (dist > (_MAX_FNAME-1))
			dist = _MAX_FNAME-1;

		strncpy(fname, ls, dist);
		fname[dist] = 0;	// add null, just in case
	}
}

extern "C" void wiipause();

// some type of info message
int MessageBox(HWND h, const char *s1, const char *s2, int i)
{
	if ( (h != NULL) && (i > -1) ) {
		// placeholder for some future time
	}

	fprintf(stderr, "%s: \"%s\"\n", s2, s1);
	wiipause();

	return 0;
}


int MulDiv(int number, int numerator, int denominator)
{
	int result;

	if (denominator == 0)
		return 0;

	longlong tmp;
	tmp = ((longlong) number) * ((longlong) numerator);
	tmp /= (longlong) denominator;
	result = (int) tmp;

	return result;
}

/* *************************************
 *
 * memory handling functions
 *
 * *************************************/

// RamTable stuff comes out of icculus.org
#ifndef NDEBUG
typedef struct RAM {
	ptr_u addr;
	int size;

	RAM *next;
} RAM;

static RAM *RamTable;
#endif

int vm_init(int min_heap_size)
{
#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
	TotalRam = 0;
#endif

	return 1;
}

struct point
{
	size_t size;
	size_t start;
};

#ifndef NDEBUG
#define MEMCHECK_SIZE 10
#else
#define MEMCHECK_SIZE 0
#endif

#ifndef NDEBUG

void setDEADBEAF( void *ptr, size_t size )
{
	if(MEMCHECK_SIZE == 0) return;
	
	u32 *p = (u32*)ptr;
	
	int i = 0;
	int count = MEMCHECK_SIZE;
	
	if(((u32)ptr) % 4 != 0)
	{
		u8 *next_memory = (u8*)ptr;
		next_memory += 4 - (((u32)next_memory) % 4);
		--count;
	}
		
	for(i = 0; i < count; i++)
	{
		p[i] = 0xDEADBEEF;
	}
	
	u8 * p8 = (u8*)ptr;
	p8 += size-4*MEMCHECK_SIZE;
	
	count = MEMCHECK_SIZE;
	if(((u32)p8) % 4 != 0)
	{
		p8 += 4 - (((u32)p8) % 4);
		--count;
	}
	
	p = (u32*) p8;
	
	for(i = 0; i < count; ++i)
	{		
		p[i] = 0xDEADBEEF;
	}
}

void offset32(void **ret_ptr, size_t nInts)
{
	u32 * p = (u32*)*ret_ptr;
	*ret_ptr = (void*)(p+nInts);
}

void checkDEADBEAF(void *ptr, const char *filename, int line, const char *func)
{
	if(MEMCHECK_SIZE == 0) return;
	
	if(ptr == NULL)
	{
		printf("Freeing a null pointer! %s - %d\n", filename, line);
		wiipause();
		return;
	}
	
	offset32(&ptr,-MEMCHECK_SIZE);
	
	RAM *item = RamTable;

	while (item != NULL) {
		if (item->addr == (ptr_u)ptr) {
			break;
		}
		item = item->next;
    }
	
	if(item == NULL)
	{
		printf("Pointer not found in RamTable! %s - %d\n", filename, line);
		wiipause();
		return;
	}
	

	u32 *p = (u32*)ptr;
	
	int i = 0;
	int count = MEMCHECK_SIZE;
	bool deadbeef = true;
	
	if(((u32)ptr) % 4 != 0)
	{
		u8 *next_memory = (u8*)ptr;
		next_memory += 4 - (((u32)next_memory) % 4);
		--count;
	}
		
	for(i = 0; i < count; i++)
	{
		if(p[i] != 0xDEADBEEF)
		{
			printf("Front DEADBEEF %d missing!\n", i);
			deadbeef = false;
		}
	}
	u8 * p8 = (u8*)ptr;
	p8 += item->size-4*MEMCHECK_SIZE;
	
	count = MEMCHECK_SIZE;
	if(((u32)p8) % 4 != 0)
	{
		p8 += 4 - (((u32)p8) % 4);
		--count;
	}
	
	p = (u32*) p8;
	
	for(i = 0; i < count; ++i)
	{		
		if(p[i] != 0xDEADBEEF)
		{
			printf("Back DEADBEEF %d missing!\n", i);
			deadbeef = false;
		}
	}
	
	if(!deadbeef)
	{
		printf("DEADBEEF failure checked at %s - %d, from %s\n", filename, line, func);
		wiipause();
	}
}
#endif


#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
void *_vm_malloc( int size, const char *filename, int line, int quiet )
#else
void *_vm_malloc( int size, int quiet )
#endif
{
#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
	AssertLoc( size >= 0 , filename, line);
#else
	Assert( size >= 0 );
#endif

	size += 2*4*MEMCHECK_SIZE;
#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
	void *ptr = memtrace_malloc(filename, line, size);
#else
	void *ptr = malloc( size );
#endif

	if (!ptr)	{
		if (quiet) {
			return NULL;
		}
		
#ifdef SCP_WII
		malloc_stats();
		printTextureStats();
		printf("MEM1 %u MEM2 %u\n", (u32)SYS_GetArena1Hi()-(u32)SYS_GetArena1Lo(), (u32)SYS_GetArena2Hi()-(u32)SYS_GetArena2Lo());
#endif
#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
		Error(filename, line, "Out of memory, %d, total alloc %d.", size,TotalRam);
#else
		Error(LOCATION, "Out of memory, %d", size);
#endif
	}

#ifndef NDEBUG
	if ( Watch_malloc )	{
		mprintf(( "Malloc %d bytes [%s(%d)]\n", size, clean_filename(filename), line ));
	}

	RAM *next = (RAM *)malloc(sizeof(RAM));

	next->addr = (ptr_u)ptr;
	next->size = size;

	next->next = RamTable;
	RamTable = next;

	TotalRam += size + sizeof(RAM);
	setDEADBEAF( ptr, size );
	offset32(&ptr,+MEMCHECK_SIZE);
	checkDEADBEAF(ptr, filename, line, "malloc");
#endif
	return ptr;
}

#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
void *_vm_realloc( void *ptr, int size, const char *filename, int line, int quiet )
#else
void *_vm_realloc( void *ptr, int size, int quiet )
#endif
{
	if (ptr == NULL)
		return vm_malloc(size);
#ifndef NDEBUG
	offset32(&ptr,-MEMCHECK_SIZE);
	
	if(size != 0) size += 2*4*MEMCHECK_SIZE;
#endif


#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
	void *ret_ptr = memtrace_realloc(filename, line, ptr, size );
#else
	void *ret_ptr = realloc(ptr, size );
#endif

	if (!ret_ptr)	{
		if (quiet && (size > 0) && (ptr != NULL)) {
			// realloc doesn't touch the original ptr in the case of failure so we could still use it
			return NULL;
		}

		Error(LOCATION, "Out of memory.");
	}

#ifndef NDEBUG
	RAM *item = RamTable;

	while (item != NULL) {
		if (item->addr == (ptr_u)ptr) {
			item->addr = (ptr_u) ret_ptr;
			TotalRam += (size - item->size);
			item->size = size;
			break;
		}
		item = item->next;
    }
	
	setDEADBEAF(ret_ptr, size );
	offset32(&ret_ptr,+MEMCHECK_SIZE);
	checkDEADBEAF(ret_ptr, filename, line, "realloc");
#endif

	return ret_ptr;
}

#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
char *_vm_strdup( const char *ptr, const char *filename, int line )
#else
char *_vm_strdup( const char *ptr )
#endif
{
	char *dst;
	int len = strlen(ptr);

#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
	dst = (char *)_vm_malloc( len+1 , filename, line, 0);
#else
	dst = (char *)vm_malloc( len+1 );
#endif

	if (!dst)
		return NULL;

	strncpy( dst, ptr, len + 1);

	return dst;
}

#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
char *_vm_strndup( const char *ptr, int size, const char *filename, int line )
#else
char *_vm_strndup( const char *ptr, int size )
#endif
{
	char *dst;
	
#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
	dst = (char *)_vm_malloc( size+1 , filename, line, 0);
#else
	dst = (char *)vm_malloc( size+1 );
#endif


	if (!dst)
		return NULL;

	strncpy( dst, ptr, size );
	// make sure it has a NULL terminiator
	dst[size] = '\0';

	return dst;
}

#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
void _vm_free( void *ptr, const char *filename, int line )
#else
void _vm_free( void *ptr )
#endif
{

#ifndef NDEBUG
	checkDEADBEAF(ptr, filename, line, "free");
	offset32(&ptr,-MEMCHECK_SIZE);
#endif
	
	if ( !ptr ) {
#ifndef NDEBUG
		mprintf(("Why are you trying to free a NULL pointer?  [%s(%d)]\n", clean_filename(filename), line));
#else
		mprintf(("Why are you trying to free a NULL pointer?\n"));
#endif
		return;
	}

#ifndef NDEBUG
	RAM *item = RamTable;
    RAM **mark = &RamTable;

	while (item != NULL) {
		if (item->addr == (ptr_u)ptr) {
			RAM *tmp = item;

			*mark = item->next;

			TotalRam -= tmp->size;

			free(tmp);

			break;
		}

		mark = &(item->next);

		item = item->next;
    }
#endif // !NDEBUG
#if !defined(NDEBUG) || defined(DEBUG_MALLOC)
	memtrace_free(filename, line, ptr);
#else
	free(ptr);
#endif
}
void vm_free_all()
{
}
#endif // SCP_UNIX
