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
//FedeasBondMaterial.cpp

#include <cstdlib>
#include "FedeasBondMaterial.h"
#include "utility/matrix/Vector.h"

XC::FedeasBondMaterial::FedeasBondMaterial(int tag, int classTag, int nhv, int ndata,
	double u1p, double q1p, double u2p, double u3p, double q3p,
	double u1n, double q1n, double u2n, double u3n, double q3n,
	double s0, double bb):
 XC::FedeasMaterial(tag, classTag, nhv, ndata)
  {
    // Fill in material parameters
    matParams[0]= u1p;
    matParams[1]= q1p;
    matParams[2]= u2p;
    matParams[3]= u3p;
    matParams[4]= q3p;
    matParams[5]= u1n;
    matParams[6]= q1n;
    matParams[7]= u2n;
    matParams[8]= u3n;
    matParams[9]= q3n;
    matParams[10]= s0;
    matParams[11]= bb;
  
    trial.Tangent()= q1p/u1p;
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasBondMaterial::FedeasBondMaterial(int tag, int classTag, int nhv, int ndata, const Vector &d):
 XC::FedeasMaterial(tag, classTag, nhv, ndata)
  {
    if(d.Size() != numData)
      {
        std::cerr << "FedeasBondMaterial::FedeasBondMaterial -- not enough input arguments\n";
        exit(-1);
      }
		
    for(int i= 0; i < numData; i++)
      matParams[i]= d(i);

    trial.Tangent()=  matParams[1]/matParams[0];
    converged.Tangent()= trial.getTangent();
  }

//! @brief Constructor.
XC::FedeasBondMaterial::FedeasBondMaterial(int tag, int classTag, int nhv, int ndata)
  : XC::FedeasMaterial(tag, classTag, nhv, ndata) {}

//! @brief Returns Young's modulus initial value.
double XC::FedeasBondMaterial::getInitialTangent(void) const
  {
    //return q1p/u1p;
    return matParams[1]/matParams[0];
  }
