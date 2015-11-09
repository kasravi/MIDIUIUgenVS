#include "utils.h"

#ifdef WIN32

int ShowMsg(char* str,...)
{
	char buffer[UTILS_MAX_MSGBOX_MESSAGE_LEN]="";
	va_list argptr;
	va_start(argptr,str);
	//vsprintf(buffer,str,argptr);
	va_end(argptr);
	return MessageBox(NULL,(LPCTSTR)buffer,"Message",0);
}

#else
////////////////////////////////////////////////////////////////////////////////
unsigned long int LinGetTickCount(void)
{
	timespec stTime;
	if(clock_gettime(CLOCK_MONOTONIC,&stTime)!=0)
	{
		printf("error calling clock_gettime (CLOCK_MONOTONIC)!\n");
		exit(0);
	}
	unsigned long int nRes =((unsigned long long int) stTime.tv_sec) * 1000LL + stTime.tv_nsec / 1000000l;
	return (nRes);
}
////////////////////////////////////////////////////////////////////////////////
//wait in ms
int LinSleep(unsigned int nWait)
{
#define MAX_USLEEP_WAIT	500000l					//max time delay given to usleep is 0.5 second
	unsigned int nWait_us = nWait * 1000l;		//convert ms to us
	if(nWait_us < nWait)
		return -1;			//overflow happened
	int nRes = -1;
	unsigned int n1 = nWait_us / MAX_USLEEP_WAIT;	//number of MAX_USLEEP_WAIT in wait time
	unsigned int n2 = nWait_us % MAX_USLEEP_WAIT;	//remain time less than MAX_USLEEP_WAIT
	for(unsigned int i = 0 ; i < n1 ; i++)
	{
		nRes = usleep(MAX_USLEEP_WAIT);
		if(nRes != 0)//error in wait
			return nRes;
	}
	return (usleep(n2));
}

////////////////////////////////////////////////////////////////////////////////
#include <dlfcn.h>
//must be linked with dl library
int GetFunctionFromLib(const char* strLibPath,const char* strFuncName,void*& func,void*& handle)
{
	handle = dlopen(strLibPath, RTLD_NOW);
	if (handle == NULL) {
		fprintf(stderr,"error opening lib %s : %s\n",strLibPath,dlerror());
		return -1;
	}
	dlerror();    // Clear any existing error
	func = dlsym(handle, strFuncName);
	if(func==NULL)
	{
		fprintf(stderr,"error finding %s in lib %s : %s\n",strFuncName,strLibPath,dlerror());
		return -2;
	}
	return 0;
}

#endif

