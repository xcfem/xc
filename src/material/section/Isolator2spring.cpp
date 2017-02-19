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

// $Revision: 1.1 $
// $Date: 2006/01/17 20:44:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/Isolator2spring.cpp,v $

// Written: K. Ryan
// Created: September 2003
// Updates: November 2005
//
// Description: This file contains the class implementation for a "two-spring isolator"
// material.  This material is based on the two-spring model originally developed by
// Koh and Kelly to represent the buckling behavior of an elastomeric bearing.  The
// material model has been modified to include material nonlinearity and optional
// strength degradation.

#include <material/section/Isolator2spring.h>
#include <cmath>
#include "material/section/ResponseId.h"

XC::Vector XC::Isolator2spring::s(2);
XC::Vector XC::Isolator2spring::s3(3);
XC::Vector XC::Isolator2spring::f0(5);
XC::Matrix XC::Isolator2spring::df(5,5);

XC::Isolator2spring::Isolator2spring(int tag, double tol_in, double k1_in, double Fy_in, double kb_in, double kvo_in, double hb_in, double Pe_in,  double po_in)
  : SectionForceDeformation(tag, SEC_TAG_Isolator2spring), tol(tol_in), k1(k1_in), Fyo(Fy_in), kbo(kb_in), kvo(kvo_in), h(hb_in), Pe(Pe_in), po(po_in), x0(5), ks(3,3)
  {
    this->revertToStart();
    pcr= sqrt(Pe*kbo*h);
    H= k1*kbo/(k1 - kbo);
  }

XC::Isolator2spring::Isolator2spring(int tag)
  :SectionForceDeformation(tag, SEC_TAG_Isolator2spring), tol(1.0e-12), k1(0.0), Fyo(0.0), kbo(0.0), kvo(0.0), h(0.0), Pe(0.0), po(0.0)
  {
    this->revertToStart();
    pcr= sqrt(Pe*kbo*h);
    H= k1*kbo/(k1 - kbo);
  }

XC::Isolator2spring::Isolator2spring(void)
  :SectionForceDeformation(0, SEC_TAG_Isolator2spring), tol(1.0e-12), k1(0.0), Fyo(0.0), kbo(0.0), kvo(0.0), h(0.0), Pe(0.0), po(0.0)
  {
    this->revertToStart();
    pcr= sqrt(Pe*kbo*h);
    H= k1*kbo/(k1 - kbo);
  }

int XC::Isolator2spring::setInitialSectionDeformation(const Vector &e)
  {
    utptInic[0]= e(0);
    utptInic[1]= e(1);
    return 0;
  }

int XC::Isolator2spring::setTrialSectionDeformation(const Vector &e)
  {
    utptTrial[0]= e(0);
    utptTrial[1]= e(1);
    return 0;
  }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::Isolator2spring::getSectionTangent(void) const
  {
    // ks is computed in getStressResultant
    return ks;
  }

//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix &XC::Isolator2spring::getInitialTangent(void) const
  {
    // Intial tangent uses nominal properties of the isolator.
    ks(0,0)= k1;
    ks(1,1)= kvo;
    ks(0,1)= ks(1,0)= 0.0;
    ks(0,2)= ks(1,2)= ks(2,2)= ks(2,1)= ks(2,0)= 0.0;
    return ks;
  }

