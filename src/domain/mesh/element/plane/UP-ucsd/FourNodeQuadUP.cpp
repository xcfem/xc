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
///////////////////////////////////////////////////////////////////////////////
// Description: This file contains the class definition for XC::FourNodeQuadUP.  //
// FourNodeQuadUP is a 4-node plane strain element for solid-fluid fully     //
// coupled analysis. This implementation is a simplified u-p formulation     //
// of Biot theory (u - solid displacement, p - fluid pressure). Each element //
// node has two DOFs for u and 1 DOF for p.                                  //
//                                                                             //
// Written by Zhaohui Yang        (May 2002)                                     //
// based on XC::FourNodeQuad element by Michael Scott                               //
///////////////////////////////////////////////////////////////////////////////

// $Revision: 1.1 $
// $Date: 2005/09/22 21:28:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/UP-ucsd/FourNodeQuadUP.cpp,v $

#include <domain/mesh/element/plane/UP-ucsd/FourNodeQuadUP.h>
#include <domain/mesh/node/Node.h>
#include <material/nD/NDMaterial.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>
#include <cstring>
#include <domain/mesh/element/utils/Information.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>
#include "domain/component/Parameter.h"

 XC::Matrix XC::FourNodeQuadUP::K(12,12);
 XC::Vector XC::FourNodeQuadUP::P(12);
double XC::FourNodeQuadUP::shp[3][4][4];
double XC::FourNodeQuadUP::pts[4][2];
double XC::FourNodeQuadUP::wts[4];
double XC::FourNodeQuadUP::dvol[4];
double XC::FourNodeQuadUP::shpBar[3][4];

XC::FourNodeQuadUP::FourNodeQuadUP(int tag, int nd1, int nd2, int nd3, int nd4,
                                   NDMaterial &m, const std::string &type, double t, double bulk, double r,
                  double p1, double p2, const BodyForces2D &bForces, double p)
  :QuadBase4N<SolidMech2D>(tag, ELE_TAG_FourNodeQuadUP,nd1,nd2,nd3,nd4,SolidMech2D(4,m,type,t,r)), bf(bForces), pressureLoad(12), kc(bulk), pressure(p), Ki(nullptr)
  {
    load.reset(12);

    pts[0][0] = -0.5773502691896258;
    pts[0][1] = -0.5773502691896258;
    pts[1][0] =  0.5773502691896258;
    pts[1][1] = -0.5773502691896258;
    pts[2][0] =  0.5773502691896258;
    pts[2][1] =  0.5773502691896258;
    pts[3][0] = -0.5773502691896258;
    pts[3][1] =  0.5773502691896258;

    wts[0] = 1.0;
    wts[1] = 1.0;
    wts[2] = 1.0;
    wts[3] = 1.0;

    // Permeabilities
    perm[0] = p1;
    perm[1] = p2;
  }

XC::FourNodeQuadUP::FourNodeQuadUP(void)
  :QuadBase4N<SolidMech2D>(ELE_TAG_FourNodeQuadUP,SolidMech2D(4,nullptr,1.0,0.0)),
  pressureLoad(12), kc(0.0), pressure(0.0), Ki(0)
  {
     load.reset(12);
     pts[0][0] = -0.577350269189626;
     pts[0][1] = -0.577350269189626;
     pts[1][0] =  0.577350269189626;
     pts[1][1] = -0.577350269189626;
     pts[2][0] =  0.577350269189626;
     pts[2][1] =  0.577350269189626;
     pts[3][0] = -0.577350269189626;
     pts[3][1] =  0.577350269189626;

     wts[0] = 1.0;
     wts[1] = 1.0;
     wts[2] = 1.0;
     wts[3] = 1.0;
  }

//! @brief Constructor virtual.
XC::Element* XC::FourNodeQuadUP::getCopy(void) const
  { return new XC::FourNodeQuadUP(*this); }

XC::FourNodeQuadUP::~FourNodeQuadUP(void)
  {
    if(Ki) delete Ki;
  }

XC::Node *XC::FourNodeQuadUP::nd1Ptr(void)
  { return theNodes[0]; }
const XC::Node *XC::FourNodeQuadUP::nd1Ptr(void) const
  { return theNodes[0]; }
XC::Node *XC::FourNodeQuadUP::nd2Ptr(void)
  { return theNodes[1]; }
