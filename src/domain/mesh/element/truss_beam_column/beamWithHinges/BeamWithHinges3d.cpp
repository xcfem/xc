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

// $Revision: 1.19 $
// $Date: 2004/06/07 23:20:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beamWithHinges/BeamWithHinges3d.cpp,v $

#include <domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges3d.h>
#include "domain/component/Parameter.h"
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <cstring>

#include <material/section/SeccionBarraPrismatica.h>
#include <domain/mesh/element/utils/coordTransformation/CrdTransf3d.h>

#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

XC::Matrix XC::BeamWithHinges3d::theMatrix(12,12);
XC::Vector XC::BeamWithHinges3d::theVector(12);
double XC::BeamWithHinges3d::workArea[200];

XC::BeamWithHinges3d::BeamWithHinges3d(int tag)
  :BeamColumnWithSectionFDTrf3d(tag, ELE_TAG_BeamWithHinges3d,2),
   ctes_scc(), beta1(0.0), beta2(0.0), rho(0.0),
   kb(6,6), q(6), kbCommit(6,6), qCommit(6),
   initialFlag(0), maxIter(0), tolerance(0.0), applied_sf(nullptr), p0(),v0()
  { load.reset(12); }

XC::BeamWithHinges3d::BeamWithHinges3d(int tag,const Material *mat,const CrdTransf *coordTransf)
  :BeamColumnWithSectionFDTrf3d(tag, ELE_TAG_BeamWithHinges3d,2,mat,coordTransf),
   ctes_scc(), beta1(0.0), beta2(0.0), rho(0.0),
   kb(6,6), q(6), kbCommit(6,6), qCommit(6),
   initialFlag(0), maxIter(0), tolerance(0.0), applied_sf(nullptr), p0(),v0()
  { load.reset(12); }

XC::BeamWithHinges3d::BeamWithHinges3d(int tag, int nodeI, int nodeJ,
                                   double e, double a, double iz,
                                   double iy, double g, double j,
                                   SeccionBarraPrismatica &sectionRefI, double lpi,
                                   SeccionBarraPrismatica &sectionRefJ, double lpj,
                                   CrdTransf3d &coordTransf,
                                   double r, int max, double tol)
  :BeamColumnWithSectionFDTrf3d(tag, ELE_TAG_BeamWithHinges3d,2,nodeI,nodeJ,coordTransf),
   ctes_scc(e,a,iz,iy,g,j), beta1(lpi), beta2(lpj), rho(r),
   kb(6,6), q(6), kbCommit(6,6), qCommit(6),
   initialFlag(0), maxIter(max), tolerance(tol), applied_sf(nullptr), p0(),v0()
  {
    load.reset(12);

    // Get copies of sections
    theSections.setSectionCopy(0,&sectionRefI);
    theSections.setSectionCopy(1,&sectionRefJ);
  }

//! @brief Copy constructor.
XC::BeamWithHinges3d::BeamWithHinges3d(const XC::BeamWithHinges3d &otro)
  :BeamColumnWithSectionFDTrf3d(otro), ctes_scc(otro.ctes_scc), beta1(otro.beta1), beta2(otro.beta2), rho(otro.rho),
   kb(otro.kb), q(otro.q), kbCommit(otro.kbCommit), qCommit(otro.qCommit),
   initialFlag(otro.initialFlag), maxIter(otro.maxIter), tolerance(otro.tolerance),
   applied_sf(nullptr), p0(otro.p0),v0(otro.v0)
  {}

// //! @brief Assignment operator.
// XC::BeamWithHinges3d &XC::BeamWithHinges3d::operator=(const XC::BeamWithHinges3d &otro)
//   {
//     BeamColumnWithSectionFDTrf3d::operator=(otro);
//     ctes_scc= otro.ctes_scc;
//     beta1= otro.beta1;
//     beta2= otro.beta2;
//     rho= otro.rho;
//     kb= otro.kb;
//     q= otro.q;
//     kbCommit= otro.kbCommit;
//     qCommit= otro.qCommit;
//     initialFlag= otro.initialFlag;
//     maxIter= otro.maxIter;
//     tolerance= otro.tolerance;
//     p0= otro.p0;
//     v0= otro.v0;
//     return *this;
//   }

//! @brief Virtual constructor.
XC::Element* XC::BeamWithHinges3d::getCopy(void) const
  { return new XC::BeamWithHinges3d(*this); }

//! @brief Destructor.
XC::BeamWithHinges3d::~BeamWithHinges3d(void)
  {
    if(applied_sf) delete applied_sf;
  }

//! @brief Return the number of element DOFs
int XC::BeamWithHinges3d::getNumDOF(void) const
  { return 12; }

//! @brief Establece el domain del elemento.
void XC::BeamWithHinges3d::setDomain(Domain *theDomain)
  {
  //This function may be called after a beam is constructed, so
  //geometry may change.  Therefore calculate all beam geometry here.

    BeamColumnWithSectionFDTrf3d::setDomain(theDomain);
    if(theDomain)
      {
        initialize_trf();

        // get element length
        const double L= theCoordTransf->getInitialLength();
        if(L == 0.0)
          {
            std::cerr << "XC::BeamWithHinges3d::setDomain() -- element has zero length\n";
            exit(-1);
          }

        // Set up section interpolation and hinge lengths
        this->setHinges();

        if(initialFlag == 2)
          theCoordTransf->update();
        else
          this->update();
      }
  }

