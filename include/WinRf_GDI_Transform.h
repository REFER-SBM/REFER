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

#ifndef GDI_TRANSFORM_VISITED
#define GDI_TRANSFORM_VISITED

// Defines mappings between windows and viewports

class cGDI_Transform{
	private:
		struct {
			double Xmax, Xmin, Ymax, Ymin;
		} World;

		struct {
			double Xmax, Xmin, Ymax, Ymin;
		} Device;

	public:
		//Sets the world window for the GDI drawings
		void GDI_SetWorld(double Xmin, double Ymin, double Xmax, double Ymax);

		//Sets the viewport window for the GDI drawings
		void GDI_SetViewport(double Xmin, double Ymin, double Xmax, double Ymax);

		//Gets the viewport window for the GDI drawings
		void GDI_GetViewport(double &Xmin, double &Ymin, double &Xmax, double &Ymax);

		//Transforms World coordinates into Viewport coordinates
		CPoint GDI_WorldToViewport(double x, double y);
		POINT2D GDI_WorldToViewport(POINT2D P);

		//Transforms Viewport coordinates into World coordinates
		POINT2D GDI_ViewportToWorld(CPoint v);
};

#endif  // GDI_TRANSFORM_VISITED