const XC::Node *XC::FourNodeQuadUP::nd2Ptr(void) const
  { return theNodes[1]; }
XC::Node *XC::FourNodeQuadUP::nd3Ptr(void)
  { return theNodes[2]; }
const XC::Node *XC::FourNodeQuadUP::nd3Ptr(void) const
  { return theNodes[2]; }
XC::Node *XC::FourNodeQuadUP::nd4Ptr(void)
  { return theNodes[3]; }
const XC::Node *XC::FourNodeQuadUP::nd4Ptr(void) const
  { return theNodes[3]; }

int XC::FourNodeQuadUP::getNumDOF(void) const
  { return 12; }

void XC::FourNodeQuadUP::setDomain(Domain *theDomain)
  {
    QuadBase4N<SolidMech2D>::setDomain(theDomain);
    theNodes.checkNumDOF(3,getTag());
    // Compute consistent nodal loads due to pressure
    this->setPressureLoadAtNodes();
  }



int XC::FourNodeQuadUP::update(void)
  {
        const Vector &disp1 = nd1Ptr()->getTrialDisp();
        const Vector &disp2 = nd2Ptr()->getTrialDisp();
        const Vector &disp3 = nd3Ptr()->getTrialDisp();
        const Vector &disp4 = nd4Ptr()->getTrialDisp();

        static double u[2][4];

        u[0][0] = disp1(0);
        u[1][0] = disp1(1);
        u[0][1] = disp2(0);
        u[1][1] = disp2(1);
        u[0][2] = disp3(0);
        u[1][2] = disp3(1);
        u[0][3] = disp4(0);
        u[1][3] = disp4(1);

        static Vector eps(3);

        int ret = 0;

        // Determine Jacobian for this integration point
        this->shapeFunction();

        // Loop over the integration points
        for(int i = 0; i < 4; i++) {

                // Interpolate strains
                //eps = B*u;
                //eps.addMatrixVector(0.0, B, u, 1.0);
                eps.Zero();
                for(int beta = 0; beta < 4; beta++) {
                        eps(0) += shp[0][beta][i]*u[0][beta];
                        eps(1) += shp[1][beta][i]*u[1][beta];
                        eps(2) += shp[0][beta][i]*u[1][beta] + shp[1][beta][i]*u[0][beta];
                }

                // Set the material strain
                ret += physicalProperties[i]->setTrialStrain(eps);
        }

        return ret;
  }


const XC::Matrix &XC::FourNodeQuadUP::getTangentStiff(void) const
  {

  K.Zero();

  double DB[3][2];

  // Determine Jacobian for this integration point
  this->shapeFunction();

  // Loop over the integration points
  for(int i = 0; i < 4; i++) {

    // Get the material tangent
    const XC::Matrix &D = physicalProperties[i]->getTangent();

    // Perform numerical integration
    //K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
    //K.addMatrixTripleProduct(1.0, B, D, intWt(i)*intWt(j)*detJ);
    for(int alpha = 0, ia = 0; alpha < 4; alpha++, ia += 3) {

      for(int beta = 0, ib = 0; beta < 4; beta++, ib += 3) {

        DB[0][0] = dvol[i] * (D(0,0)*shp[0][beta][i] + D(0,2)*shp[1][beta][i]);
        DB[1][0] = dvol[i] * (D(1,0)*shp[0][beta][i] + D(1,2)*shp[1][beta][i]);
        DB[2][0] = dvol[i] * (D(2,0)*shp[0][beta][i] + D(2,2)*shp[1][beta][i]);
        DB[0][1] = dvol[i] * (D(0,1)*shp[1][beta][i] + D(0,2)*shp[0][beta][i]);
        DB[1][1] = dvol[i] * (D(1,1)*shp[1][beta][i] + D(1,2)*shp[0][beta][i]);
        DB[2][1] = dvol[i] * (D(2,1)*shp[1][beta][i] + D(2,2)*shp[0][beta][i]);

        K(ia,ib) += shp[0][alpha][i]*DB[0][0] + shp[1][alpha][i]*DB[2][0];
        K(ia,ib+1) += shp[0][alpha][i]*DB[0][1] + shp[1][alpha][i]*DB[2][1];
        K(ia+1,ib) += shp[1][alpha][i]*DB[1][0] + shp[0][alpha][i]*DB[2][0];
        K(ia+1,ib+1) += shp[1][alpha][i]*DB[1][1] + shp[0][alpha][i]*DB[2][1];

      }
    }
  }
    if(isDead())
      K*=dead_srf;
    return K;
  }


