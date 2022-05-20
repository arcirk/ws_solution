#ifdef _WINDOWS
#include <SDKDDKVer.h>
#endif

#include <arcirk.h>

#include "ServiceMain.h"

#include "ws_service.h"

#define SERVICENAME TEXT("ws_service")
#define DISPLAYNAME TEXT("arcirk WebSocket service") //displayed in Windows Services
#define DESCRIPTION TEXT("Server WebSocket")

void __cdecl _tmain(int argc, TCHAR *argv[])
{
    ServiceMain<ws_service>(SERVICENAME, DISPLAYNAME, DESCRIPTION, argv);
}
