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

/* WinRf_ViewOptimization_Graphics.h */


#ifndef ViewOptimization_Graphics_h
#define ViewOptimization_Graphics_h

// Enumeration of modes of graphics in the ViewOptimization window
enum Graphic {DIAGRAM_TEMPERATURE_COST     =0,
			  DIAGRAM_ALL_Z_VARIABLES      =1,
			  DIAGRAM_SOME_Z_VARIABLES     =2,
			  MAP_ISOLINES                 =3};

//Parameters used to configure graphics in ViewOptimization window
class CGraphicStyle
{
public:
	Graphic iTypeOfGraphic;
	int iInitialVariable,
		iFinalVariable;
	bool bShowLegend,
		 bShowLabels;

	CGraphicStyle::CGraphicStyle()
	{
		// Initialize optimization graphics
		iTypeOfGraphic = DIAGRAM_TEMPERATURE_COST;
		iInitialVariable = 0;
		iFinalVariable = ReferApp.In.NumVariables-1;
		bShowLegend = true;
		bShowLabels = true;
	};

	CGraphicStyle::~CGraphicStyle()
	{};



};

#endif // ViewOptimization_Graphics_h