const XC::Matrix &XC::FourNodeQuadUP::getInitialStiff(void) const
  {
    if(!Ki)
      {

  K.Zero();

  double DB[3][2];

  // Determine Jacobian for this integration point
  this->shapeFunction();

  // Loop over the integration points
  for(int i = 0; i < 4; i++) {

    // Get the material tangent
    const XC::Matrix &D = physicalProperties[i]->getInitialTangent();

    // Perform numerical integration
    //K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
    //K.addMatrixTripleProduct(1.0, B, D, intWt(i)*intWt(j)*detJ);
    for(int alpha = 0, ia = 0; alpha < 4; alpha++, ia += 3) {

      for(int beta = 0, ib = 0; beta < 4; beta++, ib += 3) {

        DB[0][0] = dvol[i] * (D(0,0)*shp[0][beta][i] + D(0,2)*shp[1][beta][i]);
        DB[1][0] = dvol[i] * (D(1,0)*shp[0][beta][i] + D(1,2)*shp[1][beta][i]);
        DB[2][0] = dvol[i] * (D(2,0)*shp[0][beta][i] + D(2,2)*shp[1][beta][i]);
        DB[0][1] = dvol[i] * (D(0,1)*shp[1][beta][i] + D(0,2)*shp[0][beta][i]);
        DB[1][1] = dvol[i] * (D(1,1)*shp[1][beta][i] + D(1,2)*shp[0][beta][i]);
        DB[2][1] = dvol[i] * (D(2,1)*shp[1][beta][i] + D(2,2)*shp[0][beta][i]);

        K(ia,ib) += shp[0][alpha][i]*DB[0][0] + shp[1][alpha][i]*DB[2][0];
        K(ia,ib+1) += shp[0][alpha][i]*DB[0][1] + shp[1][alpha][i]*DB[2][1];
        K(ia+1,ib) += shp[1][alpha][i]*DB[1][0] + shp[0][alpha][i]*DB[2][0];
        K(ia+1,ib+1) += shp[1][alpha][i]*DB[1][1] + shp[0][alpha][i]*DB[2][1];

      }
    }
  }

  Ki = new XC::Matrix(K);
      }
  if(Ki == 0) {
    std::cerr << "FATAL XC::FourNodeQuadUP::getInitialStiff() -";
    std::cerr << "ran out of memory\n";
    exit(-1);
  }
    if(isDead())
      (*Ki)*=dead_srf;
    return *Ki;
  }

const XC::Matrix &XC::FourNodeQuadUP::getDamp(void) const
{
  static XC::Matrix Kdamp(12,12);
  Kdamp.Zero();

  if(rayFactors.getBetaK() != 0.0)
    Kdamp.addMatrix(1.0, this->getTangentStiff(), rayFactors.getBetaK());
  if(rayFactors.getBetaK0() != 0.0)
    Kdamp.addMatrix(1.0, this->getInitialStiff(), rayFactors.getBetaK0());
  if(rayFactors.getBetaKc() != 0.0)
    Kdamp.addMatrix(1.0, Kc, rayFactors.getBetaKc());

  int i, j, m, i1, j1;

  if(rayFactors.getAlphaM() != 0.0) {
        this->getMass();
    for(i = 0; i < 12; i += 3) {
      for(j = 0; j < 12; j += 3) {
        Kdamp(i,j) += K(i,j)*rayFactors.getAlphaM();
        Kdamp(i+1,j+1) += K(i+1,j+1)*rayFactors.getAlphaM();
          }
    }
  }

  // Determine Jacobian for this integration point
  this->shapeFunction();

  // Compute coupling matrix
  //const double vol = dvol[0] + dvol[1] + dvol[2] + dvol[3];
  for(i = 0; i < 12; i += 3) {
    i1 = i / 3;
    for(j = 2; j < 12; j += 3) {
      j1 = (j-2) / 3;
      //K(i,j) += -vol*shpBar[0][i1]*shpBar[2][j1];
      //K(i+1,j) += -vol*shpBar[1][i1]*shpBar[2][j1];
      for(m = 0; m < 4; m++) {
            Kdamp(i,j) += -dvol[m]*shp[0][i1][m]*shp[2][j1][m];
            Kdamp(i+1,j) += -dvol[m]*shp[1][i1][m]*shp[2][j1][m];
          }
      Kdamp(j,i) = Kdamp(i,j);
      Kdamp(j,i+1) = Kdamp(i+1,j);
    }
  }

  // Compute permeability matrix
  for(i = 2; i < 12; i += 3) {
    int i1 = (i-2) / 3;
    for(j = 2; j < 12; j += 3) {
      int j1 = (j-2) / 3;
      //K(i,j) = - (vol*perm[0]*shpBar[0][i1]*shpBar[0][j1] +
                //  vol*perm[1]*shpBar[1][i1]*shpBar[1][j1]);
      for(m = 0; m < 4; m++) {
            Kdamp(i,j) += - dvol[m]*(perm[0]*shp[0][i1][m]*shp[0][j1][m] +
                          perm[1]*shp[1][i1][m]*shp[1][j1][m]);
          }
    }
  }

    K = Kdamp;
    if(isDead())
      K*=dead_srf;
    return K;
  }

