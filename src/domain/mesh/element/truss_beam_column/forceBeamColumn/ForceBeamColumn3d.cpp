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

// $Revision: 1.18 $
// $Date: 2004/10/30 00:05:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/ForceBeamColumn3d.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn3d.h>
#include "domain/component/Parameter.h"
#include <domain/mesh/element/utils/Information.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <utility/recorder/response/ElementResponse.h>
#include <domain/mesh/element/utils/coordTransformation/CrdTransf3d.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h>
#include <material/section/PrismaticBarCrossSection.h>

#include <cfloat>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"


#include "material/section/ResponseId.h"


void XC::ForceBeamColumn3d::free_mem(void)
  {
    if(beamIntegr)
      delete beamIntegr;
    beamIntegr= nullptr;
  }

void XC::ForceBeamColumn3d::alloc(const BeamIntegration &bi)
  {
    free_mem();
    beamIntegr= bi.getCopy();
  }

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::ForceBeamColumn3d::ForceBeamColumn3d(int tag)
  : NLForceBeamColumn3dBase(tag,ELE_TAG_ForceBeamColumn3d), beamIntegr(nullptr), v0()
  {}

//! @brief Copy constructor.
XC::ForceBeamColumn3d::ForceBeamColumn3d(const ForceBeamColumn3d &other)
  : NLForceBeamColumn3dBase(other), beamIntegr(nullptr), v0(other.v0), maxSubdivisions(other.maxSubdivisions)
  {
    if(other.beamIntegr)
      alloc(*other.beamIntegr);
  }

//! @brief Assignment operator.
XC::ForceBeamColumn3d &XC::ForceBeamColumn3d::operator=(const ForceBeamColumn3d &other)
  {
//     NLForceBeamColumn3dBase::operator=(other);
//     if(other.beamIntegr)
//       alloc(*other.beamIntegr);
//     v0= other.v0;
//     maxSubdivisions= other.maxSubdivisions;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; assignment operator must not be called."
              << std::endl;
    return *this;    
  }

//! @brief Constructor.
XC::ForceBeamColumn3d::ForceBeamColumn3d(int tag, int numSec, const Material *m,const CrdTransf *coordTransf,const BeamIntegration *integ)
  : NLForceBeamColumn3dBase(tag,ELE_TAG_ForceBeamColumn3d,numSec,m,coordTransf), beamIntegr(nullptr), v0()
  {
    if(integ) alloc(*integ);
  }

// constructor which takes the unique element tag, sections,
// and the node XC::ID's of it's nodal end points.
// allocates the necessary space needed by each object
XC::ForceBeamColumn3d::ForceBeamColumn3d (int tag, int nodeI, int nodeJ,
                                      int numSec,const std::vector<PrismaticBarCrossSection *> &sec,
                                      BeamIntegration &bi,
                                      CrdTransf3d &coordTransf, double massDensPerUnitLength,
                                      int maxNumIters, double tolerance):
  NLForceBeamColumn3dBase(tag,ELE_TAG_ForceBeamColumn3d, numSec), beamIntegr(nullptr),v0()
  {
    theNodes.set_id_nodes(nodeI,nodeJ);

    alloc(bi);
    if(!beamIntegr)
      {
        std::cerr << "Error: XC::ForceBeamColumn3d::ForceBeamColumn3d: could not create copy of beam integration object" << std::endl;
        exit(-1);
      }

    // get copy of the transformation object
    set_transf(&coordTransf);

    this->setSectionPointers(sec);
  }

//! @brief Virtual constructor.
XC::Element* XC::ForceBeamColumn3d::getCopy(void) const
  { return new ForceBeamColumn3d(*this); }

//! @brief Destructor.
XC::ForceBeamColumn3d::~ForceBeamColumn3d(void)
  { free_mem(); }

void XC::ForceBeamColumn3d::setDomain(Domain *theDomain)
  {
    NLForceBeamColumn3dBase::setDomain(theDomain);

    // ensure connected nodes have correct number of dof's
    const int dofNode1 = theNodes[0]->getNumberDOF();
    const int dofNode2 = theNodes[1]->getNumberDOF();

    if((dofNode1 != NND) || (dofNode2 != NND))
      {
        std::cerr << "XC::ForceBeamColumn3d::setDomain(): Nd2 or Nd1 incorrect dof ";
        exit(0);
      }

    // initialize the transformation
    initialize_trf();

    // get element length
    const double L = theCoordTransf->getInitialLength();
    if(L == 0.0)
      {
        std::cerr << "XC::ForceBeamColumn3d::setDomain(): Zero element length:" << this->getTag();
        exit(0);
      }

    if(initialFlag == 0)
      this->initializeSectionHistoryVariables();
  }

//! @brief Consuma la coordinate transformation de acuerdo con el estado actual.
int XC::ForceBeamColumn3d::commitState(void)
  {
    int err = 0;
    size_t i = 0;

    // call element commitState to do any base class stuff
    if((err = this->NLForceBeamColumn3dBase::commitState()) != 0)
      { std::cerr << "ForceBeamColumn3d::commitState () - failed in base class"; }

    const size_t numSections= getNumSections();
    do
      {
        vscommit[i] = vs[i];
        i++;
      }
    while (err == 0 && i < numSections);

    if(err)
      return err;

    // commit the transformation between coord. systems
    if((err = theCoordTransf->commitState()) != 0)
      return err;

    // commit the element variables state
    kvcommit= kv;
    Secommit= Se;

    //   initialFlag= 0;  fmk - commented out, see what happens to Example3.1.tcl if uncommented
    //                         - i have not a clue why, ask remo if he ever gets in contact with us again!
    return err;
  }

//! @brief Returns to the last committed state.
int XC::ForceBeamColumn3d::revertToLastCommit(void)
  {
    int err;
    size_t i = 0;

    const size_t numSections= getNumSections();
    do
      {
        vs[i]= vscommit[i];
        err= theSections[i]->revertToLastCommit();

        theSections[i]->setTrialSectionDeformation(vs[i]);
        Ssr[i]= theSections[i]->getStressResultant();
        fs[i]= theSections[i]->getSectionFlexibility();
        i++;
      }
    while(err == 0 && i<numSections);

    if(err)
      return err;

    // revert the transformation to last commit
    if((err = theCoordTransf->revertToLastCommit()) != 0)
      return err;

    // revert the element state to last commit
    Se= Secommit;
    kv= kvcommit;

    initialFlag= 0;
    // this->update();
    return err;
  }

//! @brief Returns to the initial state.
int XC::ForceBeamColumn3d::revertToStart(void)
  {
    // revert the sections state to start
    int err;
    size_t i = 0;

    const size_t numSections= getNumSections();
    do
      {
        fs[i].Zero();
        vs[i].Zero();
        Ssr[i].Zero();
        err= theSections[i++]->revertToStart();
      }
    while(err == 0 && i<numSections);

    if(err)
      return err;

    // revert the transformation to start
    if((err = theCoordTransf->revertToStart()) != 0)
      return err;

    // revert the element state to start
    Secommit.Zero();
    kvcommit.Zero();

    Se.Zero();
    kv.Zero();
    initialFlag= 0;
    // this->update();
    return err;
  }


