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

#ifndef FEATURERAILSANDSLOTS_VISITED
	#define FEATURERAILSANDSLOTS_VISITED

	class CFeatureRailsAndSlots
	{
		private:
			//Parameters
			std::vector<long>BackupSlots;

			double ThresholdShort, ThresholdLong;
			double ThresholdLength, ThresholdAngle;
			double ThresholdSimilarDistance, ThresholdSimilarAngle;

			//Face cues
			std::vector<double> edgeLength;
			std::vector<double> edgeAngle;
			std::vector<bool> FaceQuadrilateral;
			std::vector<double> FaceArea;
			std::vector<long> SmallToBigFaces;
			std::vector<bool> FaceSlender;
			std::vector<bool> Parallelogram;

			void GetFaceCues(CDB_Model *DDBB);


			//Functions
			void RemoveVertexFromPolygon(long vert, std::vector<long> &containerface1);
			bool ContainerAreaMinorThanFeature(CDB_Model *DDBB, long containerface, long bottom,
												long &vert1, std::vector<long> &containerpolygon);

			bool IsVertexInteriorToPolygon(CDB_Model *DDBB, long vert, std::vector<long> polygon);

			void ConvertToRailData(CDB_Model *DDBB);

			bool NewFaceIsLateralFace(CDB_Model *DDBB, long bottom2, long lateral1, bool rail);

			bool GetCandidateRailSlot(CDB_Model *DDBB, long &centralFace, 
									  long &connect1, long &connect2,
									  long &lateral1, long &lateral2);

			bool RailFitsSymmetry(CDB_Model *DDBB);

			long GetMinorArea(CDB_Model *DDBB, long bottom2, std::vector<long> FeatureFaces);

			bool EdgesIntersection(CDB_Model *DDBB, long edge, long lateral1, long lateral2);
			void GetContainerFace(CDB_Model *DDBB, long centralFace, 
								  long lateral1, long lateral2, 
								  long &containerface, long &numBlind);

		public:
			void EraseRailsAndSlots();
			void GetRailsAndSlots(CDB_Model *DDBB);
	};
#endif  //FEATURERAILSANDSLOTS_VISITED
