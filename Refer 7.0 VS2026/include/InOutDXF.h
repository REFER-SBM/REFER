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

// InOutDXF.h

#ifndef InOutDXF_VISITED
	#define InOutDXF_VISITED

class InOutDXF{
		private:
			void TransformaAcadRGB( int iCode, long &R, long &G, long &B);
			// Translates the color code from AutoCAD to RGB

			int TransformaRGBAcad( int R, int G, int B);
			// Translates the color code from RGB to AutoCAD

		public:
			InOutDXF();
			~InOutDXF();

			bool readDXF( const char *file, CDB_Model *pDDBB);
			bool writeDXF( const char *file, CDB_Model *pDDBB);
	}; // class InOutDXF

#endif
