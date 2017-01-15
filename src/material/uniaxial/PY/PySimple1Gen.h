//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//$Revision: 1.4 $
//$Date: 2004/06/30 00:27:40 $
//$Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/PY/PySimple1Gen.h,v $

#include <string>
#include <vector>

#include "Simple1GenBase.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief ??.
class PySimple1Gen: public Simple1GenBase
  {	
    std::vector<std::string> pTest;
    // Variables used for reading input files:
    int NumPyEle, NumMpLoadSp, NumMp;
    double pult, y50, maxz, minz, cu, e50, phi, sr, PULT, Y50;
    int *PyEleNum, *PyNode1, *PyNode2, *PyMat, *PyDir;	// Arrays for Py Elements File
    int *pyType, stype;
    double *gamma_t, *b_t, *b_b, *Cd_t, *Cd_b, // Arrays for Soil Properties File
    	*cu_t, *cu_b, *e50_t, *e50_b, *phi_t, *phi_b, *Sr_t, *Sr_b, *pult_t, *pult_b,
    	*y50_t, *y50_b, tribcoord[2];
    std::vector<std::string> MatType;
    std::string PatternInfo;

    // Member functions for reading input files:
    void GetPyElements(const std::string &file);
    void GetSoilProperties(const std::string &file);


    // Member functions for generating output:
    void GetPySimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5);
    void GetPattern(const std::string &file6);

    void libera(void);
    // Member functions for calculating pult:
    double GetPult(const std::string &type);
    double GetY50(const std::string &type);
    double GetVStress(double z);
    double GetMp(double *vx, double *vy, double x, int length);
    void GetTributaryCoordsPy(int nodenum1);
    void GetTributaryCoordsPile(int nodenum1);

public:
    // Public member functions accessed from TclModelBuilder.cpp
    void WritePySimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5);
    void WritePySimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5, const std::string &file6);
    PySimple1Gen(void);
    ~PySimple1Gen(void);
  };
} // end of XC namespace
