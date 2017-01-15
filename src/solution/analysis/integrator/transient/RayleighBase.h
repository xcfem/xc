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
//RayleighBase.h

#ifndef RayleighBase_h
#define RayleighBase_h

#include "DampingFactorsIntegrator.h"

namespace XC {

//! @ingroup TransientIntegrator
//
//! @defgroup RayleighIntegrator Integración del sistema de ecuaciones dinámico mediante el método de Rayleigh.
//
//! @ingroup RayleighIntegrator
//
//! @brief ??.
class RayleighBase : public DampingFactorsIntegrator
  {
  protected:
    double deltaT;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    RayleighBase(SoluMethod *,int classTag);
    RayleighBase(SoluMethod *,int classTag,const RayleighDampingFactors &);
  };
} // end of XC namespace

#endif
