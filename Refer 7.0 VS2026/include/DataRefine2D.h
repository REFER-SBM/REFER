
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

#if !defined DATAREFINE2D_DEFINED
	#define DATAREFINE2D_DEFINED

	class CDataRefine2D{

		public:
		//PARAMETERS TO CONTROL 2D REFINEMENT
		bool bMergeVertices;

		bool IgnoreShortEdges;
		double ToleranceIgnoreShortEdges;

		bool MergeCubicCorners;
		double DistanceMergeCubicCorners;
		bool MergeAlignedCubicCorners;
		double MergeRotationCubicCorners;
		bool MergeCarefulLineTips;
		double MergeMaxDistVertices;
		double MergeMaxVerticesRotation;
		double ReificationMerit;
		bool MergeCasualLineTips;
		double ReMergeCasualInTol;
		double ReMergeCasualOutTol;
		bool ReMergeVerticesTriplets;
		double ReMergeMaxDistTriplets;
		double EmergentTripletsMerit;
		bool ReMergeVerticesDangling;
		double ReMergeMaxDistDangling;
		double EmergenceDanglingDeMerit;

		bool Merge_T_Vertices;
		bool Split_T_Vertices;
		double ToleranceSplitTVertices;

		bool MergeCollinearEdges;
		double ToleranceMergeCollinearEdges;

		bool SplitNon2ManifoldVertices;
		bool SplitNon2ManifoldEdges;

		bool bOptimize2D;

		double ErrorVerticality2D;
		double ErrorParallelism2D;
		double ErrorCollinearity2D;

		TListOfRegularities Regularities2D;

		CDataOptHC DataOptHC_2D;

		//PARAMETERS TO CONTROL DISTORSION
		double CoeficienteDistorsion;


		CDataRefine2D();
		~CDataRefine2D();
	};

#endif // DATAREFINE2D_DEFINED