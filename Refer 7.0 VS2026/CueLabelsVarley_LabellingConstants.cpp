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

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_LabellingConstants.h"


CCueLabelsVarley_LabellingConstants::CCueLabelsVarley_LabellingConstants()
{
	tcEa = 0.1200;  // a non-parameter for lamban and combin, but live for basic relaxation
	tcEc = 0.4500;  // a non-parameter for lamban and combin, but live for basic relaxation
	tcEd = 0.4300;  // a non-parameter for lamban and combin, but live for basic relaxation

	tcOo = MULTIPLIER_LAMBAN_OCCLUDING * DEFAULT_MERIT_LAMBAN_OCCLUDING + 1.0;
	tcOp = 0.0000;
	tcOq = MULTIPLIER_LAMBAN_CONFIDENCE * DEFAULT_MERIT_LAMBAN_CONFIDENCE;
	tcOy = 0.6250;
	tcOz = 0.9375;

	tcSo = 0.0000;  // known to be the best value?
	tcSr = 0.1144;  // as good as any (doesn't make much difference)?

	tcSm = 0.000;
	tcSn = 0.000;
	tcSp = 0.000;
	tcSq = 1.000;
	tcSu = 1.000;
	tcSv = 0.100;
	tcSw = 0.751;
	tcSx = 0.791;
	tcSy = DEFAULT_MERIT_DIFFERENT_SUBGRAPHS;
	tcSz = DEFAULT_MERIT_DIFFERENT_FEATURES;

	tcTa = 0.601;
	tcTb = 0.239;
	tcTc = 0.937;
	tcTd = 0.268;
	tcTe = 0.558;
	tcTf = 0.431;
	tcTg = 0.247;
	tcTh = 0.720;
	tcTi = 0.498;
	tcTj = 0.657;
	tcTk = DEFAULT_MERIT_AA2332_MIXED;
	tcTl = DEFAULT_MERIT_MIRROR_DISCRETE;
	tcTm = DEFAULT_MERIT_MIRROR_MACRO;
	tcTn = DEFAULT_MERIT_SUSPICIOUS_EDGE;
	tcTo = DEFAULT_MERIT_NEW_TRIANGLES;
	tcTp = DEFAULT_MERIT_BAD_VERTEX_POSITION;
	tcTq = DEFAULT_MERIT_QUADRILATERAL_LOOP;
	tcTr = DEFAULT_MERIT_EDGELENGTH_DROPOFF;
	tcTs = DEFAULT_MERIT_EDGELENGTH_POWER;
	tcTt = 0.976;
	tcTu = DEFAULT_MERIT_V232_MIXED;
	tcTv = DEFAULT_MERIT_V2332_CONVEX;
	tcTw = DEFAULT_MERIT_V2332_MIXED;
	tcTx = DEFAULT_MERIT_BIAS_232;
	tcTy = DEFAULT_MERIT_BIAS_2332;
	tcTz = DEFAULT_MERIT_K_INTERPRETATION;
}


CCueLabelsVarley_LabellingConstants::~CCueLabelsVarley_LabellingConstants()
{
}


void CCueLabelsVarley_LabellingConstants::SetIntersectionTuningConstants ()
{
   tcLba  = 0.950;
   tcLab  = 1.000;
   tcLac  = 0.9875;
   tcLbd  = 0.9695;
   tcLaa  = 0.649;

   tcMbcca = 0.883;

   tcTbaa = 1.075;
   tcTbac = 1.000;
   tcTbad = 1.000;
   tcTbda = 0.956;

   tcWbca = 0.965;
   tcWcdc = 1.000;
   tcWdcd = 1.021;
   tcWbda = 0.841;
   tcWbaa = 0.815;

   tcTaba = 0.832;
   tcXcccc = 0.905;
   tcYabd = 1.002;
   tcYccc = 1.016;
   tcYddd = 0.957;
   tcYaab = 0.7355;
   tcYabc = 0.637;
   tcYacc = 0.895;

   tcKabcd = 0.4975;
   tcKcccd = 0.5155;
   tcKcdcd = 0.538;
   tcKddcd = 0.5085;

   tcMbcda = 0.8865;
   tcMccdc = 0.593;
   tcMcdcd = 0.460;
   tcMcddc = 0.560;
   tcMdccd = 0.610;
   tcMdcdd = 0.580;

   tcTbdc = 0.964;
   tcTbca = 0.7945;
   tcTbcc = 0.813;
   tcTcca = 0.820;
   tcTcda = 0.820;
   tcTdda = 0.833;

   tcWabc = 0.804;
   tcWabd = 0.855;
   tcWacc = 0.820;
   tcWacd = 0.785;
   tcWadc = 0.883;
   tcWbcd = 0.872;
   tcWbdc = 0.867;
   tcWbdd = 0.797;
   tcWcac = 0.774;
   tcWcbd = 0.856;
   tcWdad = 0.847;

   tcXabcd = 0.8545;
   tcXabdd = 0.859;
   tcXcccd = 0.847;
   tcXcdcd = 0.84;
   tcXcddd = 0.701;
   tcXdddd = 0.736;

   tcYacd = 0.004;
   tcYadd = 0.005;
   tcYacd = 0.879;
   tcYadd = 0.855;

   tcXccccc = 1.00;
   tcXddddd = 0.90;

   tcXcccccc = 1.00;
   tcXcdcdcd = 1.00;
   tcXdddddd = 1.00;
}


void CCueLabelsVarley_LabellingConstants::SetRelaxationStatisticalTuningConstants ()
{
   tcEa  = 0.25;
   tcEc  = 0.25;
   tcEd = 0.25;

   tcKabcd = 0.013;
   tcKcccd = 0.316;
   tcKcdcd = 0.105;
   tcKddcd = 0.066;

   tcLba  = 0.097;
   tcLab  = 0.157;
   tcLac  = 0.313;
   tcLbd  = 0.056;
   tcLaa  = 0.004;

   tcMbcca = 0.178;
   tcMbcda = 0.014;
   tcMccdc = 0.123;
   tcMcdcd = 0.096;
   tcMcddc = 0.068;
   tcMdccd = 0.068;
   tcMdcdd = 0.110;

   tcTbaa = 0.174;
   tcTbac = 0.109;
   tcTbad = 0.356;
   tcTbda = 0.009;
   tcTbdc = 0.021;
   tcTaba = 0.002;
   tcTbca = 0.002;
   tcTbcc = 0.009;
   tcTcca = 0.030;
   tcTcda = 0.012;
   tcTdda = 0.007;

   tcWbca = 0.219;
   tcWcdc = 0.621;
   tcWdcd = 0.090;
   tcWabc = 0.003;
   tcWabd = 0.003;
   tcWacc = 0.003;
   tcWacd = 0.003;
   tcWadc = 0.005;
   tcWbaa = 0.001;
   tcWbcd = 0.004;
   tcWbda = 0.001;
   tcWbdc = 0.004;
   tcWbdd = 0.003;
   tcWcac = 0.003;
   tcWcbd = 0.003;
   tcWdad = 0.002;

   tcXabcd = 0.001;
   tcXabdd = 0.001;
   tcXcccc = 0.642;
   tcXcccd = 0.254;
   tcXcdcd = 0.039;
   tcXcddd = 0.022;
   tcXdddd = 0.022;

   tcYccc = 0.915;
   tcYddd = 0.038;
   tcYabd = 0.026;
   tcYaab = 0.001;
   tcYabc = 0.001;
   tcYacc = 0.002;
   tcYacd = 0.004;
   tcYadd = 0.005;

   tcXccccc = 0.972;
   tcXddddd = 0.014;

   tcXcccccc = 0.556;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}


void CCueLabelsVarley_LabellingConstants::SetRelaxationShapeTuningConstants ()
{
   tcEa  = 0.25;
   tcEc  = 0.25;
   tcEd = 0.25;

   tcKabcd = 1.000;
   tcKcccd = 1.000;
   tcKcdcd = 0.105;
   tcKddcd = 0.066;

   tcLba  = 1.000;
   tcLab  = 0.157;
   tcLac  = 0.313;
   tcLbd  = 0.056;
   tcLaa  = 0.004;

   tcMbcca = 1.000;
   tcMbcda = 0.014;
   tcMccdc = 0.123;
   tcMcdcd = 0.096;
   tcMcddc = 0.068;
   tcMdccd = 0.068;
   tcMdcdd = 0.110;

   tcTbaa = 1.000;
   tcTbac = 0.109;
   tcTbad = 0.356;
   tcTbda = 0.009;
   tcTbdc = 0.021;
   tcTaba = 0.002;
   tcTbca = 0.002;
   tcTbcc = 0.009;
   tcTcca = 0.030;
   tcTcda = 0.012;
   tcTdda = 0.007;

   tcWbca = 1.000;
   tcWcdc = 0.621;
   tcWdcd = 0.090;
   tcWbaa = 0.001;
   tcWabc = 0.003;
   tcWabd = 0.003;
   tcWacc = 0.003;
   tcWacd = 0.003;
   tcWadc = 0.005;
   tcWbcd = 0.004;
   tcWbda = 0.001;
   tcWbdc = 0.004;
   tcWbdd = 0.003;
   tcWcac = 0.003;
   tcWcbd = 0.003;
   tcWdad = 0.002;

   tcXabcd = 1.000;
   tcXabdd = 0.001;
   tcXcccc = 0.642;
   tcXcccd = 0.254;
   tcXcdcd = 0.039;
   tcXcddd = 0.022;
   tcXdddd = 0.022;

   tcYccc = 0.915;
   tcYddd = 0.038;
   tcYabd = 0.026;
   tcYaab = 0.001;
   tcYabc = 0.001;
   tcYacc = 0.002;
   tcYacd = 0.004;
   tcYadd = 0.005;

   tcXccccc = 1.000;
   tcXddddd = 0.014;

   tcXcccccc = 1.000;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}


