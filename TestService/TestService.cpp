// TestService.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

SERVICE_STATUS          MyServiceStatus; 
SERVICE_STATUS_HANDLE   MyServiceStatusHandle; 

VOID SvcDebugOut(LPSTR String, DWORD Status);
VOID  WINAPI MyServiceCtrlHandler (DWORD opcode); 
VOID  __stdcall MyServiceStart (DWORD argc, LPTSTR *argv); 
DWORD MyServiceInitialization (DWORD argc, LPTSTR *argv, 
        DWORD *specificError); 
BOOL CreateSampleService();
BOOL DeleteSampleService();


 
void main(int argc, char** argv ) 
{ 
	if(argc == 2) {
		if(strcmp(argv[1], "-i") == 0){
			if(CreateSampleService())
				printf ("install success\n");
			else
				printf ("install not success\n");

		}else if(strcmp(argv[1], "-u") == 0) {
			if(DeleteSampleService())
				printf ("service removed");

		}else {
			printf("Usge:%s -option \n i -- install \n u -- delete\n", argv[0]);
		}
		return;
	}
	
   SERVICE_TABLE_ENTRY   DispatchTable[] = 
   { 
      { "MyService", MyServiceStart}, 
      { NULL,              NULL          } 
   }; 
 
   if (!StartServiceCtrlDispatcher( DispatchTable)) 
   { 
      SvcDebugOut(" [MY_SERVICE] StartServiceCtrlDispatcher (%d)\n", 
         GetLastError()); 
   } 
} 
 
VOID SvcDebugOut(LPSTR String, DWORD Status) 
{ 
   CHAR  Buffer[1024]; 
   if (strlen(String) < 1000) 
   { 
      sprintf(Buffer, String, Status); 
      OutputDebugStringA(Buffer); 
   } 
}


void WINAPI MyServiceStart (DWORD argc, LPTSTR *argv) 
{ 
    DWORD status; 
    DWORD specificError; 
 
    MyServiceStatus.dwServiceType        = SERVICE_WIN32; 
    MyServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    MyServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | 
        SERVICE_ACCEPT_PAUSE_CONTINUE; 
    MyServiceStatus.dwWin32ExitCode      = 0; 
    MyServiceStatus.dwServiceSpecificExitCode = 0; 
    MyServiceStatus.dwCheckPoint         = 0; 
    MyServiceStatus.dwWaitHint           = 0; 
 
    MyServiceStatusHandle = RegisterServiceCtrlHandler( 
        "MyService", 
        MyServiceCtrlHandler); 
 
    if (MyServiceStatusHandle == 0) 
    { 
        //SvcDebugOut(" [MY_SERVICE] RegisterServiceCtrlHandler failed %d\n", GetLastError()); 
        return; 
    } 
 
    // Initialization code goes here. 
    status = MyServiceInitialization(argc,argv, &specificError); 
 
    // Handle error condition 
    if (status != NO_ERROR) 
    { 
        MyServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
        MyServiceStatus.dwCheckPoint         = 0; 
        MyServiceStatus.dwWaitHint           = 0; 
        MyServiceStatus.dwWin32ExitCode      = status; 
        MyServiceStatus.dwServiceSpecificExitCode = specificError; 
 
        SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus); 
        return; 
    } 
 
    // Initialization complete - report running status. 
    MyServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    MyServiceStatus.dwCheckPoint         = 0; 
    MyServiceStatus.dwWaitHint           = 0; 
 
    if (!SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus)) 
    { 
        status = GetLastError(); 
        SvcDebugOut(" [MY_SERVICE] SetServiceStatus error %ld\n",status); 
    } 
 
    // This is where the service does its work. 
    SvcDebugOut(" [MY_SERVICE] Returning the Main Thread \n",0); 

	for(;;)
	{
		Sleep(10000);
		//MessageBox(NULL, __TEXT("Hello, I'am MyService"), __TEXT(""), 0);
		Beep(2500, 300);
	}
	
 
    return; 
} 
 
// Stub initialization function. 
DWORD MyServiceInitialization(DWORD   argc, LPTSTR  *argv, 
    DWORD *specificError) 
{ 
    argv; 
    argc; 
    specificError; 
    return(0); 
}

VOID WINAPI MyServiceCtrlHandler (DWORD Opcode) 
{ 
   DWORD status; 
 
   switch(Opcode) 
   { 
      case SERVICE_CONTROL_PAUSE: 
      // Do whatever it takes to pause here. 
         MyServiceStatus.dwCurrentState = SERVICE_PAUSED; 
         break; 
 
      case SERVICE_CONTROL_CONTINUE: 
      // Do whatever it takes to continue here. 
         MyServiceStatus.dwCurrentState = SERVICE_RUNNING; 
         break; 
 
      case SERVICE_CONTROL_STOP: 
      // Do whatever it takes to stop here. 
         MyServiceStatus.dwWin32ExitCode = 0; 
         MyServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
         MyServiceStatus.dwCheckPoint    = 0; 
         MyServiceStatus.dwWaitHint      = 0; 

         if (!SetServiceStatus (MyServiceStatusHandle, 
           &MyServiceStatus))
         { 
            status = GetLastError(); 
            SvcDebugOut(" [MY_SERVICE] SetServiceStatus error %ld\n", 
               status); 
         } 
 
         SvcDebugOut(" [MY_SERVICE] Leaving MyService \n",0); 
         return; 
 
      case SERVICE_CONTROL_INTERROGATE: 
      // Fall through to send current status. 
         break; 
 
      default: 
         SvcDebugOut(" [MY_SERVICE] Unrecognized opcode %ld\n", 
             Opcode); 
   } 
 
   // Send current status. 
   if (!SetServiceStatus (MyServiceStatusHandle,  &MyServiceStatus)) 
   { 
      status = GetLastError(); 
      SvcDebugOut(" [MY_SERVICE] SetServiceStatus error %ld\n", 
         status); 
   } 
   return; 
}

BOOL CreateSampleService() 
{ 
    TCHAR szPath[MAX_PATH]; 

	SC_HANDLE schSCManager;

	// Open a handle to the SC Manager database. 
 
	schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (NULL == schSCManager) 
		printf("OpenSCManager failed (%d)\n", GetLastError());


    
    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
        printf("GetModuleFileName failed (%d)\n", GetLastError()); 
        return FALSE;
    }
 
    SC_HANDLE  schService = CreateService( 
        schSCManager,              // SCManager database 
        TEXT("MyService"),        // name of service 
        TEXT("MyService"),           // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 
 
    if (schService == NULL) 
    {
        printf("CreateService failed (%d)\n", GetLastError()); 
        return FALSE;
    }
    else
    {
        CloseServiceHandle(schService); 
        return TRUE;
    }
}

BOOL DeleteSampleService() 
{ 
    SC_HANDLE schSCManager;
	// Open a handle to the SC Manager database. 
 
	schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
 
	if (NULL == schSCManager) 
		printf("OpenSCManager failed (%d)\n", GetLastError());
	
	SC_HANDLE schService = OpenService( 
        schSCManager,       // SCManager database 
        TEXT("MyService"), // name of service 
        DELETE);            // only need DELETE access 
 
    if (schService == NULL)
    { 
        printf("OpenService failed (%d)\n", GetLastError()); 
        return FALSE;
    }
 
    if (! DeleteService(schService) ) 
    {
        printf("DeleteService failed (%d)\n", GetLastError()); 
        return FALSE;
    }
    else 
        printf("DeleteService succeeded\n"); 
 
    CloseServiceHandle(schService); 
    return TRUE;
}