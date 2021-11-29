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

// $Revision: 1.21 $
// $Date: 2004/06/07 23:20:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beamWithHinges/BeamWithHinges2d.cpp,v $

#include <domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges2d.h>
#include "domain/component/Parameter.h"
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include <utility/matrix/ID.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <cstring>

#include <material/section/PrismaticBarCrossSection.h>
#include <domain/mesh/element/utils/coordTransformation/CrdTransf2d.h>
#include "domain/load/beam_loads/BeamMecLoad.h"

#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include "material/section/repres/CrossSectionProperties2d.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

XC::Matrix XC::BeamWithHinges2d::theMatrix(6,6);
XC::Vector XC::BeamWithHinges2d::theVector(6);
double XC::BeamWithHinges2d::workArea[100];

//! @brief Default Constructor.
//!
//! @param tag: element identifier.
XC::BeamWithHinges2d::BeamWithHinges2d(int tag)
  :BeamColumnWithSectionFDTrf2d(tag, ELE_TAG_BeamWithHinges2d,2),
   ctes_scc(0.0,0.0,0.0),
   beta1(0.0), beta2(0.0), kb(3,3), q(3), kbCommit(3,3), qCommit(3),
   initialFlag(0), maxIter(0), tolerance(0.0), sp()
  {
    load.reset(6);
    p0.zero();
    v0.zero();
  }

//! @brief Constructor.
//!
//! @param tag: element identifier.
XC::BeamWithHinges2d::BeamWithHinges2d(int tag,const Material *mat,const CrdTransf *coordTransf)
  :BeamColumnWithSectionFDTrf2d(tag, ELE_TAG_BeamWithHinges2d,2,mat,coordTransf),
   ctes_scc(0.0,0.0,0.0),
   beta1(0.0), beta2(0.0), kb(3,3), q(3), kbCommit(3,3), qCommit(3),
   initialFlag(0), maxIter(0), tolerance(0.0), sp()
  {
    load.reset(6);
    p0.zero();
    v0.zero();
  }

//! @brief Constructor.
//!
//! @param tag: element identifier.
//! @param nodeI: identifier of the back end node.
//! @param nodeJ: identifier of the front end node.
//! @param E: modulus of elasticity of the interior beam region.
//! @param A: cross-section area of the interior beam region.
//! @param I: second moment area of the interior beam region.
//! @param sectionRefI: section material for the back end hinge.
//! @param lpi: length ratio of the back end hinge.
//! @param sectionRefJ: section material for the front end hinge.
//! @param lpj: length ratio of the front end hinge.
XC::BeamWithHinges2d::BeamWithHinges2d(int tag, int nodeI, int nodeJ,
                                       double e, double a, double i,
                                       PrismaticBarCrossSection &sectionRefI, double lpi,
                                       PrismaticBarCrossSection &sectionRefJ, double lpj,
                                       CrdTransf2d &coordTransf,
                                       double r, int max, double tol)
  :BeamColumnWithSectionFDTrf2d(tag, ELE_TAG_BeamWithHinges2d,2,nodeI,nodeJ,coordTransf),
   ctes_scc(e,a,i),
   beta1(lpi), beta2(lpj), kb(3,3), q(3), kbCommit(3,3), qCommit(3),
   initialFlag(0), maxIter(max), tolerance(tol), sp()
  {
    setRho(r);
    load.reset(6);

    // Get copies of sections
    theSections.setSectionCopy(0,&sectionRefI);
    theSections.setSectionCopy(1,&sectionRefJ);

    // Set up section interpolation and hinge lengths
    this->setHinges();

    p0.zero();
    v0.zero();
  }

//! @brief Virtual constructor.
XC::Element* XC::BeamWithHinges2d::getCopy(void) const
  { return new BeamWithHinges2d(*this); }


//! @brief Returns 6, the number of degrees of freedom for this element. 
int XC::BeamWithHinges2d::getNumDOF(void) const
  { return 6; }

void XC::BeamWithHinges2d::setDomain(Domain *theDomain)
  {
    //This function may be called after a beam is constructed, so
    //geometry may change.  Therefore calculate all beam geometry here.
    BeamColumnWithSectionFDTrf2d::setDomain(theDomain);
    if(theDomain)
      {
        initialize_trf();
        // get element length
        const double L = theCoordTransf->getInitialLength();
        if(L == 0.0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; element has zero length.\n";
            exit(-1);
          }
        if(initialFlag == 2)
          theCoordTransf->update();
        else
          this->update();
      }
  }

