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
// $Date: 2021-07-02 14:29:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/damping/URDDampingbeta.h,v $

// Revised by: Y Tian
// Created: 02/2020
// Revision: A
//
// Description: This file contains the definition for the URDDampingbeta class.
// URDDampingbeta provides the abstraction of an elemental damping imposition
// providing user-define damping over a frequency range
//
// Reference:
//

// What: "@(#) URDDampingbeta.cpp, revA"

#include "URDDampingbeta.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/domain/Domain.h"
#include "solution/analysis/analysis/StaticAnalysis.h"
#include "utility/actor/actor/CommMetaData.h"
#include "domain/load/pattern/TimeSeries.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

//extern StaticAnalysis *theStaticAnalysis;

// constructor:
XC::URDDampingbeta::URDDampingbeta(int tag, int nfreq, const Vector &tmpomegac, const Vector &tmpbeta, double t1, double t2, TimeSeries *f)
  : UDampingBase(tag, DMP_TAG_URDDampingbeta, t1, t2, f, nfreq, tmpomegac),
    beta(tmpbeta)
  {}



// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::URDDampingbeta::URDDampingbeta()
  : UDampingBase(0, DMP_TAG_URDDampingbeta, 0.0, 0.0, nullptr),
    beta() {}

int XC::URDDampingbeta::Initialize(void)
  { return 0; }

int XC::URDDampingbeta::update(const Vector &q)
  {
    const double t = theDomain->getCurrentTime();
    const double dT = theDomain->getDt();
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    //StaticAnalysis **theStaticAnalysis = OPS_GetStaticAnalysis();
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    StaticAnalysis *theStaticAnalysis= nullptr;
    if(theStaticAnalysis)
    {
      q0 = q;
      qd.Zero();
      for (int i = 0; i < nFilter; ++i)
	for (int j = 0; j < nComp; ++j)
	  qL(j,i) = q(j);
    }
    else if (dT > 0.0)
    {
      q0 = q;
      qd.Zero();
      if (t < td)
      {
	if (t > ta)
	{
	  for (int i = 0; i < nFilter; ++i)
	  {
	    double dTomegac = dT * omegac(i);
	    double cd = 4.0 * beta(i) / (2.0 + dTomegac);
	    double c0 = dTomegac / (2.0 + dTomegac);
	    double cL = (2.0 - dTomegac) / (2.0 + dTomegac);
	    for (int j = 0; j < nComp; ++j)
	    {
	      qd(j) += cd * (q0C(j) + q0(j) - 2.0 * qLC(j,i));
	      qL(j,i) = c0 * (q0C(j) + q0(j)) + cL * qLC(j,i);
	    }
	  }
	  qd -= qdC;
	}
	else
	{
	  for (int i = 0; i < nFilter; ++i)
	    for (int j = 0; j < nComp; ++j)
	      qL(j,i) = q(j);
	}
	if (fac) qd *= fac->getFactor(t);
      }
    }
    return 0;
  }


double XC::URDDampingbeta::getStiffnessMultiplier(void) const
  {
    const double t = theDomain->getCurrentTime();
    const double dT = theDomain->getDt();
    double km = 0.0;
    // StaticAnalysis **theStaticAnalysis= OPS_GetStaticAnalysis();
    StaticAnalysis *theStaticAnalysis= nullptr;
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    if (!theStaticAnalysis && dT > 0.0 && t > ta && t < td)
    {
      for (int i = 0; i < nFilter; ++i)
      {
	  km += 4.0 * beta(i) / (2.0 + omegac(i) * dT); 
      }
      if (fac) km *= fac->getFactor(t);
    }
    return 1.0 + km;
  }

XC::Damping *XC::URDDampingbeta::getCopy(void) const
  { return new URDDampingbeta(*this); }

//! @brief Send object members through the communicator argument.
int XC::URDDampingbeta::sendData(Communicator &comm)
  {
    int res= UDampingBase::sendData(comm);
    res+= comm.sendVector(beta,getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::URDDampingbeta::recvData(const Communicator &comm)
  {
    int res= UDampingBase::recvData(comm);
    res+= comm.receiveVector(beta, getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::URDDampingbeta::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(13);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::URDDampingbeta::recvSelf(const Communicator &comm)
  {
    inicComm(13);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids."
	        << Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data."
	            << Color::def << std::endl;
      }
    return res;
  }


void XC::URDDampingbeta::Print(std::ostream &s, int flag)
  {
    s << "\nDamping: " << this->getTag() << " Type: URDDampingbeta";
    s << "\tnumber of filters: " << nFilter << std::endl;
    s << "\tfrequency: " << omegac/6.28318530718 << std::endl;
    s << "\tbeta: " << beta << std::endl;
    s << "\tactivation time: " << ta << std::endl;
    s << "\tdeactivation time: " << td << std::endl;
  }

