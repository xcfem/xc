// -*-c++-*-
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

// $Revision: 1.0 $
// $Date: 2019-01-28 17:53:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/damping/UniformDamping.h,v $

// Written: Yuli Huang (yulee@berkeley.edu)
// Created: 02/2020
// Revision: A
//
// Description: This file contains the definition for the UDampingBase class.
// UDampingBase provides the abstraction of an elemental damping imposition
// providing uniform damping over a frequency range
//
// Reference:
// Yuli Huang, Richard Sturt, Michael Willford,
// A damping model for nonlinear dynamic analysis providing uniform damping over a frequency range,
// Computers & Structures,
// Volume 212,
// 2019,
// Pages 101-109,
// ISSN 0045-7949,
// https://doi.org/10.1016/j.compstruc.2018.10.016
//
// Yuan Tian, Yuli Huang, Zhe Qu, Yifan Fei, Xinzheng Lu,
// High-performance uniform damping model for response history analysis in OpenSees,
// Journal of Earthquake Engineering,
// 2022,
// https://doi.org/10.1080/13632469.2022.2124557
//
// What: "@(#) UDampingBase.h, revA"

#ifndef UDampingBase_h
#define UDampingBase_h

#include "Damping.h"
#include "utility/matrix/Matrix.h"

namespace XC {

//! @brief Base class for user-defined and uniform damings.
class UDampingBase: public Damping
  {
  protected:
    // internal data
    int nComp, nFilter;
    Vector omegac;
    Matrix qL, qLC;
    Vector qdC;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);  
  public:
    UDampingBase(int tag, int classTag, double ta, double td, TimeSeries *fac);
    UDampingBase(int tag, int classTag, double ta, double td, TimeSeries *fac, int nFilter, const Vector &omegac);


    int setDomain(Domain *domain, int nComp);

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);
  };
} // end of XC namespace

#endif
