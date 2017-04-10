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
// Description: This file contains the class definition for                  //
// NineFourNodeQuadUP, a 9-4-node (9 node for solid and 4 node for fluid) //
// plane strain element for solid-fluid fully coupled analysis. This         //
// implementation is a simplified u-p formulation of Biot theory             //
// (u - solid displacement, p - fluid pressure). Each element node has two   //
// DOFs for u and 1 DOF for p.                                               //
//                                                                           //
// Written by Zhaohui Yang        (March 2004)                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// $Revision: 1.1 $
// $Date: 2005/09/22 21:28:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/UP-ucsd/Nine_Four_Node_QuadUP.cpp,v $

#include "NineFourNodeQuadUP.h"
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
#include "utility/actor/actor/MatrixCommMetaData.h"

XC::Matrix XC::NineFourNodeQuadUP::K(22,22);
XC::Vector XC::NineFourNodeQuadUP::P(22);
double XC::NineFourNodeQuadUP::shgu[3][9][9];
double XC::NineFourNodeQuadUP::shgp[3][4][4];
double XC::NineFourNodeQuadUP::shgq[3][9][4];
double XC::NineFourNodeQuadUP::shlu[3][9][9];
double XC::NineFourNodeQuadUP::shlp[3][4][4];
double XC::NineFourNodeQuadUP::shlq[3][9][4];
double XC::NineFourNodeQuadUP::wu[9];
double XC::NineFourNodeQuadUP::wp[4];
double XC::NineFourNodeQuadUP::dvolu[9];
double XC::NineFourNodeQuadUP::dvolp[4];
double XC::NineFourNodeQuadUP::dvolq[4];
const int XC::NineFourNodeQuadUP::nintu=9;
const int XC::NineFourNodeQuadUP::nintp=4;
const int XC::NineFourNodeQuadUP::nenu=9;
const int XC::NineFourNodeQuadUP::nenp=4;

XC::NineFourNodeQuadUP::NineFourNodeQuadUP(int tag, int nd1, int nd2, int nd3, int nd4,int nd5, int nd6, int nd7, int nd8,int nd9, NDMaterial &m, const char *type, double t, double bulk, double r, double p1, double p2, const BodyForces2D &bForces)
  :ElemWithMaterial<9,SolidMech2D>(tag, ELE_TAG_Nine_Four_Node_QuadUP,SolidMech2D(9,&m,t,r)), bf(bForces), Ki(nullptr), kc(bulk)
  {
    load.reset(22);
    this->shapeFunction(wu, nintu, nenu, 0);
/*        for( int L = 0; L < nintu; L++) {
                for( int j = 0; j < nenu; j++) {
                printf("%5d %5d %15.6e %15.6e %15.6e\n", L+1, j+1,
                        shlu[0][j][L],shlu[1][j][L],shlu[2][j][L]);
                }
        }
        exit(-1);
*/
    this->shapeFunction(wp, nintp, nenp, 1);
    this->shapeFunction(wp, nintp, nenu, 2);

    // Permeabilities
    perm[0] = p1;
    perm[1] = p2;

    // Set connected external node IDs
    theNodes.set_id_nodes(nd1,nd2,nd3,nd4,nd5,nd6,nd7,nd8,nd9);
  }


XC::NineFourNodeQuadUP::NineFourNodeQuadUP(void)
  :ElemWithMaterial<9,SolidMech2D>(0,ELE_TAG_Nine_Four_Node_QuadUP,SolidMech2D(9,nullptr,1.0,0.0)),
    Ki(0), kc(0.0)
  {
    load.reset(22);
    this->shapeFunction(wu, nintu, nenu, 0);
    this->shapeFunction(wp, nintp, nenp, 1);
    this->shapeFunction(wp, nintp, nenu, 2);
  }

//! @brief Virtual constructor.
XC::Element* XC::NineFourNodeQuadUP::getCopy(void) const
  { return new XC::NineFourNodeQuadUP(*this); }

XC::NineFourNodeQuadUP::~NineFourNodeQuadUP()
  {
    if(Ki) delete Ki;
  }

int XC::NineFourNodeQuadUP::getNumDOF(void) const
  { return 22; }