int XC::BeamWithHinges3d::commitState(void)
  {
    int err = 0;

    // call element commitState to do any base class stuff
    if((err = this->XC::BeamColumnWithSectionFDTrf3d::commitState()) != 0)
      {
        std::cerr << "XC::BeamWithHinges3d::commitState () - failed in base class\n";
      }

    err+= theCoordTransf->commitState();

    kbCommit = kb;
    qCommit = q;

    eCommit[0] = e[0];
    eCommit[1] = e[1];

    //initialFlag = 0;
    return err;
  }

int XC::BeamWithHinges3d::revertToLastCommit(void)
  {
    int err = 0;

    // Revert the sections and then get their last commited
    // deformations, stress resultants, and flexibilities
    for(int i = 0; i < 2; i++)
      {
        if(theSections[i] != 0)
          {
            err+= theSections[i]->revertToLastCommit();
            theSections[i]->setTrialSectionDeformation(eCommit[i]);

            e[i] = eCommit[i];
            sr[i] = theSections[i]->getStressResultant();
            fs[i] = theSections[i]->getSectionFlexibility();
          }
      }

    // Commit the coordinate transformation
    err+= theCoordTransf->revertToLastCommit();

    kb = kbCommit;
    q = qCommit;

    initialFlag = 0;
    return err;
  }

int XC::BeamWithHinges3d::revertToStart(void)
  {
    int err = 0;

    for(int i = 0; i < 2; i++)
      {
        if(theSections[i] != 0)
          {
            err+= theSections[i]->revertToStart();
            fs[i].Zero();
            e[i].Zero();
            sr[i].Zero();
            eCommit[i].Zero();
          }
      }

    err+= theCoordTransf->revertToStart();

    kb.Zero();
    q.Zero();

    initialFlag = 0;
    this->update();
    return err;
  }

const XC::Matrix &XC::BeamWithHinges3d::getTangentStiff(void) const
  {
    static Matrix K;
    K= theCoordTransf->getGlobalStiffMatrix(kb, q);
    if(isDead())
      K*=dead_srf;
    return K;    
  }