////////////////////////////////////////////////////////////////////////////////
bool IsVar(const char* str)
{//variable starts with a $
	if(str[0]=='$')
		return true;
	return false;
}
////////////////////////////////////////////////////////////////////////////////
//Same as GetPrivateProfileString with use variables (only prompt for Variables and no prompt when not found)
DWORD GetString(LPCSTR lpAppName,LPCSTR lpKeyName,LPCSTR lpDefault,LPSTR lpReturnedString,DWORD nSize,LPCSTR lpFileName)
{
	DWORD nRes=GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
	if(IsVar(lpReturnedString))
	{
		char key[UTILS_MAX_VRIABLE_LEN];
		strcpy(key,lpReturnedString);
		return (GetString(UTILS_VARIABLES_SECTION,key,lpDefault,lpReturnedString,nSize,lpFileName));
	}
	if(strcmp(lpDefault,lpReturnedString)==0)
	{//only show errors in Variables definition
		if(IsVar(lpKeyName))
			ShowMsg("Variable \"%s\" not found in section \"[%s]\" ( filename \"%s\") !"
			,lpKeyName,lpAppName,lpFileName);
	}
	return nRes;
}
////////////////////////////////////////////////////////////////////////////////
//Same as GetPrivateProfileInt with use variables (only prompt for Variables and no prompt when not found)
DWORD GetInt(LPCSTR lpAppName,LPCSTR lpKeyName,INT nDefault,LPCSTR lpFileName)
{
	char str[UTILS_MAX_VRIABLE_LEN];
	DWORD nStrRes=GetString(lpAppName,lpKeyName,"",str,UTILS_MAX_VRIABLE_LEN,lpFileName);
	if(nStrRes==0)
	{
		return nDefault;
	}
	//DWORD nRes;
	int nRes;
	int nRes2 = 0;// sscanf(str, "%d", &nRes);
	if(nRes2==0)
	{
		return nDefault;
	}
	return nRes;
}
////////////////////////////////////////////////////////////////////////////////
DWORD GetIniFileString(LPCSTR lpAppName,LPCSTR lpKeyName,LPCSTR lpDefault,LPSTR lpReturnedString,DWORD nSize,LPCSTR lpFileName)
{
	DWORD nRes=GetString(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
	if(strcmp(lpDefault,lpReturnedString)==0){
		ShowMsg("Not found \"%s\" in section \"[%s]\" ( filename \"%s\") !"
			,lpKeyName,lpAppName,lpFileName);
	}
	return nRes;
}
////////////////////////////////////////////////////////////////////////////////
//with default app name
DWORD GetIniFileString(LPCSTR lpAppName,LPCSTR lpDefaultAppName,LPCSTR lpKeyName,LPCSTR lpDefault,LPSTR lpReturnedString,DWORD nSize,LPCSTR lpFileName)
{
	DWORD nRes=GetString(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
	if(strcmp(lpDefault,lpReturnedString)==0)
		nRes=GetString(lpDefaultAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
	if(strcmp(lpDefault,lpReturnedString)==0){
		ShowMsg("Not found \"%s\" in section \"[%s]\"  or section \"[%s]\" ( filename \"%s\") !"
			,lpKeyName,lpAppName,lpDefaultAppName,lpFileName);
	}
	return nRes;
}
////////////////////////////////////////////////////////////////////////////////
DWORD GetIniFileInt(LPCSTR lpAppName,LPCSTR lpKeyName,INT nDefault,LPCSTR lpFileName)
{
	DWORD nRes=GetInt(lpAppName,lpKeyName,nDefault,lpFileName);
	if((INT)nRes==nDefault){
		ShowMsg("Not found \"%s\" in section \"[%s]\" ( filename \"%s\") !"
			,lpKeyName,lpAppName,lpFileName);
	}
	return nRes;
}
////////////////////////////////////////////////////////////////////////////////
//with default app name
DWORD GetIniFileInt(LPCSTR lpAppName,LPCSTR lpDefaultAppName,LPCSTR lpKeyName,INT nDefault,LPCSTR lpFileName)
{
	DWORD nRes=GetInt(lpAppName,lpKeyName,nDefault,lpFileName);
	if((INT)nRes==nDefault)
		nRes=GetInt(lpDefaultAppName,lpKeyName,nDefault,lpFileName);
	if((INT)nRes==nDefault){
		ShowMsg("Not found \"%s\" in section \"[%s]\"  or section \"[%s]\" ( filename \"%s\") !"
			,lpKeyName,lpAppName,lpDefaultAppName,lpFileName);
	}
	return nRes;
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success
int MUTEX_INIT(MUTEX *mutex)
{
#ifdef WIN32
    *mutex = CreateMutex(0, FALSE, 0);;
    return (*mutex==0);
#else
    return pthread_mutex_init (mutex, NULL);;
#endif
    return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success
int MUTEX_LOCK(MUTEX *mutex)
{
#ifdef WIN32
    //return (WaitForSingleObject(*mutex, INFINITE)==WAIT_FAILED?1:0);
	return (WaitForSingleObject(*mutex, INFINITE)==WAIT_OBJECT_0?0:1);
#else
	return pthread_mutex_lock( mutex );
#endif
    return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success
int MUTEX_UNLOCK(MUTEX *mutex)
{
#ifdef WIN32
    return (ReleaseMutex(*mutex)==0);
#else
	return pthread_mutex_unlock( mutex );
#endif
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success
int MUTEX_CLOSE(MUTEX *mutex)
{
	int nRes = -1;
#ifdef WIN32
	nRes = (CloseHandle(*mutex)!=0) ? 0 : 1 ;
#else
	//int rc = pthread_mutex_destroy( mutex );
	//if ( rc == EBUSY )
	//{
	//	MUTEX_UNLOCK( mutex );
	//}
	return pthread_mutex_destroy( mutex );
#endif
	mutex = NULL;
	return nRes;
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success
int EVENT_INIT(EVENT *evnt,bool bManualReset,bool bInitialState)
{
#ifdef WIN32
	*evnt = CreateEvent(NULL,bManualReset,bInitialState,NULL);
	return ( (evnt != NULL) ? 0 : -1);
#else
	if(pthread_cond_init(&evnt->event,NULL) != 0)
		return -1;
	if(pthread_mutex_init(&evnt->mutex,NULL) != 0 )
		return -2;
	evnt->bSignalled = bInitialState;
	evnt->bManualReset = bManualReset;
	return 0;
#endif
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success (wait in mili seconds)
int EVENT_WAIT(EVENT *evnt,unsigned int nWait)
{
#ifdef WIN32
	return ( (WaitForSingleObject(*evnt,nWait) == WAIT_OBJECT_0) ? 0 : -1);
#else
	//test test test
	unsigned int nStart = GETTICKCOUNT;

	timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 0;

	int nRes = pthread_mutex_lock(&evnt->mutex);
	if(nRes != 0)
	{
		return nRes;
	}

	nRes = clock_gettime(CLOCK_REALTIME, &ts);
	if(nRes != 0)
	{
		pthread_mutex_unlock(&evnt->mutex);
		return nRes;
	}
	//add nWait ms to current time
	ts.tv_sec += (nWait / 1000l);
	ts.tv_nsec += (nWait % 1000l) * 1000000l;

	if(ts.tv_nsec >= 1000000000l)
	{
		ts.tv_sec += (ts.tv_nsec / 1000000000l);	//add second part of nanosecond to second of struct
		ts.tv_nsec = (ts.tv_nsec % 1000000000l);	//crop second part of nanosecond
	}
	//
	int rc = 0;
	while ((!evnt->bSignalled) && (rc == 0))
		rc = pthread_cond_timedwait(&evnt->event, &evnt->mutex, &ts);

	if(rc != 0 )
	{
		if(rc != ETIMEDOUT)
		{
			fprintf(stderr,"EVENT_WAIT: error in calling pthread_cond_timedwait (res %d)\n",rc);
		}
	}

	if ((rc == 0) && (!evnt->bManualReset))
	{//event happened so clean signal in auto reset mode
		evnt->bSignalled = false;
	}
	pthread_mutex_unlock(&evnt->mutex);
	return rc;
#endif
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success (INFINTE Wait)
int EVENT_WAIT(EVENT *evnt)
{
#ifdef WIN32
	return ( (WaitForSingleObject(*evnt,INFINITE) == WAIT_OBJECT_0) ? 0 : -1);
#else
	int nRes = pthread_mutex_lock(&evnt->mutex);
	if(nRes != 0)
		return nRes;

	int rc = 0;
	while ((!evnt->bSignalled) && (rc == 0))
		rc = pthread_cond_wait(&evnt->event, &evnt->mutex);

	if ((rc == 0) && (evnt->bManualReset))
	{
		evnt->bSignalled = false;
	}
	pthread_mutex_unlock(&evnt->mutex);
	return rc;
#endif
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success
int EVENT_SIGNAL(EVENT *evnt)
{
#ifdef WIN32
	return ( SetEvent(*evnt) ? 0 : -1);
#else
	int nRes = pthread_mutex_lock(&evnt->mutex);
	if(nRes != 0)
		return nRes;
	evnt->bSignalled = true;
	pthread_mutex_unlock(&evnt->mutex);
	return pthread_cond_signal(&evnt->event);
	//return pthread_cond_broadcast(&evnt->event);
#endif
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success
int EVENT_RESET(EVENT *evnt)
{
#ifdef WIN32
	return ( ResetEvent(*evnt) ? 0 : -1);
#else
	int nRes = pthread_mutex_lock(&evnt->mutex);
	if(nRes != 0)
		return nRes;
	evnt->bSignalled = false;
	pthread_mutex_unlock(&evnt->mutex);
	return 0;
#endif
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on success
int EVENT_CLOSE(EVENT *evnt)
{
	int nRes = -1;
#ifdef WIN32
	nRes = (CloseHandle(*evnt)!=0) ? 0 : 1 ;
#else
	pthread_mutex_destroy(&evnt->mutex);
	pthread_cond_destroy(&evnt->event);
#endif
	//evnt = NULL;		//it is not correct
	return nRes;
}
///////////////////////////////////////////////////////////////////////////////////////////
//all threads will start from here
#ifdef WIN32
void THREAD_FUNC_CALLER(void* arg)
#else
void* THREAD_FUNC_CALLER(void* arg)
#endif
{
	THREAD_FUNC_CALLER_ARG* thread_arg = (THREAD_FUNC_CALLER_ARG*) arg;
	thread_arg->func(thread_arg->arg);

	delete thread_arg;	//done so delete argument created by THREAD_START()

#ifdef WIN32
	_endthreadex( 0 );	//avoiding exception on CloseHandle
	return;
#else	//Linux
	return NULL;
#endif
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on successful
int THREAD_START(THREAD *thr,THREAD_FUNC func,void* param)
{
	THREAD_FUNC_CALLER_ARG* thread_arg = new THREAD_FUNC_CALLER_ARG;
	if(thread_arg == NULL)
		return -1;	//error creating thread param

	thread_arg->arg = param;
	thread_arg->func = func;
	int nRes = -1;
#ifdef WIN32
	*thr = (HANDLE)_beginthread(THREAD_FUNC_CALLER,0,thread_arg);
	if(*thr != INVALID_HANDLE)
	{
		nRes = 0;
	}
#else	//Linux
	nRes = pthread_create(thr , NULL,THREAD_FUNC_CALLER ,thread_arg);
#endif

	if(nRes != 0)
	{
		delete thread_arg;
	}
	return nRes;
}
///////////////////////////////////////////////////////////////////////////////////////////
//returns zero on successful
int THREAD_JOIN(THREAD *thr)
{
	int res = 0;
	#ifdef WIN32
		res = (WaitForSingleObject( *thr, INFINITE) == WAIT_OBJECT_0?0:-1);
		if(res == 0)
		{
			CloseHandle(*thr);
		}
	#else
		if(*thr != INVALID_HANDLE)
		{
			res = pthread_join(*thr,NULL);
		}
	#endif
	*thr = INVALID_HANDLE;
	return res;
}

