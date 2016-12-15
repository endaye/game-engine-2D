#include "Window.h"
#include <winuser.h>

//#include "Logger.h"

WindowData::WindowData()
	:SystemData(SystemType::Sys_Window),
	width(-1),
	height(-1),
	bits(-1),
	windowTitle(_T("")),
	fullscreen(false)
{}

WindowData::WindowData(int w, int h, const std::tstring& title, int b, bool fs)
	:SystemData(SystemType::Sys_Window),
	width(w),
	height(h),
	bits(b),
	windowTitle(title),
	fullscreen(fs)
{}

//Class window
//Declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		// if the message is WM_CREATE, the lParam contains a pointer to a CREATESTRUCT
		// the CREATESTRUCT constains the "this" pointer from the CreateWindow() method
		// the "this" pointer of our app is stored in the createstruct pcs->lpCreateParams
		CREATESTRUCT *pCS = (CREATESTRUCT*)lParam;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCS->lpCreateParams);
	}
	else
	{
		// retrieve the stored "this" pointer
		Window* pWindow = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (pWindow)
		{
			return pWindow->HandleEvent(hWnd, msg, wParam, lParam);
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);

}

LRESULT Window::HandleEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
	{
		if (!HIWORD(wParam))
		{
			this->Activate();
		}
		else
		{
			this->DeActivate();
		}
		return 0;
	}
	case WM_SIZE:
	{
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);

		m_ResizeData.mustResize = true;
		m_ResizeData.newWidth = width;
		m_ResizeData.newHeight = height;

		return 0;
	}
	case WM_DISPLAYCHANGE:
	{
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


ResizeData::ResizeData()
	:mustResize(false),
	newWidth(-1),
	newHeight(-1)
{

}
ResizeData::ResizeData(bool resize, int nw, int nh)
	:mustResize(resize),
	newWidth(nw),
	newHeight(nh)
{

}

// Constructor & Destructor
#pragma region Constructor & Destructor
Window::Window(const WindowData& data)
	:System(data),
	m_hWindow(NULL),
	m_hDC(NULL),
	m_hInst(NULL),

	m_Width(data.width),
	m_Height(data.height),
	m_Bits(data.bits),

	m_bFullScreen(data.fullscreen),
	m_Title(data.windowTitle),
	m_ResizeData()
{
}


Window::~Window()
{

}
#pragma endregion 

// Private methods
bool Window::Initialize()
{
	System::Initialize();

	unsigned int pixelFormat;			// Houlds the results after searching for a match

	DWORD dwExStyle;					// Window Extended Style
	DWORD dwStyle;						// Window Style

	RECT wndRect;						// Grabs rectangle upper left / lower right values
	wndRect.left = (long)0;				// Set left value to 0
	wndRect.right = (long)m_Width;		// Set right value to requested width
	wndRect.top = (long)0;				// Set top value to 0
	wndRect.bottom = (long)m_Height;	// Set bottom value to requested height

	WNDCLASS wndClass;					// Window class structure
	std::tstring className = m_Title;	// Window title

	wndClass.style = CS_DBLCLKS | CS_DROPSHADOW | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = NULL;
	wndClass.cbWndExtra = NULL;
	wndClass.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);		// Use Windows default icon
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		// Use Windows default cursor
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className.c_str();

	// Calculate the window size and position based upon the game size
	/*auto iWindowWidth = m_Width + GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
	auto iWindowHeight = m_Height + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);
	if (wndClass.lpszMenuName != NULL)
	{
		iWindowHeight != GetSystemMetrics(SM_CYMENU);
	}
	auto iXWindowPos = (GetSystemMetrics(SM_CXSCREEN)-iWindowWidth) / 2;
	auto iYWindowPos = (GetSystemMetrics(SM_CYSCREEN) - iWindowHeight) / 2;*/

	if (!RegisterClass(&wndClass))
	{
		//Logger::Log(_T("Failed to register window"), LOGTYPE_ERROR, true);
		return false;
	}

	if (m_bFullScreen)
	{
		DEVMODE dmScreenSettings;									// Device mode;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));		// Makes sure memory's cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);			// Size of the devmode structure
		dmScreenSettings.dmPelsWidth = m_Width;						// Selected screen width
		dmScreenSettings.dmPelsHeight = m_Height;					// Selected screen height
		dmScreenSettings.dmBitsPerPel = m_Bits;						// Selected bits per pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PANNINGHEIGHT;

		// Try to set selected mode and get results.
		// NOTE: CDS_FULLSCREEN gets rid of start bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If the mode fails, use windowed mode
			//Logger::Log(_T("The Requested Fullscreen Mode Is Not Supported By\nYour Video Card."), LOGTYPE_WARNING, true);

			dwExStyle = WS_EX_APPWINDOW;	// Window extended style
			dwStyle = WS_POPUP;				// Window style
			//ShowCursor(FALSE);			// Hide mouse pointer (Optional)
		}
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		// Window extended style
		dwStyle = WS_POPUP;									// Window style
	}

	AdjustWindowRectEx(&wndRect, dwStyle, FALSE, dwExStyle);	// Adjust window to true requested size

	if (!(m_hWindow = CreateWindowEx(
		dwExStyle,
		className.c_str(),
		className.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0,
		wndRect.right - wndRect.left,	// Calculate window width
		wndRect.bottom - wndRect.top,	// Calculate window height
		NULL,
		NULL,
		m_hInst,
		this)))
	{
		//Logger::Log(_T("Failed to window handle = NULL"), LOGTYPE_ERROR, true);
		return false;
	}


	static PIXELFORMATDESCRIPTOR pfd =			// PFD tells Windows how we want things to be
	{
		sizeof(PIXELFORMATDESCRIPTOR),		// Size of this Pixel Format Descriptor
		1,									// Version number
		PFD_DRAW_TO_WINDOW |				// Format must support Window
			PFD_SUPPORT_OPENGL |			// Format must support OpenGL
				PFD_DOUBLEBUFFER,			// Format must support Double Buffering
		PFD_TYPE_RGBA,						// Request an RGBA format
		m_Bits,								// Select our color depth
		0, 0, 0, 0, 0, 0,					// Color bits ignored
		0,									// No alpha buffer
		0,									// Shift bit ignored
		0,									// No accumulation buffer
		0, 0, 0, 0,							// Accumulation bits Ignored
		16,									// 16Bit Z-buffer (Depth Buffer)
		0,									// No stencil buffer
		0,									// No Auxiliary Buffer
		PFD_MAIN_PLANE,						// Main Drawing Layer
		0,									// Reserved
		0, 0, 0								// Layer masks ignored
	};

	if (!((m_hDC = GetDC(m_hWindow))))
	{
		//Logger::Log(_T("Can't create device context"), LOGTYPE_ERROR, true);
		return false;
	}
	if (!((pixelFormat = ChoosePixelFormat(m_hDC, &pfd))))
	{
		//Logger::Log(_T("Can't find a suitable PixelFormat"), LOGTYPE_ERROR, true);
		return false;
	}
	if (!SetPixelFormat(m_hDC, pixelFormat, &pfd))
	{
		//Logger::Log(_T("Can't set the PixelFormat"), LOGTYPE_ERROR, true);
		return false;
	}

	ShowWindow(m_hWindow, SW_SHOW);
	SetForegroundWindow(m_hWindow);
	SetFocus(m_hWindow);
	//UpdateWindow(m_hWindow);

	m_ResizeData.mustResize = true;
	m_ResizeData.newWidth = m_Width;
	m_ResizeData.newHeight = m_Height;

	if (!this->CenterWindow())
	{
		//Logger::Log(_T("Failed to center window"), LOGTYPE_ERROR, true);
		return false;
	}

	// Disable closing button debug window
	HWND hConsoleWnd = GetConsoleWindow();
	if (hConsoleWnd != NULL)
	{
		HMENU hConsoleMenu = GetSystemMenu(hConsoleWnd, FALSE);
		if (hConsoleMenu != 0)
		{
			BOOL bRet = RemoveMenu(hConsoleMenu, SC_CLOSE, MF_BYCOMMAND);
			if (!bRet)
			{
				//Logger::Log(_T("Failed to remove close button"), LOGTYPE_ERROR, true);
				return false;
			}
		}
	}

	// Success
	//Logger::Log(_T("Window is initialized"), LOGTYPE_INFO, true);
	return true;
}