//! @brief Return the resultante de tensiones.
const XC::Vector &XC::Isolator2spring::getStressResultant(void) const
  {
    double Fy;
    if(po < 1.0e-10)
      {
        // No strength degradation
        Fy= Fyo;
      }
    else
      {
        // Strength degradation based on bearing axial load
        double p2= x0(1)/po;
        if(p2<0)
          { p2= 0.0; }
        Fy= Fyo*(1-exp(-p2));
      }
    // Material stresses using rate independent plasticity, return mapping algorithm

    // Compute trial stress using elastic tangent
    double fb_try= k1*(x0(2)-sP_n);
    double xi_try= fb_try - q_n;

    // Yield function
    double Phi_try= fabs(xi_try) - Fy;

    double fspr;
    double dfsds;
    double del_gam;
    int sign;

    // Elastic step
    if(Phi_try <= 0.0)
      {
        // Stress and tangent, update plastic deformation and back stress
        fspr= fb_try;
        dfsds= k1;
        sP_n1= sP_n;
        q_n1= q_n;
      }
    // Plastic step
    else
      {
        // Consistency parameter
        del_gam= Phi_try/(k1+H);

        sign= (xi_try < 0) ? -1 : 1;
        // Return stress to yield surface
        fspr= fb_try - del_gam*k1*sign;
        dfsds= kbo;
        // Update plastic deformation and back stress
        sP_n1= sP_n + del_gam*sign;
        q_n1= q_n + del_gam*H*sign;
      }

    // Nonlinear equilibrium and kinematic equations; want to find the
    // zeros of these equations.
    f0(0)= x0(0) - fspr + x0(1)*x0(3);
    f0(1)= x0(0)*h - Pe*h*x0(3) + x0(1)*(x0(2)+h*x0(3));
    f0(2)= x0(1) - kvo*x0(4);
    f0(3)= getSectionDeformation()[0] - x0(2) - h*x0(3);
    f0(4)= -getSectionDeformation()[1] - x0(2)*x0(3) - h/2.0*x0(3)*x0(3) - x0(4);

    int iter= 0;
    double normf0= f0.Norm();
    static XC::Matrix dfinverse(5,5);

    // Solve nonlinear equations using Newton's method
    while(normf0 > tol)
      {
        iter += 1;

        // Formulate Jacobian of nonlinear equations
        df(0,0)= 1.0;
        df(0,1)= x0(3);
        df(0,2)= -dfsds;
        df(0,3)= x0(1);
        df(0,4)= 0.0;

        df(1,0)= h;
        df(1,1)= x0(2) + h*x0(3);
        df(1,2)= x0(1);
        df(1,3)= (x0(1) - Pe)*h;
        df(1,4)= 0.0;

        df(2,0)= 0.0;
        df(2,1)= 1.0;
        df(2,2)= 0.0;
        df(2,3)= 0.0;
        df(2,4)= -kvo;

        df(3,0)= 0.0;
        df(3,1)= 0.0;
        df(3,2)= -1.0;
        df(3,3)= -h;
        df(3,4)= 0.0;

        df(4,0)= 0.0;
        df(4,1)= 0.0;
        df(4,2)= -x0(3);
        df(4,3)= -(x0(2) + h*x0(3));
        df(4,4)= -1.0;

        df.Invert(dfinverse);
        // Compute improved estimate of solution x0
        x0 -= dfinverse*f0;

        if(po > 1.0e-10)
          { // Update strength according to axial load
            double p2= x0(1)/po;
            if(p2<0)
             { p2= 0.0; }
            Fy= Fyo*(1-exp(-p2));
          }

        // Apply plasticity theory again, return mapping algorithm
        fb_try= k1*(x0(2) - sP_n);
        xi_try= fb_try - q_n;

        Phi_try= fabs(xi_try) - Fy;
        // Elastic step
        if(Phi_try <= 0.0)
          {
            fspr= fb_try;
            dfsds= k1;
            sP_n1= sP_n;
            q_n1= q_n;
          }
        // Plastic step
        else
          {
            del_gam= Phi_try/(k1+H);
            sign= (xi_try < 0) ? -1 : 1;
            fspr= fb_try - del_gam*k1*sign;
            dfsds= kbo;
            sP_n1= sP_n + del_gam*sign;
            q_n1= q_n + del_gam*H*sign;
          }

        // Estimate the residual
        f0(0)= x0(0) - fspr + x0(1)*x0(3);
        f0(1)= x0(0)*h - Pe*h*x0(3) + x0(1)*(x0(2)+h*x0(3));
        f0(2)= x0(1) - kvo*x0(4);
        f0(3)= getSectionDeformation()[0] - x0(2) - h*x0(3);
        f0(4)= -getSectionDeformation()[1] - x0(2)*x0(3) - h/2.0*x0(3)*x0(3) - x0(4);

        normf0= f0.Norm();
        if(iter > 19)
          {
              std::cerr << "WARNING! Iso2spring: Newton iteration failed. Norm Resid: " << normf0  << std::endl;
              break;
          }
      }
    // Compute stiffness matrix by three step process
    const double denom= h*dfsds*(Pe - x0(1)) - x0(1)*x0(1);
    static Matrix fkin(3,2);
    fkin(0,0)= 1.0;
    fkin(1,0)= h;
    fkin(2,0)= 0.0;
    fkin(0,1)= -x0(3);
    fkin(1,1)= -(x0(2) + h*x0(3));
    fkin(2,1)= -1.0;

    static Matrix feq(3,3);
    feq(0,0)= (Pe-x0(1))*h/denom;
    feq(0,1)= feq(1,0)= x0(1)/denom;
    feq(1,1)= dfsds/denom;
    feq(0,2)= feq(1,2)= feq(2,0)= feq(2,1)= 0.0;
    feq(2,2)= 1.0/kvo;

    static Matrix ftot(2,2);
    static Matrix ktot(2,2);
    ftot.Zero();
    ftot.addMatrixTripleProduct(0.0,fkin,feq,1.0);
    ftot.Invert(ktot);

    ks(0,0)= ktot(0,0);
    ks(1,0)= ktot(1,0);
    ks(0,1)= ktot(0,1);
    ks(1,1)= ktot(1,1);
    ks(0,2)= ks(1,2)= ks(2,2)= ks(2,1)= ks(2,0)= 0.0;


    // Compute force vector
    s3(0)= x0(0);
    s3(1)= -x0(1);
    s3(2)= (x0(1)*getSectionDeformation()[0] + x0(0)*h)/2.0;
    return s3;
  }

