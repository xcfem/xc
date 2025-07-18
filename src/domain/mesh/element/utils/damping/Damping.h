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
// $Source: /usr/local/cvs/OpenSees/SRC/damping/Damping.h,v $

// Written: Yuli Huang (yulee@berkeley.edu)
// Created: 02/2020
// Revision: A
//
// Description: This file contains the definition for the Damping class.
// Damping provides the abstraction of an elemental damping imposition.
// It is an abstract base class and thus no objects of it's type can be instatiated
// It has pure virtual functions which  must be implemented in it's derived classes.
//
// Reference:
// Yuan Tian, Yuli Huang, Zhe Qu, Yifan Fei, Xinzheng Lu,
// High-performance uniform damping model for response history analysis in OpenSees,
// Journal of Earthquake Engineering,
// 2022,
// https://doi.org/10.1080/13632469.2022.2124557
//
// What: "@(#) Damping.h, revA"

#ifndef Damping_h
#define Damping_h

#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Domain;
class TimeSeries;

// class definition

class Damping: public TaggedObject, public MovableObject
  {
  protected:
    Domain *theDomain;
    TimeSeries *fac; //!< damping variation in time.
    double ta, td;
    Vector qd, q0, q0C;

    int sendData(Communicator &);
    int recvData(const Communicator &);
  private:
    void free(void);
  public:
    Damping(int tag, int classTag, double t1, double t2, TimeSeries *f);
    Damping(const Damping &);
    Damping &operator=(const Damping &);
    
    virtual ~Damping(void);

    // method to set the associated TimeSeries and Domain
    const TimeSeries *getTimeSeries(void) const;
    TimeSeries *getTimeSeries(void);
    virtual void setTimeSeries(TimeSeries *theSeries);
    
    virtual Damping *getCopy(void) const= 0;
    virtual int setDomain(Domain *domain, int nComp);
    virtual int update(const Vector &q) = 0;
    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);
    virtual const Vector &getDampingForce(void) const;
    virtual double getStiffnessMultiplier(void) const= 0;
  };
} // end of XC namespace

#endif