void XC::NineFourNodeQuadUP::setDomain(Domain *theDomain)
  {
    ElemWithMaterial<9,SolidMech2D>::setDomain(theDomain);

    int dof;
    for(int i=0; i<nenu; i++)
      {
        dof = theNodes[i]->getNumberDOF();
        if((i<nenp && dof != 3) || (i>=nenp && dof != 2))
          {
            std::cerr << "FATAL ERROR NineFourNodeQuadUP, has wrong number of DOFs at its nodes "
                      << this->getTag();
            return;
          }
      }
  }

int XC::NineFourNodeQuadUP::update()
{
  static double u[2][9];
  int i;
  for(i = 0; i < nenu; i++) {
    const XC::Vector &disp = theNodes[i]->getTrialDisp();
    u[0][i] = disp(0);
    u[1][i] = disp(1);
  }

  static XC::Vector eps(3);

  int ret = 0;

  // Determine Jacobian for this integration point
  this->globalShapeFunction(dvolu, wu, nintu, nenu, 0);

  // Loop over the integration points
  for(i = 0; i < nintu; i++) {

    // Interpolate strains
    //eps = B*u;
    //eps.addMatrixVector(0.0, B, u, 1.0);
    eps.Zero();
    for(int beta = 0; beta < nenu; beta++) {
      eps(0) += shgu[0][beta][i]*u[0][beta];
      eps(1) += shgu[1][beta][i]*u[1][beta];
      eps(2) += shgu[0][beta][i]*u[1][beta] + shgu[1][beta][i]*u[0][beta];
    }

    // Set the material strain
    ret += physicalProperties[i]->setTrialStrain(eps);
  }

  return ret;
}


const XC::Matrix &XC::NineFourNodeQuadUP::getTangentStiff(void) const
{
  int i, j, j2, j2m1, ik, ib, jk, jb;
  static XC::Matrix B(3,nenu*2);
  static XC::Matrix BTDB(nenu*2,nenu*2);

  B.Zero();
  BTDB.Zero();
  K.Zero();

  // Determine Jacobian for this integration point
  this->globalShapeFunction(dvolu, wu, nintu, nenu, 0);

  // Loop over the integration points
  for(i = 0; i < nintu; i++) {

    // Get the material tangent
    const XC::Matrix &D = physicalProperties[i]->getTangent();

        for(j=0; j<nenu; j++) {
                j2 = j*2+1;
                j2m1 = j*2;
        B(0,j2m1) = shgu[0][j][i];
                B(0,j2)   = 0.;
                B(1,j2m1) = 0.;
                B(1,j2)   = shgu[1][j][i];
                B(2,j2m1) = shgu[1][j][i];
                B(2,j2)   = shgu[0][j][i];
    }

    // Perform numerical integration
    //K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
    BTDB.addMatrixTripleProduct(1.0, B, D, dvolu[i]);
  }

  for(i = 0; i < nenu; i++) {
          if(i<nenp) ik = i*3;
      if(i>=nenp) ik = nenp*3 + (i-nenp)*2;
      ib = i*2;

          for(j = 0; j < nenu; j++) {
                  if(j<nenp) jk = j*3;
                  if(j>=nenp) jk = nenp*3 + (j-nenp)*2;
          jb = j*2;

          K(ik,jk) += BTDB(ib,jb);
                  K(ik+1,jk) += BTDB(ib+1,jb);
                  K(ik,jk+1) += BTDB(ib,jb+1);
                  K(ik+1,jk+1) += BTDB(ib+1,jb+1);
          }
  }

    if(isDead())
      K*=dead_srf;
    return K;
  }


