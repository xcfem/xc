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
//FedeasConcrMaterial.cpp

#include <cstdlib>
#include "FedeasConcrMaterial.h"
#include "utility/matrix/Vector.h"

XC::FedeasConcrMaterial::FedeasConcrMaterial(int tag, int classTag, int nhv, int ndata, double fc, double ec, double fu, double eu)
  : XC::FedeasMaterial(tag, classTag, nhv, ndata)
  {
    matParams[0]  = fc;
    matParams[1]  = ec;
    matParams[2]  = fu;
    matParams[3]  = eu;

    trial.Tangent()= 2.0*matParams[0]/matParams[1];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasConcrMaterial::FedeasConcrMaterial(int tag, int classTag, int nhv, int ndata, const Vector &d)
  : XC::FedeasMaterial(tag, classTag, nhv, ndata)
  {
    if(d.Size() != numData)
      {
        std::cerr << "XC::FedeasConcrMaterial::FedeasConcrMaterial -- not enough input arguments\n";
        exit(-1);
      }

    for(int i = 0; i < numData; i++)
      matParams[i] = d(i);

    trial.Tangent()= 2.0*matParams[0]/matParams[1];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasConcrMaterial::FedeasConcrMaterial(int tag, int classTag, int nhv, int ndata)
  : XC::FedeasMaterial(tag, classTag, nhv, ndata) {}


double XC::FedeasConcrMaterial::getInitialTangent(void) const
  {
    //return 2.0*fc/ec;
    return 2.0*matParams[0]/matParams[1];
  }
