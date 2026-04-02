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

#ifndef DB_MODEL_MATHS_VISITED
	#define DB_MODEL_MATHS_VISITED

	//public:
		//Edges
		CVector GetUnitVector2D(CVertex VH, CVertex VT);
		CVector GetUnitVector3D(CVertex VH, CVertex VT);

		void AddMidPoints(CDB_Model *DDBB,
						  std::vector<long> &NumVertices,
						  CVerticesSet &Vertices);

		double GetDistance2DVertexToLine(CVertex V, CVertex p0, CVertex p1);
		//void GetDistance2DVertexToLine(CVertex V, CVertex p0, CVertex p1, 
		//							   double &distance, CVertex &Intersection);

		//double GetDistance3DVertexToLine(CVertex V, CVertex p0, CVertex p1);

		double NearlyParallel(CDB_Model *DDBB, long iedge0, long iedge1, double ThresholdAngle);
		bool NearlyCollinear(CDB_Model *DDBB, long iedge0, long iedge1,
							 double ThresholdAngle, double ThresholdOffset);

		//Faces
		double GetFaceArea(CDB_Model *DDBB, long NumFace);
		double GetFaceArea(CDB_Model *DDBB, std::vector<long> vertices);

		CVector GetTriangleNormal(double Vertices[3][3]);
		CVector GetFaceNormal(CDB_Model *DDBB, long NumFace, bool saveNormals);
		void GetFaceNormals(CDB_Model *DDBB);

		bool IsFacePlanar(CDB_Model *DDBB, long NumFace, double threshold);
		CVertex GetFaceCentroid(CDB_Model *DDBB, long NumFace);

		bool AreAdjacentFaces(CDB_Model *DDBB, long face0, long face1);

		long ConvexHullFace(CDB_Model *DDBB);
		void OrderAdjacentFaces(CDB_Model *DDBB,
								long seed,
								std::vector<std::vector<long>> FacesInEdges,
								std::vector<bool> &orderedFace,
								long &iorderedFaces);
		void ReorientFaces(CDB_Model *DDBB);




		bool IsASlenderFace(CFace Face, 
							double ThresholdShort, double ThresholdLong,
							std::vector<double> edgeLength);
		bool IsParallelogram(CFace Face, 
							 double ThresholdLength, double ThresholdAngle,
							 bool shortSides,
							 std::vector<double> edgeLength,
							 std::vector<double> edgeAngle);

		void PairEdgesOfParallelogramFace(CFace Face,
										  std::vector<double> edgeLength,
										  long &long1, long &long2, 
										  long &short1, long &short2);

		void ReorderToMatch(std::vector<long> &Vector1, std::vector<long> &Vector2,
							long Origin1, long Next1, long Origin2, long Next2);
		void PairVertices(CDB_Model *DDBB,
						  long connect1, long connect2,
						  long pair1, long pair2,
						  std::vector<long> &VertFaceL1, std::vector<long> &VertFaceL2);
		void PairVertices(CDB_Model *DDBB, 
						  long base, long lateral1, long lateral2,
						  std::vector<long> &PairedVertL1, 
						  std::vector<long> &PairedVertL2);

		bool GetLateralFaces(CDB_Model *DDBB, long centralFace,
							 long side1, long side2,
							 long &lateral1, long &lateral2);

		bool SimilarDistances(CDB_Model *DDBB, 
							  double ThresholdSimilarDistance,
							  std::vector<long> &VertInFace1,
							  std::vector<long> &VertInFace2);
		bool SimilarAngles(CDB_Model *DDBB, 
						   double ThresholdSimilarAngle,
						   std::vector<long> &VertInFace1,
						   std::vector<long> &VertInFace2);
		bool LateralsAreSimilar(CDB_Model *DDBB,
								double ThresholdSimilarDistance,
								double ThresholdSimilarAngle,
								long connect1, long connect2,
								long lateral1, long lateral2);

		bool ExternalAreOpposite(CDB_Model *DDBB, long centralFace,
								 double ThresholdAngle,
								 long connect1, long connect2,
								 long lateral1, long lateral2);


#endif //DB_MODEL_MATHS