const XC::Matrix &XC::NineFourNodeQuadUP::getInitialStiff(void) const
  {
    if(!Ki)
      {

  int i, j, j2, j2m1, ik, ib, jk, jb;
  static XC::Matrix B(3,nenu*2);
  static XC::Matrix BTDB(nenu*2,nenu*2);

  B.Zero();
  BTDB.Zero();
  K.Zero();

  // Determine Jacobian for this integration point
  this->globalShapeFunction(dvolu, wu, nintu, nenu, 0);

  // Loop over the integration points
  for(i = 0; i < nintu; i++) {

    // Get the material tangent
    const XC::Matrix &D = physicalProperties[i]->getInitialTangent();

        for(j=0; j<nenu; j++) {
                j2 = j*2+1;
                j2m1 = j*2;
        B(0,j2m1) = shgu[0][j][i];
                B(0,j2)   = 0.;
                B(1,j2m1) = 0.;
                B(1,j2)   = shgu[1][j][i];
                B(2,j2m1) = shgu[1][j][i];
                B(2,j2)   = shgu[0][j][i];
    }

    // Perform numerical integration
    //K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
    BTDB.addMatrixTripleProduct(1.0, B, D, dvolu[i]);
  }

  for(i = 0; i < nenu; i++) {
          if(i<nenp) ik = i*3;
      if(i>=nenp) ik = nenp*3 + (i-nenp)*2;
      ib = i*2;

          for(j = 0; j < nenu; j++) {
                  if(j<nenp) jk = j*3;
                  if(j>=nenp) jk = nenp*3 + (j-nenp)*2;
          jb = j*2;
          K(ik,jk) += BTDB(ib,jb);
                  K(ik+1,jk) += BTDB(ib+1,jb);
                  K(ik,jk+1) += BTDB(ib,jb+1);
                  K(ik+1,jk+1) += BTDB(ib+1,jb+1);
          }
  }

  Ki = new Matrix(K);
      }
  if(Ki == 0) {
    std::cerr << "FATAL XC::NineFourNodeQuadUP::getInitialStiff() -";
    std::cerr << "ran out of memory\n";
    exit(-1);
  }
    if(isDead())
      (*Ki)*=dead_srf;

  return *Ki;
}


const XC::Matrix &XC::NineFourNodeQuadUP::getDamp(void) const
{
  static XC::Matrix Kdamp(22,22);
  Kdamp.Zero();

  if(rayFactors.getBetaK() != 0.0)
    Kdamp.addMatrix(1.0, this->getTangentStiff(), rayFactors.getBetaK());
  if(rayFactors.getBetaK0() != 0.0)
    Kdamp.addMatrix(1.0, this->getInitialStiff(), rayFactors.getBetaK0());
  if(rayFactors.getBetaKc() != 0.0)
    Kdamp.addMatrix(1.0, Kc, rayFactors.getBetaKc());

  int i, j, m, ik, jk;

  if(rayFactors.getAlphaM() != 0.0) {
         this->getMass();

     for(i = 0; i < nenu; i++) {
             if(i<nenp) ik = i*3;
         if(i>=nenp) ik = nenp*3 + (i-nenp)*2;

             for(j = 0; j < nenu; j++) {
                     if(j<nenp) jk = j*3;
                     if(j>=nenp) jk = nenp*3 + (j-nenp)*2;

             Kdamp(ik,jk) += K(ik,jk)*rayFactors.getAlphaM();
             Kdamp(ik+1,jk+1) += K(ik+1,jk+1)*rayFactors.getAlphaM();
                 }
     }
  }

  // Determine Jacobian for this integration point
  this->globalShapeFunction(dvolq, wp, nintp, nenu, 2);
  this->globalShapeFunction(dvolp, wp, nintp, nenp, 1);

  // Compute coupling matrix
  for(i = 0; i < nenu; i++) {
           if(i<nenp) ik = i*3;
       if(i>=nenp) ik = nenp*3 + (i-nenp)*2;

       for(j = 0; j < nenp; j++) {
               jk = j*3+2;

           for(m = 0; m < nintp; m++) {
                   Kdamp(ik,jk) += -dvolq[m]*shgq[0][i][m]*shgp[2][j][m];
                   Kdamp(ik+1,jk) += -dvolq[m]*shgq[1][i][m]*shgp[2][j][m];
                   }
           Kdamp(jk,ik) = Kdamp(ik,jk);
           Kdamp(jk,ik+1) = Kdamp(ik+1,jk);
           }
  }

  // Compute permeability matrix
  for(i = 0; i < nenp; i++) {
       ik = i*3+2;

       for(j = 0; j < nenp; j++) {
               jk = j*3+2;

           for(m = 0; m < nintp; m++) {
                   Kdamp(ik,jk) += - dvolp[m]*(perm[0]*shgp[0][i][m]*shgp[0][j][m] +
                                   perm[1]*shgp[1][i][m]*shgp[1][j][m]);
                   }
           }
  }

  K = Kdamp;
    if(isDead())
      K*=dead_srf;
  return K;
}