void CCueLabelsVarley_LabellingConstants::SetRelaxation10TuningConstants ()
{
   tcLba  = 0.051;
   tcLab  = 0.073;
   tcLac  = 0.340;
   tcLbd  = 0.184;
   tcLaa  = 0.268;
   tcMbcca = 0.141;
   tcMbcda = 0.109;
   tcTbaa = 0.211;
   tcTbac = 0.107;
   tcTbad = 0.254;
   tcTbda = 0.046;
   tcTbdc = 0.096;
   tcWbca = 0.276;
   tcWcdc = 0.205;
   tcWdcd = 0.2685;
   tcWbda = 0.001;
   tcWbaa = 0.002;
   tcWacd = 0.003;
   tcWcac = 0.137;
   tcTaba = 0.141;
   tcTbca = 0.001;
   tcXcccc = 0.385;
   tcXcccd = 0.455;
   tcXabcd = 0.246;
   tcXabdd = 0.018;
   tcYccc = 0.438;
   tcYddd = 0.110;
   tcYabd = 0.391;
   tcYaab = 0.177;
   tcYabc = 0.127;
   tcYacc = 0.155;
   tcYadd = 0.050;
   tcYacd = 0.062;
   tcKabcd = 0.257;
   tcKcccd = 0.4605;
   tcEc   = 0.227;
   tcEc   = 0.246;
   tcEd   = 0.300;

   tcKcdcd = 0.3085;
   tcKddcd = 0.280;

   tcMccdc = 0.074;
   tcMcdcd = 0.021;
   tcMcddc = 0.148;
   tcMdccd = 0.177;
   tcMdcdd = 0.002;
   tcXccccc = 0.242;
   tcXddddd = 0.001;

   tcTbcc = 0.033;
   tcTcca = 0.145;
   tcTcda = 0.126;
   tcTdda = 0.044;

   tcWabc = 0.200;
   tcWabd = 0.125;
   tcWacc = 0.111;
   tcWadc = 0.006;
   tcWbcd = 0.091;
   tcWbdc = 0.194;
   tcWbdd = 0.165;
   tcWcbd = 0.158;
   tcWdad = 0.002;

   tcXcdcd = 0.276;
   tcXcddd = 0.006;
   tcXdddd = 0.013;

   tcXcccccc = 1.00;
   tcXcdcdcd = 1.00;
   tcXdddddd = 1.00;
}


void CCueLabelsVarley_LabellingConstants::SetRelaxation20HTuningConstants ()
{
   tcSm    = 0.0677;
   tcSp    = 0.5729;
   tcSq    = 0.8500;
   tcLba   = 0.0904;
   tcLab   = 0.5500;
   tcLac   = 0.1908;
   tcLbd   = 0.1021;
   tcLaa   = 0.0243;
   tcMbcca = 0.4047;
   tcMbcda = 0.4500;
   tcTbaa  = 0.9861;
   tcTbac  = 0.0322;
   tcTbad  = 0.9165;
   tcTbda  = 0.0000;
   tcTbdc  = 0.0000;
   tcWbca  = 0.9637;
   tcWcdc  = 0.4680;
   tcWdcd  = 0.1102;
   tcWbda  = 0.0901;
   tcWbaa  = 0.2100;
   tcWacd  = 0.0747;
   tcWcac  = 0.2717;
   tcWbcd  = 0.0440;
   tcWbdc  = 0.2429;
   tcTaba  = 0.1425;
   tcTbca  = 0.0000;
   tcTdda  = 0.1502;
   tcXcccc = 0.1998;
   tcXcccd = 0.3871;
   tcXabcd = 0.8555;
   tcXabdd = 0.5119;
   tcYccc  = 0.9526;
   tcYddd  = 0.9500;
   tcYabd  = 0.5197;
   tcYaab  = 0.0302;
   tcYabc  = 0.0299;
   tcYacc  = 0.2952;
   tcYadd  = 0.1502;
   tcYacd  = 0.0757;
   tcKabcd = 0.9331;
   tcKcccd = 0.5435;
   tcEa    = 0.7460;
   tcEc    = 0.0011;
   tcEd    = 0.0139;

   tcKcdcd = 0.313;
   tcKddcd = 0.132;

   tcMccdc = 0.091;
   tcMcdcd = 0.038;
   tcMcddc = 0.165;
   tcMdccd = 0.194;
   tcMdcdd = 0.019;
   tcXccccc = 0.242;
   tcXddddd = 0.001;

   tcTbcc = 0.036;
   tcTcca = 0.124;
   tcTcda = 0.136;

   tcWabc = 0.190;
   tcWabd = 0.123;
   tcWacc = 0.115;
   tcWadc = 0.014;
   tcWbdd = 0.161;
   tcWcbd = 0.157;
   tcWdad = 0.0015;

   tcXcdcd = 0.282;
   tcXcddd = 0.002;
   tcXdddd = 0.018;

   tcXcccccc = 1.00;
   tcXcdcdcd = 1.00;
   tcXdddddd = 1.00;
}


void CCueLabelsVarley_LabellingConstants::SetRelaxation20STuningConstants ()
{
#ifdef CURRENT
   tcSm    = 0.4522;
   tcSp    = 0.7504;
   tcSq    = 0.5649;
   tcLba   = 0.0286;
   tcLab   = 0.2543;
   tcLac   = 0.3227;
   tcLbd   = 0.3722;
   tcLaa   = 0.1473;
   tcMbcca = 0.1769;
   tcMbcda = 0.0929;
   tcMccdc = 0.1043;
   tcMcdcd = 0.0110;
   tcMcddc = 0.2099;
   tcMdccd = 0.4338;
   tcTbaa  = 0.6375;
   tcTbac  = 0.0900;
   tcTbad  = 0.2514;
   tcTbda  = 0.0934;
   tcTbdc  = 0.0171;
   tcWbca  = 0.4458;
   tcWcdc  = 0.6163;
   tcWdcd  = 0.4102;
   tcWbda  = 0.0306;
   tcWbaa  = 0.7472;
   tcWacd  = 0.0000;
   tcWcac  = 0.1659;
   tcWbcd  = 0.0997;
   tcWbdc  = 0.0738;
   tcTaba  = 0.5100;
   tcTbca  = 0.1474;
   tcTdda  = 0.4602;
   tcXcccc = 0.4572;
   tcXcccd = 0.1040;
   tcXabcd = 0.5221;
   tcXabdd = 0.5028;
   tcYccc  = 0.9398;
   tcYddd  = 0.1006;
   tcYabd  = 0.2742;
   tcYaab  = 0.3270;
   tcYabc  = 0.0342;
   tcYacc  = 0.3498;
   tcYadd  = 0.0000;
   tcYacd  = 0.1466;
   tcKabcd = 0.4961;
   tcKcccd = 0.3137;
   tcKcdcd = 0.0384;
   tcKddcd = 0.1434;
   tcEa    = 0.5413;
   tcEc    = 0.9177;
   tcEd    = 0.1268;
#endif

#ifdef BEIJING
   tcSm    = 0.0295;
   tcSp    = 0.9624;
   tcSq    = 0.7491;
   tcLba   = 0.0309;
   tcLab   = 0.6750;
   tcLac   = 0.4463;
   tcLbd   = 0.3791;
   tcLaa   = 0.0229;
   tcMbcca = 0.2110;
   tcMbcda = 0.0778;
   tcMccdc = 0.0910;
   tcMcdcd = 0.0380;
   tcMcddc = 0.1650;
   tcMdccd = 0.1940;
   tcTbaa  = 0.9978;
   tcTbac  = 0.1513;
   tcTbad  = 0.5580;
   tcTbda  = 0.0909;
   tcTbdc  = 0.0299;
   tcWbca  = 0.4453;
   tcWcdc  = 0.5921;
   tcWdcd  = 0.4035;
   tcWbda  = 0.0282;
   tcWbaa  = 0.7492;
   tcWacd  = 0.0000;
   tcWcac  = 0.1008;
   tcWbcd  = 0.0984;
   tcWbdc  = 0.0635;
   tcTaba  = 0.2109;
   tcTbca  = 0.2287;
   tcTdda  = 0.4509;
   tcXcccc = 0.5070;
   tcXcccd = 0.0922;
   tcXabcd = 0.3575;
   tcXabdd = 0.4995;
   tcYccc  = 0.9356;
   tcYddd  = 0.0905;
   tcYabd  = 0.3886;
   tcYaab  = 0.3029;
   tcYabc  = 0.0495;
   tcYacc  = 0.3431;
   tcYadd  = 0.0000;
   tcYacd  = 0.1598;
   tcKabcd = 0.5004;
   tcKcccd = 0.3306;
   tcKcdcd = 0.3130;
   tcKddcd = 0.1320;
   tcEa    = 0.5419;
   tcEc    = 0.9129;
   tcEd    = 0.1178;
#endif

#ifdef GENOA
   tcSm    = 0.0304;
   tcSp    = 0.8525;
   tcSq    = 0.6182;
   tcLba   = 0.0570;
   tcLab   = 0.1504;
   tcLac   = 0.4023;
   tcLbd   = 0.2627;
   tcLaa   = 0.1045;
   tcMbcca = 0.1499;
   tcMbcda = 0.0882;
   tcMccdc = 0.0919;
   tcMcdcd = 0.0359;
   tcMcddc = 0.1650;
   tcMdccd = 0.1940;
   tcTbaa  = 0.2503;
   tcTbac  = 0.1930;
   tcTbad  = 0.9224;
   tcTbda  = 0.0117;
   tcTbdc  = 0.0329;
   tcWbca  = 0.4015;
   tcWcdc  = 0.5501;
   tcWdcd  = 0.2438;
   tcWbda  = 0.0249;
   tcWbaa  = 0.7450;
   tcWacd  = 0.1510;
   tcWcac  = 0.2891;
   tcWbcd  = 0.0992;
   tcWbdc  = 0.0300;
   tcTaba  = 0.2564;
   tcTbca  = 0.0198;
   tcTdda  = 0.5222;
   tcXcccc = 0.4414;
   tcXcccd = 0.0992;
   tcXabcd = 0.6899;
   tcXabdd = 0.5428;
   tcYccc  = 0.9357;
   tcYddd  = 0.0856;
   tcYabd  = 0.4662;
   tcYaab  = 0.1603;
   tcYabc  = 0.0943;
   tcYacc  = 0.3405;
   tcYadd  = 0.0310;
   tcYacd  = 0.1634;
   tcKabcd = 0.7729;
   tcKcccd = 0.4376;
   tcKcdcd = 0.1622;
   tcKddcd = 0.1325;
   tcEa    = 0.7490;
   tcEc    = 0.9598;
   tcEd    = 0.0308;
#endif

   tcMdcdd = 0.019;
   tcXccccc = 0.242;
   tcXddddd = 0.001;

   tcTbcc = 0.036;
   tcTcca = 0.124;
   tcTcda = 0.136;

   tcWabc = 0.190;
   tcWabd = 0.123;
   tcWacc = 0.115;
   tcWadc = 0.014;
   tcWbdd = 0.161;
   tcWcbd = 0.157;
   tcWdad = 0.0015;

   tcXcdcd = 0.282;
   tcXcddd = 0.002;
   tcXdddd = 0.018;

   tcXcccccc = 1.00;
   tcXcdcdcd = 1.00;
   tcXdddddd = 1.00;
}


