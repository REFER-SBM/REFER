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

#include <cmath>   // fabs
#include <vector>
#include <algorithm>  //sort

#include "Tools_Vector.h"

// VECTOR MANAGEMENT

bool IsValueInVector(long Value, std::vector<long> V, long &Position)
// Searches for the value "Value" in the vector V.
// Returns "true" if the value is found (together with its position "Position")
// Returns "false" otherwise
{
	bool found= false;
	Position= -1;   //Not valid value

	for(long i=0; i<(long)V.size(); i++){
		if (V[i] == Value){
			found= true;
			Position= i;
			break;
		}
	}
	return found;
}


bool IsValueInVector(long Value, std::vector<long> V)
//Returns true if "Value" is in vector V
{
	for(long i=0; i<(long)V.size(); i++){
		if (V[i] == Value) return true;
	}
	return false;
}


bool CommonVerticesInVectors(std::vector<long> V1, std::vector<long> V2)
//Returns true any "Value" in V1, is also in vector V2
{
	for(long i=0; i<(long)V1.size(); i++){
		if (IsValueInVector(V1[i], V2)) return true;
	}
	return false;
}


//bool IsAbsValueInVector(long Value, std::vector<long> V)
////Returns true if absolute value of "Value" is in vector V
//{
//	for(long i=0; i<(long)V.size(); i++){
//		if (std::abs(V[i]) == Value) return true;
//	}
//	return false;
//}


bool CompareLongSortMinorFirst(long A, long B)
// AUXILIAR for "sort"
//Returns true if long A goes before than long B
{return (A < B);}


std::vector<long> SortMinorFirst(std::vector<long> V)
//Returns long vector V sorted from minor to major
{
	sort(V.begin(), V.end(), CompareLongSortMinorFirst);
	return V;
}


bool CompareDoubleSortMinorFirst(double A, double B)
// AUXILIAR for "sort"
//Returns true if double A goes before than double B
{return (A < B);}


std::vector<double> SortMinorFirst(std::vector<double> V)
//Returns double vector V sorted from minor to major
{
	sort(V.begin(), V.end(), CompareDoubleSortMinorFirst);
	return V;
}


bool CompareLongSortMajorFirst(long A, long B)
// AUXILIAR for "sort"
//Returns true if long A goes behind than long B
{return (A > B);}


std::vector<long> SortMajorFirst(std::vector<long> V)
//Returns long vector V sorted from major to minor
{
	sort(V.begin(), V.end(), CompareLongSortMajorFirst);
	return V;
}


bool CompareDoubleSortMajorFirst(double A, double B)
// AUXILIAR for "sort"
//Returns true if double A goes behind than double B
{return (A > B);}


std::vector<double> SortMajorFirst(std::vector<double> V)
//Returns double vector V sorted from major to minor
{
	sort(V.begin(), V.end(), CompareDoubleSortMajorFirst);
	return V;
}


void SortMinorFirst(std::vector<long> &V1, std::vector<long> &V2)
//Bubble sort both vectors V1 and V2

//Vector V1 is ordered placing minor value first
//Vector V2 is ordered in the same way than V1
{
	if(V1.size() != V2.size())
		//Vectors are of different length
		return;

	bool swapped;
	do{
		swapped= false;
		for (long i= 0; i<(long)V1.size()-1; i++){
			if (V1[i] > V1[i+1]){
				std::swap(V1[i], V1[i+1]);
				std::swap(V2[i], V2[i+1]);
				swapped= true;
			}
		}
	}while(swapped);
}


void SortMinorFirst(std::vector<long> &V1, std::vector<std::vector<bool>> &V2)
//Bubble sort both vectors V1 and V2

//Vector V1 is ordered placing minor value first
//Vector V2 is ordered in the same way than V1
{
	if(V1.size() != V2.size())
		//Vectors are of different length
		return;

	bool swapped;
	do{
		swapped= false;
		for (long i= 0; i<(long)V1.size()-1; i++){
			if (V1[i] > V1[i+1]){
				std::swap(V1[i], V1[i+1]);
				std::swap(V2[i], V2[i+1]);
				swapped= true;
			}
		}
	}while(swapped);
}


void SortMinorFirst(std::vector<double> &V1,
					std::vector<long> &V2)
//Bubble sort both vectors V1 and V2

//Vector V1 is ordered placing minor value first
//Vector V2 is ordered in the same way than V1
{
	if(V1.size() != V2.size())
		//Vectors are of different length
		return;

	bool swapped;
	do{
		swapped= false;
		for (long i= 0; i<(long)V1.size()-1; i++){
			if (V1[i] > V1[i+1]){
				std::swap(V1[i], V1[i+1]);
				std::swap(V2[i], V2[i+1]);
				swapped= true;
			}
		}
	}while(swapped);
}


void SortMinorFirst(std::vector<double> &V1,
					std::vector<double> &V2,
					std::vector<long> &V3)
//Bubble sort all vectors V1, V2 and V3

//Vector V1 is ordered placing minor value first
//Vectors V2 and V3 are ordered in the same way than V1
{
	if(V1.size() != V2.size() || V1.size() != V3.size())
		//Vectors are of different length
		return;

	bool swapped;
	do{
		swapped= false;
		for (long i= 0; i<(long)V1.size()-1; i++){
			if (V1[i] > V1[i+1]){
				std::swap(V1[i], V1[i+1]);
				std::swap(V2[i], V2[i+1]);
				std::swap(V3[i], V3[i+1]);
				swapped= true;
			}
		}
	}while(swapped);
}


void SortMajorFirst(std::vector<long> &V1, std::vector<long> &V2)
//Bubble sort both vectors V1 and V2

//Vector V1 is ordered placing major value first
//Vector V2 is ordered in the same way than V1
{
	if(V1.size() != V2.size())
		//Vectors are of different length
		return;

	bool swapped;
	do{
		swapped= false;
		for (long i= 0; i<(long)V1.size()-1; i++){
			if (V1[i] < V1[i+1]){
				std::swap(V1[i], V1[i+1]);
				std::swap(V2[i], V2[i+1]);
				swapped= true;
			}
		}
	}while(swapped);
}



void SortMajorFirst(std::vector<double> &V1,
					std::vector<long> &V2)
//Bubble sort both vectors V1 and V2

//Vector V1 is ordered placing major value first
//Vector V2 is ordered in the same way than V1
{
	if(V1.size() != V2.size())
		//Vectors are of different length
		return;

	bool swapped;
	do{
		swapped= false;
		for (long i= 0; i<(long)V1.size()-1; i++){
			if (V1[i] < V1[i+1]){
				std::swap(V1[i], V1[i+1]);
				std::swap(V2[i], V2[i+1]);
				swapped= true;
			}
		}
	}while(swapped);
}


void addNewNumber(long n, std::vector<long> &v)
//Add the number n into the vector v,
//iff number n is not yet included in the vector

//The numbers are keeped ordered from smaller to larger
{
	for(long i= 0; i<(long)v.size(); i++){
		if(n == v[i])
			return;  //Does not add repeated numbers
		else if(n < v[i]){
			v.insert(v.begin()+i, n);
			return;
		}
	}
	v.push_back(n);
}
