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

#include "DB_Sketch.h"
#include "CIRecognizer.h"   // CALI library
#include "Gestures.h"


// Recognition approaches
#define REFER	0
#define CALI	1


CGestures::CGestures()
{
}


CGestures::~CGestures()
{
}


bool CGestures::IsDelete(CStroke InputStroke)
//Returns true is the stroke depicts a delete gesture

//The stroke is a delete gesture if all four conditions are true:
//   1. It is a polyline
//   2. the polyline contains more than 2 lines
//   3. The angles between consecutive lines are all acute (<22.5 degrees)
//   4. The angles between consecutive lines are opposite (the polyline zigzags)
{
	//check if stroke contains more than 2 lines
	long num_corners= InputStroke.NumCorners();
	if(num_corners<2)
		return false;

	//Check if all angles are accute and zigzaging
	int zigzag=0;
	double angmax=(NUMBER_PI/8);  // Acute means <22.5 degrees

	for (long ipoint=0; ipoint<num_corners; ipoint++){
		POINT2D p1= InputStroke.GetPoint(InputStroke.StrokeCorners[ipoint]).GetPosition();
		POINT2D p2= InputStroke.GetPoint(InputStroke.StrokeCorners[ipoint+1]).GetPosition();
		POINT2D p3= InputStroke.GetPoint(InputStroke.StrokeCorners[ipoint+2]).GetPosition();

		double ang1;
		if (p1.x!=p2.x)
			ang1=atan2((p1.y-p2.y),(p1.x-p2.x));
		else
			ang1=NUMBER_PI/2;

		double ang2;
		if(p3.x!=p2.x)
			ang2=atan2((p3.y-p2.y),(p3.x-p2.x));
		else
			ang2=NUMBER_PI/2;

		if(p2.x>p1.x || p2.x>p3.x){
			if(ang1<0)
				ang1=2*NUMBER_PI+ang1;
			if (ang2<0)
				ang2=2*NUMBER_PI+ang2;
		}

		if (std::fabs(ang1-ang2)<angmax)
			zigzag++;
		else
			return false;  //At least one angle is not accute
	}
	if (zigzag>1)return true;
	return false;
}


void CGestures::EditSketch(CStroke InputStroke,
						   CDB_Sketch *DB_InputSketch,
						   TStrokeToGesture Gesture)
// Edits the sketch, according to the command linked to the current gesture:

// Valid gestures are:
//		Delete, which is a "scratch" sign aimed at deleting all intersected strokes
{
	switch (Gesture){
		// If the scribble is a scratch command,
		// all lines intersecting its enclosing rectangle are deleted
		case Delete:{
			// Find the enclosing rectangle of the scratch gesture
			RECTANGLE2D rect= InputStroke.GetBounds();

			// Delete all strokes intersecting the enclosing rectangle
			DB_InputSketch->EraseSelectedStrokes(rect);

		}// End case Delete
		break;
	}//End switch Gesture

	return;
}


bool CGestures::ParseGestures(CStroke CurrentStroke,
							  CDB_Sketch *DB_InputSketch,
							  long RecognitionApproach,
							  long StrokeCornersApproach)
// Looks for gestures in the current stroke and proceeds accordingly

// Gestures must only convey actions to edit the sketch
// Valid detections are:
//		Delete, which is a scracth gesture to delete all intersected strokes of DB_Sketch

// Two different approaches to recognize gestures may be used:
//		REFER
//		CALI
{
	bool IsGesture=false;
	TStrokeToGesture Gesture= NoGesture;

	switch (RecognitionApproach){
		case REFER:{
			//Get corners of the input stroke (Required to parse gestures)
			CurrentStroke.FindCorners(StrokeCornersApproach);

			//DELETE GESTURE
			if (IsDelete(CurrentStroke)){
				IsGesture= true;
				Gesture= Delete;
			}
		}// End case REFER
		break;

		case CALI:{
			//Start CALI library
			CIRecognizer *CaliRecognizer;
			CaliRecognizer = new CIRecognizer();
			CIStroke *CALIStroke;      // A Stroke is the set of points from pen-down to pen-up

			// Define a new CALI stroke
			CALIStroke = new CIStroke();

			// Copy the InputStroke into the CALI stroke
			for (long ipoint=0; ipoint<CurrentStroke.GetNumPoints(); ipoint++){
				//Add the point to the current CALI stroke
				CStrokePoint PointI= CurrentStroke.GetPoint(ipoint);
				POINT2D c= PointI.GetPosition();
				CALIStroke->addPoint((int)c.x, (int)c.y);
			}

			// Define a new scribble that contains just the current stroke
			CIScribble *CALIScribble;  // A Scribble is a sequence of strokes collected within the timeout value
			CALIScribble = new CIScribble();    
			CALIScribble->addStroke(CALIStroke);

			// CALI recognizes the scribble
			CIList<CIGesture *>* gest=CaliRecognizer->recognize(CALIScribble);

			// Recognizing the scribble
			if (!strcmp("Command",(*gest)[0]->getGestureType())){
				char *name = (*gest)[0]->getName();
				if(!strcmp("Delete", name)){
					IsGesture= true;
					Gesture= Delete;
				}
			}

			//Stop CALI library
			if (gest) delete gest;
			delete CALIScribble;
			delete CALIStroke;
			delete CaliRecognizer;
		}// End case CALI
		break;

	}//End switch

	//Update the sketch following the command conveyed by the gesture
	if(IsGesture)
		EditSketch(CurrentStroke, DB_InputSketch, Gesture);

	return IsGesture;
}
