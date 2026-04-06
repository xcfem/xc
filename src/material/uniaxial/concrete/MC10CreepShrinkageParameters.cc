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

#include "material/uniaxial/concrete/MC10CreepShrinkageParameters.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor.
XC::MC10CreepShrinkageParameters::MC10CreepShrinkageParameters(void)
  : CommandEntity(), MovableObject(0),
    epsba(0.0), epsbb(0.0), epsda(0.0), epsdb(0.0),
    phiba(0.0), phibb(0.0), phida(0.0), phidb(0.0),
    cem(0.0)
  {}

//! @brief Constructor.
//! @param _epsba: ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010.
//! @param _epsbb: fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
//! @param _epsda: product of εcds,0 and βRH, as per Model Code 2010.
//! @param _epsdb: fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
//! @param _phiba: parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
//! @param _phibb: fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
//! @param _phida: product of βdc(fcm) and β(RH), as per Model Code 2010.
//! @param _phidb: fitting constant within the drying creep time evolution function as per Model Code 2010.
//! @param _cem: coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.
XC::MC10CreepShrinkageParameters::MC10CreepShrinkageParameters(const double &_epsba, const double &_epsbb, const double &_epsda, const double &_epsdb, const double &_phiba, const double &_phibb, const double &_phida, const double &_phidb, const double &_cem)
  : CommandEntity(), MovableObject(0),
    epsba(_epsba), epsbb(_epsbb), epsda(_epsda), epsdb(_epsdb),
    phiba(_phiba), phibb(_phibb), phida(_phida), phidb(_phidb),
    cem(_cem)
  {      
    //Change inputs into the proper sign convention:
    this->setup_parameters();
  }

//! @brief Sets initial values for the concrete parameters.
void XC::MC10CreepShrinkageParameters::setup_parameters(void)
  {
    //Change inputs into the proper sign convention: ntosic: changed
    epsba = -fabs(epsba);
    epsda = -fabs(epsda);
    phiba = fabs(phiba);
    phida = fabs(phida);
  }

//! @brief Get ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010
double XC::MC10CreepShrinkageParameters::getEpsba(void) const
  { return epsba; }

//! @brief Set ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010
void XC::MC10CreepShrinkageParameters::setEpsba(const double &d)
  { epsba= d; }

//! @brief Get fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
double XC::MC10CreepShrinkageParameters::getEpsbb(void) const
  { return epsbb; }

//! @brief Set fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
void XC::MC10CreepShrinkageParameters::setEpsbb(const double &d)
  { epsbb= d; }

//! @brief Get the value of epsda: product of εcds,0 and βRH, as per Model Code 2010.
double XC::MC10CreepShrinkageParameters::getEpsda(void) const
  { return epsda; }

//! @brief Set the value of epsda: product of εcds,0 and βRH, as per Model Code 2010.
void XC::MC10CreepShrinkageParameters::setEpsda(const double &d)
  { epsda= d; }

//! @brief Get fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
double XC::MC10CreepShrinkageParameters::getEpsdb(void) const
  { return epsdb; }

//! @brief Set fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
void XC::MC10CreepShrinkageParameters::setEpsdb(const double &d)
  { epsdb= d; }


//! @brief Get parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
double XC::MC10CreepShrinkageParameters::getPhiba(void) const
  { return phiba; }

//! @brief Set parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
void XC::MC10CreepShrinkageParameters::setPhiba(const double &d)
  { phiba= d; }

//! @brief Get fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
double XC::MC10CreepShrinkageParameters::getPhibb(void) const
  { return phibb; }

//! @brief Set fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
void XC::MC10CreepShrinkageParameters::setPhibb(const double &d)
  { phibb= d; }

//! @brief Get product of βdc(fcm) and β(RH), as per Model Code 2010
double XC::MC10CreepShrinkageParameters::getPhida(void) const
  { return phida; }

//! @brief Set product of βdc(fcm) and β(RH), as per Model Code 2010
void XC::MC10CreepShrinkageParameters::setPhida(const double &d)
  { phida= d; }

