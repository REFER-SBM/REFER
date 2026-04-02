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

#ifndef DB_CSG_VISITED
	#define DB_CSG_VISITED

	//--------------------------------------------------------------------
	// ORTHOGONAL DISTANCES
	class COrthoDistance
	// Orthogonal Distance is a triplet 
	// containing (Delta x, Delta y, Delta z)
	// components of a distance segment.

	//It is useful to store "orthogonal distances",
	//which are distances along a path
	//whose succesive segments are aligned with one of the main axes
	{
		public:
			//Components
			double x, y, z;

		public:
			COrthoDistance();
			COrthoDistance(double xx, double yy, double zz);
			COrthoDistance(COrthoDistance const &OD);
			~COrthoDistance();
			void Clear();
	};


	//--------------------------------------------------------------------
	//DATUMS
	enum CPlaneOrientation{
		XY,
		XZ,
		YZ,
		SLANTED,
		UNKNOWN
	};

	class CDatumPlane
	{
		private:
			CPlaneOrientation Orientation;
			long SubGraph;
			long Origin;
			long IsSymmetry;
			double normalDistance;
			std::vector<long> PlaneVertices;
			long IDDPlane;

		public:
			CDatumPlane();
			CDatumPlane(const CDatumPlane &DP);
			~CDatumPlane();
			void Clear();

			void SetOrientation(CPlaneOrientation InOrientation);
			CPlaneOrientation GetOrientation();

			void SetSubGraph(long InSubGraph);
			long GetSubGraph();

			void SetOrigin(long InOrigin);
			long GetOrigin();

			void SetIsSymmetry(long InIsSymmetry);
			long GetIsSymmetry();

			void SetNormalDistance(double InNormalDistance);
			double GetNormalDistance();

			void SetPlaneVertices(std::vector<long> InPlaneVertices);
			std::vector<long> GetPlaneVertices();

			long PlaneVerticesSize();
			void AddPlaneVertex(long InPlaneVertex);
			long GetPlaneVertex(long Position);

			void SetIDDPlane(long InIDDPlane);
			long GetIDDPlane();
	};


	//class CDatumAxis
	//{
	//	private:
	//		long IDDAxis;

	//	public:
	//		CDatumAxis();
	//		CDatumAxis(const CDatumAxis &DA);
	//		~CDatumAxis();
	//		void Clear();
	//};


	class CDatums
	{
		private:
			std::vector<long> DatumOrigins;
			std::vector<COrthoDistance> OrthoDistancesOrigins;
			std::vector<CDatumPlane> DatumPlanes;
			//std::vector<CDatumAxis> DatumAxes;

		public:
			CDatums();
			CDatums(const CDatums &DD);
			void Clear();
			~CDatums();

			//Datum origins
			long DatumOriginsSize();
			void SetDatumOrigins(std::vector<long>InOrigins);
			std::vector<long> GetDatumOrigins();
			long GetDatumOrigin(long numOrigin);
			void SetOrthoDistancesOrigins(std::vector<COrthoDistance>InOrthoDistancesOrigins);
			std::vector<COrthoDistance> GetOrthoDistancesOrigins();

			//Datum planes
			long DatumPlanesSize();
			void AddDatumPlane(CDatumPlane InDatumPlane);
			void RemoveDatumPlane(long Position);

			void SetDatumPlane(CDatumPlane InDatumPlane, long Position);
			CDatumPlane GetDatumPlane(long Position);

			long GetDatumPlaneSubGraph(long numPlane);
			CPlaneOrientation GetDatumPlaneOrientation(long numPlane);
			void SetDatumPlaneNormalDistance(long numPlane, double inNormalDistance);
			double GetDatumPlaneNormalDistance(long numPlane);
			void SetDatumPlaneIsSymmetry(long numPlane, long InIsSymmetry);
			long GetDatumPlaneIsSymmetry(long numPlane);

			void SetDatumPlaneIDDPlane(long numPlane, long inIDDPlane);
			long GetDatumPlaneIDDPlane(long numPlane);

			long DatumPlaneVerticesSize(long numPlane);
			std::vector<long> GetDatumPlaneVertices(long numPlane);
			long GetDatumPlaneVertex(long numPlane, long Position);

			//Datum axes
			//void SetDatumAxes(std::vector<CDatumAxis> InDatumAxis);
			//std::vector<CDatumAxis> GetDatumAxes();

			//long DatumAxesSize();
			//void AddDatumAxis(long InDatumAxis);
			//long RemoveDatumAxis(long Position);
			//void SetDatumAxis(CDatumAxis InDatumAxis, long Position);
			//CDatumAxis GetDatumAxis(long Position);
	};


	//--------------------------------------------------------------------
	// FEATURES
	enum FEATURE_TYPE{
		CORE,
		NODE,
		RIB,
		DIVIDER,
		RAIL,
		SLOT,
		STEP,
		POCKET,
		NONE
	};

	//enum ELEMENTS_TYPE{
	//	VERTEX_CSG,
	//	EDGE_CSG,
	//	FACE_CSG
	//};

	//enum GEOMETRIC_CONSTRAINT{
	//	COINCIDENT,
	//	COLLINEAR,
	//	COPLANAR,
	//	INTERSECTION,
	//	PARALLEL,
	//	PERPENDICULAR,
	//	HORIZONTAL,
	//	VERTICAL
	//};


	//class CProfileConstraint
	//// Geometric constraints of the Parametric Profile
	//{
	//	private:
	//		GEOMETRIC_CONSTRAINT GeomConstraint;
	//		//std::vector<ELEMENTS_TYPE> ElementsType;
	//		std::vector<std::vector<long>> Elements;
	//		std::vector<long> ParallelGroups;

	//	public:
	//		CProfileConstraint();
	//		CProfileConstraint(const CProfileConstraint &PC);
	//		~CProfileConstraint();
	//};


	class CFeature
	//FEATURES
	{
		private:
			FEATURE_TYPE FeatureType;
			double Merit;
			long SubGraph;
			std::vector<std::vector<long>> ContainerFaces;  //Dandidate datums
			std::vector<long> DatumsId;

			//Drawing 2D of the isolated feature
			//(We use B-Rep model database as it manages vertices, edges and faces)
			CDB_Model DB_Feature2D;

			//Cross section
			std::vector<long> CrossSectionVertices;

			//Parametric profile
			std::vector<CVertex> ParametricProfileCoorVertices;

			//Extrusion;
			CPlaneOrientation ProfileOrientation;   // XY=0, XZ=1, YZ=2
			long ExtrusionDirection;  //Directions X= 0, Y= 1 or Z= 2
			double LengthExtrusion;
			double RotationAngle;  //Rotation applied to align the parametric profile to the X-Y axes

		public:
			CFeature();
			CFeature(const CFeature &FT);
			~CFeature();

			void Clear();

			//Type
			void SetType(FEATURE_TYPE InFeatureType);
			FEATURE_TYPE GetType();

			//Merit
			void SetMerit(double InMerit);
			double GetMerit();

			//Subgraph
			void SetSubGraph(long InSubGraph);
			long GetSubGraph();

			//Container faces
			void AddContainerFace(std::vector<long> InContainerFace);
			std::vector<long> GetContainerFace(long numFace);

			long ContainerFacesSize();
			long ContainerFaceSize(long numFace);
			long ContainerFaceGetVertex(long numFace, long numVertex);

			//Datums
			long DatumIdSize();
			void AddDatumId(long numDatum);
			void RemoveDatumId(long Position);
			void SetDatumId(long numDatum, long Position);
			long GetDatumId(long Position);

			//Feature 2D drawing
			void SetFeature2D(CDB_Model InDB_F);   //Upload BRep database after changes
			CDB_Model GetFeature2D();              //Download BRep database before changes

			//Cross section
			//void SetCrossSection(std::vector<CVertex> In_CS);
			std::vector<long> GetCrossSection();
			//void ClearCrossSection();
			long SizeCrossSection();
			void AddCrossSectionVertex(long In_vertex);
			//void SetCrossSectionVertex(long In_vertex, long Position);
			long GetCrossSectionVertex(long Position);

			//Parametric profile
			//void SetParametricProfile(std::vector<CVertex> InProfile);
			//std::vector<CVertex> GetParametricProfile();
			//void ClearParametricProfile();
			long SizeParametricProfile();
			void AddParametricProfileVertex(CVertex In_vertex);
			void SetParametricProfileVertex(CVertex In_vertex, long Position);
			CVertex GetParametricProfileVertex(long Position);

			//Extrusion
			void SetProfileOrientation(CPlaneOrientation InProfileOrientation);
			CPlaneOrientation GetProfileOrientation();

			void SetExtrusionDirection(long InExtrusionDirection);
			long GetExtrusionDirection();

			void SetLengthExtrusion(double InLengthExtrusion);
			double GetLengthExtrusion();

			//void SetRotationAngle(double InRotationAngle);
			//double GetRotationAngle();
	};


	//-------------------------------------------------------------------------------
	// CSG MODEL
	class CDB_CSG{
		private:
			CDatums Datums;

			//Single active "internal" branch
			std::vector<CFeature> Branch;

			//Tree that contains all the saved branches (and their features)
			std::vector<std::vector<CFeature>> Tree;

		public:
			CDB_CSG();
			CDB_CSG(CDB_CSG const &DB);
			~CDB_CSG();

			void Clear();

			//Datums
			void ClearDatums();

			long DatumOriginsSize();
			void SetDatumOrigins(std::vector<long>InDatumOrigins);
			std::vector<long> GetDatumOrigins();
			long GetDatumOrigin(long numOrigin);
			void SetOrthoDistancesOrigins(std::vector<COrthoDistance>InOrthoDistancesOrigins);
			std::vector<COrthoDistance> GetOrthoDistancesOrigins();

			long DatumPlanesSize();
			void AddDatumPlane(CDatumPlane InDatumPlane);
			void SetDatumPlane(CDatumPlane InDatumPlane, long Position);
			CDatumPlane GetDatumPlane(long numDatumPlane);

			//------------------------------------------------------------
			long GetDatumPlaneSubGraph(long numPlane);
			void SetDatumPlaneIDDPlane(long numPlane, long inIDDPlane);
			long GetDatumPlaneIDDPlane(long numPlane);
			CPlaneOrientation GetDatumPlaneOrientation(long numPlane);
			void SetDatumPlaneNormalDistance(long numPlane, double inNormalDistance);
			double GetDatumPlaneNormalDistance(long numPlane);
			void SetDatumPlaneIsSymmetry(long numPlane, long InIsSymmetry);
			long GetDatumPlaneIsSymmetry(long numPlane);
			long DatumPlaneVerticesSize(long numPlane);
			std::vector<long> GetDatumPlaneVertices(long numPlane);
			long GetDatumPlaneVertex(long numPlane, long Position);
			//------------------------------------------------------------

			//Active (in progress) Branch of Features
			void ClearBranch();
			long BranchSize();
			void AddFeatureToBranch(CFeature &Feature);
			CFeature GetFeatureFromBranch(long Position);

			//Tree of Branches
			void ClearTree();

			void AddTreeBranch();   //Add the active branch to the tree

			long TreeBranchesSize();
			long TreeFeaturesSize();
			long TreeBranchSize(long numBranch);

			void SetFeature(long numBranch, long Position, CFeature InFeature);
			void SetFeature(long ith, CFeature InFeature);
			CFeature GetFeature(long numBranch, long Position);
			CFeature GetFeature(long ith);
	};
#endif  //DB_CSG