//! @brief Invokes commitState() on each section and returns the sum of the
//! result of these invocations.
int XC::BeamWithHinges2d::commitState(void)
  {
    int err = 0;

    // call element commitState to do any base class stuff
    if((err = this->XC::BeamColumnWithSectionFDTrf2d::commitState()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in base class.";
      }

    for(int i = 0; i < 2; i++)
      {
        if(theSections[i] != 0)
          err += theSections[i]->commitState();
      }

    err += theCoordTransf->commitState();

    kbCommit = kb;
    qCommit = q;


    eCommit[0]= e[0];
    eCommit[1]= e[1];

    //initialFlag = 0;
    return err;
  }

//! @brief Invokes revertToLastCommit() on each section and returns the sum of
//! the result of these invocations.
int XC::BeamWithHinges2d::revertToLastCommit(void)
  {
    int err = BeamColumnWithSectionFDTrf2d::revertToLastCommit();

    // Revert the sections and then get their last committed
    // deformations, stress resultants, and flexibilities
    for(int i = 0; i < 2; i++)
      {
        if(theSections[i] != 0)
          {
            err += theSections[i]->revertToLastCommit();
            theSections[i]->setTrialSectionDeformation(eCommit[i]);

            e[i] = eCommit[i];
            sr[i] = theSections[i]->getStressResultant();
            fs[i] = theSections[i]->getSectionFlexibility();
          }
      }

    // Commit the coordinate transformation
    err += theCoordTransf->revertToLastCommit();

    kb = kbCommit;
    q = qCommit;

    initialFlag = 0;
    //   this->update();
    return err;
  }

//! @brief Invokes revertToStart() on each section and returns the sum of the
//! result of these invocations.
int XC::BeamWithHinges2d::revertToStart(void)
  {
    int err= BeamColumnWithSectionFDTrf2d::revertToStart();

    for(int i = 0; i < 2; i++)
      {
        if(theSections[i] != 0)
          {
            err += theSections[i]->revertToStart();
            fs[i].Zero();
            e[i].Zero();
            sr[i].Zero();
            eCommit[i].Zero();
          }
      }
    err += theCoordTransf->revertToStart();
  
    kb.Zero();
    q.Zero();

    initialFlag = 0;
    // this->update();
    return err;
  }

/**
 @brief Computes the element flexibility matrix, then returns its inverse.

 Computes the element flexibility matrix, then returns its inverse, the
 element stiffness matrix. The element flexibility is the sum of the hinge
 flexibilities,
 \f$\mathbf{f}_I\f$ and \f$\mathbf{f}_J\f$, and the elastic flexibility of
 the beam interior, \f$\mathbf{f}_{mid}\f$.

 \begin{equation}
 \label{eq:fele}
 \fbas := \int_{0}^{L}{bint^T fsec bint \: dx} = \mathbf{f}_I + \mathbf{f}_{mid} + \mathbf{f}_J
 \end{equation}
 
 The flexibility of the beam interior is obtained in closed form,
 
 \begin{equation}
 \mathbf{f}_{mid} = \int_{l_I}^{L-l_J}{bint^T fsec_{mid} bint \: dx}
 \end{equation}
 
 where \f$bint\f$ is the force interpolation matrix,
 
 \begin{equation}
 bint := \left[
    \begin{array}{c c c}
       1 &           0 &               0\\
       0 & \frac{x}{L} & \frac{x}{L} - 1\\
       0 & \frac{1}{L} &     \frac{1}{L}
    \end{array} 
  \right]
 \end{equation}
 
  and \f$fsec\f$ is the elastic section flexibility of the beam interior.
 
 \begin{equation}
 fsec_{mid} = \left[
    \begin{array}{c c c}
       \frac{1}{EA} &            0 &                   0\\
                  0 & \frac{1}{EI} &                   0\\
                  0 &            0 & \frac{1}{\alpha GA}
    \end{array}
  \right]
 \end{equation}
 
 The hinge flexibilities, \f$\mathbf{f}_I\f$ and \f$\mathbf{f}_J\f$, are
 obtained by the midpoint integration rule,
 
 \begin{equation}
 {\mathbf{f}}_i = bint(x_i)^T fsec_i bint(x_i) * l_i, \:\: i=I,J
 \end{equation}
 
 where \f$x_i\f$ is the midpoint of hinge \f$i\f$, measured along the length
 of the beam, and is the point at which the force interpolation matrix,
 \f$bint\f$ is evaluated.  The flexiblity, \f$fsec_i\f$, is obtained from
 the constitutive relation for section \f$i\f$.
 
 The element stiffness is then obtained by inversion of the element
 flexibility, given by Equation \ref{eq:fele}.
 
 \begin{equation}
 \label{eq:kele}
 kbas = \fbas^{-1}
 \end{equation}
 
 The element then obtains the matrix, \f$\mathbf{A}\f$, which transforms
 the element basic
 stiffness from its corotating frame to the global frame of reference.  The
 transformed
 stiffness matrix, \f$kele\f$, is then assembled into the structural system
 of equations.
 
 \begin{equation}
 kele = \mathbf{A}^T kbas \mathbf{A}
 \end{equation}
*/
const XC::Matrix &XC::BeamWithHinges2d::getTangentStiff(void) const
  {
    static Matrix K;
    K= theCoordTransf->getGlobalStiffMatrix(kb, q);
    if(isDead())
      K*= dead_srf;
    return K;
  }

const XC::Matrix &XC::BeamWithHinges2d::getInitialStiff(void) const
  {
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
    static Matrix f(3,3);        // element flexibility
    static Vector vr(3);        // Residual element deformations

    static Matrix Iden(3,3);   // an identity matrix for matrix inverse
    Iden.Zero();
    for(int i = 0; i < 3; i++)
      Iden(i,i) = 1.0;

    // Length of elastic interior
    const double Le = L-lp[0]-lp[1];
    const double LoverEA  = Le/ctes_scc.EA();
    const double Lover3EI = Le/(3*ctes_scc.EI());
    const double Lover6EI = 0.5*Lover3EI;

    // Elastic flexibility of element interior
    static Matrix fe(2,2);
    fe(0,0) = fe(1,1) =  Lover3EI;
    fe(0,1) = fe(1,0) = -Lover6EI;

    // Equilibrium transformation matrix
    static Matrix B(2,2);
    B(0,0) = 1.0 - beta1;
    B(1,1) = 1.0 - beta2;
    B(0,1) = -beta1;
    B(1,0) = -beta2;

    // Transform the elastic flexibility of the element
    // interior to the basic system
    static Matrix fElastic(2,2);
    fElastic.addMatrixTripleProduct(0.0, B, fe, 1.0);

    // Set element flexibility to flexibility of elastic region
    f(0,0) = LoverEA;
    f(1,1) = fElastic(0,0);
    f(2,2) = fElastic(1,1);
    f(1,2) = fElastic(0,1);
    f(2,1) = fElastic(1,0);
    f(0,1) = f(1,0) = f(0,2) = f(2,0) = 0.0;

    for(int i = 0; i < 2; i++)
      {
        if(theSections[i] == 0 || lp[i] <= 0.0)
          continue;

        // Get section information
        int order = theSections[i]->getOrder();
        const ID &code = theSections[i]->getType();

        Vector s(workArea, order);
        Vector ds(&workArea[order], order);
        Vector de(&workArea[2*order], order);

        Matrix fb(&workArea[3*order], order, 3);

        const double x= xi[i];
        const double xL= x*oneOverL;
        const double xL1= xL-1.0;

        // get section flexibility matrix
        const Matrix &fSec = theSections[i]->getInitialFlexibility();

        // integrate section flexibility matrix
        // f += (b^ fs * b) * lp[i];
        //f.addMatrixTripleProduct(1.0, b, fSec, lp[i]);
        fb.Zero();
        double tmp;
        for(int ii = 0; ii < order; ii++)
          {
            switch(code(ii))
              {
              case SECTION_RESPONSE_P:
                for(int jj = 0; jj < order; jj++)
                  fb(jj,0) += fSec(jj,ii)*lp[i];
                break;
              case SECTION_RESPONSE_MZ:
                for(int jj = 0; jj < order; jj++)
                  {
                    tmp = fSec(jj,ii)*lp[i];
                    fb(jj,1) += xL1*tmp;
                    fb(jj,2) += xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VY:
                for(int jj = 0; jj < order; jj++)
                  {
                    //tmp = oneOverL*fSec(jj,ii)*lp[i]*L/lp[i];
                    tmp= fSec(jj,ii);
                    fb(jj,1)+= tmp;
                    fb(jj,2) += tmp;
                  }
                break;
              default:
                break;
              }
          }
        for(int ii = 0; ii < order; ii++)
          {
            switch (code(ii))
              {
              case SECTION_RESPONSE_P:
                for(int jj = 0; jj < 3; jj++)
                  f(0,jj) += fb(ii,jj);
                break;
              case SECTION_RESPONSE_MZ:
                for(int jj = 0; jj < 3; jj++)
                  {
                    tmp = fb(ii,jj);
                    f(1,jj) += xL1*tmp;
                    f(2,jj) += xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VY:
                for(int jj = 0; jj < 3; jj++)
                  {
                    tmp= oneOverL*fb(ii,jj);
                    f(1,jj)+= tmp;
                    f(2,jj)+= tmp;
                  }
                break;
              default:
                break;
              }
           }
      }

    // calculate element stiffness matrix
    //invert3by3Matrix(f, kb);
    static Matrix kbInit(3,3);
    if(f.Solve(Iden,kbInit) < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; could not invert flexibility.\n";
    static Matrix K;
    K= theCoordTransf->getInitialGlobalStiffMatrix(kbInit);
    if(isDead())
      K*=dead_srf;
    return K;
  }

/**
 @brief Return the element lumped mass matrix.

 Returns the element lumped mass matrix, \f$mele\f$. It is assumed that the
 mass density per unit length, \f$\rho\f$, is constant along the entire
 element, including the hinge regions.
 
 \begin{equation}
 \label{eq:mele}
 mele = \left[
    \begin{array}{c c c c c c}
       \frac{\rho L}{2} & 0 & 0 & 0 & 0 & 0\\
       0 & \frac{\rho L}{2} & 0 & 0 & 0 & 0\\
       0 & 0 & 0 & 0 & 0 & 0\\
       0 & 0 & 0 & \frac{\rho L}{2} & 0 & 0\\
       0 & 0 & 0 & 0 & \frac{\rho L}{2} & 0\\
       0 & 0 & 0 & 0 & 0 & 0
    \end{array}
  \right]
 \end{equation}
*/
const XC::Matrix &XC::BeamWithHinges2d::getMass(void) const
  {
    theMatrix.Zero();

    const double rho= getRho(); //mass per unit length.
    if(rho != 0.0)
      {
        const double L = theCoordTransf->getInitialLength();
        theMatrix(0,0) = theMatrix(1,1) = theMatrix(3,3) = theMatrix(4,4) = 0.5*L*rho;
      }
    if(isDead())
      theMatrix*=dead_srf;
    return theMatrix;
  }

//! @brief Zero the element load contributions to the residual. 
void  XC::BeamWithHinges2d::zeroLoad(void)
  {
    BeamColumnWithSectionFDTrf2d::zeroLoad();
    if(!sp.isEmpty())
      sp.Zero();
    p0.zero();
    v0.zero();
  }

//! @brief Effect of the load over the element.
int XC::BeamWithHinges2d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
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

            if(sp.isEmpty())
              sp= Matrix(3,2);
            sp+= beamMecLoad->getAppliedSectionForces(L,xi_pt,loadFactor); // Accumulate applied section forces due to element loads
            beamMecLoad->addReactionsInBasicSystem({L},loadFactor,p0); // Accumulate reactions in basic system
            beamMecLoad->addElasticDeformations(L,ctes_scc,lp1,lp2,loadFactor,v0);
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; load type unknown for element with tag: "
		      << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

int XC::BeamWithHinges2d::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    const double rho= getRho(); // mass per unit length.
    if(rho == 0.0)
      return 0;

    const XC::Vector &Raccel1= theNodes[0]->getRV(accel);
    const XC::Vector &Raccel2= theNodes[1]->getRV(accel);

    const double L= theCoordTransf->getInitialLength();
    double mass = 0.5*L*rho;

    int i,j;
    for(i= 0,j= 3;i<2;i++,j++)
      {
        load(i) += mass*Raccel1(i);
        load(j) += mass*Raccel2(i);        // Yes, this should be 'i'
      }
    return 0;
  }

//! Returns the element resisting force vector.
//!
//! Returns the element resisting force vector.  The basic element force vector
//! is obtained as the product of the basic element stiffness, \f$kbas\f$,
//! given by Equation \ref{eq:kele}, and the basic element deformations,
//! \f$vbas\f$.
//! 
//! \begin{equation}
//! \qbas = kbas vbas
//! \end{equation}
//! 
//! The basic element force vector is then transformed from the corotating
//! frame to the global frame
//! of reference.  The transformed force vector is then assembled into the
//! structural system of equations.
//!
//! \begin{equation}
//! \label{eq:qele}
//! qele = \mathbf{A}^T \qbas
//! \end{equation}
const XC::Vector &XC::BeamWithHinges2d::getResistingForce(void) const
  {
    Vector p0Vec= p0.getVector();
    static Vector retval;
    retval= theCoordTransf->getGlobalResistingForce(q, p0Vec);
    if(isDead())
      retval*=dead_srf;
    return retval;
  }

//! Returns the element resisting force vector.
//!
//! Returns the element resisting force vector, \f$\tilde{qele}\f$ with
//! inertia forces included,
//! 
//! \begin{equation}
//! \tilde{qele} = qele - mele \ddot{\mathbf u}
//! \end{equation}
//! 
//! where \f$qele\f$ and \f$mele\f$ are obtained from Equations \ref{eq:qele}
//! and \ref{eq:mele}, respectively, and \f$\ddot{\mathbf u}\f$ is the vector
//! of trial nodal accelerations for the element.
const XC::Vector &XC::BeamWithHinges2d::getResistingForceIncInertia(void) const
  {
    theVector=  this->getResistingForce();

    const double rho= getRho(); // mass per unit length.
    if(rho != 0.0)
      {

        double ag[6];
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();

        ag[0] = accel1(0);
        ag[1] = accel1(1);
        //ag[2] = accel1(2); // no rotational element mass
        ag[3] = accel2(0);
        ag[4] = accel2(1);
        //ag[5] = accel2(2); // no rotational element mass

        theVector = this->getResistingForce();

        const double L = theCoordTransf->getInitialLength();
        const double mass= 0.5*L*rho;

        for(int i = 0, j = 3; i < 2; i++, j++)
          {
            theVector(i)+= mass*ag[i];
            theVector(j)+= mass*ag[j];
          }

        // add the damping forces if rayleigh damping
        if(!rayFactors.nullValues())
          theVector += this->getRayleighDampingForces();
      }
    else
      {
        // add the damping forces if rayleigh damping
        if(!rayFactors.nullKValues())
         theVector += this->getRayleighDampingForces();
      }
    if(isDead())
      theVector*=dead_srf; //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
    return theVector;
  }

//! @brief Send members through the communicator argument.
int XC::BeamWithHinges2d::sendData(Communicator &comm)
  {
    int res= BeamColumnWithSectionFDTrf2d::sendData(comm);
    res+= comm.sendMovable(ctes_scc,getDbTagData(),CommMetaData(12));
    res+= comm.sendDoubles(beta1,beta2,getDbTagData(),CommMetaData(13));
    res+= comm.sendMatrix(fs[0],getDbTagData(),CommMetaData(14));
    res+= comm.sendMatrix(fs[1],getDbTagData(),CommMetaData(15));
    res+= comm.sendVector(sr[0],getDbTagData(),CommMetaData(16));
    res+= comm.sendVector(sr[1],getDbTagData(),CommMetaData(17));
    res+= comm.sendVector(e[0],getDbTagData(),CommMetaData(18));
    res+= comm.sendVector(e[1],getDbTagData(),CommMetaData(19));
    res+= comm.sendMatrix(kb,getDbTagData(),CommMetaData(20));
    res+= comm.sendVector(q,getDbTagData(),CommMetaData(21));
    res+= comm.sendMatrix(kbCommit,getDbTagData(),CommMetaData(22));
    res+= comm.sendVector(qCommit,getDbTagData(),CommMetaData(23));
    res+= comm.sendVector(eCommit[0],getDbTagData(),CommMetaData(24));
    res+= comm.sendVector(eCommit[1],getDbTagData(),CommMetaData(25));
    res+= comm.sendInts(initialFlag,maxIter,getDbTagData(),CommMetaData(26));
    res+= comm.sendDouble(tolerance,getDbTagData(),CommMetaData(27));
    res+= comm.sendMatrix(sp,getDbTagData(),CommMetaData(28));
    res+= p0.sendData(comm,getDbTagData(),CommMetaData(29));
    res+= v0.sendData(comm,getDbTagData(),CommMetaData(30));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::BeamWithHinges2d::recvData(const Communicator &comm)
  {
    int res= BeamColumnWithSectionFDTrf2d::recvData(comm);
    res+= comm.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(12));
    res+= comm.receiveDoubles(beta1,beta2,getDbTagData(),CommMetaData(13));
    res+= comm.receiveMatrix(fs[0],getDbTagData(),CommMetaData(14));
    res+= comm.receiveMatrix(fs[1],getDbTagData(),CommMetaData(15));
    res+= comm.receiveVector(sr[0],getDbTagData(),CommMetaData(16));
    res+= comm.receiveVector(sr[1],getDbTagData(),CommMetaData(17));
    res+= comm.receiveVector(e[0],getDbTagData(),CommMetaData(18));
    res+= comm.receiveVector(e[1],getDbTagData(),CommMetaData(19));
    res+= comm.receiveMatrix(kb,getDbTagData(),CommMetaData(20));
    res+= comm.receiveVector(q,getDbTagData(),CommMetaData(21));
    res+= comm.receiveMatrix(kbCommit,getDbTagData(),CommMetaData(22));
    res+= comm.receiveVector(qCommit,getDbTagData(),CommMetaData(23));
    res+= comm.receiveVector(eCommit[0],getDbTagData(),CommMetaData(24));
    res+= comm.receiveVector(eCommit[1],getDbTagData(),CommMetaData(25));
    res+= comm.receiveInts(initialFlag,maxIter,getDbTagData(),CommMetaData(26));
    res+= comm.receiveDouble(tolerance,getDbTagData(),CommMetaData(27));
    res+= comm.receiveMatrix(sp,getDbTagData(),CommMetaData(28));
    res+= p0.receiveData(comm,getDbTagData(),CommMetaData(29));
    res+= v0.receiveData(comm,getDbTagData(),CommMetaData(30));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::BeamWithHinges2d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(31);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::BeamWithHinges2d::recvSelf(const Communicator &comm)
  {
    inicComm(31);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

void XC::BeamWithHinges2d::Print(std::ostream &s, int flag) const
  {
    s << "\nBeamWithHinges2d, tag: " << this->getTag() << std::endl;
    s << "\tConnected Nodes: " << theNodes;
    s << "\tmechanical properties: " << ctes_scc << std::endl;

    double P, V, M1, M2;
    double L = theCoordTransf->getInitialLength();
    P = qCommit(0);
    M1 = qCommit(1);
    M2 = qCommit(2);
    V = (M1+M2)/L;

    s << "\tEnd 1 Forces (P V M): "
      << -P+p0[0] << ' ' <<  V+p0[1] << ' ' << M1 << std::endl;
    s << "\tEnd 2 Forces (P V M): "
      <<  P << ' ' << -V+p0[2] << ' ' << M2 << std::endl;

    if(theSections[0] != 0)
      {
        s << "Hinge 1, section tag: " << theSections[0]->getTag() <<
             ", length: " << beta1*L << std::endl;
        theSections[0]->Print(s,flag);
      }

    if(theSections[1] != 0)
      {
       s << "Hinge 2, section tag: " << theSections[2]->getTag() <<
            ", length: " << beta2*L << std::endl;
       theSections[1]->Print(s,flag);
      }
  }

//////////////////////////////
//Private Function Definitions

int XC::BeamWithHinges2d::update(void)
  {
    // if have completed a recvSelf() - do a revertToLastCommit
    // to get e, kb, etc. set correctly
    if(initialFlag == 2)
      this->revertToLastCommit();

    // Update the coordinate transformation
    theCoordTransf->update();

    // Convert to basic system from local coord's (eliminate rb-modes)
    static Vector v(3); // basic system deformations
    v = theCoordTransf->getBasicTrialDisp();

    static XC::Vector dv(3);
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
    static XC::Matrix f(3,3);        // element flexibility
    static XC::Vector vr(3);        // Residual element deformations

    static XC::Matrix Iden(3,3);   // an identity matrix for matrix inverse
    Iden.Zero();
    for(int i = 0; i < 3; i++)
      Iden(i,i) = 1.0;

    // Length of elastic interior
    const double Le = L-lp[0]-lp[1];
    const double LoverEA  = Le/(ctes_scc.EA());
    const double Lover3EI = Le/(3*ctes_scc.EI());
    const double Lover6EI = 0.5*Lover3EI;

    // Elastic flexibility of element interior
    static XC::Matrix fe(2,2);
    fe(0,0) = fe(1,1) =  Lover3EI;
    fe(0,1) = fe(1,0) = -Lover6EI;

    // Equilibrium transformation matrix
    static XC::Matrix B(2,2);
    B(0,0) = 1.0 - beta1;
    B(1,1) = 1.0 - beta2;
    B(0,1) = -beta1;
    B(1,0) = -beta2;

    // Transform the elastic flexibility of the element
    // interior to the basic system
    static XC::Matrix fElastic(2,2);
    fElastic.addMatrixTripleProduct(0.0, B, fe, 1.0);

    // calculate nodal force increments and update nodal forces
    static XC::Vector dq(3);
    //dq = kb * dv;   // using previous stiff matrix k,i
    dq.addMatrixVector(0.0, kb, dv, 1.0);

    int converged = 0;

    for(int j = 0; j < maxIter; j++)
      {

        // q += dq;
        q.addVector(1.0, dq, 1.0);

        // Set element flexibility to flexibility of elastic region
        f(0,0) = LoverEA;
        f(1,1) = fElastic(0,0);
        f(2,2) = fElastic(1,1);
        f(1,2) = fElastic(0,1);
        f(2,1) = fElastic(1,0);
        f(0,1) = f(1,0) = f(0,2) = f(2,0) = 0.0;

        // vr = fElastic*q + v0;
        vr(0) = LoverEA*q(0) + v0[0];
        vr(1) = fElastic(0,0)*q(1) + fElastic(0,1)*q(2) + v0[1];
        vr(2) = fElastic(1,0)*q(1) + fElastic(1,1)*q(2) + v0[2];

        for(int i = 0; i < 2; i++)
          {

            if(theSections[i] == 0 || lp[i] <= 0.0)
              continue;

            // Get section information
            int order = theSections[i]->getOrder();
            const XC::ID &code = theSections[i]->getType();

            Vector s(workArea, order);
            Vector ds(&workArea[order], order);
            Vector de(&workArea[2*order], order);

            Matrix fb(&workArea[3*order], order, 3);

            double x   = xi[i];
            double xL  = x*oneOverL;
            double xL1 = xL-1.0;

            int ii;
            // Section forces
            // s = b*q + bp*w;
            //this->getForceInterpMatrix(b, xi[i], code);
            //s.addMatrixVector(0.0, b, q, 1.0);
            for(ii = 0; ii < order; ii++)
              {
                switch(code(ii))
                  {
                  case SECTION_RESPONSE_P:
                    s(ii) = q(0);
                    break;
                  case SECTION_RESPONSE_MZ:
                    s(ii) = xL1*q(1) + xL*q(2);
                    break;
                  case SECTION_RESPONSE_VY:
                    s(ii) = oneOverL*(q(1)+q(2));
                    break;
                  default:
                    s(ii) = 0.0;
                    break;
                  }
              }

            // Add the effects of element loads, if present
            if(!sp.isEmpty())
              {
                for(ii = 0; ii < order; ii++)
                  {
                    switch(code(ii))
                      {
                      case SECTION_RESPONSE_P:
                        s(ii) += sp(0,i);
                        break;
                      case SECTION_RESPONSE_MZ:
                        s(ii) += sp(1,i);
                        break;
                      case SECTION_RESPONSE_VY:
                        s(ii) += sp(2,i);
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
                  default:
                    break;
                  }
              }
            for(ii = 0; ii < order; ii++)
              {
                switch(code(ii))
                  {
                  case SECTION_RESPONSE_P:
                    for(jj = 0; jj < 3; jj++)
                      f(0,jj) += fb(ii,jj);
                    break;
                  case SECTION_RESPONSE_MZ:
                    for(jj = 0; jj < 3; jj++)
                      {
                        tmp = fb(ii,jj);
                        f(1,jj)+= xL1*tmp;
                        f(2,jj) += xL*tmp;
                      }
                    break;
                  case SECTION_RESPONSE_VY:
                    for(jj = 0; jj < 3; jj++)
                      {
                        tmp = oneOverL*fb(ii,jj);
                        f(1,jj) += tmp;
                        f(2,jj) += tmp;
                      }
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
                    vr(0) += de(ii)*lp[i]; break;
                  case SECTION_RESPONSE_MZ:
                    tmp = de(ii)*lp[i];
                    vr(1) += xL1*tmp; vr(2) += xL*tmp; break;
                  case SECTION_RESPONSE_VY:
                    //tmp = oneOverL*de(ii)*lp[i]*L/lp[i];
                    tmp = de(ii);
                    vr(1) += tmp; vr(2) += tmp; break;
                  default:
                    break;
                  }
              }
          }
        // calculate element stiffness matrix
        //invert3by3Matrix(f, kb);
        if(f.Solve(Iden,kb) < 0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; could not invert flexibility.\n";


        // dv = v - vr;
        dv = v;
        dv.addVector(1.0, vr, -1.0);


        // determine resisting forces
        // dq = kb * dv;
        dq.addMatrixVector(0.0, kb, dv, 1.0);

        double dW = dv^ dq;

        if(fabs(dW) < tolerance)
          break;

        if((maxIter != 1) && (j == (maxIter - 1)))
          { converged = -1; }
      }
    // q += dq;
    q.addVector(1.0, dq, 1.0);

    initialFlag = 1;

    return 0;
  }

void XC::BeamWithHinges2d::setHinges(void)
  {
    for(int i = 0; i < 2; i++)
      {
        if(theSections[i] == 0)
          continue;

        // Get the number of section response quantities
        int order = theSections[i]->getOrder();

        fs[i] = Matrix(order,order);
        e[i]  = Vector(order);
        sr[i] = Vector(order);
        eCommit[i] = Vector(order);
      }
  }

void XC::BeamWithHinges2d::getForceInterpMatrix(Matrix &b, double x, const XC::ID &code)
  {
    b.Zero();

    const double L = theCoordTransf->getInitialLength();
    double xi = x/L;

    for(int i = 0; i < code.Size(); i++)
      {
	switch (code(i))
	  {
	  case SECTION_RESPONSE_MZ: // Moment, Mz, interpolation
	    b(i,1) = xi - 1.0;
	    b(i,2) = xi;
	    break;
	  case SECTION_RESPONSE_P: // Axial, P, interpolation
	    b(i,0) = 1.0;
	    break;
	  case SECTION_RESPONSE_VY: // Shear, Vy, interpolation
	    b(i,1) = b(i,2) = 1.0/L;
	    break;
	  default:
	    break;
	  }
      }
  }

void XC::BeamWithHinges2d::getDistrLoadInterpMatrix(Matrix &bp, double x, const XC::ID & code)
  {
    bp.Zero();

    const double L = theCoordTransf->getInitialLength();
    double xi = x/L;

    for(int i = 0; i < code.Size(); i++)
      {
	switch (code(i))
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
	  default:
	    break;
	  }
      }
  }

XC::Response* XC::BeamWithHinges2d::setResponse(const std::vector<std::string> &argv, Information &info)
  {
  // hinge rotations
  if(argv[0] == "plasticDeformation" || argv[0] == "plasticRotation")
    return new ElementResponse(this, 1, Vector(3));

  // global forces
  else if(argv[0] == "force" || argv[0] == "forces" ||
           argv[0] == "globalForce" || argv[0] == "globalForces")
    return new ElementResponse(this, 2, theVector);

  // stiffness
  else if(argv[0] == "stiffness")
    return new ElementResponse(this, 3, theMatrix);

  // local forces
  else if(argv[0] == "localForce" || argv[0] == "localForces")
    return new ElementResponse(this, 4, theVector);

  // section response
  else if(argv[0] == "section") {
    int sectionNum = atoi(argv[1]) - 1;

    if(sectionNum >= 0 && sectionNum < 2)
      if(theSections[sectionNum] != 0)
        {
          std::vector<std::string> argv2(argv);
          argv2.erase(argv2.begin(),argv2.begin()+2);
          return theSections[sectionNum]->setResponse(argv2,info);
        }
    return 0;
  }
  else
    return 0;
  }

int XC::BeamWithHinges2d::getResponse(int responseID, Information &eleInfo)
  {
    const double L = theCoordTransf->getInitialLength();
    static Vector force(6);
    static Vector def(3);
    double V= 0.0;
    switch (responseID)
      {
      case 1:
        {
          const Vector &v= theCoordTransf->getBasicTrialDisp();
          const double LoverEA  = L/(ctes_scc.EA());
          const double Lover3EI = L/(3*ctes_scc.EI());
          const double Lover6EI = 0.5*Lover3EI;

          const double q1 = qCommit(1);
          const double q2 = qCommit(2);

          def(0) = v(0) - LoverEA*qCommit(0);
          def(1) = v(1) - Lover3EI*q1 + Lover6EI*q2;
          def(2) = v(2) + Lover6EI*q1 - Lover3EI*q2;

          return eleInfo.setVector(def);
        }

      case 2: // global forces
        return eleInfo.setVector(this->getResistingForce());

      case 3: // stiffness
        return eleInfo.setMatrix(this->getTangentStiff());

      case 4: // local forces
        // Axial
        force(3) =  q(0);
        force(0) = -q(0)+p0[0];
        // Moment
        force(2) = q(1);
        force(5) = q(2);
        // Shear
        V = (q(1)+q(2))/L;
        force(1) =  V+p0[1];
        force(4) = -V+p0[2];
        return eleInfo.setVector(force);
      default:
        return -1;
      }
  }

int XC::BeamWithHinges2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    
    if(argv[0] == "E") //E of the beam interior
      return param.addObject(1, this);
    else if(argv[0] == "A") //A of the beam interior
      return param.addObject(3, this);
    else if(argv[0] == "I")// I of the beam interior
      return param.addObject(4, this);
    else if(argv[0] == "section") // Section parameter
      {
        if(argc <= 2)
          return -1;

        const int sectionNum= atoi(argv[1]);

        int ok = -1;

        std::vector<std::string> argv2(argv);
        argv2.erase(argv2.begin(),argv2.begin()+2);
        if(sectionNum == 1)
          ok = theSections[0]->setParameter (argv2, param);
        if(sectionNum == 2)
          ok = theSections[1]->setParameter (argv2, param);

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

int XC::BeamWithHinges2d::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case 1:
      case 3:
      case 4:
        return ctes_scc.updateParameter(parameterID,info);
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
