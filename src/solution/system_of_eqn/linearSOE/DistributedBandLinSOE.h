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
//DistributedBandLinSOE.h
                                                                        
#ifndef DistributedBandLinSOE_h
#define DistributedBandLinSOE_h


#include "solution/system_of_eqn/linearSOE/DistributedLinSOE.h"
#include "utility/matrix/Vector.h"

namespace XC {

class Graph;
class Communicator;
class LinearSOEData;
class BandGenLinSOE;
class FactoredSOEBase;
class ID;
class Vector;

//! @ingroup SOE
//
//! @brief Base class for band matrix distributed systems of equations.
class DistributedBandLinSOE: public DistributedLinSOE
  {
  protected:
    mutable std::vector<double> workArea;
    std::vector<double> myB;
    Vector myVectB;

    void setupMyB(const std::size_t &,const std::size_t &);
    int addB(const int &size,const Vector &, const ID &,const double &);
    int setB(const int &size,const Vector &, const double &);
    void zeroB(void);
    int sendBArecvBX(LinearSOEData &,const bool &,double *,const int &,ID &);
    int recvBAsendBX(FactoredSOEBase &) const;
    int sendResultsBack(FactoredSOEBase &,ID &);
    int recvXA(FactoredSOEBase &,const bool &,double *A,const int &,ID &);

    DistributedBandLinSOE(void);
  };
} // end of XC namespace


#endif

