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

#include <fstream>
#include <string>
#include <vector>
#include <cmath>        // sin, cos, sqrt, atan2
#include <time.h>       // ctime_s
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "Refer.h"
#include "resource.h"

#include "CueMainAxes.h"

#include "InOutIGES.h"

// public:


InOutIGES::InOutIGES()
{
};


InOutIGES::~InOutIGES()
{
};


int InOutIGES::digits(long number)
//returns the number of digits of number
{
	int digits= 0;
	do {number/=10; digits++;} while(number!=0);
	return digits;
}


void InOutIGES::globalSection (std::ofstream &out, CDB_Model *DDBB_IGES, const char *file, long &numG)
//The IGES files require a Global Section
//containing information describing the preprocessor
//and information needed by postprocessors to handle the file

//Global Section records are identified with the letter code "G" in column 73
//and are sequenced in columns 74-80

//The number of global lines printed is returned in numG
{
	std::string global;
	char CadenaAux[20];

	global=  "1H,,";  // 01 Parameter delimiter character
	global+= "1H;,";  // 02 Record delimiter character

	//03 Product identification from sending system (we use the short file name)
	std::string name= file;
	std::string sname= name.substr(name.rfind('\\')+1, name.length());
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", digits(sname.length()), sname.length());
	std::string shortname= CadenaAux;    shortname+= "H";    shortname+= sname;

	global+= shortname;    global+= ",";

	//04 File name (we use the file name)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", digits(name.length()), name.length());
	global+= CadenaAux;    global+= "H";    global+= name;    global+= ",";

	global+=  "9HREFER 7.0,";          // 05 Native system ID
	global+=  "14HREFER IGES 5.3,";    // 06 Preprocessor version

	global+=  "32,";     // 07 Number of binary bits for integer representation
	global+=  "38,";     // 08 Maximum power often representable in a single-precision floating point number on the sending system
	global+=  "6,";      // 09 Number of significant digits in a single-precision floating point number on the sending system
	global+=  "308,";    // 10 Maximum power of ten representable in a double-precision floating point number on the sending system
	global+=  "15,";     // 11 Number of significant digits in a double-precision floating point number on the sending system

	global+= shortname;   // 12 Product identification for the receiving system (we use the short name of the file)
	global+= ",";

	global+=  "1.0,";      // 13 Model space scale
	global+=  "2,";        // 14 Units flag
	global+=  "2HMM,";     // 15 Units Name

	global+=  "1,";        // 16 Maximum number of line weight gradations
	global+=  "0.250,";     // 17 Width of maximum line weight in units

	global+=  "15H";     // 18 Date and time of exchange file generation 15HYYYYMMDD.HHNNSS or 13HYYMMDD.HHNNSS
	char bufferDateTime[20];
	time_t rawtime = time(0);          //Returns the current calendar time encoded as a std::time_t object
	struct tm timeinfo;                //Structure containing a calendar date and time broken down into its components.
	localtime_s(&timeinfo, &rawtime);  //Convert time_t to tm as local time (expressed for the local timezone)
	strftime(bufferDateTime,sizeof bufferDateTime,"%Y%m%d.%H%M%S",&timeinfo);  //Convert local time to string
	for(long i=0; i < 15; i++) {global+= bufferDateTime[i];}
	global+=  ",";

	// 19 Minimum user-intended resolution or granularity of the model in units specified by Parameter 14.
	//WARNING: here we MUST set the acceptable precision!!!
	global+=  "0.001D0,";

	// 20 Approximate maximum coordinate value occurring in the model in units specified by Parameter 14.
	//   (It specifies a cubic volume, centered on the origin, which can enclose the entire model)
	double maxCoor= 0;
	for(long i=0; i < DDBB_IGES->GetSizeVertices(); i++) {
		CVertex Vertice= DDBB_IGES->GetVertex(i);
		maxCoor= max(maxCoor, std::abs(Vertice.x));
		maxCoor= max(maxCoor, std::abs(Vertice.y));
		maxCoor= max(maxCoor, std::abs(Vertice.z));
	}
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",maxCoor);
	global+= CadenaAux;
	global+= ",";

	global+=  ",";     // 21 Name of author
	global+=  ",";     // 22 Author’s organization
	global+=  "10,";         // 23 Flag value corresponding to the version of the Specification to which this file complies.
	global+=  "0,";          // 24 Flag value corresponding to the drafting standard to which this file complies, if any.

	// 25 Date and time the model was created or last modified, in same format as field 18.
	global+= "15H";
	for(long i=0; i < 15; i++) {global+= bufferDateTime[i];}
	//global+= ",";

	// 26 Descriptor indicating application protocol, application subset, Mil-specification, or user-defined protocol or subset, if any
	global+=  ";";

	//Prints the global information in as much lines as necessary
	//-----------------------------------------------------------
	long gl= global.length();
	long glines= gl/72;
	long grest= gl%72;
	numG= 0;

	//Print the complete lines
	for(long i=0; i < glines; i++) {
		long ii= (i*72);
		std::string lineaActual= global.substr((i*72), 72);
		out <<global.substr((i*72), 72);
		out <<"G     ";
		numG++;
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 2, numG);
		out << CadenaAux << std::endl;
	}

	//Print the last (incomplete) line
	out <<global.substr((glines*72), grest);
	for(long i=grest; i < 72; i++) {out <<" ";} //Fill with spaces until column 72
	out <<"G     ";
	numG++;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 2, numG);
	out << CadenaAux << std::endl;
}


long InOutIGES::PrintMultiLineParameter(std::ofstream &BufferParams, 
										std::string parameter,
										long &numP, long numD)
//Print parameter, in as much lines as required

//Printing is done to a buffer file

//numP is increased by the function if multiple lines are printed

//- The free-formatted part of a parameter line ends in Column 64
//- Column 65 shall contain a space character
//- Columns 66 through 72 on all parameter lines shall contain
//  the sequence number of the first line in the Directory Entry of this entity (numD)
//- Column 73 of all lines in the Parameter Data Section shall contain the letter P
//  and Columns 74 through 80 shall contain the sequence number
{
	char CadenaAux[20];

	//Calculate the required lines
	long pl= parameter.length();
	long lines= pl/64;
	long rest= pl%64;

	//Print the complete lines
	for(long i=0; i < lines; i++) {
		long ii= (i*64);
		std::string lineaActual= parameter.substr((i*64), 64);
		BufferParams <<parameter.substr((i*64), 64);

		BufferParams <<" ";  //Column 65
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
		BufferParams << CadenaAux;  //Columns 66 to 72.

		numP++;
		BufferParams <<"P ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 6, numP);
		BufferParams << CadenaAux << std::endl;
	}

	//Print the last (incomplete) line
	if(rest > 0){
		BufferParams <<parameter.substr((lines*64), rest);
		for(long i=rest; i < 64; i++) {BufferParams <<" ";} //Fill with spaces until column 64

		BufferParams <<" ";  //Column 65
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
		BufferParams << CadenaAux;

		numP++;
		BufferParams <<"P ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 6, numP);

		BufferParams << CadenaAux << std::endl;
	}

	return (lines + (rest!=0));
}


long InOutIGES::Add_000_Params(std::ofstream &BufferParams, 
							   long param1, 
							   double param2,
							   long &numP, long numD)
//Parameter data is free-formatted,.

//The Null Entity (Type 0)  may contain an arbitrary amount of data in its PD data