void CCueLabelsVarley_LabellingConstants::SetRelaxationTuningConstants (bool UseJunctionStatisticsForRelaxationLabelling,
																		bool UseShapePairStatisticsForRelaxationLabelling,
																		int RelaxationIterations,
																		bool UseMaxInRelaxationLabelling)
{
   if (UseJunctionStatisticsForRelaxationLabelling)
   {
      SetRelaxationStatisticalTuningConstants();
   }
   else
   if (UseShapePairStatisticsForRelaxationLabelling)
   {
      SetRelaxationShapeTuningConstants();
   }
   else
   if (RelaxationIterations <= 10)
   {
      SetRelaxation10TuningConstants();
   }
   else
   {
      if (UseMaxInRelaxationLabelling)
      {
         SetRelaxation20HTuningConstants();
      }
      else
      {
         SetRelaxation20STuningConstants();
      }
   }
}


void CCueLabelsVarley_LabellingConstants::SetLambanFHTuningConstants ()
{
   tcOo    = 0.0901 * 1000.0 + 1.0;
   tcOp    = 0.0466;
   tcOq    = 0.2101;
   tcOy    = 0.2114;
   tcOz    = 0.0014;
   tcSm    = 0.0303;
   tcSp    = 0.9139;
   tcSq    = 0.7979;
   tcSu    = 0.8487;
   tcSv    = 0.3616;
   tcLba   = 0.2101;
   tcLab   = 0.7525;
   tcLac   = 0.2901;
   tcLbd   = 0.0405;
   tcLaa   = 0.4398;
   tcMbcca = 0.2101;
   tcMbcda = 0.4501;
   tcTbaa  = 0.9725;
   tcTbac  = 0.0897;
   tcTbad  = 0.7164;
   tcTbda  = 0.0000;
   tcTbdc  = 0.0299;
   tcWbca  = 0.9714;
   tcWcdc  = 0.7397;
   tcWdcd  = 0.5487;
   tcWbda  = 0.1501;
   tcWbaa  = 0.5499;
   tcWacd  = 0.0316;
   tcWcac  = 0.1501;
   tcWbcd  = 0.0254;
   tcWbdc  = 0.0587;
   tcTaba  = 0.2408;
   tcTbca  = 0.0000;
   tcTdda  = 0.0043;
   tcXcccc = 0.4283;
   tcXcccd = 0.2774;
   tcXabcd = 0.2833;
   tcXabdd = 0.4894;
   tcYabd  = 0.9430;
   tcYccc  = 0.7940;
   tcYddd  = 0.5312;
   tcYaab  = 0.4048;
   tcYabc  = 0.2069;
   tcYacc  = 0.6660;
   tcYadd  = 0.5114;
   tcYacd  = 0.1675;
   tcKabcd = 0.0222;
   tcKcccd = 0.0913;

   tcKcdcd = 0.105;
   tcKddcd = 0.066;

   tcMccdc = 0.100;
   tcMcdcd = 0.100;
   tcMcddc = 0.100;
   tcMdccd = 0.100;
   tcMdcdd = 0.100;

   tcTbcc = 0.030;
   tcTcca = 0.030;
   tcTcda = 0.030;

   tcWabc = 0.036;
   tcWabd = 0.036;
   tcWacc = 0.036;
   tcWadc = 0.036;
   tcWbdd = 0.036;
   tcWcbd = 0.036;
   tcWdad = 0.036;

   tcXcdcd = 0.100;
   tcXcddd = 0.100;
   tcXdddd = 0.100;

   tcXccccc = 0.972;
   tcXddddd = 0.014;

   tcXcccccc = 0.556;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}


