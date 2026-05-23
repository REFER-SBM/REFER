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
#include <vector>
#include "Tools_Geometry.h"

#include "WinRf_GDI_Transform.h"


void cGDI_Transform::GDI_SetWorld(double Xmin, double Ymin, double Xmax, double Ymax)
// Sets the world window for the GDI drawings

//The world window is the rectangular portion of the universe
//that we intend to display
{
	World.Xmin = Xmin;
	World.Ymin = Ymin;
	World.Xmax = Xmax;
	World.Ymax = Ymax;
}


void cGDI_Transform::GDI_SetViewport(double Xmin, double Ymin, double Xmax, double Ymax)
// Sets the viewport window for the GDI drawings

//The viewport window is the rectangular portion of the screen
//where we intend to display something
//In fact, we work inside the drawing area of the active window
//(instead of the whole screen)
{
	Device.Xmin = Xmin;
	Device.Ymin = Ymin;
	Device.Xmax = Xmax;
	Device.Ymax = Ymax;
}


void cGDI_Transform::GDI_GetViewport(double &Xmin, double &Ymin, double &Xmax, double &Ymax)
// Gets the active viewport window for the GDI drawings

//The viewport window is the rectangular portion of the screen where we intend to display something
//In fact, we work inside the drawing area of the active window (instead of the whole screen)
{
	Xmin= Device.Xmin;
	Ymin= Device.Ymin;
	Xmax= Device.Xmax;
	Ymax= Device.Ymax;
}


CPoint cGDI_Transform::GDI_WorldToViewport(double x, double y)
// Transforms World coordinates into Viewport coordinates
{
	CPoint Point;

	// Assumes mode MM_TEXT (The X's increase from left to right, and Y's increase downwards)
	Point.x = (int) (Device.Xmin + ( (x - World.Xmin) * (Device.Xmax - Device.Xmin) / (World.Xmax - World.Xmin) ));
	double wxRange= (World.Xmax - World.Xmin);
	if (wxRange > 0)
		Point.x = (int)(Device.Xmin + ((x - World.Xmin) * (Device.Xmax - Device.Xmin) / (World.Xmax - World.Xmin)));
	else
		Point.x = (int)(Device.Xmin + ((x - World.Xmin) * (Device.Xmax - Device.Xmin)));

	double wyRange = (World.Ymax - World.Ymin);
	if (wyRange > 0)
		Point.y = (int) (Device.Ymax - ( (y - World.Ymin) * (Device.Ymax - Device.Ymin) / (World.Ymax - World.Ymin) ));
	else
		Point.y = (int)(Device.Ymax - ((y - World.Ymin) * (Device.Ymax - Device.Ymin) ));

	// For non-MM-TEXT modes
	//Point.y = (int) (Device.Ymin + ( (y - World.Ymin) * (Device.Ymax - Device.Ymin) / (World.Ymax - World.Ymin) ));

	return(Point);
}


POINT2D cGDI_Transform::GDI_WorldToViewport(POINT2D P)
// Transforms World coordinates into Viewport coordinates
{
	POINT2D Point;

	// Assumes mode MM_TEXT (The X's increase from left to right, and Y's increase downwards)
	Point.x = (int) (Device.Xmin + ( (P.x - World.Xmin) * (Device.Xmax - Device.Xmin) / (World.Xmax - World.Xmin) ));
	Point.y = (int) (Device.Ymax - ( (P.y - World.Ymin) * (Device.Ymax - Device.Ymin) / (World.Ymax - World.Ymin) ));

	// For non-MM-TEXT modes
	//Point.y = (int) (Device.Ymin + ( (P.y - World.Ymin) * (Device.Ymax - Device.Ymin) / (World.Ymax - World.Ymin) ));

	return(Point);
}


POINT2D cGDI_Transform::GDI_ViewportToWorld(CPoint v)
// Transforms Viewport coordinates into World coordinates

//Used to capture cursor inputs while drawing and editing sketches
{
	POINT2D Point;

	// Assumes mode MM_TEXT (The X's increase from left to right, and Y's increase downwards)
	Point.x = World.Xmin + ((v.x - Device.Xmin) * (World.Xmax - World.Xmin) / (Device.Xmax - Device.Xmin) );
	Point.y = World.Ymin - ((v.y - Device.Ymax) * (World.Ymax - World.Ymin) / (Device.Ymax - Device.Ymin) );

	// For non-MM-TEXT modes
	//Point.y = World.Ymin + ((Device.Ymax - v.y) * (World.Ymax - World.Ymin) / (Device.Ymax - Device.Ymin) );

	return(Point);
}
