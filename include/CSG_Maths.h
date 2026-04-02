
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

#if !defined CSG_MATHS_DEFINED
#define CSG_MATHS_DEFINED

	//Vertices management and queries
	bool IsVertexInVector(CVertex element, std::vector<CVertex> vertices);   //Uses ReferApp.CSG.ThresholdLocation
	long GetVertexNumInVector(CVertex Vertex, std::vector<CVertex> Vertices);
	void RemoveVertexFromVector(CVertex Vertex, std::vector<CVertex> &Vertices);

	//Other Queries
	double GetBoundingBoxArea(CDB_Model *DDBB, std::vector<long> FeatureFaces);

	long IsMainDirection(double angle, std::vector<double> MainAngles);
	long IsMainDirection(CDB_Model *DDBB, 
						 CVertex Vertex1, CVertex Vertex2, 
						 std::vector<double> MainAngles);

	//Data management
	bool IsBrokenChain(CDB_Model *DDBB_Dec_Current, 
					   long Vert1, long Vert2,
					   double threshold,
					   long &Ed1, long &Ed2);

	bool GetChainedEdges(CDB_Model *DB_DatumGraph, 
						 long V1, long V2, 
						 double threshold,
						 std::vector<long> &chain);
	long getVertexInChain(CDB_Model *DB_DatumGraph, 
						  long V1, long V2,
						  CVertex insertVert,
						  std::vector<long> chain);

	CPlaneOrientation PlaneOrientation(CDB_Model *DB_DatumGraph, 
									   CDB_CSG *DB_CSG_Model,
									   std::vector<long> Vertices,
									   std::vector<double> MainAngles);


	bool UpdateDistance(CDB_Model *DB_DatumGraph,
						std::vector<double> MainAngles,
						std::vector<double> ReductionCoefficients,
						CVertex Vertex1, CVertex Vertex2,
						COrthoDistance &Distance);

	std::vector<std::vector<std::pair<long, long>>> construct_Dijkstra_Adj(CDB_Model *DB_DatumGraph);
	void get_Dijkstra_predecessors(CDB_Model *DB_DatumGraph, long s, std::vector<long> &predecessors);
	void get_Dijkstra_path(std::vector<long> predecessors, long s, long t, std::vector<long> &path);

	COrthoDistance CalculeOrthoDistance(CDB_Model *DB_DatumGraph,
										long Origin, 
										std::vector<long> predecessors,
										long Vertex,
										std::vector<double> MainAngles,
										std::vector<double> ReductionCoefficients);
	double CalculeNormalDistance(CDB_Model *DB_DatumGraph,
								 CPlaneOrientation Orientation,
								 long Origin,
								 std::vector<long> predecessors,
								 std::vector<long> Vertices,
								 std::vector<double> MainAngles,
								 std::vector<double> ReductionCoefficients);

#endif // CSG_MATHS_DEFINED