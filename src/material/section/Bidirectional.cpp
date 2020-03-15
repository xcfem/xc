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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/Bidirectional.cpp,v $

#include <material/section/Bidirectional.h>           
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "material/section/ResponseId.h"

XC::Vector XC::Bidirectional::s(2);
XC::Matrix XC::Bidirectional::ks(2,2);

//! @brief Constructor.
XC::Bidirectional::Bidirectional(int tag, double e, double sy, double Hi, double Hk)
  : SectionForceDeformation(tag, SEC_TAG_Bidirectional), E(e), sigY(sy), Hiso(Hi), Hkin(Hk)
  {
    for(int i = 0; i < 2; i++)
      {
        eP_n[i]  = 0.0;
        eP_n1[i] = 0.0;
        q_n[i]  = 0.0;
        q_n1[i] = 0.0;
      }
    alpha_n  = 0.0;
    alpha_n1 = 0.0;
  }

//! @brief Constructor.
XC::Bidirectional::Bidirectional(int tag)
  : SectionForceDeformation(tag, SEC_TAG_Bidirectional), E(0.0), sigY(0.0), Hiso(0.0), Hkin(0.0)
  {
    for(int i = 0; i < 2; i++)
      {
        eP_n[i]  = 0.0;
        eP_n1[i] = 0.0;
        q_n[i]  = 0.0;
        q_n1[i] = 0.0;
      }
    alpha_n  = 0.0;
    alpha_n1 = 0.0;
  }

//! @brief Set the value of the initial deformation.
int XC::Bidirectional::setInitialSectionDeformation(const Vector &e)
  {
    e_n1Inic[0] = e(0);
    e_n1Inic[1] = e(1);
    return 0;
  }