//! @brief Get fitting constant within the drying creep time evolution function as per Model Code 2010.
double XC::MC10CreepShrinkageParameters::getPhidb(void) const
  { return phidb; }

//! @brief Set fitting constant within the drying creep time evolution function as per Model Code 2010.
void XC::MC10CreepShrinkageParameters::setPhidb(const double &d)
  { phidb= d; }

//! @brief Get coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.
double XC::MC10CreepShrinkageParameters::getCem(void) const
  { return cem; }

//! @brief Set coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.
void XC::MC10CreepShrinkageParameters::setCem(const double &d)
  { cem= d; }

//! @brief Send object members through the communicator argument.
int XC::MC10CreepShrinkageParameters::sendData(Communicator &comm)
  {
    int res= comm.sendDoubles(epsba, epsbb, epsda, epsdb, getDbTagData(),CommMetaData(1));
    res+= comm.sendDoubles(phiba, phibb, phida, phidb, cem, getDbTagData(), CommMetaData(2));
    return res;
  }
//! @brief Receives object members through the communicator argument.
int XC::MC10CreepShrinkageParameters::recvData(const Communicator &comm)
  {
    int res= comm.receiveDoubles(epsba, epsbb, epsda, epsdb, getDbTagData(),CommMetaData(1));
    res+= comm.receiveDoubles(phiba, phibb, phida, phidb, cem, getDbTagData(),CommMetaData(2));
    return res;
  }

//ntosic
double XC::MC10CreepShrinkageParameters::getPhiBasic(double time, double tp) const
  {	
    // ntosic: Model Code 2010 Equations
    const double tmtp= time - tp;
    const double tpa = tp * pow(9.0 / (2.0 + pow(tp, 1.2)) + 1.0, cem);
    const double phiBasic= phiba * log(pow(30.0 / tpa + 0.035, 2.0) * (tmtp / phibb) + 1.0);
    return phiBasic;
  }

//ntosic
double XC::MC10CreepShrinkageParameters::getPhiDrying(double time, double tp) const
  {
    // ntosic: Model Code 2010 Equations
    const double tmtp= time - tp;
    const double tpa= tp * pow(9.0 / (2.0 + pow(tp, 1.2)) + 1.0, cem);
    const double phiDrying = phida / (0.1 + pow(tpa,0.2)) * pow(tmtp, 1.0 / (2.3 + 3.5 / pow(tpa, 0.5))) / pow(phidb + tmtp, 1.0 / (2.3 + 3.5/pow(tpa,0.5)));
    return phiDrying;
  }
//ntosic
double XC::MC10CreepShrinkageParameters::getShrinkBasic(double time)
  {
    double shrinkBasic = epsba * (1 - exp(-0.2* epsbb * pow(time, 0.5))); //ntosic: Model Code 2010 Equations
    return shrinkBasic;
  }

//ntosic
double XC::MC10CreepShrinkageParameters::getShrinkDrying(double age, double time)
  {
    const double tD= age; //Age at initiation of drying
    double shrinkDrying = 0.0; //ntosic
    if (time - (tD) < 0)
      {
	shrinkDrying = 0.0;
      }
    else
      {
	shrinkDrying = epsda * pow(time - (tD), 0.5) / pow(epsdb + time - (tD), 0.5); //ntosic: Model Code 2010 Equations
      }
    return shrinkDrying;
  }

//! @brief Sends object through the communicator argument.
int XC::MC10CreepShrinkageParameters::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
		<< Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::MC10CreepShrinkageParameters::recvSelf(const Communicator &comm)
  {
    inicComm(3);
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

void XC::MC10CreepShrinkageParameters::Print(std::ostream &s, int flag) const
  {
    s << "MC10CreepShrinkageParameters: "
      << epsba << epsbb
      << epsda << epsdb
      << phiba << phibb
      << phida << phidb
      << cem
      << std::endl;
  }


