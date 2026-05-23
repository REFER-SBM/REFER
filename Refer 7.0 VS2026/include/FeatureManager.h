//+-----------------------------------------------------------------------------+
//| Copyright (C) 2009 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#if !defined FEATUREMANAGER_VISITED
#define FEATUREMANAGER_VISITED

	class CFeatureManager
	{
	private:
		bool DetectRibsAndDividers,
			DetectRailsAndSlots,
			DetectBosses,
			DetectStepsAndPockets,
			DetectChamfersAndRounds;    //TBD ChamfersAndRounds

		public:
			CFeatureManager();
			CFeatureManager(bool Set_DetectRibsAndDividers,
							bool Set_DetectRailsAndSlots,
							bool Set_DetectStepsAndPockets,
							bool Set_DetectChamfersAndRounds);    //TBD ChamfersAndRounds
			~CFeatureManager();

			void EraseFeatures();
			void FindFeatures(CDB_Model *DDBB);
	};
#endif // FEATUREMANAGER_VISITED