//It was added to this implementation to serve as a pattern to implement new types
//It is also useful to convey information not to be processsed by IGES translators
{
	std::string parameter;
	char CadenaAux[20];

	parameter="000";

	//Add param1 as the first parameter
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",param1);
	parameter+=",";
	parameter+= CadenaAux;

	//Add param2 as the second parameter
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",param2);
	parameter+=",";
	parameter+= CadenaAux;
	parameter+=";";

	//Print parameters, in as much lines as required
	//----------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_000_Directory(std::ofstream &out, long LineCount000, long numP, long &numD)
//The Null Entity (Type 0) is intended to be ignored by a processor.

//It was added to this implementation to serve as a pattern to implement new types

//It may be also useful to remove an entity from the current file
//without renumbering the entire file.
{
	std::string directory;
	char CadenaAux[20];

	//1 Entity type number
	directory= "     000";

	//2 Parameter data (synchronized with Add_000_Params though numP and LineCount000)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount000+1));
	directory+= CadenaAux;

	//3 Structure (This field has meaning only for the Macro Instance Entity)
	directory+= "       0";

	//4 Line Font Pattern: 0 No patter specified (default)
	directory+= "       0";

	//5 Level
	directory+= "       0";

	//6 View (zero (default: entity is visible in all views, and its display characteristics are the same in all views)
	directory+= "       0";

	//7 Transformation matrix (Zero implies the identity rotation matrix and a zero translation vector)
	directory+= "       0";

	//8 Label display Associativity Entity (Type 402, Form 5)
	//(which defines how the entity’s label and subscript are to be displayed in different views)
	directory+= "       0";

	//9 Status number
	//(contains four pieces of information which are concatenated into a single integer number)
	directory+= "00000000"; // | Visible | Independent | Geometry | Global top down |
							//Status number:
								//Comprises four two-digit values which are concatenated in
								//the order listed into a single 8-digit number which fills the
								//field; no space characters are allowed.
									//1-2
									//3-4
									//5-6
									//7-8
								//Blank Status
									//00 Visible
									//01 Blanked
								//Subordinate Entity Switch
									//00 Independent
									//01 Physically Dependent
									//02 Logically Dependent
									//03 Both (01) and (02)
								//Entity Use Flag
									//00 Geometry
									//01 Annotation
									//02 Definition
									//03 Other
									//04 Logical/Positional
									//05 2D Parametric
									//06 Construction geometry
								//Hierarchy
									//00 Global top down
									//01 Global defer
									//02 Use hierarchy property

	//10 Sequence number
	//(A number which specifies the sequence number of the DE line in the Directory Entry Section)
	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	//-----------
	numD++;
	//11 Entity type number
	directory= "     000";

	//12 Line weight number
	//(specifies the thickness (or width) to use for displaying an entity.)
	directory+= "       0"; //0= use default line weight display thickness of the receiving system

	//13 Color number
	//A non-negative color number represents "standard" colors
	//and shall be specified when the precise shade is unimportant;
	//a negated value shall be specified when the precise shade is important;
	//its absolute value references a Color Definition Entity (Type 314).
	directory+= "       0";

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount000);
	directory+= CadenaAux;

	//15 Form number
	//(indicates an individual interpretation of the entity
	//to be used when processing the parameter data for this entity 
	//for those entity types having multiple interpretations of their parameter data)
	directory+= "       0"; //zero (default).

	//16 Reserved
	directory+= "        ";

	//17 Reserved
	directory+= "        ";

	//18 Entity label
	//(This is the application-specified alphanumeric identifier or name for this entity.
	//It is used in conjunction with the entity subscript number (Field 19) 
	//to provide the application-specified alphanumeric identifier for the entity. 
	//The entity label is right-justified within the field with leading space fill.)
	directory+= "        ";

	//19 Entity subscript
	//(This is a numeric qualifier for the entity label (Field 18).)
	directory+= "       0";

	//20 Sequence number
	//(next number that used in field 10)
	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_102_Params(std::ofstream &BufferParams, 
							   std::vector <long> pointerEntities,
							   long &numP, long numD)
//Parameter data for a composite curve (102)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="102";

	//1 				INT 	N 	Number of entities
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d", pointerEntities.size());
	parameter+=",";
	parameter+= CadenaAux;

	for(long i=0; i < (long)pointerEntities.size(); i++){
		//2 				Pointer 	MC1 	Pointer to i-th constituent entity
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerEntities[i]);
		parameter+=",";
		parameter+= CadenaAux;
	}
	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_102_Directory(std::ofstream &out, long LineCount102, 
								  long colorNumber, long numLevel, 
								  long numCurve,
								  std::string formName,
								  std::string statusNumber,
								  long numP, long &numD)
//Identifies a composite curve,
//as a continuous curve that results from the grouping 
//individual constituent entities into a logical unit.
{
	std::string directory;
	char CadenaAux[20];

	directory= "     102";

	//2 Parameter data (synchronized with Add_141_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount102+1));
	directory+= CadenaAux;

	directory+= "       0       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;
	directory+= "       0       0       0";
	directory+= statusNumber;

	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     102       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount102);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       0                ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numCurve);
	directory+= CadenaAux;

	//20 Sequence number
	directory+="D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_108_01_Params(std::ofstream &BufferParams, 
								  CVertex P1, 
								  CVertex P2, 
								  CVertex P3, 
								  long pointerContour,
								  long &numP, long numD)
//Parameter data for a bounded/unbounded plane (108)

//Field 15 contains the form number (1 for bounded plane)
//The bounding perimeter is delimited by the 102 curve defined by pointerContour
{
	std::string parameter;
	char CadenaAux[20];

	parameter="108";


	//Plane equation parameters
	double a, b, c, d;
	equation_3_points_plane(P1.x, P1.y, P1.z, //Point1
							P2.x, P2.y, P2.z, //Point2
							P3.x, P3.y, P3.z, //Point3
							a, b, c, d);//Plane's parameters
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",a);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",b);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",c);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",d);
	parameter+=",";
	parameter+= CadenaAux;

	//5		PTR Pointer Pointer to the DE of the closed curve entity
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerContour);
	parameter+=",";
	parameter+= CadenaAux;

	//6 	REAL 	X 	x coordinate of display symbol
	//7 	REAL 	Y 	y coordinate of display symbol
	//8 	REAL 	Z 	z coordinate of display symbol
	//11 	REAL 	Size 	size of display symbol 
	parameter+=",0.0,0.0,0.0,0.0";

	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_108_01_Directory(std::ofstream &out, long LineCount108, 
									 long colorNumber, long numLevel, 
									 long numLine,
									 std::string formName,
									 std::string statusNumber,
									 long numP, long &numD)
//Type 108 represents planes

//A plane form 1 is bounded.
{
	std::string directory;
	char CadenaAux[20];

	directory= "     108";

	//2 Parameter data (synchronized with Add_110_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount108+1));
	directory+= CadenaAux;

	directory+= "       0       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;
	directory+= "       0       0       0";
	directory+= statusNumber;

	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     108       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount108);
	directory+= CadenaAux;

	//15 Form number (1= Positive (filled) bounded planar portion)
	directory+= "       1                ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLine);
	directory+= CadenaAux;

	//20 Sequence number
	directory+="D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_110_00_Params(std::ofstream &BufferParams, 
								  CVertex VertexEdgeITail, 
								  CVertex VertexEdgeIHead,
								  long &numP, long numD)
//Parameter data for a line(110)

//Field 15 contains the form number (0 for line segments)

//A line segment is defined by its end points
//Each end point is specified relative to definition space by triple coordinates.
//A direction is associated with the line
//by considering the start point to be listed first and the terminate point second

{
	std::string parameter;
	char CadenaAux[20];

	parameter="110";

	//Tail Vertex coordinates
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",VertexEdgeITail.x);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",VertexEdgeITail.y);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",VertexEdgeITail.z);
	parameter+=",";
	parameter+= CadenaAux;

	//Head Vertex coordinates
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",VertexEdgeIHead.x);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",VertexEdgeIHead.y);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",VertexEdgeIHead.z);
	parameter+=",";
	parameter+= CadenaAux;
	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_110_00_Directory(std::ofstream &out, long LineCount110, 
									 long colorNumber, long numLevel, 
									 long numLine,
									 std::string formName,
									 std::string statusNumber,
									 long numP, long &numD)
//Type 110 represents lines

//A line form 00 is a bounded, connected portion of a straight line 
//which has distinct start and terminate points.
{
	std::string directory;
	char CadenaAux[20];

	directory= "     110";

	//2 Parameter data (synchronized with Add_110_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount110+1));
	directory+= CadenaAux;

	directory+= "       0       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;

	directory+= "       0       0       0";
	directory+= statusNumber;

	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     110       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount110);
	directory+= CadenaAux;

	//15 Form number (0= segment)
	directory+= "       0                ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLine);
	directory+= CadenaAux;

	//20 Sequence number
	directory+="D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_116_Params(std::ofstream &BufferParams, CVertex VertexI,
							   long &numP, long numD)
//Parameter data for a 3D point (116)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="116";

	//Point Coordinates
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",VertexI.x);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",VertexI.y);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",VertexI.z);
	parameter+=",";
	parameter+= CadenaAux;

	//Pointer to the DE of the Subfigure Definition Entity specifying
	//the display symbol or zero. 
	//If zero, no display symbol is specified.
	parameter+=",0;";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter,  numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_116_Directory(std::ofstream &out, long LineCount116, 
								  long numPoint,
								  std::string formName,
								  long numP, long &numD)
//A point entity is defined by its coordinates in definition space.

//An optional pointer to a Subfigure Definition Entity (Type 308) references a display symbol

//If PD Index 4 (Pointer to Display Geometry) is 0 or defaulted, 
//Line Font Pattern, Line Weight, and Hierarchy are ignored.
{
	std::string directory;
	char CadenaAux[20];

	directory= "     116";

	//2 Parameter data (synchronized with Add_116_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount116+1));
	directory+= CadenaAux;

	directory+= "       0       0       0       0       0       000000000D";//Status number: Visible | Independent | Geometry | Global top down

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     116       0       0";

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount116);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       0                ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numPoint);
	directory+= CadenaAux;

	//20 Sequence number
	directory+="D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_123_Params(std::ofstream &BufferParams, CVector Normal,
							   long &numP, long numD)
//Parameter data for a 3D direction (123)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="123";

	//Direction vector
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Normal.x);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Normal.y);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Normal.z);
	parameter+=",";
	parameter+= CadenaAux;

	//////Pointer to the DE of the Subfigure Definition Entity specifying
	//////the display symbol or zero. 
	//////If zero, no display symbol is specified.
	////parameter+=",0";
	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_123_Directory(std::ofstream &out, long LineCount123, long numP, long &numD)
//A direction entity is a non-zero vector in Euclidean 3-space 
//that is defined by its three components (direction ratios) with respect to the coordinate axes 

