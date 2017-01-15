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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** See file 'COPYRIGHT'  in main directory for information on usage   **
** and redistribution of OpenSees, and for a DISCLAIMER OF ALL        **
** WARRANTIES.                                                        **
**                                                                    **
** Element2dGNL.cpp: implementation of the Element2dGNL class         **
** Developed by:                                                      **
**    Rohit Kaul       (rkaul@stanford.edu)                           **
**    Greg Deierlein   (ggd@stanford.edu)                             **
**                                                                    **
**           John A. Blume Earthquake Engineering Center              **
**                    Stanford University                             **
** ****************************************************************** **/

#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <cmath>
#include <cstdlib>

#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Elastic2DGNL.h"

#define Ele_TAG_Elastic2dGNL -1
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// Constructor
XC::Elastic2dGNL::Elastic2dGNL(int tag, double a, double e, double i, int Nd1, int Nd2, 
			   bool islinear, double rho)
  :UpdatedLagrangianBeam2D(tag, Ele_TAG_Elastic2dGNL, Nd1, Nd2, islinear),
   A(a), E(e), Iz(i)
{
  massDof = A*L*rho;
  massDof = massDof/2;

}

//! @brief Constructor virtual.
XC::Element* XC::Elastic2dGNL::getCopy(void) const
  { return new XC::Elastic2dGNL(*this); }

void XC::Elastic2dGNL::getLocalMass(Matrix &M) const
  {
    if(massDof < 0)
      {
        std::cerr << "XC::Elastic2dGNL::getMass - Distributed mass not implemented\n";
	M.Zero();
      }
    else if(massDof == 0)//this cond. is taken care of already
      { M.Zero(); }
    else
      {
        M.Zero();
	M(0,0) = M(1,1) = M(2,2) = M(3,3) = M(4,4) = M(5,5) = massDof;
      }
  }

void XC::Elastic2dGNL::getLocalStiff(Matrix &K) const
  {
    double EIbyL = E*Iz/L_hist;
    double l = L_hist;

    K(0, 1) = K(0, 2) = K(0, 4) = K(0, 5)=0;
    K(1, 0) = K(1, 3) =0;
    K(2, 0) = K(2, 3) =0;
    K(3, 1) = K(3, 2) = K(3, 4) = K(3, 5)=0;
    K(4, 0) = K(4, 3) =0;
    K(5, 0) = K(5, 3) =0;

	K(0,0) = K(3,3) = (A/Iz)*(EIbyL);
	K(0,3) = K(3,0) = (-A/Iz)*(EIbyL);
	K(1,1) = K(4,4) = (12/(l*l))*(EIbyL);
	K(1,4) = K(4,1) = (-12/(l*l))*(EIbyL);
	K(1,2) = K(2,1) = K(1,5) = K(5,1) = (6/l)*(EIbyL);
	K(2,4) = K(4,2) = K(4,5) = K(5,4) = (-6/l)*(EIbyL);
	K(2,2) = K(5,5) = 4*(EIbyL);
	K(2,5) = K(5,2) = 2*(EIbyL);
	

  }//getLocalStiff



void XC::Elastic2dGNL::Print(std::ostream &s, int flag)
{
    s << "\nElement No: " << this->getTag();
    s << " type: Elastic2dGNL  iNode: " << theNodes.getTagNode(0);
    s << " jNode: " << theNodes.getTagNode(1);
	if(isLinear) s << "(1st-Order):\n";
	else		 s << "(2nd-Order):\n";
}

int XC::Elastic2dGNL::sendSelf(CommParameters &cp)
{
	std::cerr << "WARNING (W_C_10) - XC::Elastic2dGNL::sendSelf(..) [" << getTag() <<"]\n";
	std::cerr << "method not implemented\n";
	return -1;
}

int XC::Elastic2dGNL::recvSelf(const CommParameters &cp)
{
        std::cerr << "WARNING (W_C_20) - XC::Elastic2dGNL::recvSelf(..) [" << getTag() <<"]\n";
        std::cerr << "method not implemented\n";
	return -1;
}