void CCueLabelsVarley_LabellingConstants::SetLambanFTuningConstants ()
{
#ifdef CURRENT
   tcOo    = 0.7964 * 1000.0 + 1.0;
   tcOp    = 0.0346;
   tcOq    = 0.6157;
   tcOy    = 0.0900;
   tcOz    = 0.0536;
   tcSm    = 0.4542;
   tcSp    = 0.8500;
   tcSq    = 0.5344;
   tcSu    = 0.8500;
   tcSv    = 0.1902;
   tcLba   = 0.0879;
   tcLab   = 0.4500;
   tcLac   = 0.5988;
   tcLbd   = 0.5503;
   tcLaa   = 0.3679;
   tcMbcca = 0.1217;
   tcMbcda = 0.2789;
   tcMccdc = 0.0982;
   tcMcdcd = 0.3773;
   tcMcddc = 0.1000;
   tcMdccd = 0.1000;
   tcTbaa  = 0.9500;
   tcTbac  = 0.0900;
   tcTbad  = 0.1163;
   tcTbda  = 0.0300;
   tcTbdc  = 0.0000;
   tcWbca  = 0.8449;
   tcWcdc  = 0.9504;
   tcWdcd  = 0.6355;
   tcWbda  = 0.4744;
   tcWbaa  = 0.5460;
   tcWacd  = 0.1502;
   tcWcac  = 0.5284;
   tcWbcd  = 0.0300;
   tcWbdc  = 0.3300;
   tcTaba  = 0.0900;
   tcTbca  = 0.0744;
   tcTdda  = 0.2735;
   tcXcccc = 0.8795;
   tcXcccd = 0.3038;
   tcXabcd = 0.9762;
   tcXabdd = 0.2964;
   tcYabd  = 0.8476;
   tcYccc  = 0.4018;
   tcYddd  = 0.1529;
   tcYaab  = 0.6438;
   tcYabc  = 0.0602;
   tcYacc  = 0.7504;
   tcYadd  = 0.4798;
   tcYacd  = 0.5371;
   tcKabcd = 0.0366;
   tcKcccd = 0.2730;
   tcKcdcd = 0.1056;
   tcKddcd = 0.0660;
   tcEa    = 0.6157;
   tcEc    = 0.9419;
   tcEd    = 0.1536;
#endif

#ifdef BEIJING
   tcOo    = 0.5643 * 1000.0 + 1.0;
   tcOp    = 0.0346;
   tcOq    = 0.6470;
   tcOy    = 0.0900;
   tcOz    = 0.0536;
   tcSm    = 0.4375;
   tcSp    = 0.8498;
   tcSq    = 0.6712;
   tcSu    = 0.8500;
   tcSv    = 0.0430;
   tcLba   = 0.0879;
   tcLab   = 0.4500;
   tcLac   = 0.6025;
   tcLbd   = 0.4555;
   tcLaa   = 0.1195;
   tcMbcca = 0.1217;
   tcMbcda = 0.2067;
   tcMccdc = 0.1000;
   tcMcdcd = 0.1000;
   tcMcddc = 0.1000;
   tcMdccd = 0.1000;
   tcTbaa  = 0.9500;
   tcTbac  = 0.0900;
   tcTbad  = 0.1163;
   tcTbda  = 0.0300;
   tcTbdc  = 0.0000;
   tcWbca  = 0.8449;
   tcWcdc  = 0.9504;
   tcWdcd  = 0.6355;
   tcWbda  = 0.5012;
   tcWbaa  = 0.4964;
   tcWacd  = 0.1502;
   tcWcac  = 0.4830;
   tcWbcd  = 0.0300;
   tcWbdc  = 0.0289;
   tcTaba  = 0.0900;
   tcTbca  = 0.0744;
   tcTdda  = 0.2735;
   tcXcccc = 0.8259;
   tcXcccd = 0.3226;
   tcXabcd = 0.7999;
   tcXabdd = 0.2983;
   tcYabd  = 0.8476;
   tcYccc  = 0.4018;
   tcYddd  = 0.1529;
   tcYaab  = 0.6438;
   tcYabc  = 0.0602;
   tcYacc  = 0.7504;
   tcYadd  = 0.4798;
   tcYacd  = 0.5064;
   tcKabcd = 0.0366;
   tcKcccd = 0.2730;
   tcKcdcd = 0.1050;
   tcKddcd = 0.0660;
   tcEa    = 0.5446;
   tcEc    = 0.9464;
   tcEd    = 0.1536;
#endif

#ifdef GENOA
   tcOo    = 0.5698 * 1000.0 + 1.0;
   tcOp    = 0.0301;
   tcOq    = 0.7498;
   tcOy    = 0.0902;
   tcOz    = 0.4048;
   tcSm    = 0.5698;
   tcSp    = 0.8493;
   tcSq    = 0.1715;
   tcSu    = 0.6750;
   tcSv    = 0.4502;
   tcLba   = 0.6498;
   tcLab   = 0.7282;
   tcLac   = 0.5473;
   tcLbd   = 0.2100;
   tcLaa   = 0.6378;
   tcMbcca = 0.0303;
   tcMbcda = 0.0903;
   tcMccdc = 0.0992;
   tcMcdcd = 0.1002;
   tcMcddc = 0.1000;
   tcMdccd = 0.1000;
   tcTbaa  = 0.9497;
   tcTbac  = 0.0304;
   tcTbad  = 0.0932;
   tcTbda  = 0.0309;
   tcTbdc  = 0.0009;
   tcWbca  = 0.8461;
   tcWcdc  = 0.9509;
   tcWdcd  = 0.6341;
   tcWbda  = 0.5514;
   tcWbaa  = 0.5964;
   tcWacd  = 0.1499;
   tcWcac  = 0.5358;
   tcWbcd  = 0.0278;
   tcWbdc  = 0.2464;
   tcTaba  = 0.3833;
   tcTbca  = 0.0989;
   tcTdda  = 0.2964;
   tcXcccc = 0.4138;
   tcXcccd = 0.5773;
   tcXabcd = 0.7013;
   tcXabdd = 0.2787;
   tcYabd  = 0.7501;
   tcYccc  = 0.4427;
   tcYddd  = 0.2866;
   tcYaab  = 0.4646;
   tcYabc  = 0.0386;
   tcYacc  = 0.7458;
   tcYadd  = 0.4792;
   tcYacd  = 0.5031;
   tcKabcd = 0.4504;
   tcKcccd = 0.3312;
   tcKcdcd = 0.0961;
   tcKddcd = 0.0670;
   tcEa    = 0.5278;
   tcEc    = 0.9378;
   tcEd    = 0.1516;
#endif

   tcMdcdd = 0.100;

   tcTbcc = 0.030;
   tcTcca = 0.030;
   tcTcda = 0.030;

   tcWabc = 0.036;
   tcWabd = 0.036;
   tcWacc = 0.036;
   tcWadc = 0.036;
   tcWbdd = 0.036;
   tcWcbd = 0.036;
   tcWdad = 0.036;

   tcXcdcd = 0.100;
   tcXcddd = 0.100;
   tcXdddd = 0.100;

   tcXccccc = 0.972;
   tcXddddd = 0.014;

   tcXcccccc = 0.556;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}


void CCueLabelsVarley_LabellingConstants::SetLambanDTuningConstants ()
{
#ifdef CURRENT
   tcOe    = 0.0900;
   tcOk    = 0.0500;
   tcOo    = 0.1653 * 1000.0 + 1.0;
   tcOp    = 0.0204;
   tcOq    = 0.9500;
   tcOy    = 0.0000;
   tcOz    = 0.0000;
   tcSm    = 0.1500;
   tcSp    = 0.8349;
   tcSq    = 0.7054;
   tcSu    = 0.9500;
   tcSv    = 0.1196;
   tcLba   = 0.0479;
   tcLab   = 0.9358;
   tcLac   = 0.6901;
   tcLbd   = 0.4869;
   tcLaa   = 0.0707;
   tcMbcca = 0.2700;
   tcMbcda = 0.1698;
   tcMccdc = 0.2052;
   tcMcdcd = 0.2100;
   tcMcddc = 0.0900;
   tcMdccd = 0.1000;
   tcTbaa  = 0.9358;
   tcTbac  = 0.2259;
   tcTbad  = 0.3681;
   tcTbda  = 0.1500;
   tcTbdc  = 0.0241;
   tcWbca  = 0.5500;
   tcWcdc  = 0.6786;
   tcWdcd  = 0.4388;
   tcWbda  = 0.3702;
   tcWbaa  = 0.6629;
   tcWacd  = 0.1697;
   tcWcac  = 0.6500;
   tcWbcd  = 0.0466;
   tcWbdc  = 0.0379;
   tcTaba  = 0.3120;
   tcTbca  = 0.1212;
   tcTdda  = 0.5567;
   tcXcccc = 0.7097;
   tcXcccd = 0.5100;
   tcXabcd = 0.3797;
   tcXabdd = 0.2700;
   tcYabd  = 0.9426;
   tcYccc  = 0.0388;
   tcYddd  = 0.0300;
   tcYaab  = 0.8358;
   tcYabc  = 0.0219;
   tcYacc  = 0.9552;
   tcYadd  = 0.4877;
   tcYacd  = 0.0477;
   tcKcccd = 0.5612;
   tcKcdcd = 0.1050;
   tcKddcd = 0.0660;
   tcEa    = 0.4327;
   tcEc    = 0.3231;
   tcEd    = 0.1383;
#endif

#ifdef BEIJING
   tcOe    = 0.0442;
   tcOk    = 0.6722;
   tcOo    = 0.1653 * 1000.0 + 1.0;
   tcOp    = 0.0204;
   tcOq    = 0.8347;
   tcOy    = 0.0000;
   tcOz    = 0.0000;
   tcSm    = 0.1500;
   tcSp    = 0.8468;
   tcSq    = 0.5591;
   tcSu    = 0.9425;
   tcSv    = 0.1930;
   tcLba   = 0.0479;
   tcLab   = 0.9358;
   tcLac   = 0.6397;
   tcLbd   = 0.5270;
   tcLaa   = 0.0350;
   tcMbcca = 0.2430;
   tcMbcda = 0.1602;
   tcMccdc = 0.1000;
   tcMcdcd = 0.1000;
   tcMcddc = 0.1000;
   tcMdccd = 0.1000;
   tcTbaa  = 0.9358;
   tcTbac  = 0.2259;
   tcTbad  = 0.2654;
   tcTbda  = 0.2964;
   tcTbdc  = 0.0241;
   tcWbca  = 0.6591;
   tcWcdc  = 0.6786;
   tcWdcd  = 0.4322;
   tcWbda  = 0.4035;
   tcWbaa  = 0.6629;
   tcWacd  = 0.1697;
   tcWcac  = 0.6359;
   tcWbcd  = 0.0466;
   tcWbdc  = 0.0379;
   tcTaba  = 0.5558;
   tcTbca  = 0.0841;
   tcTdda  = 0.5612;
   tcXcccc = 0.7074;
   tcXcccd = 0.2294;
   tcXabcd = 0.1642;
   tcXabdd = 0.3453;
   tcYabd  = 0.9426;
   tcYccc  = 0.0388;
   tcYddd  = 0.1581;
   tcYaab  = 0.8358;
   tcYabc  = 0.0219;
   tcYacc  = 0.9552;
   tcYadd  = 0.4877;
   tcYacd  = 0.0477;
   tcKcccd = 0.5612;
   tcKcdcd = 0.1050;
   tcKddcd = 0.0660;
   tcEa    = 0.4327;
   tcEc    = 0.3231;
   tcEd    = 0.1383;
#endif

#ifdef GENOA
   tcOe    = 0.1584;
   tcOk    = 0.0900;
   tcOo    = 0.3222 * 1000.0 + 1.0;
   tcOp    = 0.0164;
   tcOq    = 0.6500;
   tcOy    = 0.0000;
   tcOz    = 0.0000;
   tcSm    = 0.2764;
   tcSp    = 0.6659;
   tcSq    = 0.5735;
   tcSu    = 0.8560;
   tcSv    = 0.2700;
   tcLba   = 0.0421;
   tcLab   = 0.0485;
   tcLac   = 0.4503;
   tcLbd   = 0.2279;
   tcLaa   = 0.4836;
   tcMbcca = 0.2661;
   tcMbcda = 0.0954;
   tcMccdc = 0.1368;
   tcMcdcd = 0.1000;
   tcMcddc = 0.1000;
   tcMdccd = 0.1000;
   tcTbaa  = 0.9500;
   tcTbac  = 0.2100;
   tcTbad  = 0.1571;
   tcTbda  = 0.2722;
   tcTbdc  = 0.0122;
   tcWbca  = 0.8266;
   tcWcdc  = 0.8120;
   tcWdcd  = 0.4078;
   tcWbda  = 0.0496;
   tcWbaa  = 0.4891;
   tcWacd  = 0.3793;
   tcWcac  = 0.8497;
   tcWbcd  = 0.0610;
   tcWbdc  = 0.5006;
   tcTaba  = 0.0634;
   tcTbca  = 0.0000;
   tcTdda  = 0.7172;
   tcXcccc = 0.9127;
   tcXcccd = 0.9278;
   tcXabcd = 0.0900;
   tcXabdd = 0.1439;
   tcYabd  = 0.9413;
   tcYccc  = 0.1919;
   tcYddd  = 0.1443;
   tcYaab  = 0.2833;
   tcYabc  = 0.0499;
   tcYacc  = 0.8558;
   tcYadd  = 0.4377;
   tcYacd  = 0.0585;
   tcKcccd = 0.7253;
   tcKcdcd = 0.1561;
   tcKddcd = 0.0767;
   tcEa    = 0.7147;
   tcEc    = 0.3608;
   tcEd    = 0.1839;
#endif

   tcKabcd = tcKcccd;  // Kabcd appears to be a non-parameter

   tcMdcdd = 0.100;

   tcTbcc = 0.030;
   tcTcca = 0.030;
   tcTcda = 0.030;

   tcWabc = 0.036;
   tcWabd = 0.036;
   tcWacc = 0.036;
   tcWadc = 0.036;
   tcWbdd = 0.036;
   tcWcbd = 0.036;
   tcWdad = 0.036;

   tcXcdcd = 0.100;
   tcXcddd = 0.100;
   tcXdddd = 0.100;

   tcXccccc = 0.972;
   tcXddddd = 0.014;

   tcXcccccc = 0.556;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}