//(Used by Face entity)
{
	std::string directory;
	char CadenaAux[20];

	directory= "     123";

	//Parameter data (synchronized with Add_116_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount123+1));
	directory+= CadenaAux;

	directory+= "       0       0       0       0       0       000000000D";//Status number: Visible | Independent | Geometry | Global top down

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     123       0       0";

	//Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount123);
	directory+= CadenaAux;

	//Form number
	directory+= "       0                 DIRTION       0D";

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_126_01_Params(std::ofstream &BufferParams, 
								  CVertex P1, CVertex P2, CVector Normal,
								  long &numP, long numD)
//Parameter data for rational B-spline curve(126)

//Form 1 is for line
{
	std::string parameter;
	char CadenaAux[20];

	parameter="126";

	//Index		Type 	Name 	Description
	//-----		----	----	-----------
	//1			INT 	K 		Upper index of first sum
	parameter+=",1";
	//2 		INT 	M 		Degree of basis functions
	parameter+=",1";
	//3 		INT 	Flag1 	0 = nonplanar, 1 = planar
	parameter+=",1";
	//4 		INT 	Flag2 	0 = open curve, 1 = closed curve
	parameter+=",0";
	//5 		INT 	Flag3 	0 = rational, 1 = polynomial
	parameter+=",1";
	//6 		INT 	Flag4 	0 = nonperiodic, 1 = periodic
	parameter+=",0";

	//7 		REAL 	Knot sequence
	parameter+=",0.";
	parameter+=",0.";
	parameter+=",1.";
	parameter+=",1.";

	//14+K1+M1+K2+M2 	REAL 	W0 	First weight
	parameter+=",1.";
	parameter+=",1.";  //		WK, 2 weights

	//^+1 	REAL 	Control points
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",P1.x);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",P1.y);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",P1.z);
	parameter+= ",";
	parameter+= CadenaAux;


	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",P2.x);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",P2.y);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",P2.z);
	parameter+= ",";
	parameter+= CadenaAux;

	//^+1 	REAL 	U0 	Start parameter value
	parameter+=",0.";
	//^+1 	REAL 	U1 	End parameter value
	parameter+=",1.";

	//^+1 	REAL 	V0 	Unit normal (if curve is planar)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Normal.x);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Normal.y);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Normal.z);
	parameter+=",";
	parameter+= CadenaAux;
	parameter+= ";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_126_01_Directory(std::ofstream &out, long LineCount126_01,
									 long colorNumber, long numLevel,
									 long numEdge,
									 std::string formName,
									 std::string statusNumber,
									 long numP, long &numD)
//The rational B-spline curve represents various analytical curves of general interest

//The form 1 processes lines
{
	std::string directory;
	char CadenaAux[20];

	directory= "     126";

	//2 Parameter data (synchronized with Add_128_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount126_01+1));
	directory+= CadenaAux;

	directory+= "       0       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;

	directory+= "       0       0       0";
	directory+= statusNumber;

	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     126       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount126_01);
	directory+= CadenaAux;

	//15 Form number  
	directory+= "       1";  //(1 if line)
	directory+= "                ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numEdge);
	directory+= CadenaAux;

	//20 Sequence number
	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_128_00_Params(std::ofstream &BufferParams, 
								  std::vector <CVertex> Q_gon, 
								  long &numP, long numD)
//Parameter data for Rational B-Spline Surface (128)

//Form 0 is where Form of surface is determined from the rational B-spline parameters
{
	//In this implementation, Q_gon is processed as a ruled surface
	return Add_128_08_Params(BufferParams, Q_gon, numP, numD);
}


void InOutIGES::Add_128_00_Directory(std::ofstream &out, long LineCount128_00, 
									 long colorNumber, long numLevel,
									 long numShell, std::string formName,
									 std::string statusNumber,
									 long numP, long &numD)
//NURBS: Rational B-Spline Surface (Type 128)

//This entity is used as the primary entity for surface transfer. 
//All the other surface types must be converted (maybe with approximation) to this entity for transfer.
//This is the most flexible format to represent surfaces
//and is recommended for transferring all surfaces. 
//This entity has forms for some analytic surfaces.
//If the sending system can determine the Form of the surface,
//the Form of this entity should be set appropriately.


//The rational B-spline surface represents various analytical surfaces of general interest

//The form 0 processes surfaces determined from the rational B-spline parameters
{
	std::string directory;
	char CadenaAux[20];

	directory= "     128";

	//2 Parameter data (synchronized with Add_128_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount128_00+1));
	directory+= CadenaAux;

	directory+= "       0       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;
	directory+= "       0       0       0";
	directory+= statusNumber;

	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     128       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount128_00);
	directory+= CadenaAux;

	//15 Form number  
	directory+= "       0";  //(0 if Form of surface is determined from the rational B-spline parameters)
	directory+= "                  ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numShell);
	directory+= CadenaAux;

	//20 Sequence number
	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_128_08_Params(std::ofstream &BufferParams, 
								  std::vector <CVertex> Q_gon,
								  long &numP, long numD)
//Parameter data for Rational B-Spline Surface (128)

//Form 8 is for bilinear surfaces (2x2 control point patch, degree = 1, weight 1 for non-rational surface))
{
	std::string parameter;
	char CadenaAux[20];

	parameter="128";

	//Index		Type 	Name 	Description
	//-----		----	----	-----------
	//1			INT 	K1 		Upper index of first sum (Number of control points less one in u)
	parameter+=",1";
	//2 		INT 	K2 		Upper index of second sum (Number of control points less one in V)
	parameter+=",1";
	//3 		INT 	M1 		Degree of first basis functions (1 for line)
	parameter+=",1";
	//4 		INT 	M2 		Degree of second basis functions (1 for line)
	parameter+=",1";

	//5 		INT 	Flag1 	0= Open in first direction, 1=closed
	parameter+=",0";
	//6 		INT 	Flag2 	0= Open in second direction, 1=closed
	parameter+=",0";
	//7 		INT 	Flag3 	0=rational, 1=polynomial
	parameter+=",1";
	//8 		INT 	Flag4 	0=nonperiodic in first direction , 1=periodic
	parameter+=",0";
	//9 		INT 	Flag5 	0=nonperiodic in second direction , 1=periodic
	parameter+=",0";

	//10 		REAL 	First Knot sequence (K1+M1 knots) (knots repeated to force the line to pass through them)
	//                  Surface's knot vector in u parametric direction
	parameter+=",0.";   //U_min
	parameter+=",0.";   //U_min
	double lengthU= MODULE_3D(Q_gon[0], Q_gon[1]);
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",lengthU);
	parameter+= ",";
	parameter+= CadenaAux;   //U_max
	parameter+= ",";
	parameter+= CadenaAux;   //U_max
	//parameter+=",1.";   //U_max
	//parameter+=",1.";   //U_max

	//12+K1+M1 	REAL 	Second knot sequence (K2+M2 knots) (knots repeated to force the line to pass through them)
	//                  Surface's knot vector in v parametric direction
	parameter+=",0.";   //V_min
	parameter+=",0.";   //V_min
	double lengthV= MODULE_3D(Q_gon[0], Q_gon[3]);
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",lengthV);
	parameter+= ",";
	parameter+= CadenaAux;   //V_max
	parameter+= ",";
	parameter+= CadenaAux;   //V_max
	//parameter+=",1.";   //V_max
	//parameter+=",1.";   //V_max

	//14+K1+M1+K2+M2 	REAL 	W(0,0) 	First weight
	parameter+=",1.";
	parameter+=",1.";
	parameter+=",1.";
	parameter+=",1.";  //(K1+1)*(K2+1) = 2*2 = 4 weights

	//^+1 	REAL 	Control points
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[0].x);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[0].y);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[0].z);
	parameter+= ",";
	parameter+= CadenaAux;

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[1].x);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[1].y);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[1].z);
	parameter+= ",";
	parameter+= CadenaAux;

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[3].x);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[3].y);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[3].z);
	parameter+= ",";
	parameter+= CadenaAux;

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[2].x);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[2].y);
	parameter+= ",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Q_gon[2].z);
	parameter+= ",";
	parameter+= CadenaAux;

	//^+1 	REAL 	U0 	Start first parameter value (Starting value of u in the surface definition)
	parameter+=",0.";
	//^+1 	REAL 	U1 	End first parameter value (Ending value of u in the surface definition)
	//double lengthU= MODULE_3D(P0, P1);
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",lengthU);
	parameter+= ",";
	parameter+= CadenaAux;

	//^+1 	REAL 	V0 	Start second parameter value (Starting value of v in the surface definition)
	parameter+=",0.";
	//^+1 	REAL 	V1 	End second parameter value (Ending value of v in the surface definition)
	//double lengthV= MODULE_3D(P0, P3);
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",lengthV);
	parameter+= ",";
	parameter+= CadenaAux;
	parameter+= ";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_128_08_Directory(std::ofstream &out, long LineCount128_08,
									 long colorNumber, long numLevel, 
									 long numShell, std::string formName,
									 std::string statusNumber,
									 long numP, long &numD)
//The rational B-spline surface represents various analytical surfaces of general interest

//The form 8 processes ruled surfaces
{
	std::string directory;
	char CadenaAux[20];

	directory= "     128";

	//2 Parameter data (synchronized with Add_128_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount128_08+1));
	directory+= CadenaAux;

	directory+= "       0       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;
	directory+= "       0       0       0";
	directory+= statusNumber;

	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     128       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount128_08);
	directory+= CadenaAux;

	//15 Form number  
	directory+= "       8";  //(8 if ruled surface)
	directory+= "                  ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numShell);
	directory+= CadenaAux;

	//20 Sequence number
	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_141_Params(std::ofstream &BufferParams, 
							   long pointerUntrimmedSurface, 
							   std::vector <long> pointerCurves,
							   long &numP, long numD)
