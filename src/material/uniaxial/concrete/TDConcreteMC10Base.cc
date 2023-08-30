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


#include "TDConcreteMC10Base.h"
#include "domain/mesh/element/utils/Information.h"
#include <float.h>
#include "domain/domain/Domain.h" //Added by AMK to get current Domain time;
#include "utility/recorder/response/MaterialResponse.h"
#include <cmath>

//! @brief Sets initial values for the concrete parameters.
void XC::TDConcreteMC10Base::setup_parameters(void)
  {
    TDConcreteBase::setup_parameters();

    //ntosic: SPLIT INTO BASIC AND DRYING!
    eps_crb = 0.0; //Added by ntosic
    eps_crd = 0.0; //Added by ntosic
    eps_shb = 0.0; //Added by ntosic
    eps_shd = 0.0; //Added by ntosic
    epsP_crb = 0.0; //Added by ntosic
    epsP_crd = 0.0; //Added by ntosic
    epsP_shb = 0.0; //Added by ntosic
    epsP_shd = 0.0; //Added by ntosic
    epsP_m = 0.0; //Added by AMK
	
    //Change inputs into the proper sign convention: ntosic: changed
    fc = -1.0*fabs(fc); 
    epsba = -1.0*fabs(epsba);
    epsda = -1.0*fabs(epsda);
    phiba = 1.0*fabs(phiba);
    phida = 1.0*fabs(phida);
  }

size_t XC::TDConcreteMC10Base::resize(void)
  {
    const size_t newSize= TDConcreteBase::resize();
    if(newSize!=PHIB_i.size()) // restart.
      {
	PHIB_i.resize(newSize);
    	PHID_i.resize(newSize);
      }
    return newSize;
  }

//! @brief Constructor.
XC::TDConcreteMC10Base::TDConcreteMC10Base(int tag, int classTag)
  : TDConcreteBase(tag, classTag)
  {}

//! @brief Constructor.
XC::TDConcreteMC10Base::TDConcreteMC10Base(int tag, int classTag, double _fc, double _ft, double _Ec, double _Ecm, double _beta, double _age, double _epsba, double _epsbb, double _epsda, double _epsdb, double _phiba, double _phibb, double _phida, double _phidb, double _tcast, double _cem)
  : TDConcreteBase(tag, classTag, _fc, _ft, _Ec, _beta, _age, _tcast),
  fc(_fc), Ecm(_Ecm), epsba(_epsba), epsbb(_epsbb), epsda(_epsda), epsdb(_epsdb), phiba(_phiba), phibb(_phibb), phida(_phida), phidb(_phidb), cem(_cem)
  {
    setup_parameters();
  }


//ntosic
double XC::TDConcreteMC10Base::setPhiBasic(double time, double tp)
  {	
    // ntosic: Model Code 2010 Equations
    double tmtp = time - tp;
    double tpa = tp * pow(9.0 / (2.0 + pow(tp, 1.2)) + 1.0, cem);
    double phiBasic = phiba * log(pow(30.0 / tpa + 0.035, 2.0) * (tmtp / phibb) + 1.0);
    return phiBasic;
  }

//ntosic
double XC::TDConcreteMC10Base::setPhiDrying(double time, double tp)
  {
	// ntosic: Model Code 2010 Equations
	double tmtp = time - tp;
	double tpa = tp * pow(9.0 / (2.0 + pow(tp, 1.2)) + 1.0, cem);
	double phiDrying = phida / (0.1 + pow(tpa,0.2)) * pow(tmtp, 1.0 / (2.3 + 3.5 / pow(tpa, 0.5))) / pow(phidb + tmtp, 1.0 / (2.3 + 3.5/pow(tpa,0.5)));
	return phiDrying;
  }

//ntosic
double XC::TDConcreteMC10Base::setShrinkBasic(double time)
  {
    double shrinkBasic = epsba * (1 - exp(-0.2* epsbb * pow(time, 0.5))); //ntosic: Model Code 2010 Equations
	return shrinkBasic;
  }

//ntosic
double XC::TDConcreteMC10Base::setShrinkDrying(double time)
  {
	double tD = age; //Age at initiation of drying
	double shrinkDrying = 0.0; //ntosic
	if (time - (tD) < 0) {
		shrinkDrying = 0.0;
	}
	else {
		shrinkDrying = epsda * pow(time - (tD), 0.5) / pow(epsdb + time - (tD), 0.5); //ntosic: Model Code 2010 Equations
	}
	return shrinkDrying;
  }

//ntosic
double XC::TDConcreteMC10Base::getPHIB_i(void) const
  { return phib_i; }
//ntosic
double XC::TDConcreteMC10Base::getPHID_i(void) const
  { return phid_i; }


//ntosic
double XC::TDConcreteMC10Base::getCreepBasic(void) const
  { return eps_crb; }
//ntosic
double XC::TDConcreteMC10Base::getCreepDrying(void) const
  { return eps_crd; }
//ntosic
double XC::TDConcreteMC10Base::getShrinkBasic(void) const
  { return eps_shb; }
//ntosic
double XC::TDConcreteMC10Base::getShrinkDrying(void) const
  { return eps_shd; }

int XC::TDConcreteMC10Base::revertToLastCommit(void)
  {
    eps_total = epsP_total; //Added by AMK;
    eps_shb = epsP_shb; //ntosic
    eps_shd = epsP_shd; //ntosic
    eps_crb = epsP_crb; //ntosic
    eps_crd = epsP_crd; //ntosic
    eps_m = epsP_m;  
    
    ecmin = ecminP;
    ecmax = ecmaxP; //ntosic
    dept = deptP;

    e = eP;
    sig = sigP;
    eps = epsP;
    return 0;
  }

int XC::TDConcreteMC10Base::revertToStart(void)
  {
    ecminP = 0.0;
    ecmaxP = 0.0; //ntosic
    deptP = 0.0;

    eP = Ec;
    epsP = 0.0;
    sigP = 0.0;
    eps = 0.0;
    sig = 0.0;
    e = Ec;

    if(creepControl==0)
      { count= 0; }
    else
      { count= 1; }
    resize();
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::TDConcreteMC10Base::sendData(Communicator &comm)
  {
    int res= TDConcreteBase::sendData(comm);
    res+= comm.sendDoubles(Ecm, epsba, epsbb, epsda, epsdb,getDbTagData(),CommMetaData(3));
    res+= comm.sendDoubles(phiba, phibb, phida, phidb, getDbTagData(),CommMetaData(4));
    res+= comm.sendDoubles(cem, ecminP, ecmaxP, deptP, epsP, getDbTagData(),CommMetaData(5));
    res+= comm.sendDoubles(sigP, eP, getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::TDConcreteMC10Base::recvData(const Communicator &comm)
  {
    int res= TDConcreteBase::recvData(comm);
    res+= comm.receiveDoubles(Ecm, epsba, epsbb, epsda, epsdb,getDbTagData(),CommMetaData(3));
    res+= comm.receiveDoubles(phiba, phibb, phida, phidb, getDbTagData(),CommMetaData(4));
    res+= comm.receiveDoubles(cem, ecminP, ecmaxP, deptP, epsP, getDbTagData(),CommMetaData(5));
    res+= comm.receiveDoubles(sigP, eP, getDbTagData(),CommMetaData(6));
    return res;
  }

int XC::TDConcreteMC10Base::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::TDConcreteMC10Base::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