const XC::Matrix &XC::ForceBeamColumn3d::getInitialStiff(void) const
  {
    // check for quick return
    if(Ki.isEmpty())
      {
        static Matrix f(NEBD,NEBD);   // element flexibility matrix
        this->getInitialFlexibility(f);

        static Matrix I(NEBD,NEBD);   // an identity matrix for matrix inverse
        I.Zero();
        for(size_t i=0; i<NEBD; i++)
          I(i,i) = 1.0;

        // calculate element stiffness matrix
        // invert3by3Matrix(f, kv);
        static Matrix kvInit(NEBD, NEBD);
        if(f.Solve(I, kvInit) < 0)
          std::cerr << "%s -- could not invert flexibility, ForceBeamColumn3d::getInitialStiff()\n";
        Ki= Matrix(theCoordTransf->getInitialGlobalStiffMatrix(kvInit));
      }
    return Ki;
  }

// NEWTON , SUBDIVIDE AND INITIAL ITERATIONS
int XC::ForceBeamColumn3d::update(void)
  {
    // if have completed a recvSelf() - do a revertToLastCommit
    // to get Ssr, etc. set correctly
    if(initialFlag == 2)
      this->revertToLastCommit();

    // update the transformation
    theCoordTransf->update();

    // get basic displacements and increments
    const Vector &v = theCoordTransf->getBasicTrialDisp();

    static Vector dv(NEBD);
    dv = theCoordTransf->getBasicIncrDeltaDisp();

    if(initialFlag != 0 && dv.Norm() <= DBL_EPSILON && sp.isEmpty())
      return 0;

    static Vector vin(NEBD);
    vin = v;
    vin -= dv;
    const double L= theCoordTransf->getInitialLength();
    const double oneOverL= 1.0/L;

    const size_t numSections= getNumSections();
    double xi[SectionMatrices::maxNumSections];
    beamIntegr->getSectionLocations(numSections, L, xi);

    double wt[SectionMatrices::maxNumSections];
    beamIntegr->getSectionWeights(numSections, L, wt);

    static Vector vr(NEBD);       // element residual displacements
    static Matrix f(NEBD,NEBD);   // element flexibility matrix

    static Matrix I(NEBD,NEBD);   // an identity matrix for matrix inverse
    double dW= 0.0;                    // section strain energy (work) norm

    I.Zero();
    for(size_t i=0; i<NEBD; i++)
      I(i,i) = 1.0;

    int numSubdivide = 1;
    bool converged = false;
    static Vector dSe(NEBD);
    static Vector dvToDo(NEBD);
    static Vector dvTrial(NEBD);
    static EsfBeamColumn3d SeTrial;
    static Matrix kvTrial(NEBD, NEBD);

    dvToDo = dv;
    dvTrial = dvToDo;

    static double factor = 10;
    double dW0 = 0.0;

    maxSubdivisions= 10;

    // fmk - modification to get compatible ele forces and deformations
    //   for a change in deformation dV we try first a newton iteration, if
    //   that fails we try an initial flexibility iteration on first iteration
    //   and then regular newton, if that fails we use the initial flexiblity
    //   for all iterations.
    //
    //   if they both fail we subdivide dV & try to get compatible forces
    //   and deformations. if they work and we have subdivided we apply
    //   the remaining dV.

    while(converged == false && numSubdivide <= maxSubdivisions)
      {
        // try regular newton (if l==0), or
        // initial tangent iterations (if l==1), or
        // initial tangent on first iteration then regular newton (if l==2)

        for(int l=0;l<3;l++)
          {
            //      if(l == 1) l = 2;
            SeTrial = Se;
            kvTrial = kv;
            for(size_t i=0; i<numSections; i++)
              {
                section_matrices.getVsSubdivide()[i] = vs[i];
                section_matrices.getFsSubdivide()[i] = fs[i];
                section_matrices.getSsrSubdivide()[i] = Ssr[i];
              }

            // calculate nodal force increments and update nodal forces
            // dSe = kv * dv;
            dSe.addMatrixVector(0.0, kvTrial, dvTrial, 1.0);
            SeTrial += dSe;

            if(initialFlag != 2)
              {
                int numIters = maxIters;
                if(l == 1)
                  numIters = 10*maxIters; // allow 10 times more iterations for initial tangent

                for(int j=0; j <numIters; j++)
                  {
                    // initialize f and vr for integration
                    f.Zero();
                    vr.Zero();

                    if(beamIntegr->addElasticFlexibility(L, f) < 0)
                      {
                        vr(0) += f(0,0)*SeTrial(0);
                        vr(1) += f(1,1)*SeTrial(1) + f(1,2)*SeTrial(2);
                        vr(2) += f(2,1)*SeTrial(1) + f(2,2)*SeTrial(2);
                        vr(3) += f(3,3)*SeTrial(3) + f(3,4)*SeTrial(4);
                        vr(4) += f(4,3)*SeTrial(3) + f(4,4)*SeTrial(4);
                        vr(5) += f(5,5)*SeTrial(5);
                      }

                   // Add effects of element loads
                   vr(0)+= v0[0];
                   vr(1)+= v0[1];
                   vr(2)+= v0[2];
                   vr(3)+= v0[3];
                   vr(4)+= v0[4];

                   for(size_t i=0; i<numSections; i++)
                     {
                       const int order= theSections[i]->getOrder();
                       const ID &code = theSections[i]->getType();

                       static Vector Ss;
                       static Vector dSs;
                       static Vector dvs;
                       static Matrix fb;

                        Ss.setData(workArea, order);
                        dSs.setData(&workArea[order], order);
                        dvs.setData(&workArea[2*order], order);
                        fb.setData(&workArea[3*order], order, NEBD);

                        double xL= xi[i];
                        double xL1 = xL-1.0;
                        double wtL = wt[i]*L;

                        // calculate total section forces
                        // Ss = b*Se + bp*currDistrLoad;
                        // Ss.addMatrixVector(0.0, b[i], Se, 1.0);
                        for(int ii = 0; ii < order; ii++)
                          {
                            switch(code(ii))
                              {
                              case SECTION_RESPONSE_P:
                                Ss(ii) = SeTrial(0);
                                break;
                              case SECTION_RESPONSE_MZ:
                                Ss(ii) = xL1*SeTrial(1) + xL*SeTrial(2);
                                break;
                              case SECTION_RESPONSE_VY:
                                Ss(ii) = oneOverL*(SeTrial(1)+SeTrial(2));
                                break;
                              case SECTION_RESPONSE_MY:
                                Ss(ii) = xL1*SeTrial(3) + xL*SeTrial(4);
                                break;
                              case SECTION_RESPONSE_VZ:
                                Ss(ii) = oneOverL*(SeTrial(3)+SeTrial(4));
                                break;
                              case SECTION_RESPONSE_T:
                                Ss(ii) = SeTrial(5);
                                break;
                              default:
                                Ss(ii) = 0.0;
                                break;
                              }
                          }

                        // Add the effects of element loads, if present
                        if(!sp.isEmpty())
                          {
                            const Matrix &s_p= sp;
                            for(int ii = 0; ii < order; ii++)
                              {
                                switch(code(ii))
                                  {
                                  case SECTION_RESPONSE_P:
                                    Ss(ii)+= s_p(0,i);
                                    break;
                                  case SECTION_RESPONSE_MZ:
                                    Ss(ii)+= s_p(1,i);
                                    break;
                                  case SECTION_RESPONSE_VY:
                                    Ss(ii)+= s_p(2,i);
                                    break;
                                  case SECTION_RESPONSE_MY:
                                    Ss(ii)+= s_p(3,i);
                                    break;
                                  case SECTION_RESPONSE_VZ:
                                    Ss(ii)+= s_p(4,i);
                                    break;
                                  default:
                                    break;
                                  }
                              }
                           }
                        // dSs = Ss - Ssr[i];
                        dSs = Ss;
                        dSs.addVector(1.0, section_matrices.getSsrSubdivide()[i], -1.0);

                        // compute section deformation increments
                        if(l == 0)
                          {
                            //  regular newton
                            //  vs += fs * dSs;
                            dvs.addMatrixVector(0.0, section_matrices.getFsSubdivide()[i], dSs, 1.0);
                          }
                        else if(l == 2)
                          {
                            //  newton with initial tangent if first iteration
                            //    vs += fs0 * dSs;
                            //  otherwise regular newton
                            //    vs += fs * dSs;

                            if(j == 0)
                              {
                                const Matrix &fs0= theSections[i]->getInitialFlexibility();
                                dvs.addMatrixVector(0.0, fs0, dSs, 1.0);
                              }
                            else
                              dvs.addMatrixVector(0.0, section_matrices.getFsSubdivide()[i], dSs, 1.0);
                          }
                        else
                          {
                            //  newton with initial tangent
                            //    vs += fs0 * dSs;
                            const Matrix &fs0 = theSections[i]->getInitialFlexibility();
                            dvs.addMatrixVector(0.0, fs0, dSs, 1.0);
                          }

                        // set section deformations
                        if(initialFlag != 0)
                          section_matrices.getVsSubdivide()[i] += dvs;

                        if(theSections[i]->setTrialSectionDeformation(section_matrices.getVsSubdivide()[i]) < 0)
                          {
                            std::cerr << "ForceBeamColumn3d::update() - section failed in setTrial\n";
                            return -1;
                          }

                        // get section resisting forces
                        section_matrices.getSsrSubdivide()[i] = theSections[i]->getStressResultant();

                        // get section flexibility matrix
                        // FRANK
                        section_matrices.getFsSubdivide()[i] = theSections[i]->getSectionFlexibility();


                        // calculate section residual deformations
                        // dvs = fs * (Ss - Ssr);
                        dSs= Ss;
                        dSs.addVector(1.0, section_matrices.getSsrSubdivide()[i], -1.0);  // dSs = Ss - Ssr[i];

                        dvs.addMatrixVector(0.0, section_matrices.getFsSubdivide()[i], dSs, 1.0);

                        // integrate element flexibility matrix
                        // f = f + (b^ fs * b) * wtL;
                        //f.addMatrixTripleProduct(1.0, b[i], fs[i], wtL);

                        const Matrix &fSec = section_matrices.getFsSubdivide()[i];
                        fb.Zero();
                        double tmp;
                        for(int ii = 0; ii < order; ii++)
                          {
                            switch(code(ii))
                              {
                              case SECTION_RESPONSE_P:
                                for(int jj = 0; jj < order; jj++)
                                  fb(jj,0) += fSec(jj,ii)*wtL;
                                break;
                              case SECTION_RESPONSE_MZ:
                              for(int jj = 0; jj < order; jj++)
                                {
                                  tmp = fSec(jj,ii)*wtL;
                                  fb(jj,1) += xL1*tmp;
                                  fb(jj,2) += xL*tmp;
                                }
                              break;
                            case SECTION_RESPONSE_VY:
                              for(int jj = 0; jj < order; jj++)
                                {
                                  tmp = oneOverL*fSec(jj,ii)*wtL;
                                  fb(jj,1) += tmp;
                                  fb(jj,2) += tmp;
                                }
                              break;
                            case SECTION_RESPONSE_MY:
                              for(int jj = 0; jj < order; jj++)
                                {
                                  tmp = fSec(jj,ii)*wtL;
                                  fb(jj,3) += xL1*tmp;
                                  fb(jj,4) += xL*tmp;
                                }
                              break;
                            case SECTION_RESPONSE_VZ:
                              for(int jj = 0; jj < order; jj++)
                                {
                                  tmp = oneOverL*fSec(jj,ii)*wtL;
                                  fb(jj,3) += tmp;
                                  fb(jj,4) += tmp;
                                }
                              break;
                            case SECTION_RESPONSE_T:
                              for(int jj = 0; jj < order; jj++)
                                fb(jj,5) += fSec(jj,ii)*wtL;
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
                                for(size_t jj = 0; jj < NEBD; jj++)
                                  f(0,jj) += fb(ii,jj);
                                break;
                              case SECTION_RESPONSE_MZ:
                                for(size_t jj = 0; jj < NEBD; jj++)
                                  {
                                    tmp = fb(ii,jj);
                                    f(1,jj) += xL1*tmp;
                                    f(2,jj) += xL*tmp;
                                  }
                                break;
                              case SECTION_RESPONSE_VY:
                                for(size_t jj = 0; jj < NEBD; jj++)
                                  {
                                    tmp = oneOverL*fb(ii,jj);
                                    f(1,jj) += tmp;
                                    f(2,jj) += tmp;
                                  }
                                break;
                              case SECTION_RESPONSE_MY:
                                for(size_t jj = 0; jj < NEBD; jj++)
                                  {
                                    tmp = fb(ii,jj);
                                    f(3,jj) += xL1*tmp;
                                    f(4,jj) += xL*tmp;
                                  }
                                break;
                              case SECTION_RESPONSE_VZ:
                                for(size_t jj = 0; jj < NEBD; jj++)
                                  {
                                    tmp = oneOverL*fb(ii,jj);
                                    f(3,jj) += tmp;
                                    f(4,jj) += tmp;
                                  }
                                break;
                              case SECTION_RESPONSE_T:
                                for(size_t jj = 0; jj < NEBD; jj++)
                                  f(5,jj) += fb(ii,jj);
                                break;
                              default:
                                break;
                              }
                          }

                        // integrate residual deformations
                        // vr += (b^ (vs + dvs)) * wtL;
                        //vr.addMatrixTransposeVector(1.0, b[i], vs[i] + dvs, wtL);
                        dvs.addVector(1.0, section_matrices.getVsSubdivide()[i], 1.0);
                        double dei;
                        for(int ii = 0; ii < order; ii++)
                          {
                            dei = dvs(ii)*wtL;
                            switch(code(ii))
                              {
                              case SECTION_RESPONSE_P:
                                vr(0) += dei;
                                break;
                              case SECTION_RESPONSE_MZ:
                                vr(1) += xL1*dei; vr(2) += xL*dei;
                                break;
                              case SECTION_RESPONSE_VY:
                                tmp = oneOverL*dei;
                                vr(1) += tmp; vr(2) += tmp;
                                break;
                              case SECTION_RESPONSE_MY:
                                vr(3) += xL1*dei; vr(4) += xL*dei;
                                break;
                              case SECTION_RESPONSE_VZ:
                                tmp = oneOverL*dei;
                                vr(3) += tmp; vr(4) += tmp;
                                break;
                              case SECTION_RESPONSE_T:
                                vr(5) += dei;
                                break;
                              default:
                                break;
                              }
                         }
                     } //For i=0..numSections.

                   if(!isTorsion)
                     {
                       f(5,5) = DefaultLoverGJ;
                       vr(5) = SeTrial(5)*DefaultLoverGJ;
                     }

                   // calculate element stiffness matrix
                   // invert3by3Matrix(f, kv);
                   // FRANK
                   //          if(f.SolveSVD(I, kvTrial, 1.0e-12) < 0)
                   if(f.Solve(I, kvTrial) < 0)
                     std::cerr << "ForceBeamColumn3d::update() -- could not invert flexibility.\n";

                   // dv = vin + dvTrial  - vr
                   dv= vin;
                   dv+= dvTrial;
                   dv-= vr;

                   // dv.addVector(1.0, vr, -1.0);

                   // dSe = kv * dv;
                   dSe.addMatrixVector(0.0, kvTrial, dv, 1.0);

                   dW= dv^dSe;
                   if(dW0 == 0.0)
                     dW0 = dW;

                   SeTrial+= dSe;

                   // check for convergence of this interval
                   if(fabs(dW) < tol)
                     {
                       // set the target displacement
                       dvToDo-= dvTrial;
                       vin+= dvTrial;

                       // check if we have got to where we wanted
                       if(dvToDo.Norm() <= DBL_EPSILON)
                         { converged = true; }
                       else
                         {  // we convreged but we have more to do
                            // reset variables for start of next subdivision
                            dvTrial = dvToDo;
                            numSubdivide = 1;  // NOTE setting subdivide to 1 again maybe too much
                         }

                   // set kv, vs and Se values
                   kv= kvTrial;
                   Se= SeTrial;

                   for(size_t k=0; k<numSections; k++)
                     {
                       vs[k]= section_matrices.getVsSubdivide()[k];
                       fs[k]= section_matrices.getFsSubdivide()[k];
                       Ssr[k]= section_matrices.getSsrSubdivide()[k];
                     }

                   // break out of j & l loops
                   j = numIters+1;
                   l = 4;
                 }
	       else
                 {   //  if(fabs(dW) < tol) {
                   // if we have failed to convrege for all of our newton schemes
                   // - reduce step size by the factor specified
                   if(j == (numIters-1) && (l == 2))
                     {
                       dvTrial /= factor;
                       numSubdivide++;
                     }
                 }
              } // for(j=0; j<numIters; j++)
          } // if(initialFlag != 2)
      } // for(int l=0; l<2; l++)
    } // while (converged == false)

    // if fail to converge we return an error flag & print an error message
    if(converged == false)
      {
        std::cerr << "WARNING - ForceBeamColumn3d::update - failed to get compatible ";
        std::cerr << "element forces & deformations for element: ";
        std::cerr << this->getTag() << " (dW: << " << dW << ", dW0: " << dW0 << ")\n";

// 	if(verbosity>3)
//           {
//             std::cerr << "Section Tangent Condition Numbers:\n";
//             for(size_t i=0; i<numSections; i++)
//               {
//                 const Matrix &sectionStiff= theSections[i]->getSectionTangent();
//                 const double conditionNumber = sectionStiff.conditionNumber();
//                 std::cerr << conditionNumber << " ";
//               }
//             std::cerr << std::endl;
//           }
        return -1;
      }
    initialFlag= 1;
    return 0;
  }

