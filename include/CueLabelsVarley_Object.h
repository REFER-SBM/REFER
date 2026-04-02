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

//Get labels from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003

#ifndef _CUE_LABELS_VARLEY_OBJECT_DEFINED
	#define _CUE_LABELS_VARLEY_OBJECT_DEFINED

	typedef short int VERTEX_INDEX, EDGE_INDEX, FACE_INDEX;
	typedef unsigned long int TYPEMARK;
	typedef struct { VERTEX_LABEL Shape; TYPEMARK Vexity; } JUNCTION_TYPEMARK;
	typedef unsigned short int BUNDLE;
	typedef unsigned short int GRAPHLOOP;
	typedef unsigned long int BITS32;

	typedef struct { lREAL x,y; } POINT2Dv, VECTOR2D;
	typedef struct { lREAL x, y, z; } POINT3D, VECTOR3D;
	// Express the equation of a line as a*x+b*y+c = 0
	typedef struct { VECTOR2D Normal;  lREAL Distance; } IMPLICIT_LINE2;

	//  The normal vector is then [a,b], and the distance [c]
	////extern char *Line2String (IMPLICIT_LINE2 *Line);
	////
	////typedef struct { POINT2Dv  p0, p1;  IMPLICIT_LINE2 LineGradient; } IMPLICIT_LINE2_SEGMENT;
	////
	////extern char *Vector3DString (VECTOR3D *Vector);
	////extern char *Vector2DString (VECTOR2D *Vector);
	////
	//////  Express the equation of a face as p*x+q*y+r*z+c = 0
	//////  The normal vector is then [p,q,r], and the distance [c]
	////typedef struct { VECTOR3D Normal;  lREAL Distance; } IMPLICIT_PLANE;
	////extern char *PlaneString (IMPLICIT_PLANE *Plane);


	typedef struct VERTEX_DEFINITION
	{
	   lREAL         OccludingZ;              /*        3D, T-junctions only */
	   GRAPHLOOP     VertexSubgraph;          /* 2D        */
	   lBOOLEAN      XYKnown;                 /* 2D and 3D */
	   lBOOLEAN      DepthKnown;              /*        3D */
	   int           FaceCount;               /* 2D and 3D */
	   lBOOLEAN      IsHidden;                /*        3D */
	   ////NEIGHBORHOOD Neighbourhood;           /*        3D */
	   MERIT         C2Symmetry;              /* 2D and 3D */
	   MERIT         C3Symmetry;              /* 2D and 3D */
	   MERIT         C4Symmetry;              /* 2D and 3D */
	   MERIT         C5Symmetry;              /* 2D and 3D */
	   MERIT         C6Symmetry;              /* 2D and 3D */
	   FACE_INDEX    OccludesThisFace;        /* 2D and 3D */
	   FACE_INDEX    Findex[MAX_FACES_PER_VERTEX];          /* 2D and 3D */
	} VERTEX;

	typedef struct
	{
	   VERTEX_INDEX StartVertex;
	   VERTEX_INDEX EndVertex;
	   EDGE_INDEX   BaseLine;
	   lBOOLEAN Direction;
	   lBOOLEAN Occludes;
	} HALFEDGE;

	typedef struct EDGE_DEFINITION
	{
	   GRAPHLOOP      EdgeSubgraph;             /* 2D        */
	   lBOOLEAN       OnSubgraphBoundary;       /* 2D        */
	   IMPLICIT_LINE2 LineEquation;             /* 2D        */
	   lBOOLEAN       EquationKnown;            /* 2D        */

	   lBOOLEAN       LineDirectionKnown;       /* 2D        */
	   DEGREES        DrawnLineDirection;       /* 2D        */

	   BUNDLE         EdgeBundle;               /* 2D        */
	   lBOOLEAN       WasOriginallyOccluding;   /*        3D */

	   ////int            ParasolidNumber;          /*        3D */
	   ////int            ParasolidEntity;          /*        3D */
	   VECTOR3D       EdgeVector3D;
	   ////VECTOR3D       InsideDirection;          /*        3D */

	   lBOOLEAN       IsHidden;                 /*        3D */
	   ////MERIT          C2Symmetry;               /* 2D and 3D */
	   ////MERIT          MirrorSymmetry;           /* 2D and 3D */
	   lBOOLEAN       IsPretend;                /* 2D and 3D */    /* pretend edges might be introduced for tetrahedral vertices */

	   ////LINECURVEDATA  LineCurveData;            /* 2D and 3D */

	}  EDGE;

	typedef struct
	{
	   VERTEX_INDEX Vertex;
	   TYPEMARK     Vexity;
	   MERIT        Merit;
	}  LABELLING_HYPOTHESIS;

	typedef struct
	{
	   ////GRAPHLOOP OuterLoop, InnerLoop;
	   VERTEX_INDEX OuterVertex, InnerVertex;
	   VERTEX_INDEX OuterNextInLoop, InnerNextInLoop;
	   VERTEX_INDEX OuterPrevInLoop, InnerPrevInLoop;
	   JUNCTION_TYPEMARK *ExpectedOuterType, *ExpectedInnerType;
	   lBOOLEAN  IsBoss;
	   lREAL     Xcoeff,Ycoeff;
	   MERIT     Merit;
	} COFACIAL_CONFIGURATION;

	typedef struct { VERTEX_INDEX Number; lBOOLEAN OccludesFace; char Turn; }  CORNER;
	typedef struct { EDGE_INDEX Number; lBOOLEAN OccludesFace,IsReversed; } SIDE;

	typedef struct LOOP_DEFINITION
	{
	   GRAPHLOOP  Subgraph;             /* 2D        */
	   short int  CornerCount;          /* 2D and 3D */
	   short int  SideCount;            /* 2D and 3D */
	   //SYMCLASS   Symmetry;             /* 2D and 3D */
	   //LOOPCLASS  LoopType;           /* 2D and 3D */
	   CORNER     Corners[MAX_SIDES_PER_LOOP];
	   SIDE       Sides[MAX_SIDES_PER_LOOP];
	   int        ParasolidLoopNumber;  /*        3D */
	   int        ParasolidLoopEntity;  /*        3D */
	} LOOP;

	typedef struct { VECTOR3D Normal;  lREAL Distance; } IMPLICIT_PLANE;

	typedef struct PLANE_DEFINITION
	{
	   unsigned short IsEquationKnown;
	   IMPLICIT_PLANE PlaneEquation3D;
	} PLANE;

	typedef struct FACE_DEFINITION
	{
	   BUNDLE         NormalBundle;         /* 2D and 3D */
	   int            ParasolidNumber;      /*        3D */
	   int            ParasolidEntity;      /*        3D */
	   int            PlaneNumber;          /*        3D */
	   lBOOLEAN       IsComplete;           /* 2D        */
	   lBOOLEAN       IsPartialFace;        /*        3D */
	   lBOOLEAN       IsHidden;             /*        3D */
	   FUZZY          IsSimpleConvex;       /* 2D and 3D */
	   int            NumberOfLoops;
	   LOOP           Loops[MAX_LOOPS_PER_FACE];
	} FACE;

	typedef struct
	{
		int FeatureType;
		VERTEX_INDEX L,T,W,Y,A,B;
		EDGE_INDEX   El,Et,Ew,Ey;
		lBOOLEAN     Used;
		MERIT        Merit;
	} FEATURE;


	typedef struct MIRROR_DEFINITION
	{
	   lBOOLEAN   AIsVertex, BIsVertex;
	   int        ANumber,   BNumber;
	   FACE_INDEX Face;
	   RADIANS    alpha, beta;
	   MERIT      Merit;
	}  MIRROR;

	typedef struct CHAIN_DEFINITION
	{
	   struct CHAIN_DEFINITION *Next;        /* for use in chains, normally NULL */
	   FACE_INDEX Index;
	   int        MirrorCount;
	   int        MirrorNumber[16];
	   int        FaceCount;
	   FACE_INDEX Faces[16];
	   int        PointCount;
	   POINT3D    Points[32];
	   MERIT      Merit;
	   IMPLICIT_PLANE Equation;
	   VERTEX_INDEX MirroredVertices[MAX_VERTICES];
	   lBOOLEAN   Used;
	}  CHAINED_MIRRORS;

	typedef struct LABELLING_DEFINITION
	{
		VERTEX_INDEX      VertexCount;
		EDGE_INDEX        EdgeCount;
		JUNCTION_TYPEMARK VertexLabel[MAX_VERTICES];
		lBOOLEAN          VertexLabellingIsFinalised[MAX_VERTICES];
		TYPEMARK          FinalVertexTypes;
		TYPEMARK          VertexUnderlyingType[MAX_VERTICES];
		int               VECount[MAX_VERTICES];
		EDGE_INDEX        VENumber[MAX_VERTICES][MAX_EDGES_PER_VERTEX];
		VERTEX_INDEX      EdgeStartVertex[MAX_EDGES];
		VERTEX_INDEX      EdgeEndVertex[MAX_EDGES];
		FACE_INDEX        EdgeForwardFace[MAX_EDGES];
		FACE_INDEX        EdgeReverseFace[MAX_EDGES];
		TYPEMARK          EdgeLabel[MAX_EDGES];
		lBOOLEAN          EdgeLabelIsFinalised[MAX_EDGES];
		MERIT             Merit;
	} LABELLING;


	//Object
	//------
	typedef struct
	{
	   /* Header Information */
	   ////BITS32          ProcessingStatus;
	   int             MaxHedralVertex;
	   ////int             LabellingType;
	   int             FeatureCount;
	   int             CofacialCount;
	   ////FACE_INDEX      InitialFaceCount;
	   GRAPHLOOP       SubgraphCount;
	   ////lBOOLEAN        IsGraphConnected;

	   // extremities and mid-points, as measured in object coordinates
	   double          LeftX,  RightX,  MidX;
	   double          TopY,   BottomY, MidY;
	   double          FrontZ, BackZ;

	   // larger distance between extremities, left-to-right and top-to-bottom
	   double          MaxBound;

	   // the distance, in object coordinates, which corresponds to a distance of 1.0 in window coordinates
	   double          ViewScale;
	   ////double          ShortestEdgeLength;
	   ////double          LongestEdgeLength;

	   ////MERIT           MirrorMerit;
	   ////VERTEX_INDEX    MirroredVertices[MAX_VERTICES];

	   ////int             EllipseCount;

	   // Vertex Information
	   VERTEX          Vertices[MAX_VERTICES];
	   EDGE_INDEX      HiddenByEdge[MAX_VERTICES];            /* 2D and 3D */
	   POINT3D         Coordinates[MAX_VERTICES];             /* 2D and 3D */

	   // Edge Information
	   EDGE            Edges[MAX_EDGES];
	   lBOOLEAN        EdgeExistsTable[MAX_VERTICES][MAX_VERTICES];

	   LABELLING       Labelling;
	   lBOOLEAN        IsOnBoundary[MAX_VERTICES];
	   // relaxation labelling
	   MERIT           Vprobabilities[MAX_VERTICES][32];
	   EPROBABILITIES  Eprobabilities[MAX_EDGES];

	   // Plane Information
	   FACE_INDEX      PlaneCount;
	   PLANE           Planes[MAX_FACES];

	   // Face Information
	   FACE_INDEX      FaceCount;
	   FACE            Faces[MAX_FACES];
	   LOOP            Background;

	   // Cofacial Information
	   COFACIAL_CONFIGURATION CofacialConfigurations[MAX_COFACIAL];

	   // Feature Information
	   FEATURE         Features[MAX_FEATURES];

	   // Mirror Information
	   int             MirrorCount;
	   int             ChainCount;
	   MIRROR          Mirrors[MAX_MIRROR];
	   CHAINED_MIRRORS *Chains[MAX_MIRROR];

	   //Bundle information
	   BUNDLE          BundleCount;
	   ////BUNDLE          BaseBundle[2];
	   ////BUNDLE          VerticalBundle;
	   ////int             MirrorBundle;
	   DEGREES         BundleAngles[MAX_BUNDLES];
	   //////VECTOR3D        BundleVectors[MAX_BUNDLES];
	   ////VERTEX_INDEX    BaseVertices[3];
	   //////IMPLICIT_PLANE  BasePlane;

	   /////* Categories */
	   ////int             Genus;
	   ////short int       PocketBoss[MAX_GRAPHLOOPS];
	   ////CATEGORY        Category;

	   ////short int       TopologyMechanism;

	   ////ELLIPSE         Ellipses[MAX_ELLIPSES];
	   ////lBOOLEAN        AlignmentInitialised;
	   ////char            Comment[80];
	}  OBJECT;


	VERTEX_INDEX NewVertex (OBJECT *Object);
	EDGE_INDEX NewEdge (OBJECT *Object, lBOOLEAN IsHidden, VERTEX_INDEX Nfrom, VERTEX_INDEX Nto);
	FACE_INDEX NewFace (OBJECT *Object, lBOOLEAN IsHidden);
	int VertexEdgeIndex (OBJECT *Object, VERTEX_INDEX V, EDGE_INDEX E);

	void CheckVertexNumber (LABELLING *Labelling, VERTEX_INDEX n, char *Text);
	VERTEX_INDEX EdgeOtherEnd (OBJECT *Object, EDGE_INDEX Nedge, VERTEX_INDEX Nvert);
	double DistanceFromJunctionToJunction (OBJECT *Object, VERTEX_INDEX From, VERTEX_INDEX To);
	void ReplaceInterceptsAtSplit (OBJECT *Object, EDGE_INDEX OldEdge, EDGE_INDEX NewEdge, VERTEX_INDEX Start, VERTEX_INDEX AsFarAs);
	//void ConvertT3JunctionToT4 (OBJECT *Object, VERTEX_INDEX Nvert);
	//void ReturnT3JunctionsToT4 (OBJECT *Object);
	FACE_INDEX FaceContainingEdges (OBJECT *Object, EDGE_INDEX E1, EDGE_INDEX E2);
	VERTEX_INDEX VertexIndexOnLoop (LOOP *Loop, VERTEX_INDEX Nvert);
	EDGE_INDEX EdgeIndexOnLoop (LOOP *Loop, EDGE_INDEX Nedge);
	void RemoveVertexFromLoopRecord (LOOP *Loop, VERTEX_INDEX Nold);
	void ReplaceEdgeInLoopRecord (LOOP *Loop, EDGE_INDEX Nold, EDGE_INDEX Nnew);
	void RemoveFaceFromVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface);
	void RenumberEdge (OBJECT *Object, EDGE_INDEX Nold, EDGE_INDEX Nnew);
	void RemoveEdgeFromVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nold);
	void RemoveEdgeFromFaceRecord (OBJECT *Object, FACE_INDEX Nface, EDGE_INDEX Old);
	void DeleteEdge (OBJECT *Object, EDGE_INDEX Nedge);
	lBOOLEAN NoVerticesRemainOnFace (FACE *Face);
	void DeleteFace (OBJECT *Object, FACE_INDEX Nface);
	void RemoveFaceFromEdgeRecord (OBJECT *Object, EDGE_INDEX Nedge, FACE_INDEX F);
	void ReplaceFaceInVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Old, FACE_INDEX New);
	void ReplaceFaceInEdgeRecord (OBJECT *Object, EDGE_INDEX Nedge, FACE_INDEX Old, FACE_INDEX New);
	void RenumberFace (OBJECT *Object, FACE_INDEX Nold, FACE_INDEX Nnew);
	void ReplaceEdgeInVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nold, EDGE_INDEX Nnew);


	lBOOLEAN AnyTypemarkChanges(bool Set, lBOOLEAN NewATC);
	lBOOLEAN UseBinaryFrontalGeometry(bool Set, lBOOLEAN NewUBFG);

	void ChangeLine (LABELLING *Labelling, EDGE_INDEX Nedge, TYPEMARK Mask);
	void ChangeVertex (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Mask);

	TYPEMARK LineIsA (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Start);
	TYPEMARK LineIsB (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Start);


	typedef struct
	{
	   MERIT Wbcamerit, Wcdcmerit, Wdcdmerit;
	   MERIT Wabcmerit, Wabdmerit, Waccmerit, Wacdmerit, Wadcmerit;
	   MERIT Wbaamerit, Wbbamerit, Wbcdmerit, Wbdamerit, Wbdcmerit, Wbddmerit;
	   MERIT Wcabmerit, Wcacmerit, Wcbcmerit, Wcbdmerit, Wccbmerit, Wcdamerit, Wcdbmerit;
	   MERIT Wdabmerit, Wdacmerit, Wdadmerit, Wdbdmerit, Wdcamerit, Wdcbmerit, Wddamerit;
	}  WPROBABILITIES;

	typedef struct
	{
	   MERIT Xabcdmerit, Xbcdamerit, Xcdabmerit, Xdabcmerit;
	   MERIT Xabdcmerit, Xbdcamerit, Xdcabmerit, Xcabdmerit;
	   MERIT Xabddmerit, Xbddamerit, Xddabmerit, Xdabdmerit;
	   MERIT Xccccmerit, Xcccdmerit, Xccdcmerit, Xcdccmerit, Xdcccmerit;
	   MERIT Xcdcdmerit, Xdcdcmerit;
	   MERIT Xcdddmerit, Xdcddmerit, Xddcdmerit, Xdddcmerit, Xddddmerit;
	}  XPROBABILITIES;

	typedef struct
	{
	   MERIT Ycccmerit, Ydddmerit, Yabdmerit, Ybdamerit, Ydabmerit;
	   MERIT Yaabmerit, Yabamerit, Ybaamerit;
	   MERIT Yabbmerit, Ybbamerit, Ybabmerit;
	   MERIT Yabcmerit, Ybcamerit, Ycabmerit;
	   MERIT Yaccmerit, Yccamerit, Ycacmerit;
	   MERIT Yacdmerit, Ycdamerit, Ydacmerit;
	   MERIT Yaddmerit, Yddamerit, Ydadmerit;
	   MERIT Ybccmerit, Yccbmerit, Ycbcmerit;
	   MERIT Ybdcmerit, Ydcbmerit, Ycbdmerit;
	   MERIT Ybddmerit, Yddbmerit, Ydbdmerit;
	}  YPROBABILITIES;

		void LabelEdgeAsArrow (OBJECT *Object, EDGE_INDEX Nedge);
		void LabelEdgeAsBackArrow (OBJECT *Object, EDGE_INDEX Nedge);
		void LabelEdgeAsNotArrow (OBJECT *Object, EDGE_INDEX Nedge);
		void LabelEdgeAsNotBackArrow (OBJECT *Object, EDGE_INDEX Nedge);

		static lBOOLEAN FVCoplanarPropagated[MAX_FACES][MAX_VERTICES];
		static lBOOLEAN FECoplanarPropagated[MAX_FACES][MAX_VERTICES];

		static MERIT PFaceAndVertexCoplanar[MAX_FACES][MAX_VERTICES];
		static FUZZY FaceAndVertexCoplanar[MAX_FACES][MAX_VERTICES];
		static FUZZY FaceAndEdgeCoplanar[MAX_FACES][MAX_VERTICES];

		void SetFaceAndVertexCoplanar (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Nvert);
		void SetFaceAndEdgeCoplanar (OBJECT *Object, FACE_INDEX Nface, EDGE_INDEX Nedge);

		void SetFaceAndVertexNotCoplanar (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Nvert);
		void SetFaceAndEdgeNotCoplanar (OBJECT *Object, FACE_INDEX Nface, EDGE_INDEX Nedge);

		lBOOLEAN StoreEdgeInVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
		void ReplaceVertexInEdgeRecord (OBJECT *Object, EDGE_INDEX Nedge, VERTEX_INDEX Nold, VERTEX_INDEX Nnew);

		lBOOLEAN VertexLabelsIdentical (JUNCTION_TYPEMARK *A, JUNCTION_TYPEMARK *B);

		double LineLength (OBJECT *Object, EDGE_INDEX Nedge);
		DEGREES DirectionFromPoint2ToPoint2 (POINT2Dv *FromPoint, POINT2Dv *ToPoint);
		DEGREES CalculateLineDirection (OBJECT *Object, LABELLING *Labelling, EDGE_INDEX Nedge);
		DEGREES GetLineDirection (OBJECT *Object, EDGE_INDEX Nedge);

		EDGE_INDEX EdgeOnLoop (LOOP *Loop, EDGE_INDEX n);
		lBOOLEAN VertexIsInLoop (LOOP *Loop, VERTEX_INDEX Nvert);
		lBOOLEAN EdgeIsInLoop (LOOP *Loop, EDGE_INDEX Nedge);
		lBOOLEAN EdgeLeavesLoop (OBJECT *Object, LOOP *Loop, EDGE_INDEX Nedge);
		FACE_INDEX EdgeOtherFace (OBJECT *Object, EDGE_INDEX Nedge, FACE_INDEX F);

	lREAL DistanceFromPoint2ToLine2 (IMPLICIT_LINE2 *Line, POINT2Dv *P);

	DEGREES DirectionFromJunctionToJunction (OBJECT *Object, VERTEX_INDEX From, VERTEX_INDEX To);

	typedef struct
	{
	   lINTEGER MaximumEquations;
	   lINTEGER CurrentEquationCount;
	   lINTEGER NumberOfUnknowns;
	   lREAL  *EquationCoefficients;
	   lREAL  *EquationValues;
	   lREAL  *UnknownValues;
	   lBOOLEAN *CoefficientSet;
	}  OrthoStruct;


	VECTOR3D *GetVectorFromPoint3ToPoint3 (VECTOR3D *V, POINT3D *A, POINT3D *B);
	VECTOR3D *AddVector (VECTOR3D *Vector, VECTOR3D *Add);
	lBOOLEAN NormaliseVector (VECTOR3D *Vector);

	int GetVFaceNormal (OBJECT *Object, FACE_INDEX Nface, VECTOR3D *Normal);

	double DotProduct (VECTOR3D *Vector1, VECTOR3D *Vector2);
	VERTEX_INDEX VertexOnLoop (LOOP *Loop, VERTEX_INDEX n);

	void GetEdgeCentre (OBJECT *Object, EDGE_INDEX Nedge, POINT3D *Point);

	int GetFacePlane (OBJECT *Object, FACE_INDEX Nface, IMPLICIT_PLANE *FacePlane);

	void MakeEdgeVector (OBJECT *Object, EDGE_INDEX Nedge);

	VECTOR3D *GetCrossProduct (VECTOR3D *CrossProduct, VECTOR3D *A, VECTOR3D *B);

	double VectorMagnitude (VECTOR3D *V);

	void NormaliseImplicitPlane (IMPLICIT_PLANE *Plane);
	void FindImplicitPlane (IMPLICIT_PLANE *Plane, POINT3D *Point1, POINT3D *Point2, POINT3D *Point3);


#endif
