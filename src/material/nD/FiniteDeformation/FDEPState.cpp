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
		       const XC::straintensor& xCommitedFpInVar,
	               double xCommitedStressLikeInVar,
	               double xCommitedStrainLikeInVar,
		       const XC::stresstensor& xCommitedStressLikeKiVar,
		       const XC::straintensor& xCommitedStrainLikeKiVar )
{
	FpInVar = xFpInVar;
	StressLikeInVar = xStressLikeInVar;
	StrainLikeInVar = xStrainLikeInVar;
	StressLikeKiVar = xStressLikeKiVar;
	StrainLikeKiVar = xStrainLikeKiVar;
	//
	CommitedFpInVar = xCommitedFpInVar;
	CommitedStressLikeInVar = xCommitedStressLikeInVar;
	CommitedStrainLikeInVar = xCommitedStrainLikeInVar;
	CommitedStressLikeKiVar = xCommitedStressLikeKiVar;
	CommitedStrainLikeKiVar = xCommitedStrainLikeKiVar;
}

//------------------------------------------------------------------------------
XC::FDEPState *XC::FDEPState::newObj ()
  { return new FDEPState(*this); }

//------------------------------------------------------------------------------
XC::FDEPState::FDEPState( const XC::FDEPState& fds )
{
	setFpInVar(fds.getFpInVar());
	setStressLikeInVar(fds.getStressLikeInVar());
	setStrainLikeInVar(fds.getStrainLikeInVar());
	setStressLikeKiVar(fds.getStressLikeKiVar());
	setStrainLikeKiVar(fds.getStrainLikeKiVar());
	//
	setCommitedFpInVar(fds.getCommitedFpInVar());
	setCommitedStressLikeInVar(fds.getCommitedStressLikeInVar());
	setCommitedStrainLikeInVar(fds.getCommitedStrainLikeInVar());
	setCommitedStressLikeKiVar(fds.getCommitedStressLikeKiVar());
	setCommitedStrainLikeKiVar(fds.getCommitedStrainLikeKiVar());
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
XC::straintensor XC::FDEPState::getCommitedFpInVar() const
{
	return CommitedFpInVar;
}

//------------------------------------------------------------------------------
double XC::FDEPState::getCommitedStrainLikeInVar() const
{
	return CommitedStrainLikeInVar;
}

//------------------------------------------------------------------------------
double XC::FDEPState::getCommitedStressLikeInVar() const
{
	return CommitedStressLikeInVar;
}

//------------------------------------------------------------------------------
XC::straintensor XC::FDEPState::getCommitedStrainLikeKiVar() const
{
	return CommitedStrainLikeKiVar;
}

//------------------------------------------------------------------------------
XC::stresstensor XC::FDEPState::getCommitedStressLikeKiVar() const
{
	return CommitedStressLikeKiVar;
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
void XC::FDEPState::setCommitedFpInVar(const XC::straintensor &xCommitedFpInVar)
{
	this->CommitedFpInVar = xCommitedFpInVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setCommitedStrainLikeInVar(double xCommitedStrainLikeInVar)
{
	this->CommitedStrainLikeInVar = xCommitedStrainLikeInVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setCommitedStressLikeInVar(double xCommitedStressLikeInVar)
{
	this->CommitedStressLikeInVar = xCommitedStressLikeInVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setCommitedStrainLikeKiVar(const XC::straintensor& xCommitedStrainLikeKiVar)
{
	this->CommitedStrainLikeKiVar = xCommitedStrainLikeKiVar;
}

//---------------------------------------------------------------------------
void XC::FDEPState::setCommitedStressLikeKiVar(const XC::stresstensor& xCommitedStressLikeKiVar)
{
	this->CommitedStressLikeKiVar = xCommitedStressLikeKiVar;
}

//----------------------------------------------------------------------
int XC::FDEPState::commitState(void)
{
        CommitedFpInVar = FpInVar;
        CommitedStressLikeInVar = StressLikeInVar;
        CommitedStrainLikeInVar = StrainLikeInVar;
        CommitedStressLikeKiVar = StressLikeKiVar;
        CommitedStrainLikeKiVar = StrainLikeKiVar;

	return 0;
}

//----------------------------------------------------------------------
int XC::FDEPState::revertToLastCommit(void)
{
        FpInVar = CommitedFpInVar;
        StressLikeInVar = CommitedStressLikeInVar;
        StrainLikeInVar = CommitedStrainLikeInVar;
        StressLikeKiVar = CommitedStressLikeKiVar;
        StrainLikeKiVar = CommitedStrainLikeKiVar;

	return 0;
}

//----------------------------------------------------------------------
int XC::FDEPState::revertToStart(void)
{
	BJtensor tI2("I", 2, def_dim_2);
	BJtensor t00(2, def_dim_2, 0.0);

	FpInVar = tI2;
	StressLikeInVar = 0.0;
	StrainLikeInVar = 0.0;
	StressLikeKiVar = t00;
	StrainLikeKiVar = t00;

	CommitedFpInVar = tI2;
	CommitedStressLikeInVar = 0.0;
	CommitedStrainLikeInVar = 0.0;
	CommitedStressLikeKiVar = t00;
	CommitedStrainLikeKiVar = t00;

	return 0;
}

# endif