void XC::ForceBeamColumn3d::getForceInterpolatMatrix(double xi, Matrix &b, const XC::ID &code)
  {
    b.Zero();

    const double L = theCoordTransf->getInitialLength();
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
          case SECTION_RESPONSE_MY: // Moment, My, interpolation
            b(i,3) = xi - 1.0;
            b(i,4) = xi;
            break;
          case SECTION_RESPONSE_VZ: // Shear, Vz, interpolation
            b(i,3) = b(i,4) = 1.0/L;
            break;
          case SECTION_RESPONSE_T: // Torque, T, interpolation
            b(i,5) = 1.0;
            break;
          default:
            break;
          }
      }
  }

void XC::ForceBeamColumn3d::getDistrLoadInterpolatMatrix(double xi, Matrix &bp, const XC::ID &code)
  {
    bp.Zero();

    const double L = theCoordTransf->getInitialLength();
    for(int i = 0; i < code.Size(); i++)
      {
        switch (code(i))
          {
          case SECTION_RESPONSE_MZ: // Moment, Mz, interpolation
            bp(i,1)= xi*(xi-1)*L*L/2;
            break;
          case SECTION_RESPONSE_P: // Axial, P, interpolation
            bp(i,0)= (1-xi)*L;
            break;
          case SECTION_RESPONSE_VY: // Shear, Vy, interpolation
            bp(i,1)= (xi-0.5)*L;
            break;
          case SECTION_RESPONSE_MY: // Moment, My, interpolation
            bp(i,2)= xi*(1-xi)*L*L/2;
            break;
          case SECTION_RESPONSE_VZ: // Shear, Vz, interpolation
            bp(i,2)= (0.5-xi)*L;
            break;
          case SECTION_RESPONSE_T: // Torsion, T, interpolation
            break;
          default:
            break;
          }
      }
  }

