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

//Get faces from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003


#ifndef _CUE_LABELS_VARLEY_LABELLINGCONSTANTS_DEFINED
	#define _CUE_LABELS_VARLEY_LABELLINGCONSTANTS_DEFINED

	class CCueLabelsVarley_LabellingConstants{
		public:
			MERIT tcEa, tcEc, tcEd;
			MERIT tcKabcd, tcKcccd, tcKcdcd, tcKddcd;

			MERIT tcLba, tcLab, tcLac, tcLbd, tcLaa;

			MERIT tcMbcca, tcMbcda;
			MERIT tcMccdc, tcMcdcd, tcMcddc, tcMdccd, tcMdcdd;

			MERIT tcSo, tcSr;

			MERIT tcSm, tcSn, tcSp, tcSq, tcSu, tcSv, tcSw, tcSx, tcSy, tcSz;

			MERIT tcTa, tcTb, tcTc, tcTd, tcTe, tcTf, tcTg, tcTh, tcTi, tcTj;
			MERIT tcTk, tcTl, tcTm, tcTn, tcTo, tcTp, tcTq, tcTr, tcTs, tcTt;
			MERIT tcTu, tcTv, tcTw, tcTx, tcTy, tcTz;

			MERIT tcTbaa, tcTbac, tcTbad;
			MERIT tcTbda, tcTbdc;
			MERIT tcTaba, tcTbca, tcTbcc, tcTcca, tcTcda, tcTdda;

			MERIT tcWbca, tcWcdc, tcWdcd;
			MERIT tcWabc, tcWabd, tcWacc, tcWacd, tcWadc;
			MERIT tcWbaa, tcWbcd, tcWbda, tcWbdc, tcWbdd, tcWcac, tcWcbd, tcWdad;

			MERIT tcXabcd, tcXabdd;
			MERIT tcXcccc, tcXcccd, tcXcdcd, tcXcddd, tcXdddd;

			MERIT tcXccccc, tcXddddd;

			MERIT tcXcccccc, tcXcdcdcd, tcXdddddd;

			MERIT tcYabd, tcYccc, tcYddd;
			MERIT tcYaab, tcYabc, tcYacc, tcYacd, tcYadd;

			double tcOo;
			MERIT tcOp;
			double tcOq;
			MERIT tcOy;
			MERIT tcOz;

		private:
			void SetIntersectionTuningConstants ();

			void SetRelaxationStatisticalTuningConstants ();
			void SetRelaxationShapeTuningConstants ();
			void SetRelaxation10TuningConstants ();
			void SetRelaxation20HTuningConstants ();
			void SetRelaxation20STuningConstants ();
			void SetRelaxationTuningConstants (bool UseJunctionStatisticsForRelaxationLabelling,
											   bool UseShapePairStatisticsForRelaxationLabelling,
											   int RelaxationIterations,
											   bool UseMaxInRelaxationLabelling);

			void SetLambanFHTuningConstants ();
			void SetLambanFTuningConstants ();
			void SetLambanDTuningConstants ();
			void SetLambanVTuningConstants ();
			void SetLambanTuningConstants (bool UseMaxInRelaxationLabelling,
										   int LambanAxisOptions,
										   bool UseDeductionsInLamban);

			void SetSemiCombinedTuningConstants ();

			void SetCombinedFTuningConstants ();
			void SetCombinedDTuningConstants ();
			void SetCombinedVTuningConstants ();
			void SetCombinedTuningConstants (int LambanAxisOptions,
											 bool UseDeductionsInLamban);

		public:
			CCueLabelsVarley_LabellingConstants();
			~CCueLabelsVarley_LabellingConstants();
			void SetLabellingConstants(short int LabellingProcedure,
									   bool UseJunctionStatisticsForRelaxationLabelling,
									   bool UseShapePairStatisticsForRelaxationLabelling,
									   int RelaxationIterations,
									   bool UseMaxInRelaxationLabelling,
									   int LambanAxisOptions,
									   bool UseDeductionsInLamban);
	};

#endif  //_CUE_LABELS_VARLEY_LABELLINGCONSTANTS_DEFINED
