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
// $Source: /usr/local/cvs/OpenSees/SRC/damping/UniformDamping.cpp,v $

// Written: Yuli Huang (yulee@berkeley.edu)
// Created: 02/2020
// Revision: A
// 
// Description: This file contains the implementation for the UniformDamping class.
// UniformDamping provides the abstraction of an elemental damping imposition
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
// What: "@(#) UniformDamping.cpp, revA"

//extern StaticAnalysis *theStaticAnalysis;

#include "UniformDamping.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/domain/Domain.h"
#include "solution/analysis/analysis/StaticAnalysis.h"
#include "utility/actor/actor/CommMetaData.h"
#include "domain/load/pattern/TimeSeries.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

// constructor:
XC::UniformDamping::UniformDamping(int tag, double cd, double f1, double f2, double t1, double t2, TimeSeries *f)
  : UDampingBase(tag, DMP_TAG_UniformDamping, t1, t2, f),
    eta(cd), freq1(f1), freq2(f2), 
    alpha()
  {
    if (eta <= 0.0)
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << ":  Invalid damping ratio."
		  << Color::def << std::endl;

    if (freq1 <= 0.0 || freq2 <= 0.0 || freq1 >= freq2)
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << ":  Invalid frequency range."
		  << Color::def << std::endl;
    Initialize();
  }

// constructor:
XC::UniformDamping::UniformDamping(int tag, double cd, double f1, double f2, double t1, double t2, TimeSeries *f, int nF, const Vector &a, const Vector &w)
  : UDampingBase(tag, DMP_TAG_UniformDamping, t1, t2, f),
    eta(cd), freq1(f1), freq2(f2), 
    alpha()
  {
    if (eta <= 0.0)
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << ":  Invalid damping ratio."
		  << Color::def << std::endl;

    if (freq1 <= 0.0 || freq2 <= 0.0 || freq1 >= freq2)
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << ":  Invalid frequency range."
		  << Color::def << std::endl;
    if ((nF > 0) && (a.Size() == nF) && (w.Size() == nF))
      {
	alpha= Vector(a);
      }
    else
      {
	Initialize();
      }
  }


// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::UniformDamping::UniformDamping(void)
  : UDampingBase(0, DMP_TAG_UniformDamping, 0.0, 0.0, nullptr),
    eta(0.0), freq1(0.0), freq2(0.0),
    alpha()
  {}


int XC::UniformDamping::Initialize(void)
  {
    const double delta= 0.05;
    const double f1log= log10(freq1);
    const double f2log= log10(freq2);

    nFilter = 2;
    for (int iter = 0; iter < 100; ++iter)
      {
	const double dfreq = (f2log - f1log) / (nFilter - 1);
	alpha= Vector(nFilter);
	omegac= Vector(nFilter);

	for (int i = 0; i < nFilter; ++i)
	  {
	    omegac(i) = 6.28318530718 * pow(10.0, f1log + i * dfreq);
	  }

	const int nf = 100 * nFilter;
	const double df = (f2log - f1log) / (nf - 1);
	Vector y= Vector(nFilter);
	Matrix X= Matrix(nFilter, nFilter);
	for (int i = 0; i < nf; ++i)
	  {
	    const double omega = 6.28318530718 * pow(10.0, f1log + i * df);
	    for (int j = 0; j < nFilter; ++j)
	      {
		const double wjn = omega / omegac[j];
		const double phij = 2.0 * wjn / (1.0 + wjn * wjn);
		y(j) += phij;
		for (int k = 0; k < nFilter; ++k)
		  {
		    const double wkn= omega / omegac[k];
		    const double phik = 2.0 * wkn / (1.0 + wkn * wkn);
		    X(j, k) += phij * phik;
		  }
	      }
	  }

	alpha = y/X;


	bool converged = true;
	for (int i = 0; i < nf; ++i)
	  {
	    const double omega = 6.28318530718 * pow(10.0, f1log + i * df);
	    double err = 0.0;
	    for (int j = 0; j < nFilter; ++j)
	      {
		const double wjn = omega / omegac[j];
		const double phij = 2.0 * wjn / (1.0 + wjn * wjn);
		err += alpha(j) * phij;
	      }
	    err -= 1.0;
	    if (err > delta || err < -delta)
	      {
		converged = false;
		break;
	      }
	  }

	if(converged) break;

	++nFilter;
      }
    return 0;
  }