//! @brief Return the initial deformation del material.
const XC::Vector &XC::Isolator2spring::getInitialSectionDeformation(void) const
  {
    // Write to static variable for return
    s(0)= utptInic[0];
    s(1)= utptInic[1];
    return s;
  }

//! @brief Returns material's trial deformation.
const XC::Vector &XC::Isolator2spring::getSectionDeformation(void) const
  {
    // Write to static variable for return
    s(0)= utptTrial[0]-utptInic[0];
    s(1)= utptTrial[1]-utptInic[1];
    return s;
  }

//! @brief Returns strain at position being passed as parameter.
double XC::Isolator2spring::getStrain(const double &,const double &) const
  {
    std::cerr << "getStrain not implemented for class: "
              << nombre_clase() << std::endl;
    return 0.0;
  }

int XC::Isolator2spring::commitState(void)
  {
    sP_n= sP_n1;
    q_n= q_n1;
    return 0;
  }

int XC::Isolator2spring::revertToLastCommit(void)
  { return 0; }

int XC::Isolator2spring::revertToStart(void)
  {
    for(int i= 0; i < 2; i++)
      { utptInic[i]= 0.0; }
    for(int i= 0; i < 2; i++)
      { utptTrial[i]= 0.0; }
    sP_n= 0.0;
    sP_n1= 0.0;
    q_n= 0.0;
    q_n1= 0.0;
    x0.Zero();
    ks.Zero();
    return 0;
  }

XC::SectionForceDeformation *XC::Isolator2spring::getCopy(void) const
  { return new XC::Isolator2spring(*this); }

const XC::ResponseId &XC::Isolator2spring::getType(void) const
  { return RespIsolator2spring; }

int XC::Isolator2spring::getOrder(void) const
  { return 3; }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::Isolator2spring::sendData(CommParameters &cp)
  {
    int res= SectionForceDeformation::sendData(cp);
    res+= cp.sendDoubles(tol,k1,Fyo,kbo,kvo,h,getDbTagData(),CommMetaData(5));
    res+= cp.sendDoubles(Pe,po,utptTrial[0],utptTrial[1],utptInic[0],utptInic[1],getDbTagData(),CommMetaData(6));
    res+= cp.sendDoubles(sP_n,sP_n1,q_n,q_n1,H,pcr,getDbTagData(),CommMetaData(7));
    res+= cp.sendVector(x0,getDbTagData(),CommMetaData(8));
    res+= cp.sendMatrix(ks,getDbTagData(),CommMetaData(9));
    res+= cp.sendVector(f0,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::Isolator2spring::recvData(const CommParameters &cp)
  {
    int res= SectionForceDeformation::recvData(cp);
    res+= cp.receiveDoubles(tol,k1,Fyo,kbo,kvo,h,getDbTagData(),CommMetaData(5));
    res+= cp.receiveDoubles(Pe,po,utptTrial[0],utptTrial[1],utptInic[0],utptInic[1],getDbTagData(),CommMetaData(6));
    res+= cp.receiveDoubles(sP_n,sP_n1,q_n,q_n1,H,pcr,getDbTagData(),CommMetaData(7));
    res+= cp.receiveVector(x0,getDbTagData(),CommMetaData(8));
    res+= cp.receiveMatrix(ks,getDbTagData(),CommMetaData(9));
    res+= cp.receiveVector(f0,getDbTagData(),CommMetaData(10));
    return res;
  }

int XC::Isolator2spring::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(11);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::Isolator2spring::recvSelf(const CommParameters &cp)
  {
    inicComm(11);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::Isolator2spring::Print(std::ostream &s, int flag)
  {
    s << "Isolator2spring, tag: " << this->getTag() << std::endl;
    s << "\tol:    " << tol << std::endl;
    s << "\tk1:    " << k1 << std::endl;
    s << "\tFy:    " << Fyo << std::endl;
    s << "\tk2:    " << kbo << std::endl;
    s << "\tkv:    " << kvo << std::endl;
    s << "\th:     " << h << std::endl;
    s << "\tPe:    " << Pe << std::endl;
    s << "\tPo:    " << po << std::endl;
  }
