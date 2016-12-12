#include "WinMain.h"

//Additional include files
#ifndef _STRING_H
#include "string.h"
#endif // !_STRING_H
#ifndef _IOSTREAM_H
#include "iostream.h"
#endif // !_IOSTREAM_H

#if defined (DEBUG) | defined (_DEBUG)
int _tmain(int argc, char* argv[])
{
	UNREFERENCED_PARAMETER(argc);	//不用argc
	UNREFERENCED_PARAMETER(argv);	//不用argv

	//Logger::Log("Starting the program");
	WinMain((HINSTANCE)GetModuleHandle(NULL), 0, 0, SW_SHOW);
}
#endif 

int WINAPI WinMain(HINSTANCE hInstance,		//WINAPI是函数约定 //A handle to the current instance of the application.
	HINSTANCE hPrevInstance,				//A handle to the previous instance of the application. This parameter is always NULL. 
	LPSTR lpCmdLine,						//The command line for the application, excluding the program name
	int nCmdShow)							//Controls how the window is to be shown.
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

//Capture the obj causing memory leak (not all, but most of)
#if defined(DEBIG) | defined(_DEBUG)
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	//Enable run-time memory leak check for debug builds
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(0);
#endif

	////Create the engine
	//Engine* pEngine = new Engine();

	////Kick of the game
	//int result = pEngine->RunLoop();

	////Delete the engine
	//delete pEngine;

	//Return result
	return 0;
}