const XC::Matrix &XC::NineFourNodeQuadUP::getMass(void) const
{
  K.Zero();

  int i, j, m, ik, jk;
  double Nrho;

  // Determine Jacobian for this integration point
  this->globalShapeFunction(dvolu, wu, nintu, nenu, 0);

  // Compute consistent mass matrix
  for(i = 0; i < nenu; i++) {
          if(i<nenp) ik = i*3;
      if(i>=nenp) ik = nenp*3 + (i-nenp)*2;

          for(j = 0; j < nenu; j++) {
                  if(j<nenp) jk = j*3;
              if(j>=nenp) jk = nenp*3 + (j-nenp)*2;

          for(m = 0; m < nintu; m++) {
              Nrho = dvolu[m]*mixtureRho(m)*shgu[2][i][m]*shgu[2][j][m];
              K(ik,jk) += Nrho;
              K(ik+1,jk+1) += Nrho;
                  }
          }
  }

  // Compute compressibility matrix
  double oneOverKc = 1./kc;
  this->globalShapeFunction(dvolp, wp, nintp, nenp, 1);

  for(i = 0; i < nenp; i++) {
       ik = i*3+2;

       for(j = 0; j < nenp; j++) {
               jk = j*3+2;

           for(m = 0; m < nintp; m++) {
                  K(ik,jk) += -dvolp[m]*oneOverKc*shgp[2][i][m]*shgp[2][j][m];
          }
    }
  }
    if(isDead())
      K*=dead_srf;
    return K;
  }


int XC::NineFourNodeQuadUP::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::NineFourNodeQuadUP::addLoad - load type unknown for ele with tag: " << this->getTag() << "\n";
    return -1;
  }


int XC::NineFourNodeQuadUP::addInertiaLoadToUnbalance(const XC::Vector &accel)
{
  // accel = uDotDotG (see XC::EarthquakePattern.cpp)
  // Get R * accel from the nodes

  static XC::Vector ra(22);
  int i, j, ik;

  ra.Zero();

  for(i=0; i<nenu; i++) {
      const XC::Vector &Raccel = theNodes[i]->getRV(accel);
      if((i<nenp && 3 != Raccel.Size()) || (i>=nenp && 2 != Raccel.Size())) {
         std::cerr << "XC::NineFourNodeQuadUP::addInertiaLoadToUnbalance matrix and vector sizes are incompatable\n";
         return -1;
          }

            if(i<nenp) ik = i*3;
      if(i>=nenp) ik = nenp*3 + (i-nenp)*2;
      ra[ik] = Raccel(0);
      ra[ik+1] = Raccel(1);
  }

  // Compute mass matrix
  this->getMass();

  // Want to add ( - fact * M R * accel ) to unbalance
  for(i = 0; i < 22; i++) {
    for(j = 0; j < 22; j++)
      load(i) += -K(i,j)*ra[j];
  }

  return 0;
}

