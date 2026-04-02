
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

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "ReferDoc.h"

#include "FeatureRibsAndDividers.h"
#include "FeatureRailsAndSlots.h"
#include "FeatureStepsAndPockets.h"

#include "FeatureManager.h"


CFeatureManager::CFeatureManager()
{
	//Set flags as configured though the interactive menu
	DetectRibsAndDividers = ReferApp.GeneralParams.DetectRibsAndDividers;
	DetectRailsAndSlots = ReferApp.GeneralParams.DetectRailsAndSlots;
	DetectStepsAndPockets = ReferApp.GeneralParams.DetectStepsAndPockets;
	//DetectChamfersAndRounds = ReferApp.GeneralParams.DetectChamfersAndRounds;    //TBD ChamfersAndRounds
}


CFeatureManager::CFeatureManager(bool Set_DetectRibsAndDividers,
								bool Set_DetectRailsAndSlots,
								bool Set_DetectStepsAndPockets,
								bool Set_DetectChamfersAndRounds)    //TBD ChamfersAndRounds
:
	DetectRibsAndDividers (Set_DetectRibsAndDividers),
	DetectRailsAndSlots (Set_DetectRailsAndSlots),
	DetectStepsAndPockets (Set_DetectStepsAndPockets),
	DetectChamfersAndRounds (Set_DetectChamfersAndRounds)    //TBD ChamfersAndRounds
{
}


CFeatureManager::~CFeatureManager()
{
}

void CFeatureManager::EraseFeatures()
// Removes previous calculation of Features
{
	//Clear previous ribs and dividers
	CFeatureRibsAndDividers FRD;
	FRD.EraseRibsAndDividers();

	//Clear previous rails and slots
	CFeatureRailsAndSlots FRS;
	FRS.EraseRailsAndSlots();

	//Clear previous steps and pockets
	CFeatureStepsAndPockets FSP;
	FSP.EraseStepsAndPockets();

	//Clear previous chamfers and rounds
	//CFeatureChamfersAndRounds FChR;    //TBD ChamfersAndRounds
	//FChR.EraseChamfersAndRounds();    //TBD ChamfersAndRounds
}


void CFeatureManager::FindFeatures(CDB_Model *DDBB)
// Manages the sequential calculation of Features selected by the user

// WARNING!
// Some features are dependent on others
// Hence, the sequence is important
// Besides, every feature should check whether the information it requests as input
// has been previously calculated
{
	//Search for features according to the pre-defined sequence
	for(long i=0; i<(long)ReferApp.Fe.featuresSequence.size(); i++){
		switch(ReferApp.Fe.featuresSequence[i]){
			case RIB:
				//Find ribs and dividers
				if (DetectRibsAndDividers){
					if(DDBB->GetSizeFaces() > 0){
						CFeatureRibsAndDividers FRD;
						FRD.GetRibsAndDividers(DDBB);
					}
					else{
						AfxMessageBox("Faces required to find ribs and dividers");
					}
				}

				break;

			case RAIL:
				//Find rails and slots
				if (DetectRailsAndSlots){
					if(DDBB->GetSizeFaces() > 0){
						CFeatureRailsAndSlots FRS;
						FRS.GetRailsAndSlots(DDBB);
					}
					else{
						AfxMessageBox("Faces required to find rails and slots");
					}
				}

				break;

			case STEP:
				//Find steps and pockets
				if (DetectStepsAndPockets){
					if(DDBB->GetSizeFaces() > 0){
						if(ReferApp.Cu.SG.NumSubgraphs > 0){
							CFeatureStepsAndPockets FSP;
							FSP.GetStepsAndPockets(DDBB);
						}
						else{
							AfxMessageBox("Subgraphs required to find steps and pockets");
						}
					}
					else{
						AfxMessageBox("Faces required to find steps and pockets");
					}
				}

				break;

			//case CHAMFER:
			//	//Find chamfers and rounds
			//	if (DetectChamfersAndRounds){
			//		//To be done
			//		//Company P. and Varley P.A.C. 
			//		//A Method for Reconstructing Sketched Polyhedral Shapes with Rounds and Fillets. 
			//		//R. Taylor et al. (Eds.). Lecture Notes in Computer Science. 
			//		//(ISSN: 0302-9743(Print) 1611-3349 (Online), 
			//		//Smart Graphics 2010 Proceedings. Volume 6133. 2010, pp 152-155
			//	}
			//	break;
		}
	}
}