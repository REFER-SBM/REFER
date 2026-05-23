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

#ifndef OPENGLDEVICE_DEFINED
#define OPENGLDEVICE_DEFINED

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

class CWinRf_OpenGLDevice
{
public:
	CWinRf_OpenGLDevice(HDC& deviceContext);
	CWinRf_OpenGLDevice(HWND& window);
	CWinRf_OpenGLDevice();

	bool create(HDC& deviceContext);
	bool create(HWND& window);

	void destroy();
	void makeCurrent();
	void makeCurrentNull();
	
	virtual ~CWinRf_OpenGLDevice();

private:
	bool setDCPixelFormat(HDC& deviceContext);
	
	HWND window;             // Manejador de la ventana
	HGLRC renderContext;     // Contexto de Rendering de OpenGL
	HDC deviceContext;       // Contexto de Dispositivo de OpenGL
	//CPalette m_GLPalette;  // Paleta logica de OpenGL
};

#endif // ifndef OPENGLDEVICE_DEFINED