const XC::Vector &XC::NineFourNodeQuadUP::getResistingForce(void) const
  {
    P.Zero();

    int i, j, jk;

    // Determine Jacobian for this integration point
    this->globalShapeFunction(dvolu, wu, nintu, nenu, 0);
    this->globalShapeFunction(dvolp, wp, nintp, nenp, 1);

    // Loop over the integration points
    for(i = 0; i < nintu; i++)
      {
        // Get material stress response
        const Vector &sigma = physicalProperties[i]->getStress();

        // Perform numerical integration on internal force
        //P = P + (B^ sigma) * intWt(i)*intWt(j) * detJ;
        //P.addMatrixTransposeVector(1.0, B, sigma, intWt(i)*intWt(j)*detJ);
        for(j = 0; j < nenu; j++)
          {
            if(j<nenp) jk = j*3;
            if(j>=nenp) jk = nenp*3 + (j-nenp)*2;

            P(jk) += dvolu[i]*(shgu[0][j][i]*sigma(0) + shgu[1][j][i]*sigma(2));
            P(jk+1) += dvolu[i]*(shgu[1][j][i]*sigma(1) + shgu[0][j][i]*sigma(2));

            // Subtract equiv. body forces from the nodes
            //P = P - (N^ b) * intWt(i)*intWt(j) * detJ;
            //P.addMatrixTransposeVector(1.0, N, b, -intWt(i)*intWt(j)*detJ);
            const double r = mixtureRho(i);
            P(jk) -= dvolu[i]*(shgu[2][j][i]*r*bf[0]);
            P(jk+1) -= dvolu[i]*(shgu[2][j][i]*r*bf[1]);
          }
      }
    //  std::cerr<<"K -body"<<P<<std::endl;

    // Subtract fluid body force
    for(j = 0; j < nenp; j++)
      {
        jk = j*3+2;
        for(i = 0; i < nintp; i++)
          { P(jk) += dvolp[i]*physicalProperties.getRho()*(perm[0]*bf[0]*shgp[0][j][i] + perm[1]*bf[1]*shgp[1][j][i]); }
      }

    //  std::cerr<<"K -fluid "<<P<<std::endl;
    // Subtract other external nodal loads ... P_res = P_int - P_ext
    //P = P - load;
    P.addVector(1.0, load, -1.0);

    if(isDead())
      P*=dead_srf;
    return P;
  }

const XC::Vector &XC::NineFourNodeQuadUP::getResistingForceIncInertia(void) const
  {
    int i, j, ik;
    static double a[22];

    for(i=0; i<nenu; i++)
      {
        const XC::Vector &accel = theNodes[i]->getTrialAccel();
        if((i<nenp && 3 != accel.Size()) || (i>=nenp && 2 != accel.Size()))
          {
            std::cerr << "XC::NineFourNodeQuadUP::getResistingForceIncInertia matrix and vector sizes are incompatable\n";
            return P;
          }

        if(i<nenp) ik = i*3;
        if(i>=nenp) ik = nenp*3 + (i-nenp)*2;
        a[ik] = accel(0);
        a[ik+1] = accel(1);
        if(i<nenp) a[ik+2] = accel(2);
      }

    // Compute the current resisting force
    this->getResistingForce();
    //  std::cerr<<"K "<<P<<std::endl;

    // Compute the mass matrix
    this->getMass();

    for(i = 0; i < 22; i++)
      {
        for(j = 0; j < 22; j++)
          P(i) += K(i,j)*a[j];
      }
    //  std::cerr<<"K+M "<<P<<std::endl;

    for(i=0; i<nenu; i++)
      {
        const XC::Vector &vel = theNodes[i]->getTrialVel();
        if((i<nenp && 3 != vel.Size()) || (i>=nenp && 2 != vel.Size()))
          {
            std::cerr << "XC::NineFourNodeQuadUP::getResistingForceIncInertia matrix and vector sizes are incompatable\n";
            return P;
          }

        if(i<nenp) ik = i*3;
        if(i>=nenp) ik = nenp*3 + (i-nenp)*2;
        a[ik] = vel(0);
        a[ik+1] = vel(1);
        if(i<nenp) a[ik+2] = vel(2);
      }

    this->getDamp();

    for(i = 0; i < 22; i++)
      {
        for(j = 0; j < 22; j++)
          { P(i) += K(i,j)*a[j]; }
      }
    //  std::cerr<<"final "<<P<<std::endl;
    if(isDead())
      P*=dead_srf;
    return P;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::NineFourNodeQuadUP::sendData(CommParameters &cp)
  {
    int res= ElemWithMaterial<9,SolidMech2D>::sendData(cp);
    res+= cp.sendDoubles(bf[0],bf[1],kc,getDbTagData(),CommMetaData(8));
    res+= cp.sendMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(9,10,11,12));
    res+= cp.sendDoubles(perm[0],perm[1],getDbTagData(),CommMetaData(13));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::NineFourNodeQuadUP::recvData(const CommParameters &cp)
  {
    int res= ElemWithMaterial<9,SolidMech2D>::recvData(cp);
    res+= cp.receiveDoubles(bf[0],bf[1],kc,getDbTagData(),CommMetaData(8));
    Ki= cp.receiveMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(9,10,11,12));
    res+= cp.receiveDoubles(perm[0],perm[1],getDbTagData(),CommMetaData(13));
    return res;
  }

