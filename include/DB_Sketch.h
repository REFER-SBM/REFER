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

#ifndef _DB_SKETCH
	#define _DB_SKETCH

	// Database for Sketches made out of strokes

	class CStrokePoint
	// Used to store coordinates of points sampled when moving the cursor in sketch mode
	// The current time is also stored, to later evaluate sequences
	{
		private:
			double x, y, time;
		public:
			CStrokePoint();
			CStrokePoint(double cx, double cy, double ctime);
			CStrokePoint(const CStrokePoint &IP);
			~CStrokePoint();
			bool operator==(const CStrokePoint& right) const;
			bool operator!=(const CStrokePoint& right) const;

			void SetPosition(double cx, double cy);
			POINT2D GetPosition();
			void SetTime(double ctime);
			double GetTime();
	};


////--------------------POINT DATA STRUCTURE---------------------------
//struct point
//{
//    double x; //X POSITION
//    double y; //Y POSITION
//    point *next; //POINTER TO NEXT NODE IN THE LIST
//    point *prev; //POINTER TO PREVIOUS NODE IN THE LIST
//    double angle; //INTERMEDIATE ANGLE VALUE STORAGE
//};
//
//
//
////--------------------GRAHAM'S SCAN FUNCTIONS---------------------------
//bool isConvexPoint(point *P); //TEST POINT FOR CONVEXITY
//void addPoint(point Point); //ADDS POINT TO DOUBLELY LINKED LIST (USED DURING SORTING)
//double findAngle(double x1, double y1, double x2, double y2); //FIND ANGLE GIVEN TWO POINTS
//void grahamInit(); //INITIALIZE VARIABLES, RANDOMLY GENERATE POINTS,
//                   //LOCATE MIN POINT, AND SORT POINTS BY RELATIVE ANGLES
//void grahamScan(point *P); //ACTUAL GRAHAM'S SCAN PROCEDURE
//void grahamMain(); //SETUP, RUN GRAHAM'S SCAN, AND DISPLAY RESULTS

	enum StrokeType{
		SOLID,
		DASHED
	};


	class CStroke
	// Stores information of a single stroke:
	//    StrokePoints  is a vector that contains pointers to the sequence of points
	//                  that belong to the same sketched stroke
	{
		private:
			std::vector<CStrokePoint> StrokePoints;
		public:
			CStroke();
			CStroke(const CStroke &IS);
			~CStroke();

			CStroke operator+(const CStroke& right) const;   //Adds stroke "right" at the end of the current stroke
			CStroke operator-(const CStroke& right) const;   //Adds stroke "right" at the end of the current stroke, but in reverse order!!

			//Type of line
			StrokeType Type;   //SOLID or DASHED

			//Points edits
			//------------
			void Clear();
			void AddPoint(CStrokePoint Point);
			CStrokePoint GetPoint(long iPoint);
			void SetPoint(long Position, CStrokePoint Point);
			void RemovePoint(long iPoint);
			CStroke GetSubStroke(long PositionI, long PositionF);

			//Stroke metrics
			//--------------
			long GetNumPoints();
			RECTANGLE2D GetBounds();
			double GetLength();
			double GetLength(long numIntervals);
			double GetSlope180_180();
			double GetSlope180();
			double GetMaxSegment();
			POINT2D GetStrokeCentroid();
			//std::vector<std::vector<double>> GetNormaliseMatrix();
			bool IsClockwise();
			void TurnIntoConvexHull();

			//Stroke edits
			//------------
			void Translate(POINT2D distance);
			void Rotate(POINT2D Centre, double angle);
			void Scale(POINT2D Centre, POINT2D Scale);
			void Filter(long FilterGap);
			bool Filter(double maxDistance);
			bool Resample(long NumResampledPoints);

			//Stroke cues
			//-----------
			long FindCorners(long StrokeCornersApproach);
			long NumCorners();

			//Corners
			std::vector <long> StrokeCorners;

			//Fittings and Merits
			SEGMENT2D Segment;
			double MeritLine;

			CArc Arc;
			double MeritArc;

			//--------------------------------------
			double worstOut_x;   //Used for display
			double worstOut_y;   //Used for display
			double t_Out;
			double worstIn_x;   //Used for display
			double worstIn_y;   //Used for display
			double t_In;
			//--------------------------------------

			void ClearFitsAndMerits();
	};

	class CDB_Sketch
	// Manages information of a set of strokes
	{
		private:
			// SketchStrokes is a vector containing all the strokes that belong to the same sketch
			// It is a vector of vectors, as it allows to group related strokes,
			//.like those resulting from segmenting a stroke that depicts a polyline
			// (See the class CSegmentStrokes)
			std::vector<CStroke> SketchStrokes;

			const int INSIDE, LEFT, RIGHT, BOTTOM, TOP;
			int ComputeOutCode(double x, double y,
							   double xmin, double ymin,
							   double xmax, double ymax);
			bool LineRectangleIntersection(double x0, double y0,
										   double x1, double y1,
										   double xmin, double ymin,
										   double xmax, double ymax);

		public:
			CDB_Sketch();
			CDB_Sketch(const CDB_Sketch &Sketch);
			~CDB_Sketch();

			void Clear();
			void CopyFrom(CDB_Sketch *Origin);

			//Strokes edits
			//-------------
			void AddStroke(CStroke Stoke);
			void InsertStroke(long iPosition, CStroke Stoke);
			CStroke GetStroke(long iStroke);
			void SetStroke(long iStroke, CStroke Stoke);
			void erase(long istroke);
			bool EraseSelectedStrokes(RECTANGLE2D rect);

			//Sketch metrics
			//--------------
			long GetNumStrokes();
			RECTANGLE2D GetBounds(long istroke);
			RECTANGLE2D GetBounds();
			POINT2D GetSketchCentroid();
			POINT2D GetSubSketchCentroid(std::vector<long> Selection);

			//Sketch edits
			//------------
			void Translate(long NumStroke, POINT2D distance);
			void Rotate(long NumStroke, POINT2D Centre, double angle);
			void Scale(long NumStroke, POINT2D Centre, POINT2D Scale);

			//Sketch cues
			//-----------
			void EraseSketchCorners();
			void EraseSketchFitsAndMerits();
	};
#endif  // DB_SKETCH
