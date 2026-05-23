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

#ifndef TOOLS_VECTOR_VISITED
	#define TOOLS_VECTOR_VISITED

	// VECTOR MANAGEMENT
	bool IsValueInVector(long Value, std::vector<long> V, long &Position);
	bool IsValueInVector(long Value, std::vector<long> V);
	bool CommonVerticesInVectors(std::vector<long> V1, std::vector<long> V2);
	//bool IsAbsValueInVector(long Value, std::vector<long> V);

	std::vector<long> SortMinorFirst(std::vector<long> V);
	std::vector<double> SortMinorFirst(std::vector<double> V);

	std::vector<long> SortMajorFirst(std::vector<long> V);
	std::vector<double> SortMajorFirst(std::vector<double> V);

	void SortMinorFirst(std::vector<long> &V1, std::vector<long> &V2);
	void SortMinorFirst(std::vector<long> &V1, std::vector<std::vector<bool>> &V2);
	void SortMinorFirst(std::vector<double> &V1, std::vector<long> &V2);
	void SortMinorFirst(std::vector<double> &V1,
						std::vector<double> &V2,
						std::vector<long> &V3);

	void SortMajorFirst(std::vector<long> &V1, std::vector<long> &V2);
	void SortMajorFirst(std::vector<double> &V1, std::vector<long> &V2);

	void addNewNumber(long n, std::vector<long> &v);

#endif  //TOOLS_VECTOR_VISITED