void CCueLabelsVarley_LabellingConstants::SetLambanVTuningConstants ()
{
#ifdef CURRENT
   tcOe    = 0.2121;
   tcOk    = 0.8491;
   tcOo    = 0.0946 * 1000.0 + 1.0;
   tcOp    = 0.0929;
   tcOq    = 0.9414;
   tcOy    = 0.3451;
   tcOz    = 0.5005;
   tcSm    = 0.4528;
   tcSp    = 0.8699;
   tcSq    = 0.8754;
   tcSu    = 0.9157;
   tcSv    = 0.3231;
   tcLba   = 0.2695;
   tcLab   = 0.3940;
   tcLac   = 0.5844;
   tcLbd   = 0.2714;
   tcLaa   = 0.5007;
   tcMbcca = 0.0406;
   tcMbcda = 0.1581;
   tcMccdc = 0.0682;
   tcMcdcd = 0.1070;
   tcMcddc = 0.1030;
   tcMdccd = 0.1024;
   tcTbaa  = 0.9465;
   tcTbac  = 0.0356;
   tcTbad  = 0.2425;
   tcTbda  = 0.0148;
   tcTbdc  = 0.0303;
   tcWbca  = 0.7351;
   tcWcdc  = 0.7019;
   tcWdcd  = 0.8097;
   tcWbda  = 0.0974;
   tcWbaa  = 0.6076;
   tcWacd  = 0.0383;
   tcWcac  = 0.6045;
   tcWbcd  = 0.0952;
   tcWbdc  = 0.2235;
   tcTaba  = 0.5919;
   tcTbca  = 0.2571;
   tcTdda  = 0.9248;
   tcXcccc = 0.3970;
   tcXcccd = 0.1227;
   tcXabcd = 0.9488;
   tcXabdd = 0.1223;
   tcYabd  = 0.9442;
   tcYccc  = 0.6827;
   tcYddd  = 0.1317;
   tcYaab  = 0.6339;
   tcYabc  = 0.0988;
   tcYacc  = 0.5599;
   tcYadd  = 0.3768;
   tcYacd  = 0.0948;
   tcKcccd = 0.6946;
   tcKcdcd = 0.1027;
   tcKddcd = 0.0639;
   tcEa    = 0.4126;
   tcEc    = 0.8307;
   tcEd    = 0.3233;
#endif

#ifdef BEIJING
   tcOe    = 0.7568;
   tcOk    = 0.8501;
   tcOo    = 0.1530 * 1000.0 + 1.0;
   tcOp    = 0.0358;
   tcOq    = 0.8470;
   tcOy    = 0.1469;
   tcOz    = 0.4509;
   tcSm    = 0.4514;
   tcSp    = 0.8507;
   tcSq    = 0.8470;
   tcSu    = 0.9470;
   tcSv    = 0.9332;
   tcLba   = 0.2104;
   tcLab   = 0.6527;
   tcLac   = 0.5862;
   tcLbd   = 0.3440;
   tcLaa   = 0.4031;
   tcMbcca = 0.0364;
   tcMbcda = 0.1551;
   tcMccdc = 0.1000;
   tcMcdcd = 0.1000;
   tcMcddc = 0.1000;
   tcMdccd = 0.1000;
   tcTbaa  = 0.9503;
   tcTbac  = 0.0298;
   tcTbad  = 0.3106;
   tcTbda  = 0.0155;
   tcTbdc  = 0.0325;
   tcWbca  = 0.7449;
   tcWcdc  = 0.7187;
   tcWdcd  = 0.8125;
   tcWbda  = 0.0926;
   tcWbaa  = 0.6077;
   tcWacd  = 0.0369;
   tcWcac  = 0.5804;
   tcWbcd  = 0.0914;
   tcWbdc  = 0.2195;
   tcTaba  = 0.8470;
   tcTbca  = 0.3900;
   tcTdda  = 0.8489;
   tcXcccc = 0.4258;
   tcXcccd = 0.1168;
   tcXabcd = 0.6420;
   tcXabdd = 0.1155;
   tcYabd  = 0.8469;
   tcYccc  = 0.5478;
   tcYddd  = 0.1546;
   tcYaab  = 0.7540;
   tcYabc  = 0.0920;
   tcYacc  = 0.7029;
   tcYadd  = 0.4096;
   tcYacd  = 0.0458;
   tcKcccd = 0.5663;
   tcKcdcd = 0.1050;
   tcKddcd = 0.0660;
   tcEa    = 0.4410;
   tcEc    = 0.7508;
   tcEd    = 0.3345;
#endif

#ifdef GENOA
   tcOe    = 0.3900;
   tcOk    = 0.2668;
   tcOo    = 0.4523 * 1000.0 + 1.0;
   tcOp    = 0.0300;
   tcOq    = 0.9500;
   tcOy    = 0.6794;
   tcOz    = 0.0900;
   tcSm    = 0.5700;
   tcSp    = 0.9500;
   tcSq    = 0.4143;
   tcSu    = 0.5500;
   tcSv    = 0.8331;
   tcLba   = 0.1069;
   tcLab   = 0.1500;
   tcLac   = 0.0900;
   tcLbd   = 0.0458;
   tcLaa   = 0.0500;
   tcMbcca = 0.0300;
   tcMbcda = 0.2184;
   tcMccdc = 0.0900;
   tcMcdcd = 0.1000;
   tcMcddc = 0.1000;
   tcMdccd = 0.1000;
   tcTbaa  = 0.6500;
   tcTbac  = 0.0167;
   tcTbad  = 0.1500;
   tcTbda  = 0.0147;
   tcTbdc  = 0.0385;
   tcWbca  = 0.6645;
   tcWcdc  = 0.6535;
   tcWdcd  = 0.1277;
   tcWbda  = 0.0945;
   tcWbaa  = 0.5357;
   tcWacd  = 0.2437;
   tcWcac  = 0.5634;
   tcWbcd  = 0.1065;
   tcWbdc  = 0.2477;
   tcTaba  = 0.7494;
   tcTbca  = 0.2914;
   tcTdda  = 0.7487;
   tcXcccc = 0.6582;
   tcXcccd = 0.9032;
   tcXabcd = 0.6044;
   tcXabdd = 0.0461;
   tcYabd  = 0.8333;
   tcYccc  = 0.5241;
   tcYddd  = 0.1500;
   tcYaab  = 0.7001;
   tcYabc  = 0.1794;
   tcYacc  = 0.5500;
   tcYadd  = 0.4208;
   tcYacd  = 0.0695;
   tcKcccd = 0.9500;
   tcKcdcd = 0.0322;
   tcKddcd = 0.0300;
   tcEa    = 0.3895;
   tcEc    = 0.4510;
   tcEd    = 0.5390;
#endif

   tcKabcd = tcKcccd;  // Kabcd appears to be a non-parameter

   tcMdcdd = 0.100;

   tcTbcc = 0.030;
   tcTcca = 0.030;
   tcTcda = 0.030;

   tcWabc = 0.036;
   tcWabd = 0.036;
   tcWacc = 0.036;
   tcWadc = 0.036;
   tcWbdd = 0.036;
   tcWcbd = 0.036;
   tcWdad = 0.036;

   tcXcdcd = 0.100;
   tcXcddd = 0.100;
   tcXdddd = 0.100;

   tcXccccc = 0.972;
   tcXddddd = 0.014;

   tcXcccccc = 0.556;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}