const XC::Matrix &XC::ForceBeamColumn3d::getMass(void) const
  {
    theMatrix.Zero();
    const double L = theCoordTransf->getInitialLength();
    if(rho != 0.0)
      theMatrix(0,0) = theMatrix(1,1) = theMatrix(2,2) =
      theMatrix(6,6) = theMatrix(7,7) = theMatrix(8,8) = 0.5*L*rho;
    return theMatrix;
  }

void XC::ForceBeamColumn3d::zeroLoad(void)
  {
    NLForceBeamColumn3dBase::zeroLoad();
    sp.Zero();
    p0.zero();
    v0.zero();
  }

int XC::ForceBeamColumn3d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() 
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
    else
      {
        const double L = theCoordTransf->getInitialLength();

        // Accumulate elastic deformations in basic system
        beamIntegr->addElasticDeformations(theLoad, loadFactor, L, v0);
        if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
          {
            const size_t numSections= getNumSections();
            if(sp.isEmpty())
              sp= Matrix(5,numSections);
            const Matrix &xi= beamIntegr->getIntegrPointCoords(numSections,L);
            sp+= beamMecLoad->getAppliedSectionForces(L,xi,loadFactor); // Accumulate applied section forces due to element loads
            beamMecLoad->addReactionsInBasicSystem(L,loadFactor,p0); // Accumulate reactions in basic system
          }
        else if(BeamStrainLoad *strainLoad= dynamic_cast<BeamStrainLoad *>(theLoad)) //Prescribed deformations.
          {
            const size_t numSections= getNumSections();
            const Matrix &xi= beamIntegr->getIntegrPointCoords(numSections,L);
            theSections.addInitialSectionDeformations(*strainLoad,loadFactor,xi,L);
          }
        else
          {
            std::cerr << "XC::ForceBeamColumn3d::addLoad() -- load type unknown for element with tag: " <<
              this->getTag() << std::endl;
            return -1;
           }
      }
    return 0;
  }

