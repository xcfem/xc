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
#ifndef Inelastic2DYS02_H
#define Inelastic2DYS02_H

#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/InelasticYS2DGNL.h"

namespace XC {
class CyclicModel;

//! @ingroup OneDimensionalElem
//
//! @brief ??.
class Inelastic2DYS02: public InelasticYS2DGNL
 {
 private:
   // int cycType;
   // double wT;
   double A;
   double E;
   double Iz;
   double resFactor;
   double alfa, beta;
   double delPmax;
   double delPMaxPos, delPMaxNeg;
   CyclicModel *cModel;
 public:
   Inelastic2DYS02(int tag, double A, double E, double Iz,
 		  int Nd1, int Nd2,
 		  YieldSurface_BC *ysEnd1,  YieldSurface_BC *ysEnd2,
 		  // int cyc_type, double wt,
 		  CyclicModel *cycModel,
 		  double del_p_max,
 		  double Alpha, double Beta, int rf_algo=-1,
 		  bool islinear=false, double rho=0.0);
   Element *getCopy(void) const;
   ~Inelastic2DYS02(void);
   int commitState(void);
   int update(void);

 protected:
   void getLocalStiff(Matrix &K) const;
 };
} // end of XC namespace

#endif

