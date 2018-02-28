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
//Simple1GenBase.h

#ifndef SIMPLE1GENBASE_H
#define SIMPLE1GENBASE_H

#include <string>
#include <vector>

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
//! @ingroup PYMat
//
//! @brief ??.
class Simple1GenBase: public EntCmd
  {
  protected:	
    // Variables used for reading input files:
    int NumNodes, NumPileEle, NumLayer, NumLoad, NumSp, NumMat;
    double b, depth, stress, ru;
    int *NodeNum;  // Arrays for Nodes File
    double *Nodey, *Nodex;
    int *PileEleNum, *PileNode1, *PileNode2;		// Arrays for Pile Elements File
    double *gamma_t, *gamma_b, *z_t, *z_b, *c_t, *c_b, // Arrays for Soil Properties File
    	*zLoad_t, *zLoad_b, *load_val_t, *load_val_b, *zSp_t, *zSp_b, *sp_val_t,
    	*sp_val_b, *zMp_t, *zMp_b, *mp_val_t, *mp_val_b, *ru_t, *ru_b;
    std::vector<std::string> MatType;
    std::string PatternInfo;

    void free_mem(void);
    // Member functions for reading input files:
    void GetNodes(const std::string &file);
    void GetPileElements(const std::string &file);
    int NumRows(const std::string &file, const std::string &begin);
    double linterp(double x1, double x2, double y1, double y2, double x3);


    // Member functions for generating output:

  public:
    // Public member functions accessed from TclModelBuilder.cpp
    Simple1GenBase(void);
    ~Simple1GenBase(void);
  };
} // end of XC namespace

#endif