const XC::Matrix &XC::FourNodeQuadUP::getMass(void) const
{
  K.Zero();

  int i, j, m, i1, j1;
  double Nrho;

  // Determine Jacobian for this integration point
  this->shapeFunction();


  // Compute an ad hoc lumped mass matrix
  /*for(i = 0; i < 4; i++) {

    // average material density
    tmp = mixtureRho(i);

    for(int alpha = 0, ia = 0; alpha < 4; alpha++, ia += 3) {
      Nrho = shp[2][alpha][i]*dvol[i]*tmp;
      K(ia,ia) += Nrho;
      K(ia+1,ia+1) += Nrho;
    }
  }*/

    // Compute consistent mass matrix
  for(i = 0, i1 = 0; i < 12; i += 3, i1++) {
    for(j = 0, j1 = 0; j < 12; j += 3, j1++) {
    for(m = 0; m < 4; m++) {
    Nrho = dvol[m]*mixtureRho(m)*shp[2][i1][m]*shp[2][j1][m];
    K(i,j) += Nrho;
    K(i+1,j+1) += Nrho;
    }
    }
  }

  // Compute compressibility matrix
  //const double vol = dvol[0] + dvol[1] + dvol[2] + dvol[3];
  const double oneOverKc = 1./kc;

  for(i = 2; i < 12; i += 3) {
    i1 = (i-2) / 3;
    for(j = 2; j < 12; j += 3) {
      j1 = (j-2) / 3;
      //K(i,j) = -vol*oneOverKc*shpBar[2][i1]*shpBar[2][j1];
      for(m = 0; m < 4; m++) {
            K(i,j) += -dvol[m]*oneOverKc*shp[2][i1][m]*shp[2][j1][m];
          }
    }
  }

  /*for(i = 2; i < 12; i += 3) {
    i1 = (i-2) / 3;
    K(i,i) = -vol*oneOverKc*shpBar[2][i1];
  }*/
    if(isDead())
      K*=dead_srf;
  return K;
}

int XC::FourNodeQuadUP::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::FourNodeQuadUP::addLoad - load type unknown for ele with tag: " << this->getTag() << "\n";
    return -1;
  }


int XC::FourNodeQuadUP::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    // accel = uDotDotG (see XC::EarthquakePattern.cpp)
    // Get R * accel from the nodes
    const Vector &Raccel1 = nd1Ptr()->getRV(accel);
    const Vector &Raccel2 = nd2Ptr()->getRV(accel);
    const Vector &Raccel3 = nd3Ptr()->getRV(accel);
    const Vector &Raccel4 = nd4Ptr()->getRV(accel);

    if(3 != Raccel1.Size() || 3 != Raccel2.Size() || 3 != Raccel3.Size() || 3 != Raccel4.Size())
      {
        std::cerr << "XC::FourNodeQuadUP::addInertiaLoadToUnbalance matrix and vector sizes are incompatable\n";
        return -1;
      }
  double ra[12];

  ra[0] = Raccel1(0);
  ra[1] = Raccel1(1);
  ra[2] = 0.;
  ra[3] = Raccel2(0);
  ra[4] = Raccel2(1);
  ra[5] = 0.;
  ra[6] = Raccel3(0);
  ra[7] = Raccel3(1);
  ra[8] = 0.;
  ra[9] = Raccel4(0);
  ra[10] = Raccel4(1);
  ra[11] = 0.;

  // Compute mass matrix
  this->getMass();

  // Want to add ( - fact * M R * accel ) to unbalance
  int i, j;

  for(i = 0; i < 12; i++) {
    for(j = 0; j < 12; j++)
      load(i) += -K(i,j)*ra[j];
  }

  return 0;
}

