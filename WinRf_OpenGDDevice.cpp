//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/


#include <stdafx.h>
#include "WinRf_OpenGLDevice.h"

CWinRf_OpenGLDevice::CWinRf_OpenGLDevice()
{
	window = NULL;
	renderContext = NULL;
	deviceContext = NULL;
}

CWinRf_OpenGLDevice::~CWinRf_OpenGLDevice()
{
}

CWinRf_OpenGLDevice::CWinRf_OpenGLDevice(HWND& window)
{
	create(window);
}

CWinRf_OpenGLDevice::CWinRf_OpenGLDevice(HDC& deviceContext)
{
	create(deviceContext);
}

bool CWinRf_OpenGLDevice::create(HWND& window)
{
	HDC deviceContext = ::GetDC(window);  // Retrieve a handle identifying the private DC
	
	if (!create(deviceContext))
	{
		::ReleaseDC(window, deviceContext);

		return false;
	}

	::ReleaseDC(window, deviceContext);
	CWinRf_OpenGLDevice::window = window;
	
	return true;
}

bool CWinRf_OpenGLDevice::create(HDC& deviceContext)
{
	if (!deviceContext) return false;

	if (!setDCPixelFormat(deviceContext)) return false;

	renderContext = wglCreateContext(deviceContext);

	CWinRf_OpenGLDevice::deviceContext = deviceContext;
	
	return true;
}

void CWinRf_OpenGLDevice::destroy()
{
	if(wglGetCurrentContext() != NULL)
		wglMakeCurrent(NULL,NULL);

	// Clean up rendering context stuff
	if (renderContext != NULL)
	{
		wglDeleteContext(renderContext);
		int a = 0;
		if (::ReleaseDC(window,deviceContext)) return ;	
		a= 1;
	}
}

void CWinRf_OpenGLDevice::makeCurrent()
{
	if (renderContext != NULL)
	{
		wglMakeCurrent(deviceContext, renderContext);
	}
}

void CWinRf_OpenGLDevice::makeCurrentNull()
{
	if(wglGetCurrentContext() != NULL)
	{
		wglMakeCurrent(deviceContext, NULL);
	}
}

bool CWinRf_OpenGLDevice::setDCPixelFormat(HDC& deviceContext)
// Defines the PIXEL FORMAT for OpenGL
{

	// Retrieve the current settings for the display device
	DEVMODE resolution;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &resolution);

	// Describe de desired pixel format
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		(BYTE)resolution.dmBitsPerPel,	// Current display device color resolution	//24,	// Want 24bit color
		0,0,0,0,0,0,					// color bits ignored
		0,								// no alpha buffer
		0,								// shift bit ignored
		0,								// no accumulation buffer
		0,0,0,0,						// accum bits ignored
		32,								// Size of depth buffer (32-bit z-buffer)
		0,								// no stencil buffer
		0,								// no auxiliary buffer
		PFD_MAIN_PLANE,					// Draw in main plane
		0,								// reserved
		0,0,0 };						// layer masks ignored

	// Choose a pixel format that best matches that described in pfd
	int nPixelFormat;				// Pixel format index
	nPixelFormat = ChoosePixelFormat(deviceContext, &pfd);


	//DescribePixelFormat(deviceContext, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd); 



	// Set the pixel format for the device context
	if (!SetPixelFormat(deviceContext, nPixelFormat, &pfd)){
		return false;
	}

	return true;
}