void CCueLabelsVarley_LabellingConstants::SetLambanTuningConstants (bool UseMaxInRelaxationLabelling,
																	int LambanAxisOptions,
																	bool UseDeductionsInLamban)
{
   if (LambanAxisOptions == LambanAxesFixedIJK)
   {
      if (UseMaxInRelaxationLabelling)
      {
         SetLambanFHTuningConstants();
      }
      else
      {
         SetLambanFTuningConstants();
      }
   }
   else if (UseDeductionsInLamban)
   {
      SetLambanDTuningConstants();
   }
   else
   {
      SetLambanVTuningConstants();
   }
}


void CCueLabelsVarley_LabellingConstants::SetSemiCombinedTuningConstants ()
{
   tcSp    = 0.8523;
   tcSq    = 0.0300;
   tcLba   = 0.8382;
   tcLab   = 0.0529;
   tcLac   = 0.6086;
   tcLbd   = 0.1923;
   tcLaa   = 0.4071;
   tcMbcca = 0.3925;
   tcMbcda = 0.0538;
   tcTbaa  = 0.6401;
   tcTbac  = 0.0500;
   tcTbad  = 0.0500;
   tcTbda  = 0.0531;
   tcTbdc  = 0.0500;
   tcWbca  = 0.9481;
   tcWcdc  = 0.3858;
   tcWdcd  = 0.0522;
   tcWbda  = 0.6395;
   tcWbaa  = 0.7392;
   tcWacd  = 0.4471;
   tcWcac  = 0.6163;
   tcTaba  = 0.3538;
   tcTbca  = 0.1577;
   tcXcccc = 0.6262;
   tcXcccd = 0.6357;
   tcXabcd = 0.3832;
   tcXabdd = 0.6229;
   tcYabd  = 0.0218;
   tcYccc  = 0.6186;
   tcYddd  = 0.0138;
   tcYaab  = 0.0000;
   tcYabc  = 0.0000;
   tcYacc  = 0.6231;
   tcYadd  = 0.0223;
   tcYacd  = 0.0066;
   tcKabcd = 0.0538;
   tcKcccd = 0.6265;

   tcKcdcd = 0.105;
   tcKddcd = 0.066;

   tcMccdc = 0.123;
   tcMcdcd = 0.096;
   tcMcddc = 0.068;
   tcMdccd = 0.068;
   tcMdcdd = 0.110;

   tcTbcc = 0.009;
   tcTcca = 0.030;
   tcTcda = 0.012;
   tcTdda = 0.007;

   tcWabc = 0.036;
   tcWabd = 0.036;
   tcWacc = 0.036;
   tcWadc = 0.036;
   tcWbcd = 0.036;
   tcWbdc = 0.036;
   tcWbdd = 0.036;
   tcWcbd = 0.036;
   tcWdad = 0.036;

   tcXcdcd = 0.039;
   tcXcddd = 0.022;
   tcXdddd = 0.022;

   tcXccccc = 0.972;
   tcXddddd = 0.014;

   tcXcccccc = 0.556;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}


void CCueLabelsVarley_LabellingConstants::SetCombinedFTuningConstants ()
{
#ifdef CURRENT
   tcOo    = 0.3929 * 1000.0 + 1;
   tcOp    = 0.0318;
   tcOq    = 0.8477;
   tcOy    = 0.0918;
   tcOz    = 0.0918;
   tcSm    = 0.8921;
   tcSp    = 0.7967;
   tcSq    = 0.9500;
   tcSu    = 0.5148;
   tcSv    = 0.3777;
   tcLba   = 0.0900;
   tcLab   = 0.1535;
   tcLac   = 0.1032;
   tcLbd   = 0.0425;
   tcLaa   = 0.0318;
   tcMbcca = 0.0900;
   tcMbcda = 0.3447;
   tcMccdc = 0.1230;
   tcMcdcd = 0.0960;
   tcMcddc = 0.0680;
   tcMdccd = 0.0680;
   tcTbaa  = 0.9500;
   tcTbac  = 0.0300;
   tcTbad  = 0.2531;
   tcTbda  = 0.0000;
   tcTbdc  = 0.0529;
   tcWbca  = 0.7217;
   tcWcdc  = 0.8490;
   tcWdcd  = 0.3032;
   tcWbda  = 0.0504;
   tcWbaa  = 0.4669;
   tcWacd  = 0.1895;
   tcWcac  = 0.3866;
   tcTaba  = 0.0300;
   tcTbca  = 0.0018;
   tcXcccc = 0.9480;
   tcXcccd = 0.1470;
   tcXabcd = 0.9440;
   tcXabdd = 0.4252;
   tcYabd  = 0.8298;
   tcYccc  = 0.8861;
   tcYddd  = 0.0361;
   tcYaab  = 0.0904;
   tcYabc  = 0.0266;
   tcYacc  = 0.5965;
   tcYadd  = 0.3374;
   tcYacd  = 0.0518;
   tcKabcd = 0.2042;
   tcKcccd = 0.0922;
   tcKcdcd = 0.0300;
   tcKddcd = 0.0660;
   tcEa    = 0.5266;
   tcEc    = 0.7482;
   tcEd    = 0.1202;
#endif

#ifdef BEIJING
   tcOo    = 0.2718 * 1000.0 + 1;
   tcOp    = 0.0318;
   tcOq    = 0.8477;
   tcOy    = 0.0918;
   tcOz    = 0.0918;
   tcSm    = 0.8921;
   tcSp    = 0.7967;
   tcSq    = 0.7444;
   tcSu    = 0.5198;
   tcSv    = 0.6044;
   tcLba   = 0.0373;
   tcLab   = 0.3026;
   tcLac   = 0.1032;
   tcLbd   = 0.0425;
   tcLaa   = 0.0318;
   tcMbcca = 0.0345;
   tcMbcda = 0.3447;
   tcMccdc = 0.1230;
   tcMcdcd = 0.0960;
   tcMcddc = 0.0680;
   tcMdccd = 0.0680;
   tcTbaa  = 0.8482;
   tcTbac  = 0.2939;
   tcTbad  = 0.2518;
   tcTbda  = 0.0000;
   tcTbdc  = 0.0529;
   tcWbca  = 0.6516;
   tcWcdc  = 0.8490;
   tcWdcd  = 0.3032;
   tcWbda  = 0.0504;
   tcWbaa  = 0.4613;
   tcWacd  = 0.2115;
   tcWcac  = 0.2118;
   tcTaba  = 0.0288;
   tcTbca  = 0.0018;
   tcXcccc = 0.5238;
   tcXcccd = 0.0258;
   tcXabcd = 0.2117;
   tcXabdd = 0.3184;
   tcYabd  = 0.9494;
   tcYccc  = 0.8861;
   tcYddd  = 0.0361;
   tcYaab  = 0.0904;
   tcYabc  = 0.0266;
   tcYacc  = 0.5355;
   tcYadd  = 0.2219;
   tcYacd  = 0.0518;
   tcKabcd = 0.2085;
   tcKcccd = 0.0922;
   tcKcdcd = 0.1050;
   tcKddcd = 0.0660;
   tcEa    = 0.5266;
   tcEc    = 0.7482;
   tcEd    = 0.1285;
#endif

#ifdef GENOA
   tcOo    = 0.5477 * 1000.0 + 1;
   tcOp    = 0.7281;
   tcOq    = 0.1504;
   tcOy    = 0.0836;
   tcOz    = 0.1504;
   tcSm    = 0.6656;
   tcSp    = 0.8498;
   tcSq    = 0.8500;
   tcSu    = 0.0504;
   tcSv    = 0.3501;
   tcLba   = 0.0961;
   tcLab   = 0.4702;
   tcLac   = 0.2329;
   tcLbd   = 0.1160;
   tcLaa   = 0.0385;
   tcMbcca = 0.0919;
   tcMbcda = 0.2662;
   tcMccdc = 0.0942;
   tcMcdcd = 0.0960;
   tcMcddc = 0.0680;
   tcMdccd = 0.0680;
   tcTbaa  = 0.8476;
   tcTbac  = 0.0300;
   tcTbad  = 0.0904;
   tcTbda  = 0.0000;
   tcTbdc  = 0.0534;
   tcWbca  = 0.8614;
   tcWcdc  = 0.8500;
   tcWdcd  = 0.2314;
   tcWbda  = 0.0300;
   tcWbaa  = 0.5316;
   tcWacd  = 0.1626;
   tcWcac  = 0.3265;
   tcTaba  = 0.0896;
   tcTbca  = 0.0000;
   tcXcccc = 0.2216;
   tcXcccd = 0.4708;
   tcXabcd = 0.6235;
   tcXabdd = 0.8074;
   tcYabd  = 0.7828;
   tcYccc  = 0.8496;
   tcYddd  = 0.1709;
   tcYaab  = 0.0896;
   tcYabc  = 0.0747;
   tcYacc  = 0.5646;
   tcYadd  = 0.2402;
   tcYacd  = 0.0324;
   tcKabcd = 0.2101;
   tcKcccd = 0.0922;
   tcKcdcd = 0.0904;
   tcKddcd = 0.0894;
   tcEa    = 0.5282;
   tcEc    = 0.7454;
   tcEd    = 0.1240;
#endif

   tcMdcdd = 0.110;

   tcTbcc = 0.009;
   tcTcca = 0.030;
   tcTcda = 0.012;
   tcTdda = 0.007;

   tcWabc = 0.036;
   tcWabd = 0.036;
   tcWacc = 0.036;
   tcWadc = 0.036;
   tcWbcd = 0.036;
   tcWbdc = 0.036;
   tcWbdd = 0.036;
   tcWcbd = 0.036;
   tcWdad = 0.036;

   tcXcdcd = 0.039;
   tcXcddd = 0.022;
   tcXdddd = 0.022;

   tcXccccc = 0.972;
   tcXddddd = 0.014;

   tcXcccccc = 0.556;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}