const XC::Vector &XC::FourNodeQuadUP::getResistingForce(void) const
  {
    P.Zero();

    // Determine Jacobian for this integration point
    this->shapeFunction();
    //const double vol = dvol[0] + dvol[1] + dvol[2] + dvol[3];

    int i;
    // Loop over the integration points
    for(i = 0; i < 4; i++)
      {

        // Get material stress response
        const Vector &sigma = physicalProperties[i]->getStress();

        // Perform numerical integration on internal force
        //P = P + (B^ sigma) * intWt(i)*intWt(j) * detJ;
        //P.addMatrixTransposeVector(1.0, B, sigma, intWt(i)*intWt(j)*detJ);
        for(int alpha = 0, ia = 0; alpha < 4; alpha++, ia += 3) {

      P(ia) += dvol[i]*(shp[0][alpha][i]*sigma(0) + shp[1][alpha][i]*sigma(2));

      P(ia+1) += dvol[i]*(shp[1][alpha][i]*sigma(1) + shp[0][alpha][i]*sigma(2));

      // Subtract equiv. body forces from the nodes
      //P = P - (N^ b) * intWt(i)*intWt(j) * detJ;
      //P.addMatrixTransposeVector(1.0, N, b, -intWt(i)*intWt(j)*detJ);

      double r = mixtureRho(i);
      P(ia) -= dvol[i]*(shp[2][alpha][i]*r*bf[0]);
      P(ia+1) -= dvol[i]*(shp[2][alpha][i]*r*bf[1]);
    }
  }

  // Subtract fluid body force
  for(int alpha = 0, ia = 0; alpha < 4; alpha++, ia += 3)
    {
      //P(ia+2) += vol*rho*(perm[0]*bf[0]*shpBar[0][alpha]
        //                +perm[1]*bf[1]*shpBar[1][alpha]);
      for(i = 0; i < 4; i++)
        {
          P(ia+2) += dvol[i]*physicalProperties.getRho()*(perm[0]*bf[0]*shp[0][alpha][i] +
          perm[1]*bf[1]*shp[1][alpha][i]);
        }
    }

  // Subtract pressure loading from resisting force
  if(pressure != 0.0) {
    //P = P + pressureLoad;
    P.addVector(1.0, pressureLoad, -1.0);
  }

    // Subtract other external nodal loads ... P_res = P_int - P_ext
    //P = P - load;
    P.addVector(1.0, load, -1.0);
    if(isDead())
      P*=dead_srf;
    return P;
  }

