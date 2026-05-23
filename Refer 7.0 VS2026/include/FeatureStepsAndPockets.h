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

#ifndef FEATURESTEPSPOCKETS_VISITED
	#define FEATURESTEPSPOCKETS_VISITED


class CFeatureStepsAndPockets{
	private:
		std::vector<double> SubgraphAreas;

		//Input data
		double ThresholdShort, ThresholdLong;
		double ThresholdLength, ThresholdAngle;
		double ThresholdSimilarDistance, ThresholdSimilarAngle;
		double ThresholdAreaBases;

		//Face cues
		std::vector<double> edgeLength;
		std::vector<double> edgeAngle;
		std::vector<bool> FaceQuadrilateral;
		std::vector<double> FaceArea;
		std::vector<long> SmallToBigFaces;
		std::vector<bool> FaceSlender;
		std::vector<bool> Parallelogram;
		std::vector<long> SubgraphForFace;
		void DetermineFaceSubgraphs(CDB_Model *DDBB);
		void GetFaceCues(CDB_Model *DDBB);

		bool BasesAreSimilar(CDB_Model *DDBB, long lateral, long base1, long base2);
		//double AreaTriangle(CVertex v1, CVertex v2, CVertex v3);
		bool BasesHaveMoreArea(CDB_Model *DDBB, long lateral, long base1, long base2);


		void GetEdgesOfLateralFace(CDB_Model *DDBB, long lateral,long &long1, long &long2, long &short1, long &short2);
		bool GetBaseFaces(CDB_Model *DDBB, long Numgraph,
						  long lateral, long &base1, long &base2);

		bool IsAClosedVolume(CDB_Model *DDBB, long lateral, long base1, long base2);

		bool GetLateralFace(CDB_Model *DDBB, long Numgraph, 
							std::vector<long> faces, 
							long &lateral, long &base1, long &base2);


		bool GetOuterFaceContainingBaseFace(CDB_Model *DDBB, long outer, long numFace,
											std::vector<long> &outerfacescandidates);
		bool Intersection(CDB_Model *DDBB, CVertex P1, CVertex P2, long out);
		void FindShortesEdgesInGraph(CDB_Model *DDBB, long Numgraph, std::vector<long> &shortedges, std::vector<long> faces);
		void IntersectionBetweenVectorsOfEdges(CDB_Model *DDBB, 
											   std::vector<long> vectorI, 
											   std::vector<long> vectorO,
											   std::vector<long> &edgesintersection);
		void GetOuterEdgesIntersecting(CDB_Model *DDBB, long Numgraph, long outer,
									long containerface, std::vector<long> faces, 
									std::vector<long> &edgesintersection);

		double GetBoundingBoxArea(CDB_Model *DDBB, std::vector<long> Vertices);
		
		void DiscardingExternalSubgraphs(CDB_Model *DDBB, std::vector<long> &outerfacescandidates);

		long LookingForParallelFaces(CDB_Model *DDBB, long Numgraph,
									std::vector<long> bases, std::vector<long> outercandidates,
									std::vector<long> &containercandidates);

		long LookingForPerpendicularityEdges(CDB_Model *DDBB, long Numgraph,
									std::vector<long> bases, std::vector<long> outercandidates,
									std::vector<long> faces, std::vector<long> &containercandidates);

		void ObtainingMainBaseAndOuterFace(CDB_Model *DDBB, long &baseface, long &containerface,
									std::vector<long> bases, std::vector<long> containercandidates);

		long LookingForParallelismShortestEdges(CDB_Model *DDBB, long Numgraph, 
									std::vector<long> bases, std::vector<long> outercandidates, 
									std::vector<long> faces, std::vector<long> &containercandidates);

		long MainBase(CDB_Model *DDBB, long containerface, std::vector<long> bases);
		long LookForFaceInDDBB(CDB_Model *DDBB, CDB_Model Graph, CFace iface);
		void LoadBoundaryEdges();
		void BarycenterAdjoiningFaces(CDB_Model *DDBB, long containerface, CVertex &v1);

		void Output_Step_Pocket(CDB_Model *DDBB);

	public:
		void EraseStepsAndPockets();
		void GetStepsAndPockets(CDB_Model *DDBB);
};
#endif  //FEATURESTEPSPOCKETS_VISITED