int XC::UniformDamping::update(const Vector &q)
  {
    const double t = theDomain->getCurrentTime();
    const double dT = theDomain->getDt();
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    // StaticAnalysis **theStaticAnalysis= OPS_GetStaticAnalysis();
    StaticAnalysis *theStaticAnalysis= nullptr;
    if(theStaticAnalysis)
      {
	q0 = q;
	qd.Zero();
	for (int i = 0; i < nFilter; ++i)
	  for (int j = 0; j < nComp; ++j)
	    qL(j,i) = q(j);
      }
    else if(dT > 0.0)
      {
	q0 = q;
	qd.Zero();
	if(t < td)
	  {
	    if(t > ta)
	      {
		for (int i = 0; i < nFilter; ++i)
		  {
		    const double dTomegac = dT * omegac(i);
		    const double cd = 4.0 * alpha(i) * eta / (2.0 + dTomegac);
		    const double c0 = dTomegac / (2.0 + dTomegac);
		    const double cL = (2.0 - dTomegac) / (2.0 + dTomegac);
		    for (int j = 0; j < nComp; ++j)
		      {
			qd(j) += cd * (q0C(j) + q0(j) - 2.0 * qLC(j,i));
			qL(j,i) = c0 * (q0C(j) + q0(j)) + cL * qLC(j,i);
		      }
		  }
		qd-= qdC;
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

double XC::UniformDamping::getStiffnessMultiplier(void) const
  {
    const double t = theDomain->getCurrentTime();
    const double dT = theDomain->getDt();
    double km = 0.0;
    // StaticAnalysis **theStaticAnalysis= OPS_GetStaticAnalysis();
    StaticAnalysis *theStaticAnalysis= nullptr;
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    if(!theStaticAnalysis && dT > 0.0 && t > ta && t < td)
      {
	for (int i = 0; i < nFilter; ++i)
	  {
	    km += 4.0 * alpha(i) * eta / (2.0 + omegac(i) * dT);
	  }
	if (fac) km *= fac->getFactor(t);
      }
    return 1.0 + km;
  }

XC::Damping *XC::UniformDamping::getCopy(void) const
  { return new UniformDamping(*this); }

//! @brief Send object members through the communicator argument.
int XC::UniformDamping::sendData(Communicator &comm)
  {
    int res= UDampingBase::sendData(comm);
    res+= comm.sendDoubles(eta, freq1, freq2,getDbTagData(),CommMetaData(12));
    res+= comm.sendVector(alpha, getDbTagData(), CommMetaData(13));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::UniformDamping::recvData(const Communicator &comm)
  {
    int res= UDampingBase::recvData(comm);
    res+= comm.receiveDoubles(eta, freq1, freq2, getDbTagData(),CommMetaData(12));
    res+= comm.receiveVector(alpha, getDbTagData(), CommMetaData(13));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::UniformDamping::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(14);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::UniformDamping::recvSelf(const Communicator &comm)
  {
    inicComm(14);
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

void XC::UniformDamping::Print(std::ostream &s, int flag)
  {
    s << "\nDamping: " << this->getTag() << " Type: UniformDamping";
    s << "\tdamping ratio: " << 0.5 * eta << std::endl;
    s << "\tlower bound frequency: " << freq1 << std::endl;
    s << "\tupper bound frequency: " << freq2 << std::endl;
    s << "\tactivation time: " << ta << std::endl;
    s << "\tdeactivation time: " << td << std::endl;
  }

int XC::UniformDamping::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv[0]=="dampingRatio")
      {
	param.setValue(0.5*eta);
	return param.addObject(1, this);
      }
    return -1;
  }


int XC::UniformDamping::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case 1:
	eta = info.theDouble * 2.0;
	return 0;
      default:
	return -1;
      }
  }

