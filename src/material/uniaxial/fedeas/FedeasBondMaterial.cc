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
    data[0]= u1p;
    data[1]= q1p;
    data[2]= u2p;
    data[3]= u3p;
    data[4]= q3p;
    data[5]= u1n;
    data[6]= q1n;
    data[7]= u2n;
    data[8]= u3n;
    data[9]= q3n;
    data[10]= s0;
    data[11]= bb;
  
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
      data[i]= d(i);

    trial.Tangent()=  data[1]/data[0];
    converged.Tangent()= trial.getTangent();
  }

//! @brief Constructor.
XC::FedeasBondMaterial::FedeasBondMaterial(int tag, int classTag, int nhv, int ndata)
  : XC::FedeasMaterial(tag, classTag, nhv, ndata) {}

//! @brief Devuelve el valor noval del módulo de deformación.
double XC::FedeasBondMaterial::getInitialTangent(void) const
  {
    //return q1p/u1p;
    return data[1]/data[0];
  }
