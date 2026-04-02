
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

#if !defined CSG_DECONSTRUCT_DEFINED
#define CSG_DECONSTRUCT_DEFINED

	//----------------------------------------------------------------------
	class CCSG_Deconstruct{
		private:
			//Parameters from cues
			std::vector<double>MainAngles;
			std::vector<double>ReductionCoefficients;
			long OuterSubGraph;  //Outer subgraph, as calculated before deconstructing the model

			//Information of the father face
			std::vector<long> VerticesFatherFace;

			//Merits
			long LowMeritFeatures(std::vector<double> Merits);
			long GetNumValidFeatures();


//---Begin SelectFeature------------------------------------------------------------------------------
			//private:
			void GetSmallestFeature(CDB_Model *DDBB_Dec_Current, 
									CFeature &NewFeat,
									long &FeatureId);
			
			long GetFatherFace(CDB_Model *DDBB_Dec_Current,
							   CFeature PrevFeature);
			void GetFatherFeature(CDB_Model *DDBB_Dec_Current, 
								  CFeature &NewFeat, 
								  long &FeatureId,
								  CFeature PrevFeature);

			bool GetFatherFaces(CDB_Model *DDBB_Dec_Current, CFeature &Feat, long FeatureId);

			//public:
			long SelectFeature(CDB_Model *DDBB_Dec_Current,
							   CDB_CSG *DB_CSG_Model,
							   CFeature &NewFeat, bool &FeatureFather);
//---End SelectFeature------------------------------------------------------------------------------


//--Begin CompleteFeatureData----------------------------------------
			//private;

			//Begin-----------------------
			void GetCoreBases(CDB_Model *DDBB_Dec_Current, 
							  CDB_CSG *DB_CSG_Model, 
							  long &face, long &base1, long &base2);
			void FacesFeature(CDB_Model *DDBB_Dec_Current,
							  CDB_CSG *DB_CSG_Model,
							  CFeature &NewFeat, 
							  FEATURE_TYPE type, 
							  long FeatureId, 
							  long &base1, long &base2, long &face,
							  std::vector<long> &faces);
			//End  -----------------------

			//Begin-----------------------
			void RemoveElementFromVector(long Face, std::vector<long> &laterals);
			std::vector<long> GetEdgesIntersectedByFaces(CDB_Model *DDBB_Dec_Current,long face1, long face2);
			void DetectOppositeFaces(CDB_Model *DDBB_Dec_Current, long bottom, std::vector<long> &laterals);
			void RailSlotExtrusion(CDB_Model *DDBB_Dec_Current,
								   std::vector<long> &faces,
								   std::vector<long> &PairedVertL1,
								   std::vector<long> &PairedVertL2,
								   long &direction, double &ExtrudedLength);
			//End  -----------------------

			void ExtrusionDirectionAndLength(CDB_Model *DDBB_Dec_Current, 
											  std::vector<long> PairedVertL1,
											  std::vector<long> PairedVertL2,
											  long &direction, double &ExtrudeLength);

			//Begin-----------------------
			void AddVerticesOfFaces(CDB_Model *DDBB_Dec_Current, CDB_Model *DDBB_F2D, std::vector<long> faces);
			void CompleteEdges(CDB_Model *DDBB_Dec_Current, CDB_Model *DDBB_F2D,
							   std::vector<long> PairedVertL1,
							   std::vector<long> PairedVertL2);
			void AddEdgesOfFaces(CDB_Model *DDBB_Dec_Current, CDB_Model *DDBB_F2D, std::vector<long> faces);
			void AddDrawingFaces(CDB_Model *DDBB_Dec_Current, CDB_Model *DDBB_F2D, std::vector<long> faces);
			void AddDrawingToFeature(CDB_Model *DDBB_Dec_Current, 
									 CFeature &NewFeat, 
									 std::vector<long> faces,
									 std::vector<long> PairedVertL1,
									 std::vector<long> PairedVertL2);
			//End  -----------------------

			void PutSubgraphData(CDB_Model *DDBB_Dec_Current, CFeature &NewFeat);

			//Begin-----------------------
			void SetCrossSectionRS(CDB_Model *DDBB_Dec_Current,
								   CDB_Model *DB_DatumGraph,
								   long FeatureId, 
								   CFeature &NewFeat);

			void SetCrossSectionSP(CDB_Model *DDBB_Dec_Current, 
								   CDB_Model *DB_DatumGraph,
								   std::vector<long> PairedVertL1,
								   std::vector<long> PairedVertL2,
								   CFeature &NewFeat);

			void SetCrossSectionMid(CDB_Model *DDBB_Dec_Current, 
									CDB_Model *DB_DatumGraph,
									std::vector<long> PairedVertL1, 
									std::vector<long> PairedVertL2, 
									CFeature &Feature);

			void SetCrossSection(CDB_Model *DDBB_Dec_Current, 
								 CDB_Model *DB_DatumGraph,
								 std::vector<long> PairedVertL1, 
								 std::vector<long> PairedVertL2,
								 CFeature &NewFeat, 
								 long FeatureId);
			//End  -----------------------

			void ProfilePlaneOrientation(CDB_Model *DB_DatumGraph,
										 CDB_CSG *DB_CSG_Model, 
										 CFeature &Feature);

			//--Begin DefineDatumPlanes----------------------------------------------
			//private:
			void SplitEdge(CDB_Model *DB_DatumGraph,
						   CVertex V1, CVertex V2, long midVertex);
			void GetSymmetryPlaneForRS(CDB_Model *DDBB_Dec_Current, 
									   CDB_Model *DB_DatumGraph,
									   CFeature Feature, 
									   std::vector<long> PairedVertL1, 
									   std::vector<long> PairedVertL2, 
									   std::vector<long> &Vertices);
			void GetSymmetryPlanesForSP(CDB_Model *DBB_Dec_Current,
										CDB_Model *DB_DatumGraph,
										CDB_CSG *DB_CSG_Model, 
										CFeature Feature, 
										std::vector<long> PairedVertL1, 
										std::vector<long> PairedVertL2, 
										std::vector<long> &Vertices, 
										std::vector<CEdge> &Edges, long jj);

			bool IsSameDatumPlane(CDB_Model *DB_DatumGraph,
								  CDB_CSG *DB_CSG_Model,
								  long numPlane, 
								  long SubGraphVertices, 
								  std::vector<long> Vertices);
			bool IsDatumPlane(CDB_Model *DB_DatumGraph, 
							  CDB_CSG *DB_CSG_Model,
							  CFeature &Feature, 
							  std::vector<long> Vertices);
			void AddNewDatumPlane(CDB_Model *DB_DatumGraph,
								  CDB_CSG *DB_CSG_Model, 
								  CFeature &Feature, 
								  std::vector<long> CrossSectionVertices);
			void DatumPlaneForFeature(CDB_Model *DB_DatumGraph,
									  CDB_CSG *DB_CSG_Model, 
									  CFeature &Feature, 
									  std::vector<long> Vertices);

			//CVertex internalInsertionVertex(CDB_Model *DB_DatumGraph, 
			//								CDB_CSG *DB_CSG_Model, 
			//								std::vector<long> CrossSectionVertices);
			long internalInsertionVertex(CDB_Model *DB_DatumGraph, 
										 CDB_CSG *DB_CSG_Model, 
										 std::vector<long> CrossSectionVertices);
			CVertex GetIntersectionPoint(CVertex V1, CVertex V2, 
										 long dir2, CVertex V3);
			CVertex externalInsertionVertex(CDB_Model *DB_DatumGraph, 
											CDB_CSG *DB_CSG_Model, 
											CFeature Feature, 
											CVertex NearVert,
											std::vector<long> vertices,
											long &edgPos);
			void BridgeToContainer(CDB_Model *DDBB_Dec_Current,
								   CDB_Model *DB_DatumGraph,
								   CDB_CSG *DB_CSG_Model, 
								   CFeature Feature);

			void DefineDatumPlanesRD(CDB_Model *DDBB_Dec_Current,
									 CDB_Model *DB_DatumGraph,
									 CDB_CSG *DB_CSG_Model,
									 CFeature &Feature, 
									 std::vector<long> faces, 
									 long FeatureId);

			void DefineDatumPlanesRS(CDB_Model *DDBB_Dec_Current,
									 CDB_Model *DB_DatumGraph,
									 CDB_CSG *DB_CSG_Model,
									 std::vector<long> PairedVertL1, 
									 std::vector<long> PairedVertL2, 
									 CFeature &Feature, 
									 std::vector<long> faces, 
									 bool isBlind);

			void DefineDatumPlanesSP(CDB_Model *DDBB_Dec_Current,
									 CDB_Model *DB_DatumGraph,
									 CDB_CSG *DB_CSG_Model,
									 std::vector<long> PairedVertL1, 
									 std::vector<long> PairedVertL2, 
									 CFeature &Feature, 
									 std::vector<long> faces, 
									 long FeatureId);

			void DefineDatumPlanesCore(CDB_Model *DDBB_Dec_Current,
									   CDB_Model *DB_DatumGraph,
									   CDB_CSG *DB_CSG_Model,
									   std::vector<long> PairedVertL1, 
									   std::vector<long> PairedVertL2, 
									   CFeature &Feature, 
									   std::vector<long> faces, 
									   long FeatureId);

			//public:
			void DefineDatumPlanes(CDB_Model *DDBB_Dec_Current,
								   CDB_Model *DB_DatumGraph,
								   CDB_CSG *DB_CSG_Model,
								   std::vector<long> PairedVertL1, 
								   std::vector<long> PairedVertL2, 
								   CFeature &Feature, 
								   std::vector<long> faces, 
								   long FeatureId);
			//--End DefineDatumPlanes----------------------------------------------

			//public
			void CompleteFeatureData(CDB_Model *DDBB_Dec_Current,
									 CDB_Model *DB_DatumGraph,
									 CDB_CSG *DB_CSG_Model,
									 CFeature &NewFeat, 
									 long FeatureId, 
									 std::vector<long> &PairedVertL1,
									 std::vector<long> &PairedVertL2);
//--End CompleteFeatureData----------------------------------------------


//--Begin RemoveFeature------------------------------------
			//private:
			void RepairInterleavedEdgesRSRD(CDB_Model *DDBB_Dec_Current, 
											std::vector<long> PairedVertL1, 
											std::vector<long> PairedVertL2, 
											CFeature &Feature);
			void GetVerticesPositions(CDB_Model *DDBB_Dec_Current, 
									  CFeature Feature, 
									  std::vector<long> &FeatureVertices);
			void GettingFacesOfFeature(FEATURE_TYPE FeatureClass, 
									   long FeatureId, 
									   std::vector<long> &faces);

			void RemoveEdgesFromDec(CDB_Model *DDBB_Dec_Current, 
									std::vector<long> Vertices);
			void RemoveVerticesFromDec(CDB_Model *DDBB_Dec_Current, 
									   std::vector<long> Vertices);

			//public:
			void RemoveFeature(CDB_Model *DDBB_Dec_Current, 
							   std::vector<long> PairedVertL1, 
							   std::vector<long> PairedVertL2,
							   long FeatureId, 
							   CFeature &Feature,
							   bool FeatureFather);
//--End RemoveFeature------------------------------------


//--Begin ParametricProfiles----------------------------------------------
			//private:
			void GetOrthoDistancesToOrigins(CDB_Model *DB_DatumGraph,
											CDB_CSG *DB_CSG_Model,
											std::vector<std::vector<long>> predecessors);
			void GetOrthoDistancesToDatumPlanes(CDB_Model *DB_DatumGraph,
												CDB_CSG *DB_CSG_Model,
												std::vector<std::vector<long>> predecessors);

			void GetOrthoDistanceToFeature(CDB_Model *DB_DatumGraph,
										   CDB_CSG *DB_CSG_Model, 
										   std::vector<std::vector<long>> predecessors,
										   CFeature Feature, 
										   std::vector<COrthoDistance> &Distances);
			void TranslateToOrigin(CDB_CSG *DB_CSG_Model,
								   CFeature &Feature,
								   std::vector<COrthoDistance> &Distances);
			void TransformToSketchPlane(CDB_CSG *DB_CSG_Model,
										CFeature &Feature,
										std::vector<COrthoDistance> Distances);

			//public
			void GetParametricProfiles(CDB_Model *DB_DatumGraph,
									   CDB_CSG *DB_CSG_Model);
//--End ParametricProfiles----------------------------------------------


		public:
			CCSG_Deconstruct::CCSG_Deconstruct();
			CCSG_Deconstruct::~CCSG_Deconstruct();

			void SetUp();

			//Main function (called recursively)
			void DeconstructCSG_2D(CReferDoc* pDoc,
								   bool &FeatureFather);
	};
#endif // CSG_DECONSTRUCT_DEFINED