void CCueLabelsVarley_LabellingConstants::SetCombinedDTuningConstants ()
{
#ifdef CURRENT
   tcOe    = 0.0300;
   tcOk    = 0.3500;
   tcOo    = 0.5938 * 1000.0 + 1;
   tcOp    = 0.0363;
   tcOq    = 0.9226;
   tcOy    = 0.1504;     //tcOy = 0.400;
   tcOz    = 0.6371;
   tcSm    = 0.8500;
   tcSp    = 0.7308;
   tcSq    = 0.8342;
   tcSu    = 0.8500;
   tcSv    = 0.0371;
   tcLba   = 0.0300;
   tcLab   = 0.0500;
   tcLac   = 0.2308;
   tcLbd   = 0.1053;
   tcLaa   = 0.0900;
   tcMbcca = 0.0396;
   tcMbcda = 0.3368;
   tcMccdc = 0.0971;
   tcMcdcd = 0.0960;
   tcMcddc = 0.0300;
   tcMdccd = 0.0680;
   tcTbaa  = 0.9422;
   tcTbac  = 0.0300;
   tcTbad  = 0.0900;
   tcTbda  = 0.0001;
   tcTbdc  = 0.0300;
   tcWbca  = 0.9186;
   tcWcdc  = 0.9404;
   tcWdcd  = 0.5227;
   tcWbda  = 0.4036;
   tcWbaa  = 0.2187;
   tcWacd  = 0.0300;
   tcWcac  = 0.0900;
   tcTaba  = 0.2700;
   tcTbca  = 0.0035;
   tcXcccc = 0.5791;
   tcXcccd = 0.0314;
   tcXabcd = 0.3977;
   tcXabdd = 0.3557;
   tcYabd  = 0.9461;
   tcYccc  = 0.2584;
   tcYddd  = 0.0892;
   tcYaab  = 0.0983;
   tcYabc  = 0.0321;
   tcYacc  = 0.7480;
   tcYadd  = 0.1715;
   tcYacd  = 0.4568;
   tcKabcd = 0.0608;
   tcKcccd = 0.0376;
   tcKcdcd = 0.0686;
   tcKddcd = 0.0900;
   tcEa    = 0.6507;
   tcEc    = 0.8878;
   tcEd    = 0.0248;
#endif

#ifdef BEIJING
   tcOe    = 0.2714;
   tcOk    = 0.5099;
   tcOo    = 0.3280 * 1000.0 + 1;
   tcOp    = 0.0363;
   tcOq    = 0.9226;
   tcOy    = 0.1504;     //tcOy = 0.400;
   tcOz    = 0.4932;
   tcSm    = 0.6461;
   tcSp    = 0.7920;
   tcSq    = 0.8342;
   tcSu    = 0.9247;
   tcSv    = 0.4504;
   tcLba   = 0.0281;
   tcLab   = 0.5938;
   tcLac   = 0.2368;
   tcLbd   = 0.1053;
   tcLaa   = 0.0258;
   tcMbcca = 0.0396;
   tcMbcda = 0.4230;
   tcMccdc = 0.1230;
   tcMcdcd = 0.0960;
   tcMcddc = 0.0680;
   tcMdccd = 0.0680;
   tcTbaa  = 0.9422;
   tcTbac  = 0.0900;
   tcTbad  = 0.4373;
   tcTbda  = 0.0001;
   tcTbdc  = 0.0239;
   tcWbca  = 0.9186;
   tcWcdc  = 0.9404;
   tcWdcd  = 0.4021;
   tcWbda  = 0.5623;
   tcWbaa  = 0.4374;
   tcWacd  = 0.0203;
   tcWcac  = 0.0304;
   tcTaba  = 0.1264;
   tcTbca  = 0.0035;
   tcXcccc = 0.5791;
   tcXcccd = 0.0314;
   tcXabcd = 0.3048;
   tcXabdd = 0.3557;
   tcYabd  = 0.9461;
   tcYccc  = 0.2584;
   tcYddd  = 0.0892;
   tcYaab  = 0.0983;
   tcYabc  = 0.0321;
   tcYacc  = 0.7480;
   tcYadd  = 0.1715;
   tcYacd  = 0.4568;
   tcKabcd = 0.1089;
   tcKcccd = 0.0376;
   tcKcdcd = 0.1050;
   tcKddcd = 0.0660;
   tcEa    = 0.6507;
   tcEc    = 0.8878;
   tcEd    = 0.0248;
#endif

#ifdef GENOA
   tcOe    = 0.3539;
   tcOk    = 0.2766;
   tcOo    = 0.0001 * 1000.0 + 1;
   tcOp    = 0.5701;
   tcOq    = 0.1243;
   tcOy    = 0.2329;     //tcOy = 0.400;
   tcOz    = 0.6613;
   tcSm    = 0.9501;
   tcSp    = 0.8528;
   tcSq    = 0.9429;
   tcSu    = 0.4680;
   tcSv    = 0.5047;
   tcLba   = 0.3678;
   tcLab   = 0.9374;
   tcLac   = 0.2538;
   tcLbd   = 0.0928;
   tcLaa   = 0.0571;
   tcMbcca = 0.0371;
   tcMbcda = 0.2880;
   tcMccdc = 0.1229;
   tcMcdcd = 0.0959;
   tcMcddc = 0.0680;
   tcMdccd = 0.0680;
   tcTbaa  = 0.7501;
   tcTbac  = 0.0899;
   tcTbad  = 0.0478;
   tcTbda  = 0.0001;
   tcTbdc  = 0.0205;
   tcWbca  = 0.8487;
   tcWcdc  = 0.9519;
   tcWdcd  = 0.4435;
   tcWbda  = 0.5583;
   tcWbaa  = 0.5429;
   tcWacd  = 0.0302;
   tcWcac  = 0.3824;
   tcTaba  = 0.0299;
   tcTbca  = 0.0000;
   tcXcccc = 0.3146;
   tcXcccd = 0.5185;
   tcXabcd = 0.8759;
   tcXabdd = 0.3683;
   tcYabd  = 0.9359;
   tcYccc  = 0.2715;
   tcYddd  = 0.1018;
   tcYaab  = 0.1026;
   tcYabc  = 0.0448;
   tcYacc  = 0.6866;
   tcYadd  = 0.2172;
   tcYacd  = 0.5014;
   tcKabcd = 0.4365;
   tcKcccd = 0.0936;
   tcKcdcd = 0.1499;
   tcKddcd = 0.0730;
   tcEa    = 0.8374;
   tcEc    = 0.8501;
   tcEd    = 0.0279;
#endif

   tcMdcdd = 0.110;

   tcTbcc = 0.009;
   tcTcca = 0.030;
   tcTcda = 0.012;
   tcTdda = 0.007;

   tcWabc = 0.036;
   tcWabd = 0.036;
   tcWacc = 0.036;
   tcWadc = 0.036;
   tcWbcd = 0.036;
   tcWbdc = 0.036;
   tcWbdd = 0.036;
   tcWcbd = 0.036;
   tcWdad = 0.036;

   tcXcdcd = 0.039;
   tcXcddd = 0.022;
   tcXdddd = 0.022;

   tcXccccc = 0.972;
   tcXddddd = 0.014;

   tcXcccccc = 0.556;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}