//! @brief Set the value of the trial deformation.
int XC::Bidirectional::setTrialSectionDeformation(const Vector &e)
  {
    e_n1Trial[0] = e(0);
    e_n1Trial[1] = e(1);
    return 0;
  }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::Bidirectional::getSectionTangent(void) const
  {
    static Vector def;
    def= getSectionDeformation();
    // Compute trial stress using elastic tangent
    s(0) = E*(def[0]-eP_n[0]);
    s(1) = E*(def[1]-eP_n[1]);

    static Vector xsi(2);

    // Predicted stress minus back stress
    xsi(0) = s(0) - q_n[0];
    xsi(1) = s(1) - q_n[1];

    double normxsi = xsi.Norm();

    // Current yield stress
    double sigY_n = sigY + alpha_n*Hiso;

    // Yield function
    double f_n1 = normxsi - sigY_n;

    // Elastic step
    if(f_n1 < 0.0)
      {
        ks(0,0) = ks(1,1) = E;
        ks(0,1) = ks(1,0) = 0.0;
      }
    // Plastic step
    else
      {
        // Consistency parameter
        double dlam = f_n1/(E+Hkin+Hiso);
        double n_n1[2];

        // Normal vector
        n_n1[0] = xsi(0)/normxsi;
        n_n1[1] = xsi(1)/normxsi;

        double A = E*(E/(Hiso+Hkin+E));
        double B = E*(E*dlam/normxsi);

        //ks(0,0) = E - A*n_n1[0]*n_n1[0] - B*(1.0 - n_n1[0]*n_n1[0]);
        //ks(1,1) = E - A*n_n1[1]*n_n1[1] - B*(1.0 - n_n1[1]*n_n1[1]);
        //ks(0,1) = -A*n_n1[0]*n_n1[1] - B*(-n_n1[0]*n_n1[1]);
        //ks(1,0) = ks(0,1);

        double EB = E-B;
        double BA = B-A;

        ks(0,0) = EB + BA*n_n1[0]*n_n1[0];
        ks(1,1) = EB + BA*n_n1[1]*n_n1[1];
        ks(0,1) = BA*n_n1[0]*n_n1[1];
        ks(1,0) = ks(0,1);

        //n_n1[0] *= dlam;
        //n_n1[1] *= dlam;

        // Update plastic strains
        //eP_n1[0] = eP_n[0] + n_n1[0];
        //eP_n1[1] = eP_n[1] + n_n1[1];

        // Update back stress
        //q_n1[0] = q_n[0] + Hkin*n_n1[0];
        //q_n1[1] = q_n[1] + Hkin*n_n1[1];

        // Update effective plastic strain
        //alpha_n1 = alpha_n + dlam;
      }
    return ks;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::Bidirectional::getInitialTangent(void) const
  {
    ks(0,0) = ks(1,1) = E;
    ks(0,1) = ks(1,0) = 0.0;
    return ks;
  }

//! @brief Returns the cross-section stress resultant.
const XC::Vector &XC::Bidirectional::getStressResultant(void) const
  {
    // Compute trial stress using elastic tangent
    static Vector def;
    def= getSectionDeformation();
    s(0) = E*(def[0]-eP_n[0]);
    s(1) = E*(def[1]-eP_n[1]);

    static XC::Vector xsi(2);

    // Predicted stress minus back stress
    xsi(0) = s(0) - q_n[0];
    xsi(1) = s(1) - q_n[1];

    double normxsi = xsi.Norm();

    // Current yield stress
    double sigY_n = sigY + alpha_n*Hiso;

    // Yield function
    double f_n1 = normxsi - sigY_n;

    // Elastic step
    if(f_n1 < 0.0)
      {
        // do nothing
      }
    // Plastic step
    else
      {
        // Consistency parameter
        double dlam = f_n1/(E+Hkin+Hiso);
        double n_n1[2];
        // Normal vector
        n_n1[0] = xsi(0)/normxsi;
        n_n1[1] = xsi(1)/normxsi;
        n_n1[0] *= dlam;
        n_n1[1] *= dlam;

        // Return stresses to yield surface
        s(0) -= E*n_n1[0];
        s(1) -= E*n_n1[1];

        // Update plastic strains
        eP_n1[0] = eP_n[0] + n_n1[0];
        eP_n1[1] = eP_n[1] + n_n1[1];

        // Update back stress
        q_n1[0] = q_n[0] + Hkin*n_n1[0];
        q_n1[1] = q_n[1] + Hkin*n_n1[1];

        // Update effective plastic strain
        alpha_n1 = alpha_n + dlam;
      }
    return s;
  }

//! @brief Returns the initial generalized strains.
void XC::Bidirectional::zeroInitialSectionDeformation(void)
  {
    // Write to static variable for return
    e_n1Inic[0]= 0.0;
    e_n1Inic[1]= 0.0;
  }

//! @brief Returns the initial generalized strains.
const XC::Vector &XC::Bidirectional::getInitialSectionDeformation(void) const
  {
    // Write to static variable for return
    s(0) = e_n1Inic[0];
    s(1) = e_n1Inic[1];
    return s;
  }

//! @brief Returns material's trial generalized deformation.
const XC::Vector &XC::Bidirectional::getSectionDeformation(void) const
  {
    // Write to static variable for return
    s(0) = e_n1Trial[0]-e_n1Inic[0];
    s(1) = e_n1Trial[1]-e_n1Inic[1];
    return s;
  }

//! @brief Returns strain at the position being passed as parameter.
double XC::Bidirectional::getStrain(const double &,const double &) const
  {
    std::cerr << "getStrain not implemented in class: "
              << getClassName() << std::endl;
    return 0.0;
  }

int XC::Bidirectional::commitState(void)
  {
        eP_n[0] = eP_n1[0];
        eP_n[1] = eP_n1[1];

        q_n[0] = q_n1[0];
        q_n[1] = q_n1[1];

        alpha_n = alpha_n1;

        return 0;
  }

//! @brief Revert to the last committed state.
int XC::Bidirectional::revertToLastCommit(void)
  { return 0; }

//! @brief Revert to the last committed state.
int XC::Bidirectional::revertToStart(void)
  {
    for (int i = 0; i < 2; i++)
      {
	eP_n[i]  = 0.0;
	eP_n1[i] = 0.0;
	q_n[i]  = 0.0;
	q_n1[i] = 0.0;
      }

    alpha_n  = 0.0;
    alpha_n1 = 0.0;
    return 0;
  }

//! @brief Virtual constructor.
XC::SectionForceDeformation* XC::Bidirectional::getCopy(void) const
  {
    Bidirectional *theCopy = new Bidirectional (this->getTag(), E, sigY, Hiso, Hkin);

    for(int i = 0; i < 2; i++)
      {
        theCopy->eP_n[i]  = eP_n[i];
        theCopy->eP_n1[i] = eP_n1[i];
        theCopy->q_n[i]  = q_n[i];
        theCopy->q_n1[i] = q_n1[i];
      }

    theCopy->alpha_n  = alpha_n;
    theCopy->alpha_n1 = alpha_n1;

    return theCopy;
  }

//! @brief Get material type.
const XC::ResponseId &XC::Bidirectional::getType(void) const
  { return RespBidirectional; }

int XC::Bidirectional::getOrder(void) const
  { return 2; }

//! @brief Send object members through the channel being passed as parameter.
int XC::Bidirectional::sendData(CommParameters &cp)
  {
    int res= SectionForceDeformation::sendData(cp);
    res+= cp.sendDoubles(E,sigY,Hiso,Hkin,getDbTagData(),CommMetaData(5));
    res+= cp.sendDoubles(e_n1Trial[0],e_n1Trial[1],e_n1Inic[0],e_n1Inic[1],getDbTagData(),CommMetaData(6));
    res+= cp.sendDoubles(eP_n[0],eP_n[1],eP_n1[0],eP_n1[1],getDbTagData(),CommMetaData(7));
    res+= cp.sendDoubles(q_n[0],q_n[1],q_n1[0],q_n1[1],alpha_n,alpha_n1,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Bidirectional::recvData(const CommParameters &cp)
  {
    int res= SectionForceDeformation::recvData(cp);
    res+= cp.receiveDoubles(E,sigY,Hiso,Hkin,getDbTagData(),CommMetaData(5));
    res+= cp.receiveDoubles(e_n1Trial[0],e_n1Trial[1],e_n1Inic[0],e_n1Inic[1],getDbTagData(),CommMetaData(6));
    res+= cp.receiveDoubles(eP_n[0],eP_n[1],eP_n1[0],eP_n1[1],getDbTagData(),CommMetaData(7));
    res+= cp.receiveDoubles(q_n[0],q_n[1],q_n1[0],q_n1[1],alpha_n,alpha_n1,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Send object through the communicator argument.
int XC::Bidirectional::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; failed to send data.\n";
    return res;
  }

//! @brief Receive object through the communicator argument.
int XC::Bidirectional::recvSelf(const CommParameters &cp)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

void XC::Bidirectional::Print(std::ostream &s, int flag) const
  {
    s << "Bidirectional, tag: " << this->getTag() << std::endl;
    s << "\tE:    " << E << std::endl;
    s << "\tsigY: " << sigY<< std::endl;
    s << "\tHiso: " << Hiso << std::endl;
    s << "\tHkin: " << Hkin << std::endl;
  }
