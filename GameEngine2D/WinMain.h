#ifndef _WINMAIN_H
#define _WINMAIN_H

// C Runitme Header Files
#ifndef _WINDOW_
#include <Windows.h>
#endif // !_WINDOW_

int _tmain(int argc, char* argv[]);		// used in Debug mode, _tmain make sure console window show up
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
#endif // !_WINMAIN_H