bool Window::Update(Context& context)
{
	if (WINDOW != this)
	{
		WINDOW = this;
	}

	return true;
}

bool Window::ShutDown()
{
	if (m_bFullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(true);
	}

	// Are we able to release the DC
	// Set DC to null
	if (m_hDC && !ReleaseDC(m_hWindow, m_hDC))
	{
		//Logger::Log(_T("Failed to release DC"), LOGTYPE_ERROR, true);
		m_hDC = NULL;
		return false;
	}

	// Destroy the window
	if (m_hWindow && !DestroyWindow(m_hWindow))
	{
		//Logger::Log(_T("Failed to destroy the window"), LOGTYPE_ERROR, true);
		m_hWindow = NULL;
		return false;
	}

	// Unregister class
	if (!UnregisterClass(m_Title.c_str(), m_hInst))
	{
		//Logger::Log(_T("Failed to unregister class"), LOGTYPE_ERROR, true);
		m_hInst = NULL;
		return false;
	}

	// Success
	//Logger::Log(_T("Window is distroyed"), LOGTYPE_INFO, true);
	return true;
}

bool Window::CenterWindow()
{
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// Window extended style
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;					// Window style

	RECT R = { 0, 0, m_Width, m_Height };

	// Calculate the required sizeo of the window rectangle, based on the desired client-rectangle size.
	// The window rectangle can then be passed to the CreateWindow() function to create a window whose client area is the desired size.

	AdjustWindowRectEx(&R, dwStyle, false, dwExStyle);

	int windowwidth = R.right - R.left;
	int windowheight = R.bottom - R.top;

	int posx = GetSystemMetrics(SM_CXSCREEN) / 2 - windowwidth / 2;
	int posy = GetSystemMetrics(SM_CYSCREEN) / 2 - windowheight / 2;

	if (SetWindowPos(m_hWindow, NULL, posx, posy, windowwidth, windowheight, SWP_SHOWWINDOW) == NULL)
	{
		return false;
	}

	return true;
}