void CCueLabelsVarley_LabellingConstants::SetCombinedVTuningConstants ()
{
#ifdef CURRENT
   tcOe    = 0.0300;
   tcOk    = 0.2964;
   tcOo    = 0.4500 * 1000.0 + 1;
   tcOp    = 0.0284;
   tcOq    = 0.9498;
   tcOy    = 0.0482;     //tcOy = 0.350;
   tcOz    = 0.1035;
   tcSm    = 0.7547;
   tcSp    = 0.7499;
   tcSq    = 0.9436;
   tcSu    = 0.8500;
   tcSv    = 0.1272;
   tcLba   = 0.0299;
   tcLab   = 0.0300;
   tcLac   = 0.2700;
   tcLbd   = 0.0926;
   tcLaa   = 0.0483;
   tcMbcca = 0.0326;
   tcMbcda = 0.1147;
   tcMccdc = 0.1230;
   tcMcdcd = 0.0960;
   tcMcddc = 0.0680;
   tcMdccd = 0.0680;
   tcTbaa  = 0.9503;
   tcTbac  = 0.0253;
   tcTbad  = 0.0902;
   tcTbda  = 0.0303;
   tcTbdc  = 0.2699;
   tcWbca  = 0.8481;
   tcWcdc  = 0.7434;
   tcWdcd  = 0.2212;
   tcWbda  = 0.0502;
   tcWbaa  = 0.4502;
   tcWacd  = 0.0300;
   tcWcac  = 0.2120;
   tcTaba  = 0.1668;
   tcTbca  = 0.0300;
   tcXcccc = 0.5961;
   tcXcccd = 0.0366;
   tcXabcd = 0.6236;
   tcXabdd = 0.3331;
   tcYabd  = 0.8500;
   tcYccc  = 0.2700;
   tcYddd  = 0.0900;
   tcYaab  = 0.1417;
   tcYabc  = 0.0302;
   tcYacc  = 0.8896;
   tcYadd  = 0.1565;
   tcYacd  = 0.0923;
   tcKabcd = 0.1535;
   tcKcccd = 0.1154;
   tcKcdcd = 0.1050;
   tcKddcd = 0.0660;
   tcEa    = 0.6106;
   tcEc    = 0.8386;
   tcEd    = 0.2248;
#endif

#ifdef BEIJING
   tcOe    = 0.0302;
   tcOk    = 0.3293;
   tcOo    = 0.7049 * 1000.0 + 1;
   tcOp    = 0.0284;
   tcOq    = 0.9498;
   tcOy    = 0.0482;     //tcOy = 0.350;
   tcOz    = 0.1035;
   tcSm    = 0.7000;
   tcSp    = 0.7499;
   tcSq    = 0.9498;
   tcSu    = 0.9498;
   tcSv    = 0.3902;
   tcLba   = 0.0299;
   tcLab   = 0.0471;
   tcLac   = 0.1974;
   tcLbd   = 0.0926;
   tcLaa   = 0.0483;
   tcMbcca = 0.0304;
   tcMbcda = 0.3825;
   tcMccdc = 0.1230;
   tcMcdcd = 0.0960;
   tcMcddc = 0.0680;
   tcMdccd = 0.0680;
   tcTbaa  = 0.9503;
   tcTbac  = 0.0253;
   tcTbad  = 0.0902;
   tcTbda  = 0.0303;
   tcTbdc  = 0.2699;
   tcWbca  = 0.8481;
   tcWcdc  = 0.8842;
   tcWdcd  = 0.2212;
   tcWbda  = 0.0502;
   tcWbaa  = 0.4502;
   tcWacd  = 0.0902;
   tcWcac  = 0.2120;
   tcTaba  = 0.1811;
   tcTbca  = 0.0300;
   tcXcccc = 0.5498;
   tcXcccd = 0.0366;
   tcXabcd = 0.3338;
   tcXabdd = 0.3020;
   tcYabd  = 0.9396;
   tcYccc  = 0.0883;
   tcYddd  = 0.0300;
   tcYaab  = 0.2190;
   tcYabc  = 0.0302;
   tcYacc  = 0.8896;
   tcYadd  = 0.1557;
   tcYacd  = 0.0923;
   tcKabcd = 0.1535;
   tcKcccd = 0.1154;
   tcKcdcd = 0.1050;
   tcKddcd = 0.0660;
   tcEa    = 0.6106;
   tcEc    = 0.8256;
   tcEd    = 0.2248;
#endif

#ifdef GENOA
   tcOe    = 0.6420;
   tcOk    = 0.0570;
   tcOo    = 0.8432 * 1000.0 + 1;
   tcOp    = 0.0960;
   tcOq    = 0.8432;
   tcOy    = 0.1043;     //tcOy = 0.350;
   tcOz    = 0.1894;
   tcSm    = 0.6447;
   tcSp    = 0.7862;
   tcSq    = 0.5033;
   tcSu    = 0.6500;
   tcSv    = 0.6512;
   tcLba   = 0.0246;
   tcLab   = 0.4543;
   tcLac   = 0.1527;
   tcLbd   = 0.2700;
   tcLaa   = 0.0966;
   tcMbcca = 0.0446;
   tcMbcda = 0.3129;
   tcMccdc = 0.1296;
   tcMcdcd = 0.0300;
   tcMcddc = 0.0680;
   tcMdccd = 0.0680;
   tcTbaa  = 0.9414;
   tcTbac  = 0.0367;
   tcTbad  = 0.1455;
   tcTbda  = 0.0259;
   tcTbdc  = 0.0244;
   tcWbca  = 0.5448;
   tcWcdc  = 0.7503;
   tcWdcd  = 0.3308;
   tcWbda  = 0.0309;
   tcWbaa  = 0.4950;
   tcWacd  = 0.1852;
   tcWcac  = 0.3028;
   tcTaba  = 0.2028;
   tcTbca  = 0.0315;
   tcXcccc = 0.6919;
   tcXcccd = 0.0566;
   tcXabcd = 0.6591;
   tcXabdd = 0.5650;
   tcYabd  = 0.9429;
   tcYccc  = 0.0965;
   tcYddd  = 0.0365;
   tcYaab  = 0.2342;
   tcYabc  = 0.0434;
   tcYacc  = 0.8759;
   tcYadd  = 0.0976;
   tcYacd  = 0.0546;
   tcKabcd = 0.1580;
   tcKcccd = 0.1701;
   tcKcdcd = 0.2061;
   tcKddcd = 0.2164;
   tcEa    = 0.6108;
   tcEc    = 0.8095;
   tcEd    = 0.2283;
#endif

   tcMdcdd = 0.110;

   tcTbcc = 0.009;
   tcTcca = 0.030;
   tcTcda = 0.012;
   tcTdda = 0.007;

   tcWabc = 0.036;
   tcWabd = 0.036;
   tcWacc = 0.036;
   tcWadc = 0.036;
   tcWbcd = 0.036;
   tcWbdc = 0.036;
   tcWbdd = 0.036;
   tcWcbd = 0.036;
   tcWdad = 0.036;

   tcXcdcd = 0.039;
   tcXcddd = 0.022;
   tcXdddd = 0.022;

   tcXccccc = 0.972;
   tcXddddd = 0.014;

   tcXcccccc = 0.556;
   tcXcdcdcd = 0.333;
   tcXdddddd = 0.111;
}


void CCueLabelsVarley_LabellingConstants::SetCombinedTuningConstants (int LambanAxisOptions,
																	  bool UseDeductionsInLamban)
{
   if (LambanAxisOptions == LambanAxesFixedIJK)
   {
      SetCombinedFTuningConstants();
   }
   else if (UseDeductionsInLamban)
   {
      SetCombinedDTuningConstants();
   }
   else
   {
      SetCombinedVTuningConstants();
   }
}


void CCueLabelsVarley_LabellingConstants::SetLabellingConstants (short int LabellingProcedure,
																 bool UseJunctionStatisticsForRelaxationLabelling,
																 bool UseShapePairStatisticsForRelaxationLabelling,
																 int RelaxationIterations,
																 bool UseMaxInRelaxationLabelling,
																 int LambanAxisOptions,
																 bool UseDeductionsInLamban)
{
   //if (UseBinaryFrontalGeometry(false, cFALSE))
   //{
   //   UseBFGTuningConstants();
   //}
   //else
	switch(LabellingProcedure)
	{
		case LABELLING_COMBIN_ONLY:
			SetIntersectionTuningConstants();
		break;

		case LABELLING_RELAXN_COMBIN:
			SetRelaxationTuningConstants(UseJunctionStatisticsForRelaxationLabelling,
										 UseShapePairStatisticsForRelaxationLabelling,
										 RelaxationIterations,
										 UseMaxInRelaxationLabelling);
		break;

		case LABELLING_LAMBAN_RELAXN_COMBIN:
			SetLambanTuningConstants(UseMaxInRelaxationLabelling,
									 LambanAxisOptions,
									 UseDeductionsInLamban);
		break;

		case LABELLING_COMBIN_RELAXN:
			SetSemiCombinedTuningConstants();
		break;

		case LABELLING_COMBIN_LAMBAN_RELAXN:
			SetCombinedTuningConstants(LambanAxisOptions,
									   UseDeductionsInLamban);
		break;
	}
}
