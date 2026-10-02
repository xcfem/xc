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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.1 $
// $Date: 2005/12/19 22:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHTBaseAlphaF.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the implementation of the XC::HHTBaseAlphaF class.
//
// What: "@(#) HHTBaseAlphaF.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/hht/HHTBaseAlphaF.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>

//! @brief Constructor.
XC::HHTBaseAlphaF::HHTBaseAlphaF(SolutionStrategy *owr, int classTag)
  : HHTBase(owr,classTag, 1.0),
    alphaF(1.0) {}

//! @brief Constructor.
XC::HHTBaseAlphaF::HHTBaseAlphaF(SolutionStrategy *owr, int classTag,
				 double _alphaI, double _alphaF)
    : HHTBase(owr, classTag, _alphaI),
      alphaF(_alphaF) {}

//! @brief Constructor.
XC::HHTBaseAlphaF::HHTBaseAlphaF(SolutionStrategy *owr, int classTag,
				 double _alphaI, double _alphaF,
				 const RayleighDampingFactors &rF)
    : HHTBase(owr, classTag,_alphaI,rF),
      alphaF(_alphaF) {}

//! @brief Constructor.
XC::HHTBaseAlphaF::HHTBaseAlphaF(SolutionStrategy *owr, int classTag,
				 double _alphaI, double _alphaF,
				 double beta, double gamma)
  : HHTBase(owr, classTag,_alphaI, beta, gamma),
    alphaF(_alphaF) {}

//! @brief Constructor.
XC::HHTBaseAlphaF::HHTBaseAlphaF(SolutionStrategy *owr, int classTag,
				 double _alphaI, double _alphaF,
				 double beta, double gamma,
				 const RayleighDampingFactors &rF)
  : HHTBase(owr, classTag,_alphaI, beta, gamma, rF),
    alphaF(_alphaF) {}


//! @brief Send object members through the communicator argument.
int XC::HHTBaseAlphaF::sendData(Communicator &comm)
  {
    int res= HHTBase::sendData(comm);
    res+= comm.sendDouble(alphaF,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::HHTBaseAlphaF::recvData(const Communicator &comm)
  {
    int res= HHTBase::recvData(comm);
    res+= comm.receiveDouble(alphaF,getDbTagData(),CommMetaData(9));
    return res;
  }

void XC::HHTBaseAlphaF::Print(std::ostream &s, int flag) const
  {
    HHTBase::Print(s,flag);
    s << "  alphaI: " << alphaI()
      << " alphaF: " << alphaF
      << " beta: " << beta
      << " gamma: " << gamma
      << std::endl;
  }