int XC::NineFourNodeQuadUP::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(14);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::NineFourNodeQuadUP::recvSelf(const CommParameters &cp)
  {
    inicComm(14);
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

void XC::NineFourNodeQuadUP::Print(std::ostream &s, int flag)
  {
        s << "\nNineFourNodeQuadUP, element id:  " << this->getTag() << std::endl;
        s << "\tConnected external nodes:  " << theNodes;
        s << "\tthickness:  " << physicalProperties.getThickness() << std::endl;
        s << "\tmass density:  " << physicalProperties.getRho() << std::endl;
        //s << "\tsurface pressure:  " << pressure << std::endl;
        s << "\tbody forces:  " << bf << std::endl;
        physicalProperties.Print(s,flag);
        s << "\tStress (xx yy xy)" << std::endl;
        for(int i = 0; i < 9; i++)
          s << "\t\tGauss point " << i+1 << ": " << physicalProperties[i]->getStress();
}


XC::Response *XC::NineFourNodeQuadUP::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    if(argv[0] == "force" || argv[0] == "forces")
      return new ElementResponse(this, 1, P);
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      return new ElementResponse(this, 2, K);
    else if(argv[0] == "mass")
      return new ElementResponse(this, 3, K);
    else if(argv[0] == "damp")
      return new ElementResponse(this, 4, K);
    else if(argv[0] == "material" || argv[0] == "integrPoint")
      {
        const int pointNum = atoi(argv[1]);
        if(pointNum > 0 && pointNum <= nenu)
          return setMaterialResponse(physicalProperties[pointNum-1],argv,2,eleInfo);
        else
          return 0;
      }
    // otherwise response quantity is unknown for the quad class
    else
      return 0;
  }

int XC::NineFourNodeQuadUP::getResponse(int responseID, Information &eleInfo)
{
        switch (responseID) {

                case 1:
                        return eleInfo.setVector(this->getResistingForce());

                case 2:
                        return eleInfo.setMatrix(this->getTangentStiff());

                case 3:
                        return eleInfo.setMatrix(this->getMass());

                case 4:
                        return eleInfo.setMatrix(this->getDamp());

                default:
                        return -1;
        }
}