//Parameter data for surface boundary consisting of curves lying on a surface (141)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="141";

	//Index in list 	Type of data 	Name 	Description
	//1 				INT 	Type 	The type of boundary being represented
										//0=Entities reference model space curves
										//1=Entities reference model space curves and
										//associated parameter space curves
	parameter+=",1";

	//2 				INT 	Pref 	Preferred representation of trimming curves.
										//0 = Unspecified
										//1 = Model Space
										//2 = Parameter Space
										//3 = Equal
	parameter+=",3";

	//3 				Pointer 	Surface 	The untrimmed surface to be bounded
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerUntrimmedSurface);
	parameter+=",";
	parameter+= CadenaAux;

	//4 				INT 	N 	Number of curves in boundary
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d", pointerCurves.size());
	parameter+=",";
	parameter+= CadenaAux;

	for(long i=0; i < (long)pointerCurves.size(); i++){
		//5 				Pointer 	MC1 	Pointer to i-th model space curve
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerCurves[i]);
		parameter+=",";
		parameter+= CadenaAux;

		//6 				INT 	Flag 1 	Orientation flag: 0 = No reversal
												//1 = The direction of the model space curve 
												//    does not require reversal; PSCPT and CRVPT orientations agree
												//2 = The direction of the model space curve 
												//    needs to be reversed; PSCPT and CRVPT orientations disagree
		parameter+=",1";

		//7 				INT 	K1 	How many parameter curves for this model curve
		parameter+=",1";

		//8 				Pointer 	PC(1,1) 	First parameter curve for model curve 1
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerCurves[i]+2);
		parameter+=",";
		parameter+= CadenaAux;
	}
	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_141_Directory(std::ofstream &out, long LineCount141, 
								  long colorNumber, long numLevel, 
								  long numShell,
								  std::string formName,
								  long numP, long &numD)
//Identifies a surface boundary consisting of curves lying on a surface.
{
	std::string directory;
	char CadenaAux[20];

	directory= "     141";

	//2 Parameter data (synchronized with Add_141_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount141+1));
	directory+= CadenaAux;

	directory+= "       0       0";//Status number: Visible | Physically Dependent | Geometry | Global top down
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;
	directory+= "       0       0       000010000D";//Status number: Visible | Physically Dependent | Geometry | Global top down

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     141       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount141);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       0";
	directory+= "                  ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numShell);
	directory+= CadenaAux;

	//20 Sequence number
	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_142_Params(std::ofstream &BufferParams, 
							   long pointerUntrimmedSurface, 
								long pointerBoundary, long pointerBoundaryPar,
							   long &numP, long numD)
//Parameter data for surface boundary consisting of curves lying on a surface (142)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="142";


	//Index in list 	Type of data 	Name 	Description
	//1 				INT 	Flag1 	Indicates how curve was created:
	//									0 = Unspecified
	//									1 = Projection
	//									2 = Intersection of surfaces
	//									3 = Isoparametric curve
	parameter+=",1";

	//2 				Pointer 		Surface 	Points to surface curve lies on
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerUntrimmedSurface);
	parameter+=",";
	parameter+= CadenaAux;

	//3 				Pointer 		Curve 	Definition of curve
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d", pointerBoundary);
	parameter+=",";
	parameter+= CadenaAux;

	//4 				Pointer 		Mapping 	Entity that provides mapping from curve to surface
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d", pointerBoundaryPar);
	parameter+=",";
	parameter+= CadenaAux;

	//5 				INT 			Representation 	Preferred representation of curve:
	//													0 = Unspecified
	//													1 = S(B(t))
	//													2 = C(t)
	//													3 = Both equal 
	parameter+=",1";

	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_142_Directory(std::ofstream &out, long LineCount142, 
								  long colorNumber, long numLevel, 
								  long numShell,
								  std::string formName,
								  long numP, long &numD)
//Identifies a surface boundary consisting of curves lying on a surface.
{
	std::string directory;
	char CadenaAux[20];

	directory= "     142";

	//2 Parameter data (synchronized with Add_141_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount142+1));
	directory+= CadenaAux;

	directory+= "       0       0";//Status number: Visible | Physically Dependent | Geometry | Global top down
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;
	directory+= "       0       0       000010500D";//Status number: Visible | Physically Dependent|2D Parametric | Global top down

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     142       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount142);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       0";
	directory+= "                  ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numShell);
	directory+= CadenaAux;

	//20 Sequence number
	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_143_Params(std::ofstream &BufferParams, 
							   long pointerUntrimmedSurface, long pointerBoundary,
							   long &numP, long numD)
//Parameter data for surface bounded by Boundary Entities (143)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="143";

	//Index in list		Type of data 	Name 	Description
	//1 				INT 			Type 	The type of boundary being represented
												//0=Entities reference model space curves
												//1=Entities reference model space curves and
												//associated parameter space curves
	parameter+=",1";

	//2 				Pointer			Surface 	Points to unbounded surface
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerUntrimmedSurface);
	parameter+=",";
	parameter+= CadenaAux;

	//3 				INT				N 	Number of Boundary entities
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",1);
	parameter+=",";
	parameter+= CadenaAux;

	//4 				Pointer			B1 	Pointer to first boundary entity
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerBoundary);
	parameter+=",";
	parameter+= CadenaAux;

	//...
	//3+N 				Pointer			BN 	Pointer to last boundary entity 

	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_143_Directory(std::ofstream &out, long LineCount143, 
								  long colorNumber, long numLevel, 
								  long numShell,
								  std::string formName,
								  long numP, long &numD)
//Represents a surface bounded by Boundary Entities.
{
	std::string directory;
	char CadenaAux[20];

	directory= "     143";

	//2 Parameter data (synchronized with Add_143_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount143+1));
	directory+= CadenaAux;

	directory+= "       0       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;
	directory+= "       0       0       000000000D";//Status number: Visible | independent | Geometry | Global top down

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     143       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount143);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       0";
	directory+= "                  ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numShell);
	directory+= CadenaAux;

	//20 Sequence number
	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_144_Params(std::ofstream &BufferParams, 
							   long pointerUntrimmedSurface, long pointerBoundary,
							   long &numP, long numD)
//Parameter data for surface bounded by Boundary Entities (144)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="144";

	//Index in list		Type of data 	Name 	Description
	//1 				Pointer 		Surface 	Entity to be trimmed
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerUntrimmedSurface);
	parameter+=",";
	parameter+= CadenaAux;

	//2 				INT 			Flag 	0=Boundary is boundary of surface, 1=otherwise
	parameter+=",1";

	//3 				INT 			N 	Number of closed curves that make up inner boundary
	parameter+=",0";

	//4 				Pointer 		OuterBound 	Pointer to Curve on Parametric Surface
	//									(Type 142) entity that is outer bound
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerBoundary);
	parameter+=",";
	parameter+= CadenaAux;

	//5 				Pointer 		Inner1 	Pointer to first inner curve boundary
	//. 	.
	//. 	
	//5+N 				Pointer 		InnerN 	Pointer to last inner curve boundary 

	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_144_Directory(std::ofstream &out, long LineCount144, 
								  long colorNumber, long numLevel, 
								  long numShell,
								  std::string formName,
								  long numP, long &numD)
//Represents a surface bounded by Boundary Entities.
{
	std::string directory;
	char CadenaAux[20];

	directory= "     144";

	//2 Parameter data (synchronized with Add_143_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount144+1));
	directory+= CadenaAux;

	directory+= "       0       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;
	directory+= "       0       0       000000000D";//Status number: Visible | independent | Geometry | Global top down

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     144       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount144);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       0";
	directory+= "                  ";
	directory+= formName;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numShell);
	directory+= CadenaAux;

	//20 Sequence number
	directory+= "D";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_190_00_Params(std::ofstream &BufferParams, 
								  long DEloc, long DEnormal,
								  long &numP, long numD)
//Parameter data for Plane Surface (190)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="190";

	//Normals to face
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",DEloc);
	parameter+=",";
	parameter+= CadenaAux;
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",DEnormal);
	parameter+=",";
	parameter+= CadenaAux;
	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_190_00_Directory(std::ofstream &out, long LineCount190_00,
									 long numP, long &numD)
//The Plane Surface is given by a point on the plane and the normal. 

//Form 0 corresponds to unparametrized surfaces

//(Used by Face entity)
{
	std::string directory;
	char CadenaAux[20];

	directory= "     190";

	//2 Parameter data (synchronized with Add_116_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount190_00+1));
	directory+= CadenaAux;

	directory+= "       0       0       0       0       0       000000000D";//Status number: Visible | Independent | Geometry | Global top down

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     190       0       0";

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount190_00);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       0                               0D";

	//20 Sequence number
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_314_Params(std::ofstream &BufferParams, long &numP, long numD)
//Parameter data is for Color Definition (314)

