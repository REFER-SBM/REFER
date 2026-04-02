#ifndef _GESTURES_
	#define _GESTURES_


	enum TStrokeToGesture {
		NoGesture,
		Delete
		//Translate,
		//Rotate,
		//Scale
	};


	class CGestures
	{
		private:
			bool IsDelete(CStroke InputStroke);

			void EditSketch(CStroke InputStroke,
							CDB_Sketch *DB_InputSketch,
							TStrokeToGesture Gesture);

		public:
			CGestures::CGestures();
			CGestures::~CGestures();

			bool ParseGestures(CStroke CurrentStroke,
							   CDB_Sketch *DB_InputSketch,
							   long RecognitionApproach,
							   long StrokeCornersApproach);
	};
#endif
