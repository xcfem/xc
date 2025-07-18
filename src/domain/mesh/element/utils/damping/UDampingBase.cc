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
// $Source: /usr/local/cvs/OpenSees/SRC/damping/UDampingBase.cpp,v $

// Written: Yuli Huang (yulee@berkeley.edu)
// Created: 02/2020
// Revision: A
// 
// Description: This file contains the implementation for the UDampingBase class.
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
// What: "@(#) UDampingBase.cpp, revA"

//extern StaticAnalysis *theStaticAnalysis;

#include "UDampingBase.h"
#include "domain/domain/Domain.h"
#include "solution/analysis/analysis/StaticAnalysis.h"
#include "utility/actor/actor/CommMetaData.h"
#include "domain/load/pattern/TimeSeries.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

// constructor:
XC::UDampingBase::UDampingBase(int tag, int classTag, double t1, double t2, TimeSeries *f)
  : Damping(tag, classTag, t1, t2, f),
    nComp(0), nFilter(0),
    omegac(), qL(), qLC(), qdC()
  {}

// constructor:
XC::UDampingBase::UDampingBase(int tag, int classTag, double t1, double t2, TimeSeries *f, int nF, const Vector &w)
  : Damping(tag, classTag, t1, t2, f),
    nComp(0), nFilter(0),
    omegac(), qL(), qLC(), qdC()
  {
    if ((nF > 0) && (w.Size() == nF))
      {
	nFilter = nF;
	omegac= Vector(w);
      }
  }


int XC::UDampingBase::commitState(void)
  {
    Damping::commitState();
    qdC= qd;
    qLC= qL;
    return 0;
  }


int XC::UDampingBase::revertToLastCommit(void)
  {
    Damping::revertToLastCommit();
    qd= qdC;
    qL = qLC;
    return 0;
  }


int XC::UDampingBase::revertToStart(void)
  {
    Damping::revertToStart();
    qd.Zero();
    qdC.Zero();
    qL.Zero();
    qLC.Zero();
    return 0;
  }


int XC::UDampingBase::setDomain(Domain *domain, int nC)
  {
    Damping::setDomain(domain, nC);
    nComp = nC;
  
    qdC= Vector(nComp);
    qL= Matrix(nComp, nFilter);
    qLC= Matrix(nComp, nFilter);
  
    return 0;
  }


//! @brief Send object members through the communicator argument.
int XC::UDampingBase::sendData(Communicator &comm)
  {
    int res= Damping::sendData(comm);
    res+= comm.sendInts(nComp, nFilter, getDbTagData(), CommMetaData(7));
    res+= comm.sendVector(omegac, getDbTagData(), CommMetaData(8));
    res+= comm.sendMatrix(qL, getDbTagData(), CommMetaData(9));
    res+= comm.sendMatrix(qLC, getDbTagData(), CommMetaData(10));
    res+= comm.sendVector(qdC, getDbTagData(), CommMetaData(11));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::UDampingBase::recvData(const Communicator &comm)
  {
    int res= Damping::recvData(comm);
    res+= comm.receiveInts(nComp, nFilter, getDbTagData(), CommMetaData(7));
    res+= comm.receiveVector(omegac, getDbTagData(), CommMetaData(8));
    res+= comm.receiveMatrix(qL, getDbTagData(), CommMetaData(9));
    res+= comm.receiveMatrix(qLC, getDbTagData(), CommMetaData(10));
    res+= comm.receiveVector(qdC, getDbTagData(), CommMetaData(11));
    return res;
  }


