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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite XC::Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              July 2004
//# UPDATE HISTORY:
//#
//===============================================================================
#ifndef FDEPState_CPP
#define FDEPState_CPP

#include "material/nD/FiniteDeformation/FDEPState.h"
#include <utility/matrix/nDarray/Tensor.h>

//-------------------------------------------------------------------------------
// Normal Constructor 00
//-------------------------------------------------------------------------------
XC::FDEPState::FDEPState ()
{
	int err;
	err = this->revertToStart();
}

//-------------------------------------------------------------------------------
// Normal Constructor 01
//-------------------------------------------------------------------------------
XC::FDEPState::FDEPState ( const XC::straintensor& xFpInVar,
	               double xStressLikeInVar,
	               double xStrainLikeInVar,
		       const XC::stresstensor& xStressLikeKiVar,
		       const XC::straintensor& xStrainLikeKiVar,
		       //
		       const XC::straintensor& xCommittedFpInVar,
	               double xCommittedStressLikeInVar,
	               double xCommittedStrainLikeInVar,
		       const XC::stresstensor& xCommittedStressLikeKiVar,
		       const XC::straintensor& xCommittedStrainLikeKiVar )
{
	FpInVar = xFpInVar;
	StressLikeInVar = xStressLikeInVar;
	StrainLikeInVar = xStrainLikeInVar;
	StressLikeKiVar = xStressLikeKiVar;
	StrainLikeKiVar = xStrainLikeKiVar;
	//
	CommittedFpInVar = xCommittedFpInVar;
	CommittedStressLikeInVar = xCommittedStressLikeInVar;
	CommittedStrainLikeInVar = xCommittedStrainLikeInVar;
	CommittedStressLikeKiVar = xCommittedStressLikeKiVar;
	CommittedStrainLikeKiVar = xCommittedStrainLikeKiVar;
}

//! @brief Virtual constructor.
XC::FDEPState *XC::FDEPState::getCopy(void)
  { return new FDEPState(*this); }

//------------------------------------------------------------------------------
XC::FDEPState::FDEPState( const FDEPState& fds )
{
	setFpInVar(fds.getFpInVar());
	setStressLikeInVar(fds.getStressLikeInVar());
	setStrainLikeInVar(fds.getStrainLikeInVar());
	setStressLikeKiVar(fds.getStressLikeKiVar());
	setStrainLikeKiVar(fds.getStrainLikeKiVar());
	//
	setCommittedFpInVar(fds.getCommittedFpInVar());
	setCommittedStressLikeInVar(fds.getCommittedStressLikeInVar());
	setCommittedStrainLikeInVar(fds.getCommittedStrainLikeInVar());
	setCommittedStressLikeKiVar(fds.getCommittedStressLikeKiVar());
	setCommittedStrainLikeKiVar(fds.getCommittedStrainLikeKiVar());
}

// Get member variables
//------------------------------------------------------------------------------
XC::straintensor XC::FDEPState::getFpInVar() const
{
	return FpInVar;
}

//------------------------------------------------------------------------------
double XC::FDEPState::getStrainLikeInVar() const
{
	return StrainLikeInVar;
}

//------------------------------------------------------------------------------
double XC::FDEPState::getStressLikeInVar() const
{
	return StressLikeInVar;
}

//------------------------------------------------------------------------------
XC::straintensor XC::FDEPState::getStrainLikeKiVar() const
{
	return StrainLikeKiVar;
}

//------------------------------------------------------------------------------
XC::stresstensor XC::FDEPState::getStressLikeKiVar() const
{
	return StressLikeKiVar;
}

//------------------------------------------------------------------------------
XC::straintensor XC::FDEPState::getCommittedFpInVar() const
{
	return CommittedFpInVar;
}

//------------------------------------------------------------------------------
double XC::FDEPState::getCommittedStrainLikeInVar() const
{
	return CommittedStrainLikeInVar;
}

//------------------------------------------------------------------------------
double XC::FDEPState::getCommittedStressLikeInVar() const
{
	return CommittedStressLikeInVar;
}

//------------------------------------------------------------------------------
XC::straintensor XC::FDEPState::getCommittedStrainLikeKiVar() const
{
	return CommittedStrainLikeKiVar;
}

//------------------------------------------------------------------------------
XC::stresstensor XC::FDEPState::getCommittedStressLikeKiVar() const
{
	return CommittedStressLikeKiVar;
}

// Set member variables
//---------------------------------------------------------------------------
void XC::FDEPState::setFpInVar(const XC::straintensor &xFpInVar)
{
	this->FpInVar = xFpInVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setStrainLikeInVar(double xStrainLikeInVar)
{
	this->StrainLikeInVar = xStrainLikeInVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setStressLikeInVar(double xStressLikeInVar)
{
	this->StressLikeInVar = xStressLikeInVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setStrainLikeKiVar(const XC::straintensor& xStrainLikeKiVar)
{
	this->StrainLikeKiVar = xStrainLikeKiVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setStressLikeKiVar(const XC::stresstensor& xStressLikeKiVar)
{
	this->StressLikeKiVar = xStressLikeKiVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setCommittedFpInVar(const XC::straintensor &xCommittedFpInVar)
{
	this->CommittedFpInVar = xCommittedFpInVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setCommittedStrainLikeInVar(double xCommittedStrainLikeInVar)
{
	this->CommittedStrainLikeInVar = xCommittedStrainLikeInVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setCommittedStressLikeInVar(double xCommittedStressLikeInVar)
{
	this->CommittedStressLikeInVar = xCommittedStressLikeInVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setCommittedStrainLikeKiVar(const XC::straintensor& xCommittedStrainLikeKiVar)
{
	this->CommittedStrainLikeKiVar = xCommittedStrainLikeKiVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setCommittedStressLikeKiVar(const XC::stresstensor& xCommittedStressLikeKiVar)
{
	this->CommittedStressLikeKiVar = xCommittedStressLikeKiVar;
}

//----------------------------------------------------------------------
int XC::FDEPState::commitState(void)
{
        CommittedFpInVar = FpInVar;
        CommittedStressLikeInVar = StressLikeInVar;
        CommittedStrainLikeInVar = StrainLikeInVar;
        CommittedStressLikeKiVar = StressLikeKiVar;
        CommittedStrainLikeKiVar = StrainLikeKiVar;

	return 0;
}

//----------------------------------------------------------------------
int XC::FDEPState::revertToLastCommit(void)
{
        FpInVar = CommittedFpInVar;
        StressLikeInVar = CommittedStressLikeInVar;
        StrainLikeInVar = CommittedStrainLikeInVar;
        StressLikeKiVar = CommittedStressLikeKiVar;
        StrainLikeKiVar = CommittedStrainLikeKiVar;

	return 0;
}

//! @brief Revert the material to its initial state.
int XC::FDEPState::revertToStart(void)
  {
    BJtensor tI2("I", 2, def_dim_2);
    BJtensor t00(2, def_dim_2, 0.0);

    FpInVar = tI2;
    StressLikeInVar = 0.0;
    StrainLikeInVar = 0.0;
    StressLikeKiVar = t00;
    StrainLikeKiVar = t00;

    CommittedFpInVar = tI2;
    CommittedStressLikeInVar = 0.0;
    CommittedStrainLikeInVar = 0.0;
    CommittedStressLikeKiVar = t00;
    CommittedStrainLikeKiVar = t00;

    return 0;
  }

# endif