int
XC::NineFourNodeQuadUP::setParameter(const std::vector<std::string> &argv, Parameter &param)
{
        // quad mass density per unit volume
    if(argv[0] == "rho")
      return param.addObject(1, this);
    // a material parameter
    else if(argv[0] == "material") {
                int pointNum = atoi(argv[1]);
                if(pointNum > 0 && pointNum <= nenu) {
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

int XC::NineFourNodeQuadUP::updateParameter(int parameterID, Information &info)
{
  switch (parameterID) {
    case -1:
      return -1;

        case 1:
	  physicalProperties.setRho(info.theDouble);
                this->getMass();        // update mass matrix
                return 0;

        default:
                if(parameterID >= 100) { // material parameter
                        int pointNum = parameterID/100;
                        if(pointNum > 0 && pointNum <= nenu)
                                return physicalProperties[pointNum-1]->updateParameter(parameterID-100*pointNum, info);
                        else
                                return -1;
                } else // unknown
                        return -1;
  }
}

void XC::NineFourNodeQuadUP::globalShapeFunction(double *dvol, double *w, int nint, int nen, int mode) const
{
  static double coord[2][9], xs[2][2], det, temp;
  int i, j, k, m;

  for(i=0; i<3; i++) {
     for(j=0; j<nen; j++) {
        for(k=0; k<nint; k++) {
           if(mode==0) shgu[i][j][k] = shlu[i][j][k];
           if(mode==1) shgp[i][j][k] = shlp[i][j][k];
           if(mode==2) shgq[i][j][k] = shlq[i][j][k];
                }
         }
  }

  for(i=0; i<nen; i++) {
         const XC::Vector &coordRef = theNodes[i]->getCrds();
         coord[0][i] = coordRef(0);
         coord[1][i] = coordRef(1);
  }

  for(m=0; m<nint; m++) {
      for(i=0; i<2; i++) {
              for(j=0; j<2; j++) {
                   xs[i][j] = 0.0;
                   for(k=0; k<nen; k++) {
                    if(mode==0) xs[i][j] += coord[j][k]*shgu[i][k][m];
                    if(mode==1) xs[i][j] += coord[j][k]*shgp[i][k][m];
                    if(mode==2) xs[i][j] += coord[j][k]*shgq[i][k][m];
               }
                  }
          }

          det = xs[0][0]*xs[1][1] - xs[0][1]*xs[1][0];

          if(det < 0.0) {
          std::cerr << "WARNING NineFourNodeQuadUP: Determinant<=0 in tag "
                         << this->getTag();
                  exit(-1);
          }

      for(i=0; i<nen; i++) {
          if(mode==0) {
             temp = (shgu[0][i][m]*xs[1][1] - shgu[1][i][m]*xs[0][1])/det;
                     shgu[1][i][m] = (-shgu[0][i][m]*xs[1][0] + shgu[1][i][m]*xs[0][0])/det;
                     shgu[0][i][m] = temp;
          }
          if(mode==1) {
             temp = (shgp[0][i][m]*xs[1][1] - shgp[1][i][m]*xs[0][1])/det;
                     shgp[1][i][m] = (-shgp[0][i][m]*xs[1][0] + shgp[1][i][m]*xs[0][0])/det;
                     shgp[0][i][m] = temp;
          }
          if(mode==2) {
             temp = (shgq[0][i][m]*xs[1][1] - shgq[1][i][m]*xs[0][1])/det;
                     shgq[1][i][m] = (-shgq[0][i][m]*xs[1][0] + shgq[1][i][m]*xs[0][0])/det;
                     shgq[0][i][m] = temp;
          }
          }

          dvol[m] = w[m]*physicalProperties.getThickness()*det;

  }  //end of m loop

}


void XC::NineFourNodeQuadUP::shapeFunction(double *w, int nint, int nen, int mode) const
{
  static const double ra[] = {-0.5,0.5,0.5,-0.5,0.,0.5,0.,-0.5,0.};
  static const double sa[] = {-0.5,-0.5,0.5,0.5,-0.5,0.,0.5,0.,0.};

  double g, r, s, shl19, shl29, shl39, tempr, temps;
  int ic, il, is;

  g = 0.;
  if(nint == 4) {
          g=2./sqrt(3.0);
          w[0] = w[1] = w[2] = w[3] = 1.;
  }
  if(nint == 9) {
          g=2.*sqrt(3.0/5.0);
          w[0] = w[1] = w[2] = w[3] = 25./81.;
          w[4] = w[5] = w[6] = w[7] = 40./81.;
          w[8] = 64./81.;
  }

  for(int i=0; i<nint; i++) {
    r = g*ra[i];
        s = g*sa[i];
        shl19 = shl29 = shl39 = 0.;
        if(nen > 4) {
                tempr = 1.-r*r;
                temps = 1.-s*s;
                if(nen == 9) {
                        if(mode==0) {
                             shlu[0][8][i] = -2.*r*temps;
                           shl19 = 0.5*shlu[0][8][i];
                           shlu[1][8][i] = -2.*s*tempr;
                           shl29 = 0.5*shlu[1][8][i];
                           shlu[2][8][i] = temps*tempr;
                           shl39 = 0.5*shlu[2][8][i];
                        }
                        if(mode==2) {
                             shlq[0][8][i] = -2.*r*temps;
                           shl19 = 0.5*shlq[0][8][i];
                           shlq[1][8][i] = -2.*s*tempr;
                           shl29 = 0.5*shlq[1][8][i];
                           shlq[2][8][i] = temps*tempr;
                           shl39 = 0.5*shlq[2][8][i];
                        }
        }
                if(mode==0) {
                   shlu[0][4][i] = -r*(1.-s) - shl19;
                   shlu[1][4][i] = -0.5*tempr - shl29;
           shlu[2][4][i] = 0.5*tempr*(1.-s) - shl39;
                   shlu[0][5][i] = 0.5*temps - shl19;
                   shlu[1][5][i] = -s*(1.+r) - shl29;
           shlu[2][5][i] = 0.5*temps*(1.+r) - shl39;
                   shlu[0][6][i] = -r*(1.+s) - shl19;
                   shlu[1][6][i] = 0.5*tempr - shl29;
           shlu[2][6][i] = 0.5*tempr*(1.+s) - shl39;
                   shlu[0][7][i] = -0.5*temps - shl19;
                   shlu[1][7][i] = -s*(1.-r) - shl29;
           shlu[2][7][i] = 0.5*temps*(1.-r) - shl39;
                }
                if(mode==2) {
                   shlq[0][4][i] = -r*(1.-s) - shl19;
                   shlq[1][4][i] = -0.5*tempr - shl29;
           shlq[2][4][i] = 0.5*tempr*(1.-s) - shl39;
                   shlq[0][5][i] = 0.5*temps - shl19;
                   shlq[1][5][i] = -s*(1.+r) - shl29;
           shlq[2][5][i] = 0.5*temps*(1.+r) - shl39;
                   shlq[0][6][i] = -r*(1.+s) - shl19;
                   shlq[1][6][i] = 0.5*tempr - shl29;
           shlq[2][6][i] = 0.5*tempr*(1.+s) - shl39;
                   shlq[0][7][i] = -0.5*temps - shl19;
                   shlq[1][7][i] = -s*(1.-r) - shl29;
           shlq[2][7][i] = 0.5*temps*(1.-r) - shl39;
                }
        }

    for(int k=0; k<4; k++) {
        tempr = 0.5 + ra[k]*r;
                temps = 0.5 + sa[k]*s;
                if(mode==0) {
                   shlu[0][k][i] = ra[k]*temps - 0.5*shl19;
                   shlu[1][k][i] = tempr*sa[k] - 0.5*shl29;
           shlu[2][k][i] = tempr*temps - 0.5*shl39;
                }
                if(mode==1) {
                   shlp[0][k][i] = ra[k]*temps - 0.5*shl19;
                   shlp[1][k][i] = tempr*sa[k] - 0.5*shl29;
           shlp[2][k][i] = tempr*temps - 0.5*shl39;
                }
                if(mode==2) {
                   shlq[0][k][i] = ra[k]*temps - 0.5*shl19;
                   shlq[1][k][i] = tempr*sa[k] - 0.5*shl29;
           shlq[2][k][i] = tempr*temps - 0.5*shl39;
                }
        }

    if(nen > 4) {
        for(int m=4; m<8; m++) {
            ic = m - 4;
                        il = m - 3;
                        is = 1;
                        if(m==7) {
                ic = 0;
                                il = 3;
                                is = 3;
                        }
            for(int j=ic; j<=il; j+=is) {
                        if(mode==0) {
                   shlu[0][j][i] = shlu[0][j][i] - 0.5*shlu[0][m][i];
                   shlu[1][j][i] = shlu[1][j][i] - 0.5*shlu[1][m][i];
                   shlu[2][j][i] = shlu[2][j][i] - 0.5*shlu[2][m][i];
                                }
                        if(mode==2) {
                   shlq[0][j][i] = shlq[0][j][i] - 0.5*shlq[0][m][i];
                   shlq[1][j][i] = shlq[1][j][i] - 0.5*shlq[1][m][i];
                   shlq[2][j][i] = shlq[2][j][i] - 0.5*shlq[2][m][i];
                                }
                        }
                }  //end of m loop
        }
  }  //end of i loop
}


double XC::NineFourNodeQuadUP::mixtureRho(int i) const
  {
    double rhoi= physicalProperties[i]->getRho();
    //double e = 0.7;  //physicalProperties[i]->getVoidRatio();
    //double n = e / (1.0 + e);
    //return n * rho + (1.0-n) * rhoi;
    return rhoi;
  }

