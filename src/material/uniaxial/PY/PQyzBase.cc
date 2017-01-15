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
/* *********************************************************************
**    Module:        PQyzBase.cpp 
**
**    Purpose:        Provide a simple p-y spring for OpenSees.
**
**    Developed by Ross XC::W. Boulanger
**    (C) Copyright 2001, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2001/12/15
// $Source: /OpenSees/SRC/material/uniaxial/PQyzBase.cpp

// Written: RWB
// Created: Dec 2001
// Revision: A
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class implementation for XC::PQyzBase


#include "material/uniaxial/PY/PQyzBase.h"


// Controls on internal iteration between spring components
const int PYmaxIterations = 20;
const double PYtolerance = 1.0e-12;

/////////////////////////////////////////////////////////////////////
//! @brief Constructor with data
XC::PQyzBase::PQyzBase(int tag, int classtag, int soil, double rult, double v50,double dash_pot)
  :PYBase(tag,classtag,soil,rult,v50,dash_pot)
  {}

//! @brief Constructor.
XC::PQyzBase::PQyzBase(int tag,int classtag)
  :PYBase(tag,classtag) {}

/////////////////////////////////////////////////////////////////////
//! @brief Default constructor.
XC::PQyzBase::PQyzBase(void)
  :PYBase(0,0) {}

/////////////////////////////////////////////////////////////////////
int XC::PQyzBase::commitState(void)
  {
    PYBase::commitState();
    CNF= TNF; // Commit trial history variables for Near Field component
    CGap= TGap; // Commit trial history variables for the Gap
    return 0;
  }

/////////////////////////////////////////////////////////////////////
int XC::PQyzBase::revertToLastCommit(void)
  {
    // Reset to committed values
    PYBase::revertToLastCommit();
    TNF= CNF;
    TGap= CGap;
    return 0;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::PQyzBase::sendData(CommParameters &cp)
  {
    int res= PYBase::sendData(cp);
    res+= cp.sendDoubles(NFkrig,Elast,nd,getDbTagData(),CommMetaData(9));
    res+= cp.sendMovable(CNF,getDbTagData(),CommMetaData(10));
    res+= cp.sendMovable(TNF,getDbTagData(),CommMetaData(11));
    res+= cp.sendMovable(CGap,getDbTagData(),CommMetaData(12));
    res+= cp.sendMovable(TGap,getDbTagData(),CommMetaData(13));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::PQyzBase::recvData(const CommParameters &cp)
  {
    int res= PYBase::recvData(cp);
    res+= cp.receiveDoubles(NFkrig,Elast,nd,getDbTagData(),CommMetaData(9));
    res+= cp.receiveMovable(CNF,getDbTagData(),CommMetaData(10));
    res+= cp.receiveMovable(TNF,getDbTagData(),CommMetaData(11));
    res+= cp.receiveMovable(CGap,getDbTagData(),CommMetaData(12));
    res+= cp.receiveMovable(TGap,getDbTagData(),CommMetaData(13));
    return res;
  }

/////////////////////////////////////////////////////////////////////

