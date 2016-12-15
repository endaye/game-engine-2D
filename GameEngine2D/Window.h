#ifndef _WINDOW_H
#define _WINDOW_H
#include "System.h"

#ifndef _WINDOWS_
#include <Windows.h>
#endif

//Addisional include files
#ifndef _CONTEXT_H
#include "context.h"
#endif

#ifndef _STRING_H
#include "string.h"
#endif

//Struct
struct WindowData : public SystemData
{
	WindowData();
	WindowData(int w, int h, 
		const std::tstring& title = _T("EndayeEngine V1.0"),	// 窗口标题栏
			int b = 32,					// bit unit 
				bool fs = false);		// is fullscreen

	int width;
	int height;
	
	int bits;

	std::tstring windowTitle;
	
	bool fullscreen;

};

struct ResizeData
{
	ResizeData();
	ResizeData(bool resize, int nw, int nh);

	bool mustResize;

	int newWidth;
	int newHeight;
};

class Window :
	public System
{
	friend class Engine;
public:
	int GetWidth;
	int GetHeight;

	HWND GetWindowHandle();
	HDC GetDeviceContent();
	HINSTANCE GetInstance();

	ResizeData& GetResizeData() { return m_ResizeData; }

	LRESULT HandleEvent(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
protected:
	Window(const WindowData& data);
	virtual ~Window();

	virtual bool Initialize() override;
	virtual bool Update(Context& context) override;
	virtual bool ShutDown() override;

private:
	bool CenterWindow();
	LRESULT CALLBACK Window_Procedure(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Members
	HWND m_hWindow;
	HDC m_hDC;
	HINSTANCE m_hInst;

	int m_Width;
	int m_Height;

	int m_Bits;

	bool m_bFullScreen;

	ResizeData m_ResizeData;

	std::tstring m_Title;
};

#endif // !_WINDOW_H