const XC::Matrix &XC::BeamWithHinges3d::getInitialStiff(void) const
  {
    const double L = theCoordTransf->getInitialLength();
    const double oneOverL = 1.0/L;

    // Section locations along element length ...
    double xi[2];

    // and their integration weights
    double lp[2];

    lp[0] = beta1*L;
    lp[1] = beta2*L;

    xi[0] = 0.5*lp[0];
    xi[1] = L-0.5*lp[1];

    // element properties
    static XC::Matrix f(6,6);        // element flexibility
    static XC::Matrix Iden(6,6);   // an identity matrix for matrix inverse
    Iden.Zero();
    int i;
    for(i = 0; i < 6; i++)
      Iden(i,i) = 1.0;

    // Length of elastic interior
    const double Le = L-lp[0]-lp[1];
    const double LoverEA   = Le/(ctes_scc.EA());
    const double Lover3EIz = Le/(3*ctes_scc.EIz());
    const double Lover6EIz = 0.5*Lover3EIz;
    const double Lover3EIy = Le/(3*ctes_scc.EIy());
    const double Lover6EIy = 0.5*Lover3EIy;
    const double LoverGJ   = Le/(ctes_scc.GJ());

    // Elastic flexibility of element interior
    static XC::Matrix fe(4,4);
    fe(0,0) = fe(1,1) =  Lover3EIz;
    fe(0,1) = fe(1,0) = -Lover6EIz;
    fe(2,2) = fe(3,3) =  Lover3EIy;
    fe(2,3) = fe(3,2) = -Lover6EIy;

    // Equilibrium transformation matrix
    static XC::Matrix B(4,4);
    B(0,0) = B(2,2) = 1.0 - beta1;
    B(1,1) = B(3,3) = 1.0 - beta2;
    B(0,1) = B(2,3) = -beta1;
    B(1,0) = B(3,2) = -beta2;

    // Transform the elastic flexibility of the element
    // interior to the basic system
    static XC::Matrix fElastic(4,4);
    fElastic.addMatrixTripleProduct(0.0, B, fe, 1.0);

    // Set element flexibility to flexibility of elastic region
    f.Zero();
    f(0,0) = LoverEA;
    f(1,1) = fElastic(0,0);
    f(2,2) = fElastic(1,1);
    f(1,2) = fElastic(0,1);
    f(2,1) = fElastic(1,0);
    f(3,3) = fElastic(2,2);
    f(4,4) = fElastic(3,3);
    f(3,4) = fElastic(2,3);
    f(4,3) = fElastic(3,2);
    f(5,5) = LoverGJ;

    for(i = 0; i < 2; i++)
      {
        if(theSections[i] == 0 || lp[i] <= 0.0)
          continue;

        // Get section information
        int order = theSections[i]->getOrder();
        const XC::ID &code = theSections[i]->getType();

        Vector s(workArea, order);
        Vector ds(&workArea[order], order);
        Vector de(&workArea[2*order], order);

        Matrix fb(&workArea[3*order], order, 6);

        const double xL = xi[i]*oneOverL;
        const double xL1 = xL-1.0;

        // get section flexibility matrix
        const XC::Matrix &fSec = theSections[i]->getInitialFlexibility();

        // integrate section flexibility matrix
        // f += (b^ fs * b) * lp[i];
        //f.addMatrixTripleProduct(1.0, b, fSec, lp[i]);
        int ii, jj;
        fb.Zero();
        double tmp;
        for(ii = 0; ii < order; ii++)
          {
            switch(code(ii))
              {
              case SECTION_RESPONSE_P:
                for(jj = 0; jj < order; jj++)
                  fb(jj,0) += fSec(jj,ii)*lp[i];
                break;
              case SECTION_RESPONSE_MZ:
                for(jj = 0; jj < order; jj++)
                  {
                    tmp = fSec(jj,ii)*lp[i];
                    fb(jj,1) += xL1*tmp;
                    fb(jj,2) += xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VY:
                for(jj = 0; jj < order; jj++)
                  {
                    //tmp = oneOverL*fSec(jj,ii)*lp[i]*L/lp[i];
                    tmp = fSec(jj,ii);
                    fb(jj,1) += tmp;
                    fb(jj,2) += tmp;
                  }
                break;
              case SECTION_RESPONSE_MY:
                for(jj = 0; jj < order; jj++)
                  {
                    tmp = fSec(jj,ii)*lp[i];
                    fb(jj,3) += xL1*tmp;
                    fb(jj,4) += xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VZ:
                for(jj = 0; jj < order; jj++)
                  {
                    //tmp = oneOverL*fSec(jj,ii)*lp[i]*L/lp[i];
                    tmp = fSec(jj,ii);
                    fb(jj,3) += tmp;
                    fb(jj,4) += tmp;
                  }
                break;
              case SECTION_RESPONSE_T:
                for(jj = 0; jj < order; jj++)
                  fb(jj,5) += fSec(jj,ii)*lp[i];
                break;
              default:
                break;
              }
          }
    for(ii = 0; ii < order; ii++) {
      switch (code(ii)) {
      case SECTION_RESPONSE_P:
        for(jj = 0; jj < 6; jj++)
          f(0,jj) += fb(ii,jj);
        break;
      case SECTION_RESPONSE_MZ:
        for(jj = 0; jj < 6; jj++) {
          tmp = fb(ii,jj);
          f(1,jj) += xL1*tmp;
          f(2,jj) += xL*tmp;
        }
        break;
      case SECTION_RESPONSE_VY:
        for(jj = 0; jj < 6; jj++) {
          tmp = oneOverL*fb(ii,jj);
          f(1,jj) += tmp;
          f(2,jj) += tmp;
        }
        break;
      case SECTION_RESPONSE_MY:
        for(jj = 0; jj < 6; jj++) {
          tmp = fb(ii,jj);
          f(3,jj) += xL1*tmp;
          f(4,jj) += xL*tmp;
        }
        break;
      case SECTION_RESPONSE_VZ:
        for(jj = 0; jj < 6; jj++) {
          tmp = oneOverL*fb(ii,jj);
          f(3,jj) += tmp;
          f(4,jj) += tmp;
        }
        break;
      case SECTION_RESPONSE_T:
        for(jj = 0; jj < 6; jj++)
          f(5,jj) += fb(ii,jj);
        break;
      default:
        break;
      }
    }

  }
  // calculate element stiffness matrix
  static Matrix kbInit(6,6);
    if(f.Solve(Iden,kbInit) < 0)
      std::cerr << "XC::BeamWithHinges3d::update() -- could not invert flexibility\n";
    static Matrix K;
    K= theCoordTransf->getInitialGlobalStiffMatrix(kbInit);
    if(isDead())
      K*=dead_srf;
    return K;    
  }

const XC::Matrix &XC::BeamWithHinges3d::getMass(void) const
  {
    theMatrix.Zero();
    if(rho != 0.0)
      {
        double L = theCoordTransf->getInitialLength();
        theMatrix(0,0) = theMatrix(1,1) = theMatrix(2,2) =
          theMatrix(6,6) = theMatrix(7,7) = theMatrix(8,8) = 0.5*L*rho;
      }
    if(isDead())
      theMatrix*=dead_srf;
    return theMatrix;
  }

void XC::BeamWithHinges3d::zeroLoad(void)
  {
    if(applied_sf != 0) applied_sf->Zero();

    p0.zero();
    v0.zero();
    BeamColumnWithSectionFDTrf3d::zeroLoad();
  }

//! @brief Effect of the load on the element.
int XC::BeamWithHinges3d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << nombre_clase() 
                << "; load over inactive element: "
                << getTag() << std::endl;
    else
      {
        if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
          {
            const double L = theCoordTransf->getInitialLength();
            const double lp1 = beta1*L;
            const double lp2 = beta2*L;

            // Section locations along element length ...
            double xi[2];
            xi[0] = 0.5*lp1;
            xi[1] = L-0.5*lp2;

            const Matrix xi_pt(xi,2,1);
            if(!applied_sf)
              {
                applied_sf = new XC::Matrix(6,2);
                if(!applied_sf)
                  std::cerr << "XC::BeamWithHinges3d::addLoad() -- out of memory\n";
              }
            (*applied_sf)+= beamMecLoad->getAppliedSectionForces(L,xi_pt,loadFactor); // Accumulate applied section forces due to element loads
            beamMecLoad->addReactionsInBasicSystem(L,loadFactor,p0); // Accumulate reactions in basic system
            beamMecLoad->addElasticDeformations(L,ctes_scc,lp1,lp2,loadFactor,v0);
          }
        else
          {
            std::cerr << "XC::BeamWithHinges3d::addLoad() -- load type unknown for element with tag: " << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

int XC::BeamWithHinges3d::addInertiaLoadToUnbalance(const XC::Vector &accel)
{
  if(rho == 0.0)
    return 0;

  const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
  const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

  double L = theCoordTransf->getInitialLength();
  double mass = 0.5*L*rho;

  int i,j;
  for(i = 0, j = 6; i < 3; i++, j++) {
    load(i) += mass*Raccel1(i);
    load(j) += mass*Raccel2(i);        // Yes, this should be 'i'
  }

  return 0;
}

const XC::Vector &XC::BeamWithHinges3d::getResistingForce(void) const
  {
    Vector p0Vec= p0.getVector();
    static Vector retval;
    retval= theCoordTransf->getGlobalResistingForce(q, p0Vec);
    if(isDead())
      retval*=dead_srf;
    return retval;
  }

const XC::Vector &XC::BeamWithHinges3d::getResistingForceIncInertia(void) const
  {
    theVector = this->getResistingForce();

    if(rho != 0.0)
      {
        double ag[12];
        const XC::Vector &accel1 = theNodes[0]->getTrialAccel();
        const XC::Vector &accel2 = theNodes[1]->getTrialAccel();

        ag[0] = accel1(0);
        ag[1] = accel1(1);
        ag[2] = accel1(2);
        ag[6] = accel2(0);
        ag[7] = accel2(1);
        ag[8] = accel2(2);

        theVector = this->getResistingForce();

        double L = theCoordTransf->getInitialLength();
        double mass = 0.5*L*rho;

        int i,j;
        for(i = 0, j = 6; i < 3; i++, j++)
          {
            theVector(i) += mass*ag[i];
            theVector(j) += mass*ag[j];
          }
        // add the damping forces if rayleigh damping
        if(!rayFactors.Nulos())
          theVector += this->getRayleighDampingForces();
      }
    else
      {
        // add the damping forces if rayleigh damping
        if(!rayFactors.KNulos())
          theVector += this->getRayleighDampingForces();
      }
    if(isDead())
      theVector*=dead_srf; //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
    return theVector;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::BeamWithHinges3d::sendData(CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf3d::sendData(cp);
    res+= cp.sendDoubles(beta1,beta2,rho,getDbTagData(),CommMetaData(12));
    res+= cp.sendMatrix(fs[0],getDbTagData(),CommMetaData(13));
    res+= cp.sendMatrix(fs[1],getDbTagData(),CommMetaData(14));
    res+= cp.sendVector(sr[0],getDbTagData(),CommMetaData(15));
    res+= cp.sendVector(sr[1],getDbTagData(),CommMetaData(16));
    res+= cp.sendVector(e[0],getDbTagData(),CommMetaData(17));
    res+= cp.sendVector(e[1],getDbTagData(),CommMetaData(18));
    res+= cp.sendMatrix(kb,getDbTagData(),CommMetaData(19));
    res+= cp.sendVector(q,getDbTagData(),CommMetaData(20));
    res+= cp.sendMatrix(kbCommit,getDbTagData(),CommMetaData(21));
    res+= cp.sendVector(qCommit,getDbTagData(),CommMetaData(22));
    res+= cp.sendVector(eCommit[0],getDbTagData(),CommMetaData(23));
    res+= cp.sendVector(eCommit[1],getDbTagData(),CommMetaData(24));
    res+= cp.sendInts(initialFlag,maxIter,getDbTagData(),CommMetaData(25));
    res+= cp.sendDouble(tolerance,getDbTagData(),CommMetaData(26));
    res+= cp.sendMatrixPtr(applied_sf,getDbTagData(),MatrixCommMetaData(26,27,28,29));
    res+= p0.sendData(cp,getDbTagData(),CommMetaData(30));
    res+= v0.sendData(cp,getDbTagData(),CommMetaData(31));
    res+= cp.sendMovable(ctes_scc,getDbTagData(),CommMetaData(32));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::BeamWithHinges3d::recvData(const CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf3d::recvData(cp);
    res+= cp.receiveDoubles(beta1,beta2,rho,getDbTagData(),CommMetaData(12));
    res+= cp.receiveMatrix(fs[0],getDbTagData(),CommMetaData(13));
    res+= cp.receiveMatrix(fs[1],getDbTagData(),CommMetaData(14));
    res+= cp.receiveVector(sr[0],getDbTagData(),CommMetaData(15));
    res+= cp.receiveVector(sr[1],getDbTagData(),CommMetaData(16));
    res+= cp.receiveVector(e[0],getDbTagData(),CommMetaData(17));
    res+= cp.receiveVector(e[1],getDbTagData(),CommMetaData(18));
    res+= cp.receiveMatrix(kb,getDbTagData(),CommMetaData(19));
    res+= cp.receiveVector(q,getDbTagData(),CommMetaData(20));
    res+= cp.receiveMatrix(kbCommit,getDbTagData(),CommMetaData(21));
    res+= cp.receiveVector(qCommit,getDbTagData(),CommMetaData(22));
    res+= cp.receiveVector(eCommit[0],getDbTagData(),CommMetaData(23));
    res+= cp.receiveVector(eCommit[1],getDbTagData(),CommMetaData(24));
    res+= cp.receiveInts(initialFlag,maxIter,getDbTagData(),CommMetaData(25));
    res+= cp.receiveDouble(tolerance,getDbTagData(),CommMetaData(26));
    applied_sf= cp.receiveMatrixPtr(applied_sf,getDbTagData(),MatrixCommMetaData(26,27,28,29));
    res+= p0.receiveData(cp,getDbTagData(),CommMetaData(30));
    res+= v0.receiveData(cp,getDbTagData(),CommMetaData(31));
    res+= cp.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(32));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::BeamWithHinges3d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(33);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::BeamWithHinges3d::recvSelf(const CommParameters &cp)
  {
    inicComm(33);
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

void XC::BeamWithHinges3d::Print(std::ostream &s, int flag)
  {
    s << "\nBeamWithHinges3d, tag: " << this->getTag() << std::endl;
    s << "\tConnected Nodes: " << theNodes;
    ctes_scc.Print(s);

    double P, Mz1, Mz2, Vy, My1, My2, Vz, T;
    double L = theCoordTransf->getInitialLength();
    double oneOverL = 1.0/L;

  P   = qCommit(0);
  Mz1 = qCommit(1);
  Mz2 = qCommit(2);
  Vy  = (Mz1+Mz2)*oneOverL;
  My1 = qCommit(3);
  My2 = qCommit(4);
  Vz  = (My1+My2)*oneOverL;
  T   = qCommit(5);

  s << "\tEnd 1 Forces (P Mz Vy My Vz T): "
    << -P+p0[0] << ' ' << Mz1 << ' ' <<  Vy+p0[1] << ' ' << My1 << ' ' <<  Vz+p0[3] << ' ' << -T << std::endl;
  s << "\tEnd 2 Forces (P Mz Vy My Vz T): "
    <<  P << ' ' << Mz2 << ' ' << -Vy+p0[2] << ' ' << My2 << ' ' << -Vz+p0[4] << ' ' <<  T << std::endl;

  if(theSections[0] != 0) {
    s << "Hinge 1, section tag: " << theSections[0]->getTag() <<
      ", length: " << beta1*L << std::endl;
    theSections[0]->Print(s,flag);
  }

  if(theSections[1] != 0) {
    s << "Hinge 2, section tag: " << theSections[2]->getTag() <<
      ", length: " << beta2*L << std::endl;
    theSections[1]->Print(s,flag);
  }
}

//////////////////////////////
//Private Function Definitions

void XC::BeamWithHinges3d::checkNodePtrs(Domain *theDomain)
  {
    assert(theNodes[0]);
    assert(theNodes[1]);
    // check for correct # of DOF's
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();
    if((dofNd1 != 6) || (dofNd2 != 6))
      {
        std::cerr << "XC::BeamWithHinges3d::setNodePtrs() -- nodal dof is not three\n";
        exit(-1);
      }
  }

int XC::BeamWithHinges3d::update(void)
{
  // if have completed a recvSelf() - do a revertToLastCommit
  // to get e, kb, etc. set correctly
  if(initialFlag == 2)
    this->revertToLastCommit();

  // Update the coordinate transformation
  theCoordTransf->update();

  // Convert to basic system from local coord's (eliminate rb-modes)
  static XC::Vector v(6);                                // basic system deformations
  v = theCoordTransf->getBasicTrialDisp();

  static XC::Vector dv(6);
  dv = theCoordTransf->getBasicIncrDeltaDisp();

  double L = theCoordTransf->getInitialLength();
  double oneOverL = 1.0/L;

  // Section locations along element length ...
  double xi[2];
  // and their integration weights
  double lp[2];

  lp[0] = beta1*L;
  lp[1] = beta2*L;

  xi[0] = 0.5*lp[0];
  xi[1] = L-0.5*lp[1];

  // element properties
  static XC::Matrix f(6,6);        // element flexibility
  static XC::Vector vr(6);        // Residual element deformations

  static XC::Matrix Iden(6,6);   // an identity matrix for matrix inverse
  Iden.Zero();
  for(int i = 0; i < 6; i++)
    Iden(i,i) = 1.0;

  // Length of elastic interior
  double Le = L-lp[0]-lp[1];
  double LoverEA   = Le/(ctes_scc.EA());
  double Lover3EIz = Le/(3*ctes_scc.EIz());
  double Lover6EIz = 0.5*Lover3EIz;
  double Lover3EIy = Le/(3*ctes_scc.EIy());
  double Lover6EIy = 0.5*Lover3EIy;
  double LoverGJ   = Le/(ctes_scc.GJ());

  // Elastic flexibility of element interior
  static XC::Matrix fe(4,4);
  fe(0,0) = fe(1,1) =  Lover3EIz;
  fe(0,1) = fe(1,0) = -Lover6EIz;
  fe(2,2) = fe(3,3) =  Lover3EIy;
  fe(2,3) = fe(3,2) = -Lover6EIy;

  // Equilibrium transformation matrix
  static XC::Matrix B(4,4);
  B(0,0) = B(2,2) = 1.0 - beta1;
  B(1,1) = B(3,3) = 1.0 - beta2;
  B(0,1) = B(2,3) = -beta1;
  B(1,0) = B(3,2) = -beta2;

  // Transform the elastic flexibility of the element
  // interior to the basic system
  static XC::Matrix fElastic(4,4);
  fElastic.addMatrixTripleProduct(0.0, B, fe, 1.0);

  // calculate nodal force increments and update nodal forces
  static XC::Vector dq(6);
  //dq = kb * dv;   // using previous stiff matrix k,i
  dq.addMatrixVector(0.0, kb, dv, 1.0);

  for(int j = 0; j < maxIter; j++)
    {

    // q += dq;
    q.addVector(1.0, dq, 1.0);

    // Set element flexibility to flexibility of elastic region
    f.Zero();
    f(0,0) = LoverEA;
    f(1,1) = fElastic(0,0);
    f(2,2) = fElastic(1,1);
    f(1,2) = fElastic(0,1);
    f(2,1) = fElastic(1,0);
    f(3,3) = fElastic(2,2);
    f(4,4) = fElastic(3,3);
    f(3,4) = fElastic(2,3);
    f(4,3) = fElastic(3,2);
    f(5,5) = LoverGJ;

    // vr = fElastic * q;
    vr(0) = LoverEA*q(0) + v0[0];
    vr(1) = fElastic(0,0)*q(1) + fElastic(0,1)*q(2) + v0[1];
    vr(2) = fElastic(1,0)*q(1) + fElastic(1,1)*q(2) + v0[2];
    vr(3) = fElastic(2,2)*q(3) + fElastic(2,3)*q(4) + v0[3];
    vr(4) = fElastic(3,2)*q(3) + fElastic(3,3)*q(4) + v0[4];
    vr(5) = LoverGJ*q(5);

    for(int i = 0; i < 2; i++) {

      if(theSections[i] == 0 || lp[i] <= 0.0)
        continue;

      // Get section information
      int order = theSections[i]->getOrder();
      const XC::ID &code = theSections[i]->getType();

      Vector s(workArea, order);
      Vector ds(&workArea[order], order);
      Vector de(&workArea[2*order], order);

      Matrix fb(&workArea[3*order], order, 6);

      double xL = xi[i]*oneOverL;
      double xL1 = xL-1.0;

      int ii;
      // Section forces
      // s = b*q + bp*currDistrLoad;
      //this->getForceInterpMatrix(b, xi[i], code);
      //s.addMatrixVector(0.0, b, q, 1.0);
      for(ii = 0; ii < order; ii++) {
        switch(code(ii)) {
        case SECTION_RESPONSE_P:
          s(ii) = q(0);
          break;
        case SECTION_RESPONSE_MZ:
          s(ii) = xL1*q(1) + xL*q(2);
          break;
        case SECTION_RESPONSE_VY:
          s(ii) = oneOverL*(q(1)+q(2));
          break;
        case SECTION_RESPONSE_MY:
          s(ii) = xL1*q(3) + xL*q(4);
          break;
        case SECTION_RESPONSE_VZ:
          s(ii) = oneOverL*(q(3)+q(4));
          break;
        case SECTION_RESPONSE_T:
          s(ii) = q(5);
          break;
        default:
          s(ii) = 0.0;
          break;
        }
      }

      // Add the effects of element loads, if present
      if(applied_sf != 0)
        {
          const XC::Matrix &s_p = *applied_sf;
          for(ii = 0; ii < order; ii++)
            {
              switch(code(ii))
                {
                case SECTION_RESPONSE_P:
                  s(ii) += s_p(0,i);
                  break;
                case SECTION_RESPONSE_MZ:
                  s(ii) += s_p(1,i);
                  break;
                case SECTION_RESPONSE_VY:
                  s(ii) += s_p(2,i);
                  break;
                case SECTION_RESPONSE_MY:
                  s(ii) += s_p(3,i);
                  break;
                case SECTION_RESPONSE_VZ:
                  s(ii) += s_p(4,i);
                  break;
                case SECTION_RESPONSE_T:
                  s(ii) += s_p(5,i);
                  break;
                default:
                  break;
                }
            }
        }

      // Increment in section forces
      // ds = s - sr
      ds = s;
      ds.addVector(1.0, sr[i], -1.0);

      // compute section deformation increments and update current deformations
      // e += fs * ds;
      de.addMatrixVector(0.0, fs[i], ds, 1.0);
      if(initialFlag != 0)
        e[i].addVector(1.0, de, 1.0);

      // set section deformations
      theSections[i]->setTrialSectionDeformation(e[i]);

      // get section resisting forces
      sr[i] = theSections[i]->getStressResultant();

      // get section flexibility matrix
      fs[i] = theSections[i]->getSectionFlexibility();

      // ds = s - sr;
      ds = s;
      ds.addVector(1.0, sr[i], -1.0);

      de.addMatrixVector(0.0, fs[i], ds, 1.0);

      // integrate section flexibility matrix
      // f += (b^ fs * b) * lp[i];
      //f.addMatrixTripleProduct(1.0, b, fSec, lp[i]);
      int jj;
      fb.Zero();
      double tmp;
      const XC::Matrix &fSec = fs[i];
      for(ii = 0; ii < order; ii++)
        {
          switch(code(ii))
            {
            case SECTION_RESPONSE_P:
              for(jj = 0; jj < order; jj++)
                fb(jj,0) += fSec(jj,ii)*lp[i];
              break;
            case SECTION_RESPONSE_MZ:
              for(jj = 0; jj < order; jj++)
                {
                  tmp= fSec(jj,ii)*lp[i];
                  fb(jj,1) += xL1*tmp;
                  fb(jj,2) += xL*tmp;
                }
              break;
            case SECTION_RESPONSE_VY:
              for(jj = 0; jj < order; jj++)
                {
                  //tmp = oneOverL*fSec(jj,ii)*lp[i]*L/lp[i];
                  tmp= fSec(jj,ii);
                  fb(jj,1) += tmp;
                  fb(jj,2) += tmp;
                }
              break;
            case SECTION_RESPONSE_MY:
              for(jj = 0; jj < order; jj++)
                {
                  tmp = fSec(jj,ii)*lp[i];
                  fb(jj,3) += xL1*tmp;
                  fb(jj,4) += xL*tmp;
                }
              break;
            case SECTION_RESPONSE_VZ:
              for(jj = 0; jj < order; jj++)
                {
                  //tmp = oneOverL*fSec(jj,ii)*lp[i]*L/lp[i];
                  tmp= fSec(jj,ii);
                  fb(jj,3) += tmp;
                  fb(jj,4) += tmp;
                }
              break;
            case SECTION_RESPONSE_T:
              for(jj = 0; jj < order; jj++)
                fb(jj,5) += fSec(jj,ii)*lp[i];
              break;
            default:
              break;
            }
        }
      for(ii = 0; ii < order; ii++)
        {
          switch (code(ii))
            {
            case SECTION_RESPONSE_P:
              for(jj = 0; jj < 6; jj++)
                f(0,jj) += fb(ii,jj);
              break;
            case SECTION_RESPONSE_MZ:
              for(jj = 0; jj < 6; jj++)
                {
                  tmp = fb(ii,jj);
                  f(1,jj) += xL1*tmp;
                  f(2,jj) += xL*tmp;
                }
              break;
            case SECTION_RESPONSE_VY:
              for(jj = 0; jj < 6; jj++)
                {
                  tmp = oneOverL*fb(ii,jj);
                  f(1,jj) += tmp;
                  f(2,jj) += tmp;
                }
              break;
            case SECTION_RESPONSE_MY:
              for(jj = 0; jj < 6; jj++)
                {
                  tmp = fb(ii,jj);
                  f(3,jj) += xL1*tmp;
                  f(4,jj) += xL*tmp;
                }
              break;
            case SECTION_RESPONSE_VZ:
              for(jj = 0; jj < 6; jj++)
                {
                  tmp = oneOverL*fb(ii,jj);
                  f(3,jj) += tmp;
                  f(4,jj) += tmp;
                }
              break;
            case SECTION_RESPONSE_T:
              for(jj = 0; jj < 6; jj++)
                f(5,jj) += fb(ii,jj);
              break;
            default:
              break;
            }
        }

      // UNCOMMENT WHEN DISTRIBUTED LOADS ARE ADDED TO INTERFACE
      // vr.addMatrixVector(1.0, vElastic, currDistrLoad, 1.0);

      // vr += (b^ (e+de)) * lp[i];
      de.addVector(1.0, e[i], 1.0);
      //vr.addMatrixTransposeVector(1.0, b, de, lp[i]);
      for(ii = 0; ii < order; ii++)
        {
          switch(code(ii))
            {
            case SECTION_RESPONSE_P:
              vr(0) += de(ii)*lp[i];
              break;
            case SECTION_RESPONSE_MZ:
              tmp = de(ii)*lp[i];
              vr(1) += xL1*tmp; vr(2) += xL*tmp;
              break;
            case SECTION_RESPONSE_VY:
              //tmp = oneOverL*de(ii)*lp[i]*L/lp[i];
              tmp = de(ii);
              vr(1) += tmp; vr(2) += tmp;
              break;
            case SECTION_RESPONSE_MY:
              tmp = de(ii)*lp[i];
              vr(3) += xL1*tmp; vr(4) += xL*tmp;
              break;
            case SECTION_RESPONSE_VZ:
              //tmp = oneOverL*de(ii)*lp[i]*L/lp[i];
              tmp = de(ii);
              vr(3) += tmp; vr(4) += tmp;
              break;
            case SECTION_RESPONSE_T:
              vr(5) += de(ii)*lp[i];
              break;
            default:
              break;
            }
        }
      }

    // calculate element stiffness matrix
    if(f.Solve(Iden,kb) < 0)
      std::cerr << "XC::BeamWithHinges3d::update() -- could not invert flexibility\n";

    // dv = v - vr;
    dv = v;
    dv.addVector(1.0, vr, -1.0);

    // determine resisting forces
    // dq = kb * dv;
    dq.addMatrixVector(0.0, kb, dv, 1.0);

    double dW = dv^ dq;

    if(fabs(dW) < tolerance)
      break;
  }

  // q += dq;
  q.addVector(1.0, dq, 1.0);

  initialFlag = 1;

  return 0;
}

void XC::BeamWithHinges3d::setHinges(void)
  {
    for(int i = 0; i < 2; i++)
      {
        if(theSections[i] == 0) continue;
        // Get the number of section response quantities
        int order = theSections[i]->getOrder();

        fs[i] = Matrix(order,order);
        e[i]  = Vector(order);
        sr[i] = Vector(order);
        eCommit[i] = Vector(order);
      }
  }

void XC::BeamWithHinges3d::getForceInterpMatrix(Matrix &b, double x, const XC::ID &code)
  {
    b.Zero();

    const double L = theCoordTransf->getInitialLength();
    const double xi = x/L;

    for(int i = 0; i < code.Size(); i++)
      {
        switch (code(i))
          {
          case SECTION_RESPONSE_MZ:                // Moment, Mz, interpolation
            b(i,1) = xi - 1.0;
            b(i,2) = xi;
            break;
          case SECTION_RESPONSE_P:                // Axial, P, interpolation
            b(i,0) = 1.0;
            break;
          case SECTION_RESPONSE_VY:                // Shear, Vy, interpolation
            b(i,1) = b(i,2) = 1.0/L;
            break;
          case SECTION_RESPONSE_MY:                // Moment, My, interpolation
            b(i,3) = xi - 1.0;
            b(i,4) = xi;
            break;
          case SECTION_RESPONSE_VZ:                // Shear, Vz, interpolation
            b(i,3) = b(i,4) = 1.0/L;
            break;
          case SECTION_RESPONSE_T:                // Torsion, T, interpolation
            b(i,5) = 1.0;
            break;
          default:
            break;
          }
      }
  }

void XC::BeamWithHinges3d::getDistrLoadInterpMatrix(Matrix &bp, double x, const XC::ID & code)
  {
    bp.Zero();

    const double L = theCoordTransf->getInitialLength();
    const double xi = x/L;

    for(int i = 0; i < code.Size(); i++)
      {
        switch(code(i))
          {
          case SECTION_RESPONSE_MZ:                // Moment, Mz, interpolation
            bp(i,1) = 0.5*xi*(xi-1);
            break;
          case SECTION_RESPONSE_P:                // Axial, P, interpolation
            bp(i,0) = 1.0-xi;
            break;
          case SECTION_RESPONSE_VY:                // Shear, Vy, interpolation
            bp(i,1) = xi-0.5;
            break;
          case SECTION_RESPONSE_MY:                // Moment, My, interpolation
            bp(i,2) = 0.5*xi*(xi-1);
            break;
          case SECTION_RESPONSE_VZ:                // Shear, Vz, interpolation
            bp(i,2) = xi-0.5;
            break;
          case SECTION_RESPONSE_T:                // Torsion, T, interpolation
            bp(i,3) = 1.0-xi;
            break;
          default:
            break;
          }
      }
  }

XC::Response *XC::BeamWithHinges3d::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    // hinge rotations
    if(argv[0] == "plasticDeformation" || argv[0] == "plasticRotation")
      return new ElementResponse(this, 1, Vector(3));
    // global forces
    else if(argv[0] == "force" || argv[0] == "forces" || argv[0] == "globalForce" || argv[0] == "globalForces")
      return new ElementResponse(this, 2, theVector);
    // stiffness
    else if(argv[0] == "stiffness")
      return new ElementResponse(this, 3, theMatrix);
    // local forces
    else if(argv[0] == "localForce" || argv[0] == "localForces")
      return new ElementResponse(this, 4, theVector);
    // section response
    else if(argv[0] == "section")
      {
        int sectionNum = atoi(argv[1]) - 1;
        if(sectionNum >= 0 && sectionNum < 2)
        if(theSections[sectionNum] != 0)
          {
            std::vector<std::string> argv2(argv);
            argv2.erase(argv2.begin(),argv2.begin()+2);
            return theSections[sectionNum]->setResponse(argv2, info);
          }
        return 0;
      }
    else
      return 0;
  }

int XC::BeamWithHinges3d::getResponse(int responseID, Information &eleInfo)
  {
    double V, N, T, M1, M2;
    const double L = theCoordTransf->getInitialLength();
    static XC::Vector force(12);
    static XC::Vector def(6);

    switch (responseID)
      {
      case 1:
        {
          const XC::Vector &v = theCoordTransf->getBasicTrialDisp();
          const double LoverEA   = L/(ctes_scc.EA());
          const double Lover3EIz = L/(3*ctes_scc.EIz());
          const double Lover6EIz = 0.5*Lover3EIz;
          const double LoverGJ   = L/(ctes_scc.GJ());
          const double Lover3EIy = L/(3*ctes_scc.EIy());
          const double Lover6EIy = 0.5*Lover3EIy;

          const double q1 = qCommit(1);
          const double q2 = qCommit(2);
          const double q3 = qCommit(3);
          const double q4 = qCommit(4);

          def(0) = v(0) - LoverEA*qCommit(0);
          def(1) = v(1) - Lover3EIz*q1 + Lover6EIz*q2;
          def(2) = v(2) + Lover6EIz*q1 - Lover3EIz*q2;
          def(3) = v(3) - Lover3EIy*q3 + Lover6EIy*q4;
          def(4) = v(4) + Lover6EIy*q3 - Lover3EIy*q4;
          def(5) = v(5) - LoverGJ*qCommit(5);
          return eleInfo.setVector(def);
        }
      case 2: // global forces
        return eleInfo.setVector(this->getResistingForce());
      case 3: // stiffness
        return eleInfo.setMatrix(this->getTangentStiff());
      case 4: // local forces
        // Axial
        N = q(0);
        force(6) =  N;
        force(0) = -N+p0[0];
        // Torsion
        T = q(5);
        force(9) =  T;
        force(3) = -T;

        // Moments about z and shears along y
        M1 = q(1);
        M2 = q(2);
        force(5)  = M1;
        force(11) = M2;
        V = (M1+M2)/L;
        force(1) =  V+p0[1];
        force(7) = -V+p0[2];

        // Moments about y and shears along z
        M1 = q(3);
        M2 = q(4);
        force(4)  = M1;
        force(10) = M2;
        V = (M1+M2)/L;
        force(2) = -V+p0[3];
        force(8) =  V+p0[4];
        return eleInfo.setVector(force);
      default:
        return -1;
      }
  }

int XC::BeamWithHinges3d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    // E of the beam interior
    if(argv[0] == "E")
      return param.addObject(1, this);
    // A of the beam interior
    else if(argv[0] == "A")
      return param.addObject(3, this);
    // I of the beam interior
    else if(argv[0] == "Iz")
      return param.addObject(4, this);
    // Section parameter
    else if(argv[0] == "section")
      {
        if(argc <= 2) return -1;
        const int sectionNum = atoi(argv[1]);
        int ok = -1;

        std::vector<std::string> argv2(argv);
        argv2.erase(argv2.begin(),argv2.begin()+2);
        if(sectionNum == 1)
          ok= theSections[0]->setParameter (argv2, param);
        if(sectionNum == 2)
          ok= theSections[1]->setParameter (argv2, param);

        if(ok < 0)
          return -1;
        else if(ok < 100)
          return sectionNum*100 + ok;
        else
          return -1;
      }
    // Unknown parameter
    else
      return -1;
  }

int XC::BeamWithHinges3d::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case 1:
        this->ctes_scc.E() = info.theDouble;
        return 0;
      case 3:
        this->ctes_scc.A() = info.theDouble;
        return 0;
      case 4:
        this->ctes_scc.Iz() = info.theDouble;
        return 0;
      default:
        if(parameterID >= 100)
          { // section quantity
            const int sectionNum = parameterID/100;
            if(sectionNum == 1)
              return theSections[0]->updateParameter (parameterID-100, info);
            if(sectionNum == 2)
              return theSections[1]->updateParameter (parameterID-2*100, info);
            else
              return -1;
          }
        else // unknown
          return -1;
      }
  }