int XC::ForceBeamColumn3d::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    // Check for a quick return
    if(rho == 0.0)
      return 0;

    // get R * accel from the nodes
    const Vector &Raccel1= theNodes[0]->getRV(accel);
    const Vector &Raccel2= theNodes[1]->getRV(accel);

    const double L = theCoordTransf->getInitialLength();
    //const double m = 0.5*rho*L;

    // Should be done through p0[0]
    /*
    load(0) -= m*Raccel1(0);
    load(1) -= m*Raccel1(1);
    load(2) -= m*Raccel1(2);
    load(6) -= m*Raccel2(0);
    load(7) -= m*Raccel2(1);
    load(8) -= m*Raccel2(2);
    */
    return 0;
  }

const XC::Vector &XC::ForceBeamColumn3d::getResistingForceIncInertia(void) const
  {
    // Compute the current resisting force
    theVector = getResistingForce();

    if(rho != 0.0)
      {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();

        const double L = theCoordTransf->getInitialLength();
        const double m = 0.5*rho*L;

        theVector(0) += m*accel1(0);
        theVector(1) += m*accel1(1);
        theVector(2) += m*accel1(2);
        theVector(6) += m*accel2(0);
        theVector(7) += m*accel2(1);
        theVector(8) += m*accel2(2);

        // add the damping forces if rayleigh damping
        if(!rayFactors.nullValues())
          theVector+= this->getRayleighDampingForces();
      }
    else
      {
        // add the damping forces if rayleigh damping
        if(!rayFactors.nullKValues())
          theVector+= this->getRayleighDampingForces();
      }
    if(isDead())
      theVector*=dead_srf;
    return theVector;
  }

