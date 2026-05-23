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

#ifndef _CUE_LABELS_VARLEY_LAMBAN_DEFINED
	#define _CUE_LABELS_VARLEY_LAMBAN_DEFINED

	typedef struct { lBOOLEAN Yes,No; } KNOWLEDGE;
	typedef struct { EDGE_INDEX E,F; MERIT M; } CANDIDATE;

	class CCueLabelsVarley_Lamban{
		private:
			CCueLabelsVarley_LabellingConstants *LC;
			CCueLabelsVarley_LabellingParameters *LP;

			KNOWLEDGE Knowledge[MAX_EDGES*MAX_EDGES/2];

			CANDIDATE Candidates[MAX_EDGES*MAX_EDGES/2];
			int MaxCandidates;
			int CandidateCount;

			MERIT tcMp;
			MERIT tcMd;
			MERIT pAnyTwoLinesAreParallel;
			MERIT pOppositeSidesOfQuadrilateralAreParallel;
			MERIT pContinueDespiteOneFailure;

			lNATURAL LambanAxisOptions;

			lBOOLEAN UseDeductionsInLamban;

			DEGREES plusI, minusI;
			DEGREES plusJ, minusJ;
			DEGREES plusK, minusK;
			BUNDLE Ibundle,Jbundle,Kbundle;

			lBOOLEAN FullFace[MAX_FACES];
			MERIT    FaceBundleMerit[MAX_FACES];
			MERIT tcOk;

			MERIT tcOe;
			MERIT tcOy;
			MERIT tcOz;

			VERTEX_INDEX Ttranslation[MAX_VERTICES];
			EDGE_INDEX CollinearLineE[MAX_COLLINEAR];
			EDGE_INDEX CollinearLineF[MAX_COLLINEAR];
			MERIT CollinearLineMerit[MAX_COLLINEAR];
			int CollinearLineCount;

			MERIT FoMIndeterminate;
			lREAL Iscaling,Jscaling,Kscaling;

			lBOOLEAN DirectionsAreFixed;

			MERIT tcOp;

			lBOOLEAN OldStyle;


			lINTEGER CountOfVariables;
			lINTEGER CountOfVariableVectors;

			lBOOLEAN VexityInversion;


			void LearnThatEdgesAreParallel (EDGE_INDEX A, EDGE_INDEX B);
			void LearnThatEdgesAreNotParallel (EDGE_INDEX A, EDGE_INDEX B);
			void DetermineKnowledgeFromLJunction (OBJECT *Object, VERTEX_INDEX V);
			void DetermineKnowledgeFromWJunction (OBJECT *Object, VERTEX_INDEX V);
			void DetermineKnowledgeFromYJunction (OBJECT *Object, VERTEX_INDEX V);
			void DetermineKnowledgeFromTJunction (OBJECT *Object, VERTEX_INDEX V);
			void DetermineKnowledgeFromT4Junction (LABELLING *Labelling, VERTEX_INDEX V);
			void DetermineKnowledgeFromMJunction (LABELLING *Labelling, VERTEX_INDEX V);
			void DetermineKnowledgeFromX4Junction (LABELLING *Labelling, VERTEX_INDEX V);
			void DetermineKnowledgeFromKJunction (LABELLING *Labelling, VERTEX_INDEX V);
			void DetermineKnowledgeFromZJunction (LABELLING *Labelling, VERTEX_INDEX V);
			void DetermineKnowledgeFromX5Junction (LABELLING *Labelling, VERTEX_INDEX V);
			void DetermineKnowledgeFromX6Junction (LABELLING *Labelling, VERTEX_INDEX V);
			void DetermineKnowledgeFromZ6Junction (LABELLING *Labelling, VERTEX_INDEX V);
			void InitialiseParallelKnowledge (OBJECT *Object);

			lBOOLEAN EdgesKnownToBeParallel (EDGE_INDEX A, EDGE_INDEX B);
			lBOOLEAN EdgesKnownNotToBeParallel (EDGE_INDEX A, EDGE_INDEX B);
			BUNDLE LowestUnusedBundleNumber (OBJECT *Object);
			void GroupEdgesTogether (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F);
			lBOOLEAN AreOppositeSidesOfQuadrilateral (OBJECT *Object, EDGE_INDEX A, EDGE_INDEX B);
			MERIT FoMAnglesParallel2D (DEGREES A, DEGREES B);
			MERIT FoMParallel2D (VECTOR2D *V1, VECTOR2D *V2);
			MERIT FoMLinesParallel (OBJECT *Object, EDGE_INDEX A, EDGE_INDEX B);
			void CreateParallelLineGroups (OBJECT *Object);

			int EdgesInBundle (OBJECT *Object, int Bundle);
			void GetBundleAngles (OBJECT *Object);
			void SortBundleNumbers (OBJECT *Object);

			lINTEGER HighestIndex4 (lREAL A, lREAL B, lREAL C, lREAL D);
			void ChooseIJKAxes (OBJECT *Object, BUNDLE *I, BUNDLE *J, BUNDLE *K);
			void ChooseIJAxes (OBJECT *Object, BUNDLE *I, BUNDLE *J);
			lREAL LengthOfBundle (OBJECT *Object, int Bundle);
			void SortBundleNumbersByLength (OBJECT *Object);
			DEGREES OppositeDirection (DEGREES FromDirection);
			DEGREES NormaliseDirection (DEGREES FromDirection);
			BUNDLE ChooseBisectingBundle (OBJECT *Object, BUNDLE Ibundle, BUNDLE Jbundle);
			lBOOLEAN FaceUsesAllBundles (OBJECT *Object, FACE_INDEX Nface);
			lBOOLEAN LoopUsesBundle (OBJECT *Object, LOOP *Loop, int Bundle);
			lINTEGER nuts (lINTEGER NotThis, lINTEGER NorThis, lINTEGER First,
						   lINTEGER Second, lINTEGER Third, lINTEGER Default);
			EDGE_INDEX EdgeLeavingLoopAtVertex (OBJECT *Object, LOOP *Loop, VERTEX_INDEX Vertex);
			lBOOLEAN IdentifyLambanAxes (OBJECT *Object);


			lINTEGER CountAndRecordTJunctions (LABELLING *Labelling);
			int CountCofacialPairs (OBJECT *Object, LABELLING *Labelling);
			int CountCollinearLines (OBJECT *Object, LABELLING *Labelling);
			MERIT FoMDistanceZero (double D);
			MERIT FoMLinesCollinear (OBJECT *Object, EDGE_INDEX Eab, EDGE_INDEX Ecd);
			OrthoStruct *InitialiseEquations (lINTEGER MaxEquations, lINTEGER Variables);
			void GetPerkinsDepthRatios (DEGREES A0, DEGREES A1, DEGREES A2, lREAL *R0, lREAL *R1, lREAL *R2);
			lINTEGER NextEquation (OrthoStruct *Set);
			void SetCoefficient (OrthoStruct *Set, lINTEGER Equation, lINTEGER Variable, lREAL Coefficient);
			void SetRHSValue (OrthoStruct *Set, lINTEGER Equation, lREAL Coefficient);
			lBOOLEAN SplitLengthIntoComponents (lREAL L, DEGREES Langle, lREAL *A, DEGREES Aangle, lREAL *B, DEGREES Bangle);
			int VariableIndex (LABELLING *Labelling, VERTEX_INDEX v, EDGE_INDEX e);
			lREAL Squarea (POINT2Dv *A, POINT2Dv *B, POINT2Dv *C);
			void AddDisjointSubgraphEquations (OBJECT *Object, LABELLING *Labelling, OrthoStruct *IEquations, OrthoStruct *JEquations, OrthoStruct *KEquations);
			lREAL RowProduct (lREAL Ra[], lREAL Rb[], lINTEGER RowSize);
			lREAL ColumnProduct (lREAL Ma[], lREAL Rb[], lINTEGER ColumnSize, lINTEGER RowSize);
			lBOOLEAN Ortholin2 (lREAL A[], lINTEGER Nrows, lINTEGER Ncols, lREAL B[], lREAL EPS, lREAL X[], lINTEGER MaxIterations);
			lBOOLEAN SolveEquations (OrthoStruct *Set);
			void DeleteEquations (OrthoStruct *Set);
			lREAL VariableValue (OrthoStruct *Set, lINTEGER VariableNumber);
			int Tvertex (LABELLING *Labelling, int n);
			lBOOLEAN SolveLambanEquations (OBJECT *Object);

			lREAL VariableSumOverAllVectors (lREAL VariableSpace[], lINTEGER Variable);
			void CreateTrialVariablesByReflecting (lREAL VariableSlot[], lREAL VariableTotals[], lREAL TryTheseVariables[]);
			void CreateTrialVariablesByReflectingAndExpanding (lREAL VariableSlot[], lREAL VariableTotals[], lREAL TryTheseVariables[]);
			void CreateTrialVariablesByContraction (lREAL VariableSlot[], lREAL VariableTotals[], lREAL TryTheseVariables[]);
			void UpdateVariableTotals (lREAL VariableSlot[], lREAL VariableTotals[], lREAL TryTheseVariables[]);
			void ContractAboutBestPoint(lREAL VariableSpace[], lREAL VariableTotals[], lREAL ResultVector[], lINTEGER BestSoFar, lREAL (*ObjectiveFunction)(lREAL []));
			lREAL DownhillOptimise (lINTEGER NumVars, lREAL StartValues[], lREAL DeltaValues[], lREAL (*ObjectiveFunction)(lREAL []), lINTEGER MaxIterations, lREAL Tolerance);
			lBOOLEAN DetermineZCoordinates (OBJECT *Object);

			lBOOLEAN BoundaryDepthInverted (OBJECT *Object);
			void InvertVexity (OBJECT *Object);

			lBOOLEAN GenerateLambanZCoordinates (OBJECT *Object);

			VERTEX_INDEX VertexOnLoop (LOOP *Loop, VERTEX_INDEX n);
			lREAL ChooseAppropriateZCoordinate (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Nvert);
			lBOOLEAN Cramer3 (lREAL augmatrix[], lREAL result[]);
			void NormaliseImplicitPlane (IMPLICIT_PLANE *Plane);
			void NegateVector (VECTOR3D *Vector);
			void MakeFaceVisible (IMPLICIT_PLANE *Plane);
			void SetFacePlane (OBJECT *Object, FACE_INDEX Nface, IMPLICIT_PLANE *FacePlane);
			lBOOLEAN MakeFaceEquation3 (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Vertices[]);
			void AllocateWorkspace (lINTEGER VertexCount, lREAL *A, lREAL *B);
			lBOOLEAN GetPlaneFittingWeightedVertices (lINTEGER VertexCount, POINT3D Vertices[], lREAL Weights[], IMPLICIT_PLANE *Plane);
			void MakeFaceEquationW (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Vertices[], int N, lREAL Weighting[]);
			void MakeFaceEquationN (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Vertices[], int N);
			lBOOLEAN MakeFaceEquationUsingVertexDepths (OBJECT *Object, FACE_INDEX Nface);
			lBOOLEAN MakeFaceNormalUsingCubicCorners (OBJECT *Object, FACE_INDEX Nface, VECTOR3D *Normal);
			void SetLineDirection (OBJECT *Object, EDGE_INDEX Nedge, lBOOLEAN IsKnown, DEGREES Direction);
			DEGREES GetLineDirection (OBJECT *Object, EDGE_INDEX Nedge);
			DEGREES CalculateLineDirection (OBJECT *Object, LABELLING *Labelling, EDGE_INDEX Nedge);
			void CalculateAllLineDirections (OBJECT *Object);
			DEGREES DirectionAlongLineFromJunction (OBJECT *Object, EDGE_INDEX Nedge, VERTEX_INDEX Nvert);
			void MakeFaceDistanceGivenNormal (OBJECT *Object, FACE_INDEX Nface, VECTOR3D *Normal);
			lBOOLEAN CCueLabelsVarley_Lamban::MakeFaceEquationForLambanLabelling (OBJECT *Object, FACE_INDEX Nface);
			lBOOLEAN FindRegionPlanes (OBJECT *Object, LABELLING *Labelling);

			MERIT FoMParallel3D (VECTOR3D *V1, VECTOR3D *V2);
			MERIT FoMPerpendicular3D (VECTOR3D *V1, VECTOR3D *V2);
			lBOOLEAN LabelUsingPlanes (OBJECT *Object);

		public:
			CCueLabelsVarley_Lamban(CCueLabelsVarley_LabellingConstants *eLC,
									CCueLabelsVarley_LabellingParameters *eLP);
			~CCueLabelsVarley_Lamban();

			lBOOLEAN LabelLinesUsingLamban (OBJECT *Object);

	};

#endif
