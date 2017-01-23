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
#ifndef QuadraticCyclic_H
#define QuadraticCyclic_H

#include "CyclicModel.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

namespace XC {
//! \ingroup OneDimensionalElem
//
//! @brief ??.
class QuadraticCyclic : public CyclicModel
  {
  private:
    double a, b, c;
    double weightFactor, facty;
    double qx1, qy1, qx2, qy2, qx3, qy3;
    static Matrix X;
    static Vector Y, A;
  public:
    QuadraticCyclic(int tag, double wt=0.9, double qy=0.33);

    void Print (std::ostream &, int = 0);
    CyclicModel *getCopy();
  protected:
    int createFullCycleTask();
    int createHalfCycleTask();
    double getTaskFactor();
  private:
    int solveQuad(double x1, double y1, double x2,double y2, double x3, double y);
    double getQuadFactor(double x1, double y1, double dx);
    int createTask(void);
  };
} // end of XC namespace

#endif