//! @brief Send members through the communicator argument.
int XC::ForceBeamColumn3d::sendData(Communicator &comm)
  {
    int res= NLForceBeamColumn3dBase::sendData(comm);
    res+= sendBeamIntegrationPtr(beamIntegr,25,26,getDbTagData(),comm);
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ForceBeamColumn3d::recvData(const Communicator &comm)
  {
    int res= NLForceBeamColumn3dBase::recvData(comm);
    beamIntegr= receiveBeamIntegrationPtr(beamIntegr,25,26,getDbTagData(),comm);
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::ForceBeamColumn3d::sendSelf(Communicator &comm)
  {
    inicComm(27);
    int res= sendData(comm);
    
    const int dataTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ForceBeamColumn3d::sendSelf() - failed to send ID data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ForceBeamColumn3d::recvSelf(const Communicator &comm)
  {
    inicComm(27);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ForceBeamColumn3d::recvSelf -- failed to receive ID data\n";
    else
      res+= recvData(comm);
    return res;
  }

//! @brief Return the initial flexibility matrix.
int XC::ForceBeamColumn3d::getInitialFlexibility(Matrix &fe) const
  {
    fe.Zero();

    const double L = theCoordTransf->getInitialLength();
    const double oneOverL  = 1.0/L;

    // Flexibility from elastic interior
    beamIntegr->addElasticFlexibility(L, fe);

    const size_t numSections= getNumSections();
    double xi[SectionMatrices::maxNumSections];
    beamIntegr->getSectionLocations(numSections, L, xi);

    double wt[SectionMatrices::maxNumSections];
    beamIntegr->getSectionWeights(numSections, L, wt);

    for(size_t i= 0; i < numSections; i++)
      {
        int order= theSections[i]->getOrder();
        const ID &code = theSections[i]->getType();

        Matrix fb(workArea, order, NEBD);

        double xL  = xi[i];
        double xL1 = xL-1.0;
        double wtL = wt[i]*L;

        const Matrix &fSec = theSections[i]->getInitialFlexibility();
        fb.Zero();
        double tmp;
        for(int ii=0;ii<order; ii++)
          {
            switch(code(ii))
              {
              case SECTION_RESPONSE_P:
                for(int jj = 0; jj < order; jj++)
                  fb(jj,0) += fSec(jj,ii)*wtL;
                break;
              case SECTION_RESPONSE_MZ:
                for(int jj = 0; jj < order; jj++)
                  {
                    tmp = fSec(jj,ii)*wtL;
                    fb(jj,1) += xL1*tmp;
                    fb(jj,2) += xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VY:
                for(int jj = 0; jj < order; jj++)
                  {
                    tmp = oneOverL*fSec(jj,ii)*wtL;
                    fb(jj,1) += tmp;
                    fb(jj,2) += tmp;
                  }
                break;
              case SECTION_RESPONSE_MY:
                for(int jj = 0; jj < order; jj++)
                  {
                    tmp = fSec(jj,ii)*wtL;
                    fb(jj,3) += xL1*tmp;
                    fb(jj,4) += xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VZ:
                for(int jj = 0; jj < order; jj++)
                  {
                    tmp = oneOverL*fSec(jj,ii)*wtL;
                    fb(jj,3) += tmp;
                    fb(jj,4) += tmp;
                  }
                break;
              case SECTION_RESPONSE_T:
                for(int jj = 0; jj < order; jj++)
                  fb(jj,5) += fSec(jj,ii)*wtL;
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
                for(size_t jj = 0; jj < NEBD; jj++)
                  fe(0,jj) += fb(ii,jj);
                  break;
              case SECTION_RESPONSE_MZ:
                for(size_t jj = 0; jj < NEBD; jj++)
                  {
                    tmp = fb(ii,jj);
                    fe(1,jj) += xL1*tmp;
                    fe(2,jj) += xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VY:
                for(size_t jj = 0; jj < NEBD; jj++)
                  {
                    tmp = oneOverL*fb(ii,jj);
                    fe(1,jj) += tmp;
                    fe(2,jj) += tmp;
                  }
                break;
              case SECTION_RESPONSE_MY:
                for(size_t jj = 0; jj < NEBD; jj++)
                  {
                    tmp = fb(ii,jj);
                    fe(3,jj) += xL1*tmp;
                    fe(4,jj) += xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VZ:
                for(size_t jj = 0; jj < NEBD; jj++)
                  {
                    tmp = oneOverL*fb(ii,jj);
                    fe(3,jj) += tmp;
                    fe(4,jj) += tmp;
                  }
                break;
             case SECTION_RESPONSE_T:
               for(size_t jj = 0; jj < NEBD; jj++)
                 fe(5,jj) += fb(ii,jj);
               break;
             default:
                break;
             }
          }
      }
    if(!isTorsion)
      fe(5,5)= DefaultLoverGJ;
    return 0;
  }

void XC::ForceBeamColumn3d::compSectionDisplacements(std::vector<Vector> &sectionCoords,std::vector<Vector> &sectionDispls) const
  {
    // get basic displacements and increments
    static Vector ub(NEBD);
    ub = theCoordTransf->getBasicTrialDisp();

    const double L = theCoordTransf->getInitialLength();

    // get integration point positions and weights
    const size_t numSections= getNumSections();
    static double pts[SectionMatrices::maxNumSections];
    beamIntegr->getSectionLocations(numSections, L, pts);

    // setup Vandermode and CBDI influence matrices

    // get CBDI influence matrix
    Matrix ls(numSections, numSections);
    getCBDIinfluenceMatrix(numSections, pts, L, ls);

    // get section curvatures
    Vector kappa_y(numSections);  // curvature
    Vector kappa_z(numSections);  // curvature
    static XC::Vector vs; // section deformations

    for(size_t i=0; i<numSections; i++)
      {
        // THIS IS VERY INEFFICIENT ... CAN CHANGE IF RUNS TOO SLOW
        int sectionKey1 = 0;
        int sectionKey2 = 0;
        const XC::ID &code = theSections[i]->getType();
        for(int j = 0; j < code.Size(); j++)
          {
            if(code(j) == SECTION_RESPONSE_MZ)
              sectionKey1 = j;
            if(code(j) == SECTION_RESPONSE_MY)
              sectionKey2 = j;
          }
        if(sectionKey1 == 0)
          {
            std::cerr << "FATAL NLBeamColumn3d::compSectionResponse - section does not provide Mz response\n";
            exit(-1);
          }
        if(sectionKey2 == 0)
          {
            std::cerr << "FATAL NLBeamColumn3d::compSectionResponse - section does not provide My response\n";
            exit(-1);
          }

        // get section deformations
        vs = theSections[i]->getSectionDeformation();

        kappa_z(i) = vs(sectionKey1);
        kappa_y(i) = vs(sectionKey2);
      }

    Vector v(numSections), w(numSections);
    static XC::Vector xl(NDM), uxb(NDM);
    static XC::Vector xg(NDM), uxg(NDM);
    // double theta;                             // angle of twist of the sections

    // v = ls * kappa_z;
    v.addMatrixVector (0.0, ls, kappa_z, 1.0);
    // w = ls * kappa_y *  (-1);
    w.addMatrixVector (0.0, ls, kappa_y, -1.0);

    double xi;
    for(size_t i=0; i<numSections; i++)
      {
        xi = pts[i];

        xl(0) = xi * L;
        xl(1) = 0;
        xl(2) = 0;

        // get section global coordinates
        sectionCoords[i] = theCoordTransf->getPointGlobalCoordFromLocal(xl);

        // compute section displacements
        //theta  = xi * ub(5); // consider linear variation for angle of twist. CHANGE LATER!!!!!!!!!!
        uxb(0) = xi * ub(0); // consider linear variation for axial displacement. CHANGE LATER!!!!!!!!!!
        uxb(1) = v(i);
        uxb(2) = w(i);

        // get section displacements in global system
        sectionDispls[i] = theCoordTransf->getPointGlobalDisplFromBasic(xi, uxb);
      }
    return;
  }

void XC::ForceBeamColumn3d::Print(std::ostream &s, int flag) const
  {
    const size_t numSections= getNumSections();
    // flags with negative values are used by GSA
    if(flag == -1)
      {
        int eleTag = this->getTag();
        s << "EL_BEAM\t" << eleTag << "\t";
        s << theSections[0]->getTag() << "\t" << theSections[numSections-1]->getTag();
        s  << "\t" << theNodes.getTagNode(0) << "\t" << theNodes.getTagNode(1);
        s << "\t0\t0.0000000\n";
      }
    // flags with negative values are used by GSA
    else if(flag < -1)
      {
        const int eleTag = this->getTag();
        const int counter = (flag +1) * -1;
        const double P  = Secommit(0);
        const double MZ1 = Secommit(1);
        const double MZ2 = Secommit(2);
        const double MY1 = Secommit(3);
        const double MY2 = Secommit(4);
        const double L = theCoordTransf->getInitialLength();
        const double VY = (MZ1+MZ2)/L;
        theVector(1) =  VY;
        theVector(4) = -VY;
        const double VZ = (MY1+MY2)/L;
        const double T  = Secommit(5);

        s << "FORCE\t" << eleTag << "\t" << counter << "\t0";
        s << "\t" << -P+p0[0] << "\t"  <<  VY+p0[1] << "\t"  << -VZ+p0[3]  << std::endl;
        s << "FORCE\t" << eleTag << "\t" << counter << "\t1";
        s << "\t"  << P  << ' '  << -VY+p0[2] << ' ' << VZ+p0[4] << std::endl;
        s << "MOMENT\t" << eleTag << "\t" << counter << "\t0";
        s << "\t" << -T << "\t"  << MY1 << "\t" << MZ1 << std::endl;
        s << "MOMENT\t" << eleTag << "\t" << counter << "\t1";
        s << "\t" << T << ' ' << MY2 << ' '  <<  MZ2 << std::endl;
      }
    // flag set to 2 used to print everything .. used for viewing data for UCSD renderer
    else if(flag == 2)
      {
        static XC::Vector xAxis(3);
        static XC::Vector yAxis(3);
        static XC::Vector zAxis(3);

        theCoordTransf->getLocalAxes(xAxis, yAxis, zAxis);

        s << "#ForceBeamColumn3D\n";
        s << "#LocalAxis " << xAxis(0) << " " << xAxis(1) << " " << xAxis(2);
        s << " " << yAxis(0) << " " << yAxis(1) << " " << yAxis(2) << " ";
        s << zAxis(0) << " " << zAxis(1) << " " << zAxis(2) << std::endl;

        const XC::Vector &node1Crd = theNodes[0]->getCrds();
        const XC::Vector &node2Crd = theNodes[1]->getCrds();
        const XC::Vector &node1Disp = theNodes[0]->getDisp();
        const XC::Vector &node2Disp = theNodes[1]->getDisp();

        s << "#NODE " << node1Crd(0) << " " << node1Crd(1) << " " << node1Crd(2)
          << " " << node1Disp(0) << " " << node1Disp(1) << " " << node1Disp(2)
          << " " << node1Disp(3) << " " << node1Disp(4) << " " << node1Disp(5) << std::endl;

        s << "#NODE " << node2Crd(0) << " " << node2Crd(1) << " " << node2Crd(2)
          << " " << node2Disp(0) << " " << node2Disp(1) << " " << node2Disp(2)
          << " " << node2Disp(3) << " " << node2Disp(4) << " " << node2Disp(5) << std::endl;

        const double P  = Secommit(0);
        const double MZ1 = Secommit(1);
        const double MZ2 = Secommit(2);
        const double MY1 = Secommit(3);
        const double MY2 = Secommit(4);
        const double L = theCoordTransf->getInitialLength();
        const double VY = (MZ1+MZ2)/L;
        theVector(1) =  VY;
        theVector(4) = -VY;
        const double VZ = (MY1+MY2)/L;
        const double T  = Secommit(5);
        s << "#END_FORCES " << -P+p0[0] << ' '  <<  VY+p0[1] << ' '  << -VZ+p0[3] << ' '
          << -T << ' '  << MY1 << ' ' << MZ1 << std::endl;
        s << "#END_FORCES "  << P  << ' '  << -VY+p0[2] << ' ' << VZ+p0[4] << ' '
          << T << ' ' << MY2 << ' '  <<  MZ2 << std::endl;

        // plastic hinge rotation
        static XC::Vector vp(6);
        static XC::Matrix fe(6,6);
        this->getInitialFlexibility(fe);
        vp = theCoordTransf->getBasicTrialDisp();
        vp.addMatrixVector(1.0, fe, Se, -1.0);
        s << "#PLASTIC_HINGE_ROTATION " << vp[1] << " " << vp[2] << " " << vp[3] << " " << vp[4]
          << " " << 0.1*L << " " << 0.1*L << std::endl;

        // allocate array of vectors to store section coordinates and displacements
        const size_t numSections= getNumSections();
	static std::vector<Vector> coords;
	static std::vector<Vector> displs;
        coords.resize(numSections);
        displs.resize(numSections);
        for(size_t i= 0;i<numSections;i++)
          coords[i] = Vector(NDM);
        for(size_t i= 0;i<numSections;i++)
          displs[i] = Vector(NDM);

        // compute section location & displacements
        this->compSectionDisplacements(coords, displs);

        // spit out the section location & invoke print on the scetion
       for(size_t i=0; i<numSections; i++)
         {
           s << "#SECTION " << (coords[i])(0) << " " << (coords[i])(1) << " " << (coords[i])(2);
           s << " " << (displs[i])(0) << " " << (displs[i])(1) << " " << (displs[i])(2) << std::endl;
           theSections[i]->Print(s, flag);
         }
      }
    else
      {
        s << "\nElement: " << this->getTag() << " Type: ForceBeamColumn3d ";
        s << "\tConnected Nodes: " << theNodes;
        s << "\tNumber of Sections: " << numSections;
        s << "\tMass density: " << rho << std::endl;
        beamIntegr->Print(s, flag);
        const double P  = Secommit(0);
        const double MZ1 = Secommit(1);
        const double MZ2 = Secommit(2);
        const double MY1 = Secommit(3);
        const double MY2 = Secommit(4);
        const double L = theCoordTransf->getInitialLength();
        const double VY = (MZ1+MZ2)/L;
        theVector(1) =  VY;
        theVector(4) = -VY;
        const double VZ = (MY1+MY2)/L;
        const double T  = Secommit(5);
        s << "\tEnd 1 Forces (P MZ VY MY VZ T): "
          << -P+p0[0] << " " << MZ1 << " " <<  VY+p0[1] << " "
          << MY1 << " " << -VZ+p0[3] << " " << T << std::endl;
        s << "\tEnd 2 Forces (P MZ VY MY VZ T): "
          << P        << " " << MZ2 << " " << -VY+p0[2] << " "
          << MY2 << " " <<  VZ+p0[4] << " " << -T << std::endl;

        if(flag == 1)
          {
            for(size_t i = 0; i < numSections; i++)
              s << "\numSections "<<i<<" :" << *theSections[i];
          }
      }
  }

std::ostream &XC::operator<<(std::ostream &s, const ForceBeamColumn3d &E)
  {
    E.Print(s);
    return s;
  }

XC::Response *XC::ForceBeamColumn3d::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
{
  //
  // we compare argv[0] for known response types
  //

  // global force -
  if(argv[0] == "forces" || argv[0] == "force"
      || argv[0] == "globalForce" || argv[0] == "globalForces")
    return new ElementResponse(this, 1, theVector);

  // local force -
  else if(argv[0] == "localForce" || argv[0] == "localForces")
    return new ElementResponse(this, 2, theVector);

  // chord rotation -
  else if(argv[0] == "chordRotation" || argv[0] == "chordDeformation"
           || argv[0] == "basicDeformation")
    return new ElementResponse(this, 3, Vector(6));

  // plastic rotation -
  else if(argv[0] == "plasticRotation" || argv[0] == "plasticDeformation")
    return new ElementResponse(this, 4, Vector(6));

  // point of inflection
  else if(argv[0] == "inflectionPoint")
    return new ElementResponse(this, 5, Vector(2));

  // tangent drift
  else if(argv[0] == "tangentDrift")
    return new ElementResponse(this, 6, Vector(4));

  // section response -
  else if(argv[0] == "section")
    {
      if(argv.size() <= 2)
      return 0;

      const size_t numSections= getNumSections();
      size_t sectionNum = atoi(argv[1]);
      if(sectionNum > 0 && sectionNum <= numSections)
        {
          std::vector<std::string> argv2(argv);
          argv2.erase(argv2.begin(),argv2.begin()+2);
          return theSections[sectionNum-1]->setResponse(argv2, eleInformation);
        }
      else
        return 0;
    }
  else
    return 0;
}

int XC::ForceBeamColumn3d::getResponse(int responseID, Information &eleInfo)
  {
    static XC::Vector vp(6);
    static XC::Matrix fe(6,6);

    if(responseID == 1)
      return eleInfo.setVector(this->getResistingForce());

    else if(responseID == 2)
      {
        // Axial
        double N = Se(0);
        theVector(6) =  N;
        theVector(0) = -N+p0[0];

        // Torsion
        double T = Se(5);
        theVector(9) =  T;
        theVector(3) = -T;

        // Moments about z and shears along y
        double M1 = Se(1);
        double M2 = Se(2);
        theVector(5)  = M1;
        theVector(11) = M2;
        double L = theCoordTransf->getInitialLength();
        double V = (M1+M2)/L;
        theVector(1) =  V+p0[1];
        theVector(7) = -V+p0[2];

        // Moments about y and shears along z
        M1 = Se(3);
        M2 = Se(4);
        theVector(4)  = M1;
        theVector(10) = M2;
        V = -(M1+M2)/L;
        theVector(2) = -V+p0[3];
        theVector(8) =  V+p0[4];

        return eleInfo.setVector(theVector);
     }

  // Chord rotation
  else if(responseID == 3) {
    vp = theCoordTransf->getBasicTrialDisp();
    return eleInfo.setVector(vp);
  }

  // Plastic rotation
  else if(responseID == 4) {
    this->getInitialFlexibility(fe);
    vp = theCoordTransf->getBasicTrialDisp();
    vp.addMatrixVector(1.0, fe, Se, -1.0);
    return eleInfo.setVector(vp);
  }

  // Point of inflection
  else if(responseID == 5) {
    static XC::Vector LI(2);
    LI(0) = 0.0;
    LI(1) = 0.0;

    double L = theCoordTransf->getInitialLength();

    if(fabs(Se(1)+Se(2)) > DBL_EPSILON)
      LI(0) = Se(1)/(Se(1)+Se(2))*L;

    if(fabs(Se(3)+Se(4)) > DBL_EPSILON)
      LI(1) = Se(3)/(Se(3)+Se(4))*L;

    return eleInfo.setVector(LI);
  }

  // Tangent drift
  else if(responseID == 6) {
    double d2z = 0.0;
    double d2y = 0.0;
    double d3z = 0.0;
    double d3y = 0.0;

    double L = theCoordTransf->getInitialLength();

    const size_t numSections= getNumSections();
    double wts[SectionMatrices::maxNumSections];
    beamIntegr->getSectionWeights(numSections, L, wts);

    double pts[SectionMatrices::maxNumSections];
    beamIntegr->getSectionLocations(numSections, L, pts);

    // Location of inflection point from node I
    double LIz = 0.0;
    if(fabs(Se(1)+Se(2)) > DBL_EPSILON)
      LIz = Se(1)/(Se(1)+Se(2))*L;

    double LIy = 0.0;
    if(fabs(Se(3)+Se(4)) > DBL_EPSILON)
      LIy = Se(3)/(Se(3)+Se(4))*L;

    for(size_t i = 0; i < numSections; i++) {
      double x = pts[i]*L;
      const XC::ID &type = theSections[i]->getType();
      int order = theSections[i]->getOrder();
      double kappa = 0.0;
      if(x < LIz) {
        for(int j = 0; j < order; j++)
          if(type(j) == SECTION_RESPONSE_MZ)
            kappa += vs[i](j);
        double b = -LIz+x;
        d2z += (wts[i]*L)*kappa*b;
      }
      kappa = 0.0;
      if(x < LIy) {
        for(int j = 0; j < order; j++)
          if(type(j) == SECTION_RESPONSE_MY)
            kappa += vs[i](j);
        double b = -LIy+x;
        d2y += (wts[i]*L)*kappa*b;
      }
    }

    d2z += beamIntegr->getTangentDriftI(L, LIz, Se(1), Se(2));
    d2y += beamIntegr->getTangentDriftI(L, LIy, Se(3), Se(4), true);

    for(size_t i = numSections-1; i >= 0; i--) {
      double x = pts[i]*L;
      const XC::ID &type = theSections[i]->getType();
      int order = theSections[i]->getOrder();
      double kappa = 0.0;
      if(x > LIz) {
        for(int j = 0; j < order; j++)
          if(type(j) == SECTION_RESPONSE_MZ)
            kappa += vs[i](j);
        double b = x-LIz;
        d3z += (wts[i]*L)*kappa*b;
      }
      kappa = 0.0;
      if(x > LIy) {
        for(int j = 0; j < order; j++)
          if(type(j) == SECTION_RESPONSE_MY)
            kappa += vs[i](j);
        double b = x-LIy;
        d3y += (wts[i]*L)*kappa*b;
      }
    }

    d3z += beamIntegr->getTangentDriftJ(L, LIz, Se(1), Se(2));
    d3y += beamIntegr->getTangentDriftJ(L, LIy, Se(3), Se(4), true);

    static XC::Vector d(4);
    d(0) = d2z;
    d(1) = d3z;
    d(2) = d2y;
    d(3) = d3y;

    return eleInfo.setVector(d);
  }

  else
    return -1;
}

int XC::ForceBeamColumn3d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv.size() < 1)
      return -1;

    // If the parameter belongs to the element itself
    if(argv[0] == "rho")
      return param.addObject(1, this);
  
        const size_t numSections= getNumSections();
  // If the parameter belongs to a section or lower
  if (strstr(argv[0].c_str(),"section") != 0) {
    
    if (argv.size() < 3)
      return -1;
    
    // Get section and material tag numbers from user input
    int paramSectionTag = atoi(argv[1]);
    
    // Find the right section and call its setParameter method
    int ok = 0;
    for(size_t i = 0; i < numSections; i++)
      if (paramSectionTag == theSections[i]->getTag())
	ok += setSectionParameter(theSections[i],argv,2,param);

    return ok;
  }
  
  else if (strstr(argv[0].c_str(),"integration") != 0) {
    
    if (argv.size() < 2)
      return -1;

        std::vector<std::string> argv1(argv);
        argv1.erase(argv1.begin(),argv1.begin()+1);
    return beamIntegr->setParameter(argv1, param);
  }
  else {
    return -1;
  }
}

int XC::ForceBeamColumn3d::updateParameter (int parameterID, Information &info)
{
  // If the parameterID value is not equal to 1 it belongs
  // to section or material further down in the hierarchy.

  if(parameterID == 1) {

    this->rho = info.theDouble;
    return 0;

  }
  else if(parameterID > 0 ) {

    // Extract the section number
    int sectionNumber = (int)( floor((double)parameterID) / (100000) );

    int ok = -1;
    const size_t numSections= getNumSections();
    for(size_t i=0; i<numSections; i++) {
      if(sectionNumber == theSections[i]->getTag()) {
        ok = theSections[i]->updateParameter(parameterID, info);
      }
    }

    if(ok < 0) {
      std::cerr << "XC::ForceBeamColumn3d::updateParameter() - could not update parameter. " << std::endl;
      return ok;
    }
    else {
      return ok;
    }
  }
  else {
    std::cerr << "XC::ForceBeamColumn3d::updateParameter() - could not update parameter. " << std::endl;
    return -1;
  }
}

void XC::ForceBeamColumn3d::setSectionPointers(const std::vector<PrismaticBarCrossSection *> &secPtrs)
  {
    const size_t numSections= getNumSections();
    if(numSections > section_matrices.getMaxNumSections())
      {
        std::cerr << "Error: XC::ForceBeamColumn3d::setSectionPointers -- max number of sections exceeded";
      }
    isTorsion= setSections(secPtrs);

    if(!isTorsion)
      std::cerr << "XC::ForceBeamColumn3d::ForceBeamColumn3d -- no torsion detected in sections, " <<
        "continuing with element torsional stiffness GJ/L = " << 1.0/DefaultLoverGJ;
  }
