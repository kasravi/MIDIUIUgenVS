#pragma once

#define UTILS_MAX_VRIABLE_LEN	0x100
#define UTILS_VARIABLES_SECTION	"variable"

#define KEY_EXIST_IN_MAP(map,key)	(map.find(key)!=map.end())

#ifdef WIN32
#include <windows.h>
#include <process.h>
#define stricmp			_stricmp
#define WAIT			Sleep
#define GETTICKCOUNT	GetTickCount()
#define UTILS_MAX_MSGBOX_MESSAGE_LEN	0x800
int ShowMsg(char* str,...);

#else	//LINUX
//headers -->
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ini.h"
//headers <--
//types -->
typedef int 				INT;
typedef unsigned long int	DWORD;
typedef unsigned int		UINT;
typedef const char* 		LPCSTR;
typedef char* 				LPSTR;
typedef int					SOCKET;
typedef int 				OVERLAPPED;
typedef int 				HANDLE;

#define SOCKET_ERROR	(-1)
#define INVALID_SOCKET	(-1)
//types <--
//proc -->
unsigned long int LinGetTickCount(void);
int	LinSleep(unsigned int nWait);
int GetFunctionFromLib(const char* strLibPath,const char* strFuncName,void*& func,void*& handle);
#define stricmp					strcasecmp
#define GETTICKCOUNT			LinGetTickCount()
#define WAIT					LinSleep
#define ShowMsg(format, ...)	fprintf (stderr, format, __VA_ARGS__)
#define closesocket				close
//proc <--
#endif
////////////////////////////////////////////////////////////////////////////////////////////

//Same as GetPrivateProfileString with use variables (only prompt for Variables and no prompt when not found)
DWORD GetString(LPCSTR lpAppName,LPCSTR lpKeyName,LPCSTR lpDefault,LPSTR lpReturnedString,DWORD nSize,LPCSTR lpFileName);
//Same as GetPrivateProfileInt with use variables (only prompt for Variables and no prompt when not found)
DWORD GetInt(LPCSTR lpAppName,LPCSTR lpKeyName,INT nDefault,LPCSTR lpFileName);

//Prompt on not found
DWORD GetIniFileString(LPCSTR lpAppName,LPCSTR lpKeyName,LPCSTR lpDefault,LPSTR lpReturnedString,DWORD nSize,LPCSTR lpFileName);
DWORD GetIniFileInt(LPCSTR lpAppName,LPCSTR lpKeyName,INT nDefault,LPCSTR lpFileName);
//Prompt on not found (Use default)
DWORD GetIniFileString(LPCSTR lpAppName,LPCSTR lpDefaultAppName,LPCSTR lpKeyName,LPCSTR lpDefault,LPSTR lpReturnedString,DWORD nSize,LPCSTR lpFileName);
DWORD GetIniFileInt(LPCSTR lpAppName,LPCSTR lpDefaultAppName,LPCSTR lpKeyName,INT nDefault,LPCSTR lpFileName);


////////////////////////////////////////////////////////////////////////////////////////////
//portable mutex and thread implementation
//Headers

////////////////////////////////////////////////////////////////////////////////////////////
//MUTEX -->
//Data types
#ifdef WIN32
#define MUTEX HANDLE
#else
#define MUTEX pthread_mutex_t
#endif
//Functions
int MUTEX_INIT(MUTEX *mutex);
int MUTEX_LOCK(MUTEX *mutex);
int MUTEX_UNLOCK(MUTEX *mutex);
int MUTEX_CLOSE(MUTEX *mutex);
//MUTEX <--
////////////////////////////////////////////////////////////////////////////////////////////
//EVENT -->
//Data types
#ifdef WIN32
#define EVENT HANDLE
#else
struct EVENT
{
	pthread_mutex_t	mutex;
	pthread_cond_t event;
	volatile bool bSignalled;
	bool bManualReset;
};
#endif
//Functions
int EVENT_INIT(EVENT *evnt,bool bManualReset,bool bInitialState);
int EVENT_WAIT(EVENT *evnt,unsigned int nWait);
int EVENT_WAIT(EVENT *evnt);		//INFINTE Wait
int EVENT_SIGNAL(EVENT *evnt);
int EVENT_RESET(EVENT *evnt);
int EVENT_CLOSE(EVENT *evnt);
//MUTEX <--
///////////////////////////////////////////////////////////////////////////////////////////
//THREAD -->
//Data types
typedef void (*THREAD_FUNC)(void*);
#ifdef WIN32
#define THREAD 	HANDLE
#else
#define THREAD 	pthread_t
#endif

#define INVALID_HANDLE	((THREAD)-1L)

struct THREAD_FUNC_CALLER_ARG
{
	THREAD_FUNC func;
	void* arg;
};
//Functions
#ifdef WIN32
void THREAD_FUNC_CALLER(void* arg);
#else
void* THREAD_FUNC_CALLER(void* arg);
#endif
int THREAD_START(THREAD *thr,THREAD_FUNC func,void* param);
int THREAD_JOIN(THREAD *thr);
//THREAD <--