//Note: Since this Specification includes no mechanism for specifying background color, 
//exchange partners need to realize that it is possible for entities 
//to have the same color as the display background;
//this makes them appear "invisible" even though they are present
{
	std::string parameter;

	parameter="314";

	//First color coordinate (red) as a percent of full intensity (range 0.0 to 100.0)
	parameter+=",80.0";
	//Second color coordinate (red) as a percent of full intensity (range 0.0 to 100.0)
	parameter+=",80.0";
	//Third color coordinate (red) as a percent of full intensity (range 0.0 to 100.0)
	parameter+=",90.0";

	//Color name (optional).
	parameter+=",19HRGB( 080, 080, 090);";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_314_Directory(std::ofstream &out, long LineCount314, long numP, long &numD)
//The Color Definition Entity specifies the relationship of the primary (red, green, and blue) colors
//to the intensity level of the respective graphics devices as a percent of the full intensity range
{
	std::string directory;
	char CadenaAux[20];

	directory= "     314";

	//2 Parameter data (synchronized with Add_116_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount314+1));
	directory+= CadenaAux;

	directory+= "       0       0       0       0       0       000000200D";  //Status number->Entity Use Flag->02= Definition

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     314       0       1";  //Color Number (1 Black, 8 White)

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount314);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       0                   Color       0D";

	//20 Sequence number
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_402_07_Params(std::ofstream &BufferParams, 
								  std::vector <long> pointerFacets,
								  long &numP, long numD)
//Parameter data for level (402_07)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="402";

	//1 				INT 	N 	Number of entities
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d", pointerFacets.size());
	parameter+=",";
	parameter+= CadenaAux;

	for(long i=0; i < (long)pointerFacets.size(); i++){
		//2 				Pointer 	MC1 	Pointer to the DE of the i-th entity
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%d",pointerFacets[i]);
		parameter+=",";
		parameter+= CadenaAux;
	}
	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_402_07_Directory(std::ofstream &out, long LineCount402_07,
									 long colorNumber, long numLevel, long numP, long &numD)
//The Group Associativity allows a collection of entities 
//to be maintained as a single, logical entity

//Form 1 defines an unordered group with back pointers
//Form 7 defines an unordered group without back pointers
{
	std::string directory;
	char CadenaAux[20];

	directory= "     402";

	//2 Parameter data (synchronized with Add_406_03_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount402_07+1));
	directory+= CadenaAux;

	directory+= "       0       0";

	//5 Level
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;

	directory+= "       0       0       000000000D";

	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     402       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount402_07);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       7";
	//Form 1 defines an unordered group with back pointers
	//Form 7 defines an unordered group without back pointers

	directory+= "                   Group       0D";

	//20 Sequence number
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


long InOutIGES::Add_406_03_Params(std::ofstream &BufferParams, 
								  long numLevel, long &numP, long numD)
//Parameter data for level (406_03)
{
	std::string parameter;
	char CadenaAux[20];

	parameter="406";

	//Number of property values (NP=2)
	parameter+=",2";
	//Function description code (Default = 0)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", digits(numLevel), numLevel);
	parameter+=",";
	parameter+= CadenaAux;

	//Color name (optional).
	parameter+=",8HLayer ";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 2, numLevel);
	parameter+= CadenaAux;
	parameter+=";";

	//Print parameter, in as much lines as required
	//---------------------------------------------
	return PrintMultiLineParameter(BufferParams, parameter, numP, numD);   // numP is increased by the function if multiple lines are required
}


void InOutIGES::Add_406_03_Directory(std::ofstream &out, long LineCount406_03,
									 long colorNumber, long numLevel, long numP, long &numD)
//The Level Function Property specifies the meaning or intended use of a level in the sending system
//to the intensity level of the respective graphics devices as a percent of the full intensity range
{
	std::string directory;
	char CadenaAux[20];

	directory= "     406";

	//2 Parameter data (synchronized with Add_406_03_Params)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, (numP-LineCount406_03+1));
	directory+= CadenaAux;

	directory+= "       0       0";

	//5 Level
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, numLevel);
	directory+= CadenaAux;

	directory+= "       0       0       000000300D";  //Status number->Entity Use Flag->03= Other


	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;

	//Second line
	numD++;
	directory= "     406       0";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, colorNumber);
	directory+= CadenaAux;

	//14 Parameter line count
	//(This is the quantity of lines in the Parameter Data Section 
	//which contain the parameter data record for this entity)
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 8, LineCount406_03);
	directory+= CadenaAux;

	//15 Form number
	directory+= "       3                   Layer       0D";

	//20 Sequence number
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 7, numD);
	directory+= CadenaAux;
	out << directory << std::endl;
}


bool InOutIGES::TrimTgonPatch(const char *file, CDB_Model DDBB_IGES,
							  long colorNumber, long layerNumber,
							  bool type143,
							  long numTgons,
							  std::vector <CVertex> T_gon,
							  std::vector <CVertex> Q_Patch,
							  CVector Normal,
							  long &numP, long &numD,
							  std::ofstream &out,
							  std::ofstream &BufferParams)
//Writes into an IGES file 
//a NURBS surface inscribed into a quadrilateral polygonal patch 
//and delimited by a triangular polygon

//NURBS surface may be type 143 or type 144 (depending on the flag type143)

