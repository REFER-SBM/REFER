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

#if !defined CUELEVELS_DEFINED
	#define CUELEVELS_DEFINED


class CCueLevels
	{
		private:
			double GetRotationToPredominant(CDB_Model *DDBB);
			long GetCuspPyramid(CDB_Model *DDBB);
			double GetPyramidVertexRotation(CDB_Model *DDBB, long cusp, long IndexVertex);

			bool GetAngles(CDB_Model *DDBB, double Rotation, long cusp, long Index,
						   double &AnguloMaximo, double &AnguloMedio, double &AnguloMinimo);
			int GetLevel(double AnguloMaximo, double AnguloMedio, double AnguloMinimo);

			void GetLevelWithHiddenEdges(CDB_Model *DDBB, long numVertex, long &Level);

		public:
			void GetLevels(CDB_Model *DDBB);
	};

#endif // CUELEVELS_DEFINED