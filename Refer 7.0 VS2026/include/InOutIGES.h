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

// InOutIGES.h

#ifndef InOutIGES_VISITED
	#define InOutIGES_VISITED

class InOutIGES{
		public:
			InOutIGES();
			~InOutIGES();

		private:
			int digits(long number);

			void globalSection (std::ofstream &out, CDB_Model *DDBB_IGES, const char *file, long &numG);

			long InOutIGES::PrintMultiLineParameter(std::ofstream &BufferParams, 
													std::string parameter,
													long &numP, long numD);

			long Add_000_Params(std::ofstream &BufferParams, 
								long param1, 
								double param2, 
								long &numP, long numD);
			void Add_000_Directory(std::ofstream &out, long LineCount000,
								   long numP, long &numD);

			long Add_102_Params(std::ofstream &BufferParams, 
								std::vector <long> pointerCurves,
								long &numP, long numD);
			void Add_102_Directory(std::ofstream &out, long LineCount102,
								   long colorNumber, long numLevel, 
								   long numCurve,
								   std::string formName,
								   std::string statusNumber,
								   long numP, long &numD);

			long Add_108_01_Params(std::ofstream &BufferParams, 
								   CVertex P1, 
								   CVertex P2, 
								   CVertex P3, 
								   long pointerContour,
								   long &numP, long numD);
			void Add_108_01_Directory(std::ofstream &out, long LineCount108,
									  long colorNumber, long numLevel, 
									  long numLine,
									  std::string formName,
									  std::string statusNumber,
									  long numP, long &numD);

			long Add_110_00_Params(std::ofstream &BufferParams, 
								   CVertex VertexEdgeITail, 
								   CVertex VertexEdgeIHead, 
								   long &numP, long numD);
			void Add_110_00_Directory(std::ofstream &out, long LineCount110,
									  long colorNumber, long numLevel, 
									  long numLine,
									  std::string formName,
									  std::string statusNumber,
									  long numP, long &numD);

			long Add_116_Params(std::ofstream &BufferParams, 
								CVertex VertexI, 
								long &numP, long numD);
			void Add_116_Directory(std::ofstream &out, long LineCount116, 
								   long numPoint,
								   std::string formName,
								   long numP, long &numD);

			long Add_123_Params(std::ofstream &BufferParams, 
								CVector Normal, 
								long &numP, long numD);
			void Add_123_Directory(std::ofstream &out, long LineCount123, long numP, long &numD);

			long Add_126_01_Params(std::ofstream &BufferParams, 
								   CVertex P1, CVertex P2, CVector Normal,
								   long &numP, long numD);
			void Add_126_01_Directory(std::ofstream &out, long LineCount126_01, 
									  long colorNumber, long numLevel, 
									  long numEdge,
									  std::string formName,
									  std::string statusNumber,
									  long numP, long &numD);

			long Add_128_00_Params(std::ofstream &BufferParams, 
								   std::vector <CVertex> Q_gon,
								   long &numP, long numD);
			void Add_128_00_Directory(std::ofstream &out, long LineCount128_00,
									  long colorNumber, long numLevel,
									  long numShell,
									  std::string formName,
									  std::string statusNumber,
									  long numP, long &numD);

			long Add_128_08_Params(std::ofstream &BufferParams, 
								   std::vector <CVertex> Q_gon,
								   long &numP, long numD);
			void Add_128_08_Directory(std::ofstream &out, long LineCount128_08, 
									  long colorNumber, long numLevel, 
									  long numShell,
									  std::string formName,
									  std::string statusNumber,
									  long numP, long &numD);

			long Add_141_Params(std::ofstream &BufferParams, 
								long pointerUntrimmedSurface, 
								std::vector <long> pointerCurves,
								long &numP, long numD);
			void Add_141_Directory(std::ofstream &out, long LineCount141,
								   long colorNumber, long numLevel, 
								   long numShell,
								   std::string formName,
								   long numP, long &numD);

			long Add_142_Params(std::ofstream &BufferParams, 
								long pointerUntrimmedSurface, 
								long pointerBoundary, long pointerBoundaryPar,
								long &numP, long numD);
			void Add_142_Directory(std::ofstream &out, long LineCount142,
								   long colorNumber, long numLevel, 
								   long numShell,
								   std::string formName,
								   long numP, long &numD);

			long Add_143_Params(std::ofstream &BufferParams, 
								long pointerUntrimmedSurface, long pointerBoundary, 
								long &numP, long numD);
			void Add_143_Directory(std::ofstream &out, long LineCount143,
								   long colorNumber, long numLevel, 
								   long numShell,
								   std::string formName,
								   long numP, long &numD);

			long Add_144_Params(std::ofstream &BufferParams, 
								long pointerUntrimmedSurface, long pointerBoundary, 
								long &numP, long numD);
			void Add_144_Directory(std::ofstream &out, long LineCount144,
								   long colorNumber, long numLevel, 
								   long numShell,
								   std::string formName,
								   long numP, long &numD);

			long Add_190_00_Params(std::ofstream &BufferParams, 
								   long DEloc, long DEnormal, 
								   long &numP, long numD);
			void Add_190_00_Directory(std::ofstream &out, long LineCount190_00, long numP, long &numD);

			long Add_314_Params(std::ofstream &BufferParams, 
								long &numP, long numD);
			void Add_314_Directory(std::ofstream &out, long LineCount314, long numP, long &numD);

			long Add_402_07_Params(std::ofstream &BufferParams, 
								   std::vector <long> pointerFacets,
								   long &numP, long numD);
			void Add_402_07_Directory(std::ofstream &out, long LineCount402_07, 
									  long colorNumber, long numLevel, long numP, long &numD);

			long Add_406_03_Params(std::ofstream &BufferParams, 
								   long numLevel, long &numP, long numD);
			void Add_406_03_Directory(std::ofstream &out, long LineCount406_03, 
									  long colorNumber, long numLevel, long numP, long &numD);

			bool TrimTgonPatch(const char *file, CDB_Model DDBB_IGES,
							   long colorNumber, long layerNumber, bool type143,
							   long numTgons,
							   std::vector <CVertex> T_gon,
							   std::vector <CVertex> Q_Patch,
							   CVector Normal,
							   long &numP, long &numD,
							   std::ofstream &out,
							   std::ofstream &BufferParams);

			bool TrimTgonPlane(const char *file, CDB_Model DDBB_IGES,
							   long colorNumber, long layerNumber, bool type143,
							   long numTgons,
							   std::vector <CVertex> T_gon,
							   CVector Normal,
							   long &numP, long &numD,
							   std::ofstream &out,
							   std::ofstream &BufferParams);

			bool TrimNgonPatch(const char *file, CDB_Model DDBB_IGES,
							   long colorNumber, long layerNumber, bool type143,
							   long numNgons,
							   std::vector <CVertex> N_gon,
							   std::vector <CVertex> Q_Patch,
							   CVector Normal,
							   long &numP, long &numD,
							   std::ofstream &out,
							   std::ofstream &BufferParams);

			bool PolygonalFacesToNURBS(const char *file, CDB_Model DDBB_IGES,
									   long colorNumber, long layerNumber,
									   bool type143,
									   long &numP, long &numD,
									   std::ofstream &out,
									   std::ofstream &BufferParams);

			bool PolygonalFacesTo3Mesh(const char *file, CDB_Model DDBB_IGES,
									   long colorNumber, long layerNumber,
									   bool type143,
									   long &numP, long &numD,
									   std::ofstream &out,
									   std::ofstream &BufferParams);

		public:
			bool writeIGES( const char *file, CDB_Model *pDDBB);
	}; // class InOutIGES

#endif