//Returns TRUE if the process is sucessful
{
	//Number of sides of the polygonal contour
	long N= T_gon.size();
	if(N != 3) return false;

	//Polygon coordinates in the surface parametric space (U,V)
	double lengthU= MODULE_3D(Q_Patch[0], Q_Patch[1]);
	double lengthV= MODULE_3D(Q_Patch[0], Q_Patch[3]);
	std::vector <CVertex> T_par;
	CVertex PP0(0,0,0);
	T_par.push_back(PP0);
	CVertex PP1(lengthU,0,0);
	T_par.push_back(PP1);
	CVertex PP3(0,lengthV,0);
	T_par.push_back(PP3);

	CVector NNormal (0, 0, 1);

	//Define quadrilateral NURBS surface
	//and trim the patch through the polygonal contour
	//------------------------------------------------
	if(type143){
		//NURBS surface type 143
		numD++;
		long pointerUntrimmedSurface= numD;
		long LineCount128_08= Add_128_08_Params(BufferParams, 
												Q_Patch,
												numP, numD);  // numP is increased by the function if multiple lines are required
		Add_128_08_Directory(out, LineCount128_08, 
							 colorNumber, layerNumber, 
							 numTgons, " T_gon",
							 "00010000",   //Status Number: |Visible|Physically Dependent|Geometry|Global top down|
							 //"00000000",   //Status Number: |Visible|Independent|Geometry|Global top down|
							 numP, numD);  // numD is increased by the function if multiple lines are required

		//Convert polygon edges into NURBS curves
		std::vector<long> pointerCurves;
		for(long j=0; j < N; j++){
			numD++;
			pointerCurves.push_back(numD);
			long LineCount126_01= Add_126_01_Params(BufferParams, 
													T_gon[j], T_gon[(j+1)%N], Normal,
													numP, numD);  // numP is increased by the function if multiple lines are required
			Add_126_01_Directory(out, LineCount126_01, 
								 colorNumber, layerNumber, 
								 numTgons, "   T_gon",
								 "00010000",   //Status number: |Visible|Physically Dependent|Geometry|Global Top Down|
								 numP, numD);  // numD is increased by the function if multiple lines are required

			numD++;
			long LineCount126_01_Par= Add_126_01_Params(BufferParams, 
														T_par[j], T_par[(j+1)%N], NNormal,
														numP, numD);  // numP is increased by the function if multiple lines are required
			Add_126_01_Directory(out, LineCount126_01_Par, 
								 colorNumber, layerNumber,
								 numTgons, "   T_gon",
								 "00010500",   //Status number: |Visible|Physically Dependent|2D Parametric|Global Top Down|
								 numP, numD);  // numD is increased by the function if multiple lines are required
		}

		//Chain NURBS curves
		numD++;
		long pointerBoundary= numD;
		long LineCount141= Add_141_Params(BufferParams, 
										  pointerUntrimmedSurface, pointerCurves,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_141_Directory(out, LineCount141, 
						  colorNumber, layerNumber, 
						  numTgons,  " T_gon",
						  numP, numD);  // numD is increased by the function if multiple lines are required

		//Trim NURBS surface
		numD++;
		long LineCount143= Add_143_Params(BufferParams, 
										  pointerUntrimmedSurface, pointerBoundary,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_143_Directory(out, LineCount143, 
						  colorNumber, layerNumber,
						  numTgons,  " T_gon",
						  numP, numD);  // numD is increased by the function if multiple lines are required
	}


	else{
		//NURBS surface type 144
		numD++;
		long LineCount128_00= Add_128_00_Params(BufferParams, 
												Q_Patch,
												numP, numD);  // numP is increased by the function if multiple lines are required
		long pointerUntrimmedSurface= numD;
		Add_128_00_Directory(out, LineCount128_00, 
							 colorNumber, layerNumber, 
							 numTgons, " T_gon",
							 //"01010000",   //Status Number: |Blanked|Physically Dependent|Geometry|Global top down|
							 "00000000",   //Status Number: |Visible|Independent|Geometry|Global top down|
							 numP, numD);  // numD is increased by the function if multiple lines are required

		//Convert polygon edges into NURBS curves
		std::vector<long> pointerCurves126;
		std::vector<long> pointerCurves110;
		for(long j=0; j < N; j++){
			numD++;
			pointerCurves126.push_back(numD);
			long LineCount126_01= Add_126_01_Params(BufferParams, 
													T_par[j], T_par[(j+1)%N], NNormal,
													numP, numD);  // numP is increased by the function if multiple lines are required
			Add_126_01_Directory(out, LineCount126_01, 
								 colorNumber, layerNumber, 
								 numTgons, "   T_gon",
								 "01010500",   //Status number: |Blanked|Physically Dependent|2D Parametric|Global Top Down|
								 numP, numD);  // numD is increased by the function if multiple lines are required

			numD++;
			pointerCurves110.push_back(numD);
			long LineCount110_00_Par= Add_110_00_Params(BufferParams, 
														T_gon[j], T_gon[(j+1)%N],
														numP, numD);  // numP is increased by the function if multiple lines are required
			Add_110_00_Directory(out, LineCount110_00_Par, 
								 colorNumber, layerNumber,
								 numTgons, "   T_gon",
								 "01010000",   //Status number: |Blanked|Physically Dependent|Geometry|Global Top Down|
								 numP, numD);  // numD is increased by the function if multiple lines are required
		}

		//Chain NURBS curves
		numD++;
		long pointerCurves102= numD;
		long LineCount102= Add_102_Params(BufferParams, 
										  pointerCurves126,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_102_Directory(out, LineCount102, 
						  colorNumber, layerNumber, 
						  numTgons,  "   T_gon",
						  "01010500",   //Status number: |Blanked|Physically Dependent|2D Parametric|Global Top Down|
						  numP, numD);  // numD is increased by the function if multiple lines are required

		numD++;
		long pointerCurvesPar102= numD;
		LineCount102= Add_102_Params(BufferParams, 
									 pointerCurves110,
									 numP, numD);  // numP is increased by the function if multiple lines are required
		Add_102_Directory(out, LineCount102, 
						  colorNumber, layerNumber, 
						  numTgons,  "   T_gon",
						  "01010000",   //Status number: |Blanked|Physically Dependent|Geometry|Global Top Down|
						  numP, numD);  // numD is increased by the function if multiple lines are required

		//Chain NURBS curves
		numD++;
		long pointerBoundary= numD;
		long LineCount142= Add_142_Params(BufferParams, 
										  pointerUntrimmedSurface, pointerCurves102, pointerCurvesPar102,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_142_Directory(out, LineCount142, 
						  colorNumber, layerNumber, 
						  numTgons, " T_gon",
						  numP, numD);  // numD is increased by the function if multiple lines are required

		//Trim NURBS surface
		numD++;
		long LineCount144= Add_144_Params(BufferParams, 
										  pointerUntrimmedSurface, pointerBoundary,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_144_Directory(out, LineCount144, 
						  colorNumber, layerNumber, 
						  numTgons, " T_gon",
						  numP, numD);  // numD is increased by the function if multiple lines are required
	}

	return true;
}


bool InOutIGES::TrimTgonPlane(const char *file, CDB_Model DDBB_IGES,
							  long colorNumber, long layerNumber,
							  bool type143,
							  long numTgons,
							  std::vector <CVertex> T_gon,
							  CVector Normal,
							  long &numP, long &numD,
							  std::ofstream &out,
							  std::ofstream &BufferParams)
//Writes into an IGES file 
//a plane face delimited by a triangular polygon (type 108)

//WARNING: This alternative definition of triangular faces as bounded planes
//only works for a single face!!!!
{
	//Number of sides of the polygonal contour
	long N= T_gon.size();
	if(N != 3) return false;

	//Define polygon sides
	std::vector <long> pointerSides110;
	for(long j=0; j < N; j++){
		numD++;
		pointerSides110.push_back(numD);
		long LineCount110_00_Par= Add_110_00_Params(BufferParams, 
													T_gon[j], T_gon[(j+1)%N],
													numP, numD);  // numP is increased by the function if multiple lines are required
		Add_110_00_Directory(out, LineCount110_00_Par, 
							 colorNumber, layerNumber,
							 numTgons, " T_Plane",
							 "01010000",   //Status number: |Blanked|Physically Dependent|Geometry|Global Top Down|
							 numP, numD);  // numD is increased by the function if multiple lines are required
	}

	//Chain polygon sides
	numD++;
	long pointerContour= numD;
	long LineCount102= Add_102_Params(BufferParams, 
									  pointerSides110,
									  numP, numD);  // numP is increased by the function if multiple lines are required
	Add_102_Directory(out, LineCount102, 
					  colorNumber, layerNumber, 
					  numTgons,  " T_Plane",
					  "01010000",   //Status number: |Blanked|Physically Dependent|Geometry|Global Top Down|
					  numP, numD);  // numD is increased by the function if multiple lines are required

	//Define bounded plane
	long LineCount108_01= Add_108_01_Params(BufferParams, 
											T_gon[0], T_gon[1], T_gon[2],
											pointerContour,
											numP, numD);  // numP is increased by the function if multiple lines are required
	Add_108_01_Directory(out, LineCount108_01, 
						 colorNumber, layerNumber, 
						 numTgons, " T_Plane",
						 "00000000",   //Status number: |Blanked|Physically Dependent|Geometry|Global Top Down|
						 numP, numD);  // numD is increased by the function if multiple lines are required

	return true;
}


bool InOutIGES::TrimNgonPatch(const char *file, CDB_Model DDBB_IGES,
							  long colorNumber, long layerNumber,
							  bool type143,
							  long numNgons,
							  std::vector <CVertex> N_gon,
							  std::vector <CVertex> Q_Patch,
							  CVector Normal,
							  long &numP, long &numD,
							  std::ofstream &out,
							  std::ofstream &BufferParams)
//Writes into an IGES file 
//a NURBS surface inscribed into a quadrilateral polygonal patch 
//and delimited by a N-sides polygon

//NURBS surface may be type 143 or type 144 (depending on the flag type143)

//Returns TRUE if the process is sucessful
{
	//Number of sides of the polygonal contour
	long N= N_gon.size();

	//Polygon coordinates in the surface parametric space (U,V)
	double lengthU= MODULE_3D(Q_Patch[0], Q_Patch[1]);
	double lengthV= MODULE_3D(Q_Patch[0], Q_Patch[3]);
	std::vector <CVertex> N_par;
	CVertex PP0(0,0,0);
	N_par.push_back(PP0);
	CVertex PP1(lengthU,0,0);
	N_par.push_back(PP1);
	CVertex PP3(0,lengthV,0);
	N_par.push_back(PP3);

	//Define quadrilateral NURBS surface
	//and trim the patch through the polygonal contour
	//------------------------------------------------
	if(type143){
		//NURBS surface type 143
		numD++;
		long pointerUntrimmedSurface= numD;
		long LineCount128_08= Add_128_08_Params(BufferParams, 
												Q_Patch,
												numP, numD);  // numP is increased by the function if multiple lines are required
		Add_128_08_Directory(out, LineCount128_08, 
							 colorNumber, layerNumber, 
							 numNgons, " T_gon",
							 "00010000",   //Status Number: |Visible|Physically Dependent|Geometry|Global top down|
							 //"00000000",   //Status Number: |Visible|Independent|Geometry|Global top down|
							 numP, numD);  // numD is increased by the function if multiple lines are required

		//Convert polygon edges into NURBS curves
		std::vector<long> pointerCurves;
		for(long j=0; j < N; j++){
			numD++;
			pointerCurves.push_back(numD);
			long LineCount126_01= Add_126_01_Params(BufferParams, 
													//N_par[j], N_par[(j+1)%N], Normal,
													N_gon[j], N_gon[(j+1)%N], Normal,
													numP, numD);  // numP is increased by the function if multiple lines are required
			Add_126_01_Directory(out, LineCount126_01, 
								 colorNumber, layerNumber, 
								 numNgons, "   N_gon",
								 "00010000",   //Status number: |Visible|Physically Dependent|Geometry|Global Top Down|
								 numP, numD);  // numD is increased by the function if multiple lines are required

			numD++;
			CVector NNormal (0, 0, 1);
			long LineCount126_01_Par= Add_126_01_Params(BufferParams, 
														//N_gon[j], N_gon[(j+1)%N], NNormal,
														N_par[j], N_par[(j+1)%N], NNormal,
														numP, numD);  // numP is increased by the function if multiple lines are required
			Add_126_01_Directory(out, LineCount126_01_Par, 
								 colorNumber, layerNumber,
								 numNgons, "   N_gon",
								 "00010500",   //Status number: |Visible|Physically Dependent|2D Parametric|Global Top Down|
								 numP, numD);  // numD is increased by the function if multiple lines are required
		}

		//Chain NURBS curves
		numD++;
		long pointerBoundary= numD;
		long LineCount141= Add_141_Params(BufferParams, 
										  pointerUntrimmedSurface, pointerCurves,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_141_Directory(out, LineCount141, 
						  colorNumber, layerNumber, 
						  numNgons,  " N_gon",
						  numP, numD);  // numD is increased by the function if multiple lines are required

		//Trim NURBS surface
		numD++;
		long LineCount143= Add_143_Params(BufferParams, 
										  pointerUntrimmedSurface, pointerBoundary,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_143_Directory(out, LineCount143, 
						  colorNumber, layerNumber,
						  numNgons,  " N_gon",
						  numP, numD);  // numD is increased by the function if multiple lines are required
	}

	else{
		//NURBS surface type 144
		numD++;
		long LineCount128_00= Add_128_00_Params(BufferParams, 
												Q_Patch,
												numP, numD);  // numP is increased by the function if multiple lines are required
		long pointerUntrimmedSurface= numD;
		Add_128_00_Directory(out, LineCount128_00, 
							 colorNumber, layerNumber, 
							 numNgons, " N_gon",
							 //"01010000",   //Status Number: |Blanked|Physically Dependent|Geometry|Global top down|
							 "00000000",   //Status Number: |Visible|Independent|Geometry|Global top down|
							 numP, numD);  // numD is increased by the function if multiple lines are required

		//Convert polygon edges into NURBS curves
		std::vector<long> pointerCurves126;
		std::vector<long> pointerCurves110;
		for(long j=0; j < N; j++){
			numD++;
			pointerCurves126.push_back(numD);
			long LineCount126_01= Add_126_01_Params(BufferParams, 
													N_par[j], N_par[(j+1)%N], Normal,
													numP, numD);  // numP is increased by the function if multiple lines are required
			Add_126_01_Directory(out, LineCount126_01, 
								 colorNumber, layerNumber, 
								 numNgons, "   N_gon",
								 "01010500",   //Status number: |Blanked|Physically Dependent|2D Parametric|Global Top Down|
								 numP, numD);  // numD is increased by the function if multiple lines are required

			numD++;
			pointerCurves110.push_back(numD);
			long LineCount110_00_Par= Add_110_00_Params(BufferParams, 
														N_gon[j], N_gon[(j+1)%N],
														numP, numD);  // numP is increased by the function if multiple lines are required
			Add_110_00_Directory(out, LineCount110_00_Par, 
								 colorNumber, layerNumber,
								 numNgons, "   N_gon",
								 "01010000",   //Status number: |Blanked|Physically Dependent|Geometry|Global Top Down|
								 numP, numD);  // numD is increased by the function if multiple lines are required
		}

		//Chain NURBS curves
		numD++;
		long pointerCurves= numD;
		long LineCount102= Add_102_Params(BufferParams, 
										  pointerCurves126,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_102_Directory(out, LineCount102, 
						  colorNumber, layerNumber, 
						  numNgons,  "   N_gon",
						  "01010500",   //Status number: |Blanked|Physically Dependent|2D Parametric|Global Top Down|
						  numP, numD);  // numD is increased by the function if multiple lines are required

		numD++;
		long pointerCurvesPar= numD;
		LineCount102= Add_102_Params(BufferParams, 
									 pointerCurves110,
									 numP, numD);  // numP is increased by the function if multiple lines are required
		Add_102_Directory(out, LineCount102, 
						  colorNumber, layerNumber, 
						  numNgons,  "   N_gon",
						  "01010000",   //Status number: |Blanked|Physically Dependent|Geometry|Global Top Down|
						  numP, numD);  // numD is increased by the function if multiple lines are required

		//Chain NURBS curves
		numD++;
		long pointerBoundary= numD;
		long LineCount142= Add_142_Params(BufferParams, 
										  pointerUntrimmedSurface, pointerCurves, pointerCurvesPar,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_142_Directory(out, LineCount142, 
						  colorNumber, layerNumber, 
						  numNgons, " N_gon",
						  numP, numD);  // numD is increased by the function if multiple lines are required

		//Trim NURBS surface
		numD++;
		long LineCount144= Add_144_Params(BufferParams, 
										  pointerUntrimmedSurface, pointerBoundary,
										  numP, numD);  // numP is increased by the function if multiple lines are required
		Add_144_Directory(out, LineCount144, 
						  colorNumber, layerNumber, 
						  numNgons, " N_gon",
						  numP, numD);  // numD is increased by the function if multiple lines are required
	}

	return true;
}


bool InOutIGES::PolygonalFacesToNURBS(const char *file, CDB_Model DDBB_IGES,
									  long colorNumber, long layerNumber,
									  bool type143,
									  long &numP, long &numD,
									  std::ofstream &out,
									  std::ofstream &BufferParams)
//Writes the BRep model of the database 'DDBB' into a IGES file
//Returns TRUE if the process is sucessful
{
	long numTgons= 0;
	long numQgons= 0;
	long numNgons= 0;

	for(long i=0; i < DDBB_IGES.GetSizeFaces(); i++){
		CFace faceI= DDBB_IGES.GetFace(i);
		std::vector <long> verticesI= DDBB_IGES.GetAllVerticesInFace(faceI);
		long N= DDBB_IGES.GetSizeVerticesInFace(i);

		if(N == 3){
			std::vector <CVertex> T_gon;
			for(long j=0; j < N; j++){
				T_gon.push_back(DDBB_IGES.GetVertex(verticesI[j]));
			}

			CVector Normal= faceI.GetNormal(true);

			numTgons++;

			//////////////////////////////////////////////////////////////////////
			//Triangular polygonal faces (T_gons) are repesented as NURBS patches: 
			//(NURBS quadrilateral surfaces trimmed by polyline contours defined by chains of NURBS curves)

			//Get quadrilateral convex hull
			//(Convert the triangular polygon into a quadrilateral patch, 
			//by adding a "rotational symmetric" of P0 relative to P2-P1)
			std::vector <CVertex> Q_Patch;
			CVertex O ((T_gon[2].x+T_gon[1].x)/2, (T_gon[2].y+T_gon[1].y)/2, (T_gon[2].z+T_gon[1].z)/2);
			CVertex T_gon3 (-(T_gon[0].x-O.x)+O.x, -(T_gon[0].y-O.y)+O.y,-(T_gon[0].z-O.z)+O.z);
			//CVertex T_gon3= T_gon[2]);   //Collapsing quadrilaterals into triangles produces singularities!!

			Q_Patch.push_back(T_gon[0]);
			Q_Patch.push_back(T_gon[1]);
			Q_Patch.push_back(T_gon3);
			Q_Patch.push_back(T_gon[2]);

			TrimTgonPatch(file, DDBB_IGES,
						  colorNumber, layerNumber, type143,
						  numTgons, T_gon, Q_Patch, Normal,
						  numP, numD,
						  out, BufferParams);

			//////////////////////////////////////////////////////////////////////
			//Alternative definition of triangular faces as bounded planes
			//TrimTgonPlane(file, DDBB_IGES,
			//			  colorNumber, layerNumber, type143,
			//			  numTgons, T_gon, Normal,
			//			  numP, numD,
			//			  out, BufferParams);
			//////////////////////////////////////////////////////////////////////
		}

		else if(N == 4){
			//Q-gons are represented as ruled surfaces delimited by four straigh segments
			std::vector <CVertex> Q_gon;
			for(long j=0; j < N; j++){
				Q_gon.push_back(DDBB_IGES.GetVertex(verticesI[j]));
			}

			numD++;
			long LineCount128_08= Add_128_08_Params(BufferParams, 
													Q_gon,
													numP, numD);  // numP is increased by the function if multiple lines are required
			numQgons++;
			Add_128_08_Directory(out, LineCount128_08, 
								 colorNumber, layerNumber, 
								 numQgons, " Q_gon",
								 "00000000",   //Status Number: |Visible|Independent|Geometry|Global top down|
								 numP, numD);  // numD is increased by the function if multiple lines are required
		}

		else{
			////n-gons are represented as trimmed quadrilateral NURBS
			std::vector <CVertex> N_gon;
			for(long j=0; j < N; j++){
				N_gon.push_back(DDBB_IGES.GetVertex(verticesI[j]));
			}

			//Get quadrilateral convex hull
			std::vector <CVertex> Q_Patch;
			//Q_Patch.push_back(N_gon[0]);
			//Q_Patch.push_back(N_gon[1]);
			//Q_Patch.push_back(N_gon[2]);
			//Q_Patch.push_back(N_gon[3]);

			numNgons++;
			//TrimNgonPatch(file, DDBB_IGES,
			//			  colorNumber, layerNumber, type143,
			//			  numNgons, N_gon, Q_Patch, Normal,
			//			  numP, numD,
			//			  out, BufferParams);
		}
	}

	return true;
}


bool InOutIGES::PolygonalFacesTo3Mesh(const char *file, CDB_Model DDBB_IGES,
									  long colorNumber, long layerNumber,
									  bool type143,
									  long &numP, long &numD,
									  std::ofstream &out,
									  std::ofstream &BufferParams)
//Writes the tesellated BRep model of the database 'DDBB' into a IGES file
//Returns TRUE if the process is sucessful
{
	long numTgons= 0;
	std::vector<long> pointerFacets;

	for(long i=0; i < DDBB_IGES.GetSizeFaces(); i++){
		CFace faceI= DDBB_IGES.GetFace(i);
		if (DDBB_IGES.GetSizeVerticesInFace(i) < 3) continue;   //Improbable faces with less than three vertices are skipped

		//Recover triangulation
		std::vector<long> Triangles= faceI.GetFaceTriangulation();
		if ((long)Triangles.size() == 0) continue;   //Faces not triangulated are skipped

		//Recover normal vectors
		std::vector<CVector> Normals= faceI.GetFaceTrianglesNormals();

		//Write triangles of the current face
		for(long t=0; t<(long)Triangles.size(); t+=3 ) {
			std::vector <CVertex> T_gon;
			for(long j=0; j < 3; j++){
				T_gon.push_back(DDBB_IGES.GetVertex(Triangles[t+j]));
			}

			numTgons++;
			pointerFacets.push_back(numD+1);

			//////////////////////////////////////////////////////////////////////
			////Triangular polygonal faces (T_gons) are repesented as NURBS patches: 
			////(NURBS quadrilateral surfaces trimmed by polyline contours defined by chains of NURBS curves)

			//Get quadrilateral convex hull
			//(Convert the triangular polygon into a quadrilateral patch, 
			//by adding a "rotational symmetric" of P0 relative to P2-P1)
			std::vector <CVertex> Q_Patch;
			CVertex O ((T_gon[2].x+T_gon[1].x)/2, (T_gon[2].y+T_gon[1].y)/2, (T_gon[2].z+T_gon[1].z)/2);
			CVertex T_gon3 (-(T_gon[0].x-O.x)+O.x, -(T_gon[0].y-O.y)+O.y,-(T_gon[0].z-O.z)+O.z);
			//CVertex T_gon3= T_gon[2]);   //Collapsing quadrilaterals into triangles produces singularities!!

			Q_Patch.push_back(T_gon[0]);
			Q_Patch.push_back(T_gon[1]);
			Q_Patch.push_back(T_gon3);
			Q_Patch.push_back(T_gon[2]);

			TrimTgonPatch(file, DDBB_IGES,
						  colorNumber, layerNumber, type143,
						  numTgons, T_gon, Q_Patch, Normals[t/3],
						  numP, numD,
						  out, BufferParams);

			//////////////////////////////////////////////////////////////////////
			////Alternative definition of triangular faces as bounded planes
			//TrimTgonPlane(file, DDBB_IGES,
			//			  colorNumber, layerNumber, type143,
			//			  numTgons, T_gon, Normals[t/3],
			//			  numP, numD,
			//			  out, BufferParams);
			//////////////////////////////////////////////////////////////////////
		}
	}

	//Group facets
	long LineCount402_07= Add_402_07_Params(BufferParams, 
											pointerFacets,
											numP, numD);  // numP is increased by the function if multiple lines are required
	Add_402_07_Directory(out, LineCount402_07, colorNumber, layerNumber, numP, numD);  // numD is increased by the function if multiple lines are required


	return true;
}


bool InOutIGES::writeIGES( const char *file, CDB_Model *pDDBB)
//Writes the BRep model of the database 'DDBB' into a IGES file
//Returns TRUE if the process is sucessful

//Refer BRep faces are saved as trimmed NURBS surfaces or as planes

//Normal vectors of faces must also be calculated in advance,
//during the "tessellation" stage.
//Normals must be unit vectors pointing outwards from the solid object.
{
	//Check that the required tessellation has been calculated
	if (!ReferApp.GeneralParams.bTessellate3D){
		CString sMensaje;
		sMensaje+="WARNING! IGES file may be incomplete as 3D Tesellate is not active";
		AfxMessageBox(sMensaje);
		
	}
	else{
		if (!ReferApp.T3D.FacesOutward){
			CString sMensaje;
			sMensaje+="WARNING! IGES file may be inconsistent as faces have not be reoriented";
			AfxMessageBox(sMensaje);
		}
		if (!ReferApp.T3D.NormalsToFaces){
			CString sMensaje;
			sMensaje+="WARNING! IGES file may be inconsistent as face normals have not been calculated";
			AfxMessageBox(sMensaje);
		}
	};

	//If main axes are calculated, the model is aligned to them, before outputing its data
	//(otherwise the database aligned with the inflation reference system is selected)
	CDB_Model DDBB_IGES= *pDDBB;
	CCueMainAxes MainAxes;
	MainAxes.TransformToMainAxes(&DDBB_IGES, true);

	//Opens the file "file"
	//---------------------
	std::ofstream out (file);

	if (!out.is_open ()) {
		CString sMensaje;
		sMensaje+="File ";
		sMensaje+=file;
		sMensaje+=" cannot be opened to write";
		AfxMessageBox(sMensaje);
		return false;
	};

	//Create a buffer file to temporarily store the parameter data section of the IGES file
	//-------------------------------------------------------------------------------------
	std::ofstream BufferParams ("buffer.IGES");

	if (!BufferParams.is_open ()) {
		CString sMensaje;
		sMensaje+="File buffer.IGES cannot be opened to write";
		AfxMessageBox(sMensaje);
		return false;
	};

	//Writes the header of a IGES file (Type T,. for start)
	//-----------------------------------------------------
	     //123456789012345678901234567890123456789012345678901234567890123456789012_4567890
	     //         1         2         3         4         5         6         7         8
	if(DDBB_IGES.GetSizeFaces() > 0){
		if(ReferApp.GeneralParams.bIGS_NgonFaces){
			if(ReferApp.GeneralParams.bIGS_Type_143)
				out <<"#REFER 7.0 BRep solid (IGES NURBS faces type 143)                       S      1" << std::endl;
			else
				out <<"#REFER 7.0 BRep solid (IGES NURBS faces type 144)                       S      1" << std::endl;
		}
		else{
			if(ReferApp.GeneralParams.bIGS_Type_143)
				out <<"#REFER 7.0 BRep mesh (IGES triangular faces type 143)                   S      1" << std::endl;
			else
				out <<"#REFER 7.0 BRep mesh (IGES triangular faces type 144)                   S      1" << std::endl;
		}
	}
	else{
		out <<"#REFER 7.0 BRep wireframe (IGES format)                                 S      1" << std::endl;
	}

	//Writes the global information (Type G)
	//--------------------------------------
	long numG= 0;
	globalSection (out, &DDBB_IGES, file, numG);

	//Pre-creates the parameter section (Type P)
	//While writes the directory entry section (Type D)
	//-------------------------------------------------
	long numD= 0;
	long numP= 0;
	long colorNumber= 0;  //Default color
	long layerNumber= 0;  //Default layer

	//Color information
	//-----------------
	numD++;
	colorNumber= -numD;
	long LineCount314= Add_314_Params(BufferParams, numP, numD);  // numP is increased by the function if multiple lines are required
	Add_314_Directory(out, LineCount314, numP, numD);  // numD is increased by the function if multiple lines are required

	//Layer information
	//-----------------
	numD++;
	layerNumber= 1;
	long LineCount406_03= Add_406_03_Params(BufferParams, layerNumber, numP, numD);  // numP is increased by the function if multiple lines are required
	Add_406_03_Directory(out, LineCount406_03, colorNumber, layerNumber, numP, numD);  // numD is increased by the function if multiple lines are required


	//3D model
	//--------
	if(DDBB_IGES.GetSizeFaces() > 0){
		//Represent faces
		//---------------
		if(ReferApp.GeneralParams.bIGS_NgonFaces){
			//Represent polygonal faces as NURBS
			PolygonalFacesToNURBS(file, DDBB_IGES,
								  colorNumber, layerNumber,
								  ReferApp.GeneralParams.bIGS_Type_143,
								  numP, numD,
								  out, BufferParams);
		}
		else{
			//Represent polygonal faces as triangular meshes
			PolygonalFacesTo3Mesh(file, DDBB_IGES,
								  colorNumber, layerNumber,
								  ReferApp.GeneralParams.bIGS_Type_143,
								  numP, numD,
								  out, BufferParams);
		}
		////else{
		////	//Represent polygonal faces as IGES faces (type 510)
		////	//(Uses centre points and normals to define planes of faces contained in database DDBB)
		////	//-------------------------------------------------------------------------------------
		////	for(long i=0; i < DDBB_IGES.GetSizeFaces(); i++) {
		////		//Location (face centroid)
		////		numD++;
		////		long DEloc= numD;
		////		CFace currentFace= DDBB_IGES.GetFace(i);
		////		CVertex VertexC= GetFaceCentroid(&DDBB_IGES, i);
		////		long LineCount116= Add_116_Params(BufferParams, VertexC, numP, numD);  // numP is increased by the function if multiple lines are required
		////		Add_116_Directory(out, LineCount116, 
		////						  i+1,"    Face",
		////						  numP, numD);  // numD is increased by the function if multiple lines are required

		////		//Normal
		////		numD++;
		////		long DEnormal= numD;
		////		CVector Normal= currentFace.GetNormal(true);
		////		long LineCount123= Add_123_Params(BufferParams, Normal, numP, numD);  // numP is increased by the function if multiple lines are required
		////		Add_123_Directory(out, LineCount123, numP, numD);  // numD is increased by the function if multiple lines are required

		////		//Unbounded plane
		////		numD++;
		////		long LineCount190_00= Add_190_00_Params(BufferParams, 
		////												DEloc, DEnormal,
		////												numP, numD);  // numP is increased by the function if multiple lines are required
		////		Add_190_00_Directory(out, LineCount190_00, numP, numD);  // numD is increased by the function if multiple lines are required

		////		//Bounded plane (Type 143 ???)

		////		// TBD

		////		//Loop (Type 508)
		////		//Defines a loop, specifying a bounded face, for B-Rep Geometries. 

		////		// TBD

		////		//Face (Type 510)

		////		// TBD

		////		//Shell (Type 514)

		////		// TBD

		////		//Manifold Solid B-Rep Object (Type 186)

		////		// TBD
		////	};
		////}
	}
	else{
		//Output wireframe
		//----------------
		CString sMensaje;
		sMensaje+="IGES file will contain a wireframe, as faces have not been calculated";
		AfxMessageBox(sMensaje);

		//Writes as points all the vertices contained in database DDBB
		for(long i=0; i < DDBB_IGES.GetSizeVertices(); i++) {
			numD++;

			//Params information
			CVertex VertexI= DDBB_IGES.GetVertex(i);
			long LineCount116= Add_116_Params(BufferParams, VertexI, numP, numD);  // numP is increased by the function if multiple lines are required

			//Entry information
			Add_116_Directory(out, LineCount116, 
							  i+1, "   Point",
							  numP, numD);  // numD is increased by the function if multiple lines are required
		};

		//Writes as segments all the edges contained in database DDBB
		for(long i=0; i < DDBB_IGES.GetSizeEdges(); i++) {
			numD++;

			//Params information
			CEdge EdgeI= DDBB_IGES.GetEdge(i);
			CVertex VertexEdgeITail= DDBB_IGES.GetVertex(EdgeI.Tail);
			CVertex VertexEdgeIHead= DDBB_IGES.GetVertex(EdgeI.Head);
			long LineCount110_00= Add_110_00_Params(BufferParams, 
													VertexEdgeITail, VertexEdgeIHead,
													numP, numD);  // numP is increased by the function if multiple lines are required

			//Entry information
			Add_110_00_Directory(out, LineCount110_00,
								 colorNumber, layerNumber,
								 i+1, "    Line",
								 "00000000",    //Status Number: |Visible|Independent|Geometry|Global top down|
								 numP, numD);  // numD is increased by the function if multiple lines are required
		}
	}

	BufferParams.close();

	//Writes the parameter data section (Type P)
	//(This information was pre-stored in a buffer)
	//---------------------------------------------
	std::ifstream buffer ("buffer.IGES");
	if (!buffer.is_open ()){
		AfxMessageBox("Cannot open the file buffer.IGES to read");
		return false;
	};

	//Copy the buffer file to the out file
	std::string linea;
	while (getline(buffer, linea)){
		out << linea << std::endl;
	}

	buffer.close();             // Close the buffer file
	std::remove("buffer.IGES"); // Delete the buffer file

	//Writes the termination (Type T)
	//-------------------------------
	char CadenaAux[20];

	out << "S      1",
	out << "G ";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 6, numG);
	out << CadenaAux;
	out << "D ";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 6, numD);
	out << CadenaAux;
	out << "P ";
	sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 6, numP);
	out << CadenaAux;
	out << "                                        T      1"<< std::endl;

	//Closes the file
	//---------------
	out.close ();

	return true;
};