const XC::Vector &XC::FourNodeQuadUP::getResistingForceIncInertia(void) const
  {
    int i, j;

    const Vector &accel1 = nd1Ptr()->getTrialAccel();
    const Vector &accel2 = nd2Ptr()->getTrialAccel();
    const Vector &accel3 = nd3Ptr()->getTrialAccel();
    const Vector &accel4 = nd4Ptr()->getTrialAccel();

    static double a[12];

    a[0] = accel1(0);
    a[1] = accel1(1);
    a[2] = accel1(2);
    a[3] = accel2(0);
    a[4] = accel2(1);
    a[5] = accel2(2);
    a[6] = accel3(0);
    a[7] = accel3(1);
    a[8] = accel3(2);
    a[9] = accel4(0);
    a[10] = accel4(1);
    a[11] = accel4(2);

    // Compute the current resisting force
    this->getResistingForce();
    //std::cerr<<"K "<<P<<std::endl;

    // Compute the mass matrix
    this->getMass();

    for(i = 0; i < 12; i++)
      {
        for(j = 0; j < 12; j++)
          P(i) += K(i,j)*a[j];
      }
  //std::cerr<<"K+M "<<P<<std::endl;

  // dynamic seepage force
  /*for(i = 0, k = 0; i < 4; i++, k += 3) {
    // loop over integration points
    for(j = 0; j < 4; j++) {
      P(i+2) -= physicalProperties.getRho()*dvol[j]*(shp[2][i][j]*a[k]*perm[0]*shp[0][i][j]
                             +shp[2][i][j]*a[k+1]*perm[1]*shp[1][i][j]);
    }
  }*/
  //std::cerr<<"K+M+fb "<<P<<std::endl;

  const Vector &vel1 = nd1Ptr()->getTrialVel();
  const Vector &vel2 = nd2Ptr()->getTrialVel();
  const Vector &vel3 = nd3Ptr()->getTrialVel();
  const Vector &vel4 = nd4Ptr()->getTrialVel();

  a[0] = vel1(0);
  a[1] = vel1(1);
  a[2] = vel1(2);
  a[3] = vel2(0);
  a[4] = vel2(1);
  a[5] = vel2(2);
  a[6] = vel3(0);
  a[7] = vel3(1);
  a[8] = vel3(2);
  a[9] = vel4(0);
  a[10] = vel4(1);
  a[11] = vel4(2);

  this->getDamp();

  for(i = 0; i < 12; i++) {
    for(j = 0; j < 12; j++) {
      P(i) += K(i,j)*a[j];
    }
  }
    if(isDead())
      P*=dead_srf;
    //std::cerr<<"final "<<P<<std::endl;
    return P;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::FourNodeQuadUP::sendData(CommParameters &cp)
  {
    int res= QuadBase4N<SolidMech2D>::sendData(cp);
    res+= cp.sendDoubles(bf[0],bf[1],getDbTagData(),CommMetaData(2));
    res+= cp.sendDoubles(pressure,kc,perm[0],perm[1],getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::FourNodeQuadUP::recvData(const CommParameters &cp)
  {
    int res= QuadBase4N<SolidMech2D>::recvData(cp);
    res+= cp.receiveDoubles(bf[0],bf[1],getDbTagData(),CommMetaData(2));
    res+= cp.receiveDoubles(pressure,kc,perm[0],perm[1],getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::FourNodeQuadUP::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::FourNodeQuadUP::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
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

void XC::FourNodeQuadUP::Print(std::ostream &s, int flag)
  {
        s << "\nFourNodeQuadUP, element id:  " << this->getTag() << std::endl;
        s << "\tConnected external nodes:  " << theNodes;
        s << "\tthickness:  " << physicalProperties.getThickness() << std::endl;
        s << "\tmass density:  " << physicalProperties.getRho() << std::endl;
        s << "\tsurface pressure:  " << pressure << std::endl;
        s << "\tbody forces:  " << bf << std::endl;
        physicalProperties.Print(s,flag);
        s << "\tStress (xx yy xy)" << std::endl;
        for(int i = 0; i < 4; i++)
                s << "\t\tGauss point " << i+1 << ": " << physicalProperties[i]->getStress();
  }


XC::Response *XC::FourNodeQuadUP::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
{
    if(argv[0] == "force" || argv[0] == "forces")
                return new XC::ElementResponse(this, 1, P);

    else if(argv[0] == "stiff" || argv[0] == "stiffness")
                return new XC::ElementResponse(this, 2, K);

        else if(argv[0] == "material" || argv[0] == "integrPoint") {
                int pointNum = atoi(argv[1]);
                if(pointNum > 0 && pointNum <= 4)
                        return setMaterialResponse(physicalProperties[pointNum-1],argv,2,eleInfo);
            else
                        return 0;
        }

    // otherwise response quantity is unknown for the quad class
    else
                return 0;
}

int
XC::FourNodeQuadUP::getResponse(int responseID, Information &eleInfo)
{
        switch (responseID) {

                case 1:
                        return eleInfo.setVector(this->getResistingForce());

                case 2:
                        return eleInfo.setMatrix(this->getTangentStiff());

                default:
                        return -1;
        }
}

int
XC::FourNodeQuadUP::setParameter(const std::vector<std::string> &argv, Parameter &param)
{
        // quad mass density per unit volume
    if(argv[0] == "rho")
      return param.addObject(1, this);
    else if(argv[0] == "pressure")
      return param.addObject(2, this);
    // a material parameter
    else if(argv[0] == "material") {
                int pointNum = atoi(argv[1]);
                if(pointNum > 0 && pointNum <= 4) {
                        int ok =  setMaterialParameter(physicalProperties[pointNum-1],argv,2,param);
                        if(ok < 0)
                                return -1;
                    else if(ok >= 0 && ok < 100)
                                return pointNum*100 + ok;
                        else
                                return -1;
                }
            else
                        return -1;
        }

    // otherwise parameter is unknown for the XC::Truss class
    else
                return -1;

}

int XC::FourNodeQuadUP::updateParameter(int parameterID, Information &info)
{
  switch (parameterID) {
    case -1:
      return -1;

        case 1:
	  physicalProperties.setRho(info.theDouble);
                this->getMass();        // update mass matrix
                return 0;
        case 2:
                pressure = info.theDouble;
                this->setPressureLoadAtNodes();        // update consistent nodal loads
                return 0;
        default:
                if(parameterID >= 100) { // material parameter
                        int pointNum = parameterID/100;
                        if(pointNum > 0 && pointNum <= 4)
                                return physicalProperties[pointNum-1]->updateParameter(parameterID-100*pointNum, info);
                        else
                                return -1;
                } else // unknown
                        return -1;
  }
}

void XC::FourNodeQuadUP::shapeFunction(void) const
{
        double xi, eta, oneMinuseta, onePluseta, oneMinusxi, onePlusxi,
                     detJ, oneOverdetJ, J[2][2], L[2][2], L00, L01, L10, L11,
                                 L00oneMinuseta, L00onePluseta, L01oneMinusxi, L01onePlusxi,
                                 L10oneMinuseta, L10onePluseta, L11oneMinusxi, L11onePlusxi,
                                 vol = 0.0;
  int k, l;

        for(k=0; k<3; k++) {
                for(l=0; l<4; l++) {
                        shpBar[k][l] = 0.0;
                }
        }

        // loop over integration points
        for(int i=0; i<4; i++) {
                xi = pts[i][0];
                eta = pts[i][1];
          const XC::Vector &nd1Crds = nd1Ptr()->getCrds();
          const XC::Vector &nd2Crds = nd2Ptr()->getCrds();
          const XC::Vector &nd3Crds = nd3Ptr()->getCrds();
          const XC::Vector &nd4Crds = nd4Ptr()->getCrds();

          oneMinuseta = 1.0-eta;
          onePluseta = 1.0+eta;
          oneMinusxi = 1.0-xi;
          onePlusxi = 1.0+xi;

          shp[2][0][i] = 0.25*oneMinusxi*oneMinuseta;        // N_1
          shp[2][1][i] = 0.25*onePlusxi*oneMinuseta;                // N_2
          shp[2][2][i] = 0.25*onePlusxi*onePluseta;                // N_3
          shp[2][3][i] = 0.25*oneMinusxi*onePluseta;                // N_4

          J[0][0] = 0.25 * (-nd1Crds(0)*oneMinuseta + nd2Crds(0)*oneMinuseta +
                                nd3Crds(0)*(onePluseta) - nd4Crds(0)*(onePluseta));

          J[0][1] = 0.25 * (-nd1Crds(0)*oneMinusxi - nd2Crds(0)*onePlusxi +
                                nd3Crds(0)*onePlusxi + nd4Crds(0)*oneMinusxi);

          J[1][0] = 0.25 * (-nd1Crds(1)*oneMinuseta + nd2Crds(1)*oneMinuseta +
                                nd3Crds(1)*onePluseta - nd4Crds(1)*onePluseta);

          J[1][1] = 0.25 * (-nd1Crds(1)*oneMinusxi - nd2Crds(1)*onePlusxi +
                                nd3Crds(1)*onePlusxi + nd4Crds(1)*oneMinusxi);

          detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];
          oneOverdetJ = 1.0/detJ;

          // L = inv(J)
          L[0][0] =  J[1][1]*oneOverdetJ;
          L[1][0] = -J[0][1]*oneOverdetJ;
          L[0][1] = -J[1][0]*oneOverdetJ;
          L[1][1] =  J[0][0]*oneOverdetJ;

    L00 = 0.25*L[0][0];
    L10 = 0.25*L[1][0];
    L01 = 0.25*L[0][1];
    L11 = 0.25*L[1][1];

          L00oneMinuseta = L00*oneMinuseta;
          L00onePluseta  = L00*onePluseta;
          L01oneMinusxi  = L01*oneMinusxi;
          L01onePlusxi   = L01*onePlusxi;

          L10oneMinuseta = L10*oneMinuseta;
          L10onePluseta  = L10*onePluseta;
          L11oneMinusxi  = L11*oneMinusxi;
          L11onePlusxi   = L11*onePlusxi;

          // B: See Cook, Malkus, Plesha p. 169 for the derivation of these terms
    shp[0][0][i] = -L00oneMinuseta - L01oneMinusxi;        // N_1,1
    shp[0][1][i] =  L00oneMinuseta - L01onePlusxi;                // N_2,1
    shp[0][2][i] =  L00onePluseta  + L01onePlusxi;                // N_3,1
    shp[0][3][i] = -L00onePluseta  + L01oneMinusxi;        // N_4,1

    shp[1][0][i] = -L10oneMinuseta - L11oneMinusxi;        // N_1,2
    shp[1][1][i] =  L10oneMinuseta - L11onePlusxi;                // N_2,2
    shp[1][2][i] =  L10onePluseta  + L11onePlusxi;                // N_3,2
    shp[1][3][i] = -L10onePluseta  + L11oneMinusxi;        // N_4,2

                dvol[i] = detJ * physicalProperties.getThickness() * wts[i];
    vol += dvol[i];

          for(k=0; k<3; k++) {
                  for(l=0; l<4; l++) {
                    shpBar[k][l] += shp[k][l][i] * dvol[i];
                        }
                }
        }

        for(k=0; k<3; k++) {
          for(l=0; l<4; l++) {
            shpBar[k][l] /= vol;
                }
        }
}


double XC::FourNodeQuadUP::mixtureRho(int i) const
  {
    const double rhoi= physicalProperties[i]->getRho();
    //const double e = 0.7;  //physicalProperties[i]->getVoidRatio();
    //const double n = e / (1.0 + e);
    //return n * physicalProperties.getRho() + (1.0-n) * rhoi;
    return rhoi;
  }

void XC::FourNodeQuadUP::setPressureLoadAtNodes(void)
{
        pressureLoad.Zero();

        if(pressure == 0.0)
                return;

        const XC::Vector &node1 = nd1Ptr()->getCrds();
        const XC::Vector &node2 = nd2Ptr()->getCrds();
        const XC::Vector &node3 = nd3Ptr()->getCrds();
        const XC::Vector &node4 = nd4Ptr()->getCrds();

        double x1 = node1(0);
        double y1 = node1(1);
        double x2 = node2(0);
        double y2 = node2(1);
        double x3 = node3(0);
        double y3 = node3(1);
        double x4 = node4(0);
        double y4 = node4(1);

        double dx12 = x2-x1;
        double dy12 = y2-y1;
        double dx23 = x3-x2;
        double dy23 = y3-y2;
        double dx34 = x4-x3;
        double dy34 = y4-y3;
        double dx41 = x1-x4;
        double dy41 = y1-y4;

        double pressureOver2 = pressure*physicalProperties.getThickness()/2.0;

        // Contribution from side 12
        pressureLoad(0) += pressureOver2*dy12;
        pressureLoad(3) += pressureOver2*dy12;
        pressureLoad(1) += pressureOver2*-dx12;
        pressureLoad(4) += pressureOver2*-dx12;

        // Contribution from side 23
        pressureLoad(3) += pressureOver2*dy23;
        pressureLoad(6) += pressureOver2*dy23;
        pressureLoad(4) += pressureOver2*-dx23;
        pressureLoad(7) += pressureOver2*-dx23;

        // Contribution from side 34
        pressureLoad(6) += pressureOver2*dy34;
        pressureLoad(9) += pressureOver2*dy34;
        pressureLoad(7) += pressureOver2*-dx34;
        pressureLoad(10) += pressureOver2*-dx34;

        // Contribution from side 41
        pressureLoad(9) += pressureOver2*dy41;
        pressureLoad(0) += pressureOver2*dy41;
        pressureLoad(10) += pressureOver2*-dx41;
        pressureLoad(1) += pressureOver2*-dx41;

        //pressureLoad = pressureLoad*physicalProperties.getThickness();
}

