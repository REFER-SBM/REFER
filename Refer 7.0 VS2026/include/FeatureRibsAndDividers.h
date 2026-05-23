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

#ifndef FEATURERIBSANDDIVIDERS_VISITED
	#define FEATURERIBSANDDIVIDERS_VISITED

	class CFeatureRibsAndDividers
	{
		private:
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
			bool GetThinWall(CDB_Model *DDBB, long &border,
							 long &connect1, long &connect2,
							 long &lateral1, long &lateral2);

			bool RibFitsSymmetry(CDB_Model *DDBB);

			//bool DividerIsNotASubgraph(CDB_Model *DDBB, long NumDiv);

			std::vector<long> GetContainerFaces(CDB_Model *DDBB,
												long border,
												long lateral1, long lateral2);

			void ConvertRibInDivider(CDB_Model *DDBB, long border, long lateral1, long lateral2);

			bool ClosingEdgeIsShort(CDB_Model *DDBB, long lateral1, long lateral2);

		public:
			void EraseRibsAndDividers();
			void GetRibsAndDividers(CDB_Model *DDBB);
	};
#endif  //FEATURERIBSANDDIVIDERS_VISITED
