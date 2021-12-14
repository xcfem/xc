// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//$Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/PY/generators/TzSimple1Gen.h,v $

#ifndef TZSIMPLE1GEN_H
#define TZSIMPLE1GEN_H

#include "Simple1GenBase.h"

namespace XC {
//! @ingroup PYMat
//
//! @brief Constructs TzSimple1 materials (Boulanger, 2003) for
//! pre-defined zeroLength elements.
class TzSimple1Gen: public Simple1GenBase
  {
  // Variables used for reading input files:
  int NumTzEle, NPile, NumMt, NumMtLoadSp;
  double p, zground, TULT, Z50, ca, delta, Sa;
  int *TzEleNum, *TzNode1, *TzNode2, *TzMat, *TzDir;	// Arrays for Py Elements File
  int *tzType;
  double *p_t, *p_b, *ca_t, *ca_b, *delta_t, *delta_b,
    *zMt_t, *zMt_b, *mt_val_t, *mt_val_b, tribcoord[2], *Sa_b, *Sa_t,
    *tult_t, *tult_b, *z50_t, *z50_b;  
  
   // Member functions for reading input files:
   void GetTzElements(const std::string &file);
   void GetSoilProperties(const std::string &file);
   double GetTult(const std::string &type);
   double GetZ50(const std::string &type);
   double GetMt(double *vx, double *vy, double x, int length);
   void GetTributaryCoordsTz(int nodenum1);
   void GetTributaryCoordsPile(int nodenum1);
  
    void free_mem(void);
  // Member functions for generating output:
  void GetTzSimple1(const std::string &file1,const std::string &file2,const std::string &file3,const std::string &file4, const std::string &file5);
  void GetPattern(const std::string &file6);
  
  // Member functions for calculating tult:
  double GetVStress(double z);
  
 public:
  
  void WriteTzSimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5);
  void WriteTzSimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5, const std::string &file6);
  
  TzSimple1Gen();
  ~TzSimple1Gen();
};
} // end of XC namespace

#endif
