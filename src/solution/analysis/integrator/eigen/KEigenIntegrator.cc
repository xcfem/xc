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


#include "KEigenIntegrator.h"
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>

//! @brief Constructor.
XC::KEigenIntegrator::KEigenIntegrator(AnalysisAggregation *owr)
  : EigenIntegrator(owr) {}

//! @brief Identity matrix assembly.
int XC::KEigenIntegrator::formM(void)
  {
    int retval= 0;
    AnalysisModel *mdl= getAnalysisModelPtr();
    EigenSOE *theSOE= getEigenSOEPtr();
    if(!mdl || !theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no AnalysisModel or EigenSOE has been set\n";
	retval= -1;
      }
    else
      {
	flagK = 1; //mass matrix (see formEleTangent)
        theSOE->identityM();
      }
    return retval;
  }

//! @brief Asks the element being passed as parameter
//! to build its mass matrix. Doing nothing here because M must
//! be the identity matrix.
int XC::KEigenIntegrator::formEleTangM(FE_Element *)
  { return 0; }

//! @brief Asks the DOF group being passed as parameter
//! to build its mass matrix. Doing nothing here because M must
//! be the identity matrix.
int XC::KEigenIntegrator::formNodTangM(DOF_Group *)
  { return 0; }



