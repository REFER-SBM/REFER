
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

#if !defined DATATESSELLATE3D_DEFINED
	#define DATATESSELLATE3D_DEFINED

	class CDataTessellate3D{

		public:
		bool FacesOutward;
		bool TriangulateFaces;
		bool NormalsToFaces;

		CDataTessellate3D();
		~CDataTessellate3D();
	};

#endif // DATATESSELLATE3D_DEFINED