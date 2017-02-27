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
// $Date: 2005/09/06 13:45:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/ForceBeamColumn2d.cpp,v $

/*
 * References
 *

State Determination Algorithm
---
Neuenhofer, A. and F. C. Filippou (1997). "Evaluation of Nonlinear Frame Finite
Element Models." Journal of Structural Engineering, 123(7):958-966.

Spacone, E., V. Ciampi, and F. C. Filippou (1996). "Mixed Formulation of
Nonlinear Beam Finite Element." Computers and Structures, 58(1):71-83.


Plastic Hinge Integration
---
Scott, M. H. and G. L. Fenves (2006). "Plastic Hinge Integration Methods for
Force-Based Beam-Column Elements." Journal of Structural Engineering,
132(2):244-252.


Analytical Response Sensitivity (DDM)
---
Scott, M. H., P. Franchin, G. L. Fenves, and F. C. Filippou (2004).
"Response Sensitivity for Nonlinear Beam-Column Elements."
Journal of Structural Engineering, 130(9):1281-1288.


Software Design
---
Scott, M. H., G. L. Fenves, F. T. McKenna, and F. C. Filippou (2007).
"Software Patterns for Nonlinear Beam-Column Models."
Journal of Structural Engineering, Approved for publication, February 2007.

 *
 */


#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"
#include <domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn2d.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <utility/recorder/response/ElementResponse.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include <domain/mesh/element/utils/coordTransformation/CrdTransf2d.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h>
#include <material/section/SeccionBarraPrismatica.h>


#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"


void XC::ForceBeamColumn2d::libera(void)
  {
    if(beamIntegr)
      delete beamIntegr;
    beamIntegr= nullptr;
  }

void XC::ForceBeamColumn2d::alloc(const BeamIntegration &bi)
  {
    libera();
    beamIntegr= bi.getCopy();
  }

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::ForceBeamColumn2d::ForceBeamColumn2d(int tag)
  : NLForceBeamColumn2dBase(tag,ELE_TAG_ForceBeamColumn2d), beamIntegr(nullptr), v0()
  {}

//! @brief Copy constructor.
XC::ForceBeamColumn2d::ForceBeamColumn2d(const ForceBeamColumn2d &otro)
  : NLForceBeamColumn2dBase(otro), beamIntegr(nullptr), v0(otro.v0), maxSubdivisions(otro.maxSubdivisions)
  {
    if(otro.beamIntegr)
      alloc(*otro.beamIntegr);
  }

//! @brief Assignment operator.
XC::ForceBeamColumn2d &XC::ForceBeamColumn2d::operator=(const ForceBeamColumn2d &otro)
  {
//     NLForceBeamColumn2dBase::operator=(otro);
//     if(otro.beamIntegr)
//       alloc(*otro.beamIntegr);
//     v0= otro.v0;
//     maxSubdivisions= otro.maxSubdivisions;
    std::cerr << "ForceBeamColumn2d: No se debe llamar al operador de asignación."
              << std::endl;
    return *this;    
  }

//! @brief Constructor.
XC::ForceBeamColumn2d::ForceBeamColumn2d(int tag,int numSec,const Material *m,const CrdTransf *trf,const BeamIntegration *integ):
  NLForceBeamColumn2dBase(tag,ELE_TAG_ForceBeamColumn2d,numSec,m,trf), beamIntegr(nullptr), v0()
  {
    if(integ) alloc(*integ);
  }

// constructor which takes the unique element tag, sections,
// and the node XC::ID's of it's nodal end points.
// allocates the necessary space needed by each object
XC::ForceBeamColumn2d::ForceBeamColumn2d (int tag, int nodeI, int nodeJ,
                                          int numSec,const std::vector<SeccionBarraPrismatica *> &sec,
                                          BeamIntegration &bi,
                                          CrdTransf2d &coordTransf, double massDensPerUnitLength,
                                          int maxNumIters, double tolerance):
  NLForceBeamColumn2dBase(tag,ELE_TAG_ForceBeamColumn2d,0),beamIntegr(nullptr), v0()
  {
    theNodes.set_id_nodos(nodeI,nodeJ);

    alloc(bi);
    if(!beamIntegr)
      {
        std::cerr << "Error: ForceBeamColumn2d::ForceBeamColumn2d: could not create copy of beam integration object" << std::endl;
        exit(-1);
      }

    // get copy of the transformation object
    set_transf(&coordTransf);

    this->setSectionPointers(sec);
 }


//! @brief Virtual constructor.
XC::Element* XC::ForceBeamColumn2d::getCopy(void) const
  { return new ForceBeamColumn2d(*this); }

//! @brief Destructor
XC::ForceBeamColumn2d::~ForceBeamColumn2d(void)
  { libera(); }

void XC::ForceBeamColumn2d::setDomain(Domain *theDomain)
  {
    NLForceBeamColumn2dBase::setDomain(theDomain);

    // ensure connected nodes have correct number of dof's
    int dofNode1 = theNodes[0]->getNumberDOF();
    int dofNode2 = theNodes[1]->getNumberDOF();

    if((dofNode1 != NND) || (dofNode2 != NND))
      {
        std::cerr << "ForceBeamColumn2d::setDomain(): Nd2 or Nd1 incorrect dof ";
        exit(0);
      }

    // initialize the transformation
    initialize_trf();

    // get element length
    double L = theCoordTransf->getInitialLength();
    if(L == 0.0)
      {
        std::cerr << "ForceBeamColumn2d::setDomain(): Zero element length:" << this->getTag();
        exit(0);
      }
    if(initialFlag == 0)
      this->initializeSectionHistoryVariables();
  }

int XC::ForceBeamColumn2d::commitState(void)
  {
    int err = 0;

    // call element commitState to do any base class stuff
    if((err = NLForceBeamColumn2dBase::commitState()) != 0)
      { std::cerr << "ForceBeamColumn2d::commitState () - failed in base class"; }

    const size_t numSections= getNumSections();
    size_t i= 0;
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
    kvcommit = kv;
    Secommit = Se;

    //   initialFlag= 0;  fmk - commented out, see what happens to Example3.1.tcl if uncommented
    //                         - i have not a clue why, ask remo if he ever gets in contact with us again!
    return err;
  }

int XC::ForceBeamColumn2d::revertToLastCommit(void)
  {
    int err;
    const size_t numSections= getNumSections();
    size_t i= 0;
    do
      {
        vs[i] = vscommit[i];
        err = theSections[i]->revertToLastCommit();

        theSections[i]->setTrialSectionDeformation(vs[i]);
        Ssr[i]= theSections[i]->getStressResultant();
        fs[i]= theSections[i]->getSectionFlexibility();
        i++;
      }
    while(err == 0 && i < numSections);

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

int XC::ForceBeamColumn2d::revertToStart(void)
  {
    // revert the sections state to start
    int err;

    const size_t numSections= getNumSections();
    size_t i= 0;
    do
      {
        fs[i].Zero();
        vs[i].Zero();
        Ssr[i].Zero();
        err = theSections[i++]->revertToStart();
      }
    while(err == 0 && i < numSections);

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


const XC::Matrix &XC::ForceBeamColumn2d::getInitialStiff(void) const
  {
    // check for quick return
    if(Ki.Nula())
      {
        static Matrix f(NEBD, NEBD); // element flexibility matrix
        this->getInitialFlexibility(f);
        static Matrix kvInit(NEBD, NEBD);
        f.Invert(kvInit);
        Ki= Matrix(theCoordTransf->getInitialGlobalStiffMatrix(kvInit));
      }
    return Ki;
  }

//NEWTON , SUBDIVIDE AND INITIAL ITERATIONS
int XC::ForceBeamColumn2d::update(void)
  {
    // if have completed a recvSelf() - do a revertToLastCommit
    // to get Ssr, etc. set correctly
    if(initialFlag == 2)
      this->revertToLastCommit();

    // update the transformation
    theCoordTransf->update();

    // get basic displacements and increments
    const Vector &v= theCoordTransf->getBasicTrialDisp();

    static Vector dv(NEBD);
    dv= theCoordTransf->getBasicIncrDeltaDisp();

    if(initialFlag != 0 && dv.Norm() <= DBL_EPSILON && (sp.Nula()))
      return 0;

    static Vector vin(NEBD);
    vin= v;
    vin-= dv;
    const double L= theCoordTransf->getInitialLength();
    const double oneOverL= 1.0/L;

    const size_t numSections= getNumSections();
    std::vector<double> xi(section_matrices.getMaxNumSections());
    beamIntegr->getSectionLocations(numSections, L, &xi[0]);

    std::vector<double> wt(section_matrices.getMaxNumSections());
    beamIntegr->getSectionWeights(numSections, L, &wt[0]);

    static Vector vr(NEBD);       // element residual displacements
    static Matrix f(NEBD,NEBD);   // element flexibility matrix

    static Matrix I(NEBD,NEBD);   // an identity matrix for matrix inverse
    double dW= 0.0;                   // section strain energy (work) norm

    I.Zero();
    for(size_t i=0; i<NEBD; i++)
      I(i,i) = 1.0;

    int numSubdivide = 1;
    bool converged = false;
    static Vector dSe(NEBD);
    static Vector dvToDo(NEBD);
    static Vector dvTrial(NEBD);
    static Vector SeTrial(NEBD);
    static Matrix kvTrial(NEBD, NEBD);

    dvToDo= dv;
    dvTrial= dvToDo;

    static double factor = 10;
    double dW0 = 0.0;

    maxSubdivisions= 4; //XXX 10

    // fmk - modification to get compatable ele forces and deformations
    //   for a change in deformation dV we try first a newton iteration, if
    //   that fails we try an initial flexibility iteration on first iteration
    //   and then regular newton, if that fails we use the initial flexiblity
    //   for all iterations.
    //
    //   if they both fail we subdivide dV & try to get compatable forces
    //   and deformations. if they work and we have subdivided we apply
    //   the remaining dV.

    while(converged == false && numSubdivide <= maxSubdivisions)
      {
        // try regular newton (if l==0), or
        // initial tangent on first iteration then regular newton (if l==1), or
        // initial tangent iterations (if l==2)
        for(int l=0; l<3; l++)
          {
            //      if(l == 1) l = 2;
            SeTrial= Se;
            kvTrial= kv;
            for(size_t i=0;i<numSections;i++)
              {
                section_matrices.getVsSubdivide()[i]= vs[i];
                section_matrices.getFsSubdivide()[i]= fs[i];
                section_matrices.getSsrSubdivide()[i]= Ssr[i];
              }

            // calculate nodal force increments and update nodal forces
            // dSe = kv * dv;
            dSe.addMatrixVector(0.0, kvTrial, dvTrial, 1.0);
            SeTrial+= dSe;

            if(initialFlag != 2)
              {
                int numIters = maxIters;
                if(l == 1)
                  numIters= 10*maxIters; // allow 10 times more iterations for initial tangent
                for(int j=0;j<numIters;j++)
                  {
                    // initialize f and vr for integration
                    f.Zero();
                    vr.Zero();
                    if(beamIntegr->addElasticFlexibility(L, f) < 0)
                      {
                        vr(0) += f(0,0)*SeTrial(0);
                        vr(1) += f(1,1)*SeTrial(1) + f(1,2)*SeTrial(2);
                        vr(2) += f(2,1)*SeTrial(1) + f(2,2)*SeTrial(2);
                      }

                    // Add effects of element loads
                    vr(0)+= v0[0];
                    vr(1)+= v0[1];
                    vr(2)+= v0[2];

                    for(size_t i=0;i<numSections; i++)
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
    
                        const double xL= xi[i];
                        const double xL1= xL-1.0;
                        const double wtL= wt[i]*L;
    
                        // calculate total section forces
                        // Ss = b*Se + bp*currDistrLoad;
                        // Ss.addMatrixVector(0.0, b[i], Se, 1.0);
                        for(int ii= 0;ii<order;ii++)
                          {
                            switch(code(ii))
                              {
                              case SECTION_RESPONSE_P:
                                Ss(ii)= SeTrial(0);
                                break;
                              case SECTION_RESPONSE_MZ:
                                Ss(ii)=  xL1*SeTrial(1) + xL*SeTrial(2);
                                break;
                              case SECTION_RESPONSE_VY:
                                Ss(ii)= oneOverL*(SeTrial(1)+SeTrial(2));
                                break;
                              default:
                                Ss(ii)= 0.0;
                                break;
                              }
                          }
    
                        // Add the effects of element loads, if present
                        if(!sp.Nula())
                          {
                            const Matrix &s_p = sp;
                            for(int ii=0;ii<order;ii++)
                              {
                                switch(code(ii))
                                  {
                                  case SECTION_RESPONSE_P:
                                    Ss(ii) += s_p(0,i);
                                    break;
                                  case SECTION_RESPONSE_MZ:
                                    Ss(ii) += s_p(1,i);
                                    break;
                                  case SECTION_RESPONSE_VY:
                                    Ss(ii) += s_p(2,i);
                                    break;
                                  default:
                                    break;
                                  }
                              }
                          }
    
                        // dSs = Ss - Ssr[i];
                        dSs= Ss;
                        dSs.addVector(1.0, section_matrices.getSsrSubdivide()[i], -1.0);
    
                        // compute section deformation increments
                        if(l==0)
                          {
                            //  regular newton
                            //    vs += fs * dSs;
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
                                const Matrix &fs0 = theSections[i]->getInitialFlexibility();
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
                          section_matrices.getVsSubdivide()[i]+= dvs;
    
                        if(theSections[i]->setTrialSectionDeformation(section_matrices.getVsSubdivide()[i]) < 0)
                          {
                            std::cerr << "ForceBeamColumn2d::update() - section failed in setTrial\n";
                            return -1;
                          }
    
                        // get section resisting forces
                        section_matrices.getSsrSubdivide()[i] = theSections[i]->getStressResultant();
    
                        // get section flexibility matrix
                        section_matrices.getFsSubdivide()[i] = theSections[i]->getSectionFlexibility();
    
                        // calculate section residual deformations
                        // dvs = fs * (Ss - Ssr);
                        dSs = Ss;
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
                                for(int jj= 0;jj<order;jj++)
                                  fb(jj,0) += fSec(jj,ii)*wtL;
                                break;
                              case SECTION_RESPONSE_MZ:
                                for(int jj= 0;jj<order;jj++)
                                  {
                                    tmp= fSec(jj,ii)*wtL;
                                    fb(jj,1)+= xL1*tmp;
                                    fb(jj,2)+= xL*tmp;
                                  }
                                break;
                              case SECTION_RESPONSE_VY:
                                for(int jj= 0;jj<order;jj++)
                                  {
                                    tmp = oneOverL*fSec(jj,ii)*wtL;
                                    fb(jj,1) += tmp;
                                    fb(jj,2) += tmp;
                                  }
                                break;
                              default:
                                break;
                              }
                          }
    
                        for(int ii = 0; ii < order; ii++)
                          {
                            switch(code(ii))
                              {
                              case SECTION_RESPONSE_P:
                                for(size_t jj= 0;jj<NEBD; jj++)
                                  f(0,jj) += fb(ii,jj);
                                break;
                              case SECTION_RESPONSE_MZ:
                                for(size_t jj= 0;jj<NEBD;jj++)
                                  {
                                    tmp = fb(ii,jj);
                                    f(1,jj) += xL1*tmp;
                                    f(2,jj) += xL*tmp;
                                  }
                                break;
                              case SECTION_RESPONSE_VY:
                                for(size_t jj= 0;jj<NEBD;jj++)
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
    
                        // integrate residual deformations
                        // vr += (b^ (vs + dvs)) * wtL;
                        //vr.addMatrixTransposeVector(1.0, b[i], vs[i] + dvs, wtL);
                        dvs.addVector(1.0, section_matrices.getVsSubdivide()[i], 1.0);

                        double dei;
                        for(int ii= 0;ii<order;ii++)
                          {
                            dei= dvs(ii)*wtL;
                            switch(code(ii))
                              {
                              case SECTION_RESPONSE_P:
                                vr(0)+= dei;
                                break;
                              case SECTION_RESPONSE_MZ:
                                vr(1)+= xL1*dei; vr(2) += xL*dei;
                                break;
                              case SECTION_RESPONSE_VY:
                                tmp= oneOverL*dei;
                                vr(1)+= tmp; vr(2) += tmp;
                                break;
                              default:
                                break;
                              }
                          }

                      } //For i=0..numSections.
                    // calculate element stiffness matrix
                    // invert3by3Matrix(f, kv);
                    if(f.Solve(I, kvTrial) < 0)
                      std::cerr << "ForceBeamColumn2d::update() -- could not invert flexibility\n";
    
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
                        dvToDo -= dvTrial;
                        vin+= dvTrial;
                        // check if we have got to where we wanted
                        if(dvToDo.Norm() <= DBL_EPSILON)
                          { converged = true; }
                        else
                          { // we convreged but we have more to do
                            std::cerr << dvToDo << dvTrial << std::endl;
                            // reset variables for start of next subdivision
                            dvTrial = dvToDo;
                            numSubdivide = 1;  // NOTE setting subdivide to 1 again maybe too much
                          }
    
                        // set kv, vs and Se values
                        kv= kvTrial;
                        Se= SeTrial;
    
                        for(size_t k=0; k<numSections; k++)
                          {
                            vs[k] = section_matrices.getVsSubdivide()[k];
                            fs[k] = section_matrices.getFsSubdivide()[k];
                            Ssr[k] = section_matrices.getSsrSubdivide()[k];
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
        std::cerr << "WARNING - ForceBeamColumn2d::update - failed to get compatible ";
        std::cerr << "element forces & deformations for element: ";
        std::cerr << getTag() << "(dW: << " << dW  << ", dW0: " << dW0 << ")\n";
        return -1;
      }

    initialFlag= 1;
    return 0;
  }

void XC::ForceBeamColumn2d::getForceInterpolatMatrix(double xi, Matrix &b, const XC::ID &code)
  {
    b.Zero();

    const double L = theCoordTransf->getInitialLength();
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
          default:
            break;
         }
      }
  }

void XC::ForceBeamColumn2d::getDistrLoadInterpolatMatrix(double xi, Matrix &bp, const XC::ID &code)
  {
    bp.Zero();

    const double L = theCoordTransf->getInitialLength();
    for(int i = 0; i < code.Size(); i++)
      {
        switch (code(i))
          {
          case SECTION_RESPONSE_MZ:                // Moment, Mz, interpolation
            bp(i,1) = xi*(xi-1)*L*L/2;
            break;
          case SECTION_RESPONSE_P:                // Axial, P, interpolation
            bp(i,0) = (1-xi)*L;
            break;
          case SECTION_RESPONSE_VY:                // Shear, Vy, interpolation
            bp(i,1) = (xi-0.5)*L;
            break;
          default:
            break;
          }
      }
  }

const XC::Matrix &XC::ForceBeamColumn2d::getMass(void) const
  {
    theMatrix.Zero();
    const double L = theCoordTransf->getInitialLength();
    if(rho != 0.0)
      theMatrix(0,0) = theMatrix(1,1) = theMatrix(3,3) = theMatrix(4,4) = 0.5*L*rho;
    return theMatrix;
  }

void XC::ForceBeamColumn2d::zeroLoad(void)
  {
    NLForceBeamColumn2dBase::zeroLoad();
    sp.Zero();
    p0.zero();
    v0.zero();
  }

//! @brief Calcula la respuesta del elemento a the load being passed as parameter.
int XC::ForceBeamColumn2d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << nombre_clase() 
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
    else
      {
        const double L = theCoordTransf->getInitialLength();

        // Accumulate elastic deformations in basic system
        beamIntegr->addElasticDeformations(theLoad, loadFactor, L, v0);
        if(const BeamMecLoad *beamMecLoad= dynamic_cast<const BeamMecLoad *>(theLoad))
          {
            const size_t numSections= getNumSections();
            if(sp.Nula())
              sp= Matrix(3,numSections);
            const Matrix &xi= beamIntegr->getIntegrPointCoords(numSections,L);
            sp+= beamMecLoad->getAppliedSectionForces(L,xi,loadFactor); // Accumulate applied section forces due to element loads
            beamMecLoad->addReactionsInBasicSystem(L,loadFactor,p0); // Accumulate reactions in basic system
          }
        else if(const BeamStrainLoad *strainLoad= dynamic_cast<const BeamStrainLoad *>(theLoad)) //Prescribed deformations.
          {
            const size_t numSections= getNumSections();
            const Matrix &xi= beamIntegr->getIntegrPointCoords(numSections,L);
            theSections.addInitialSectionDeformations(*strainLoad,loadFactor,xi,L);
          }
        else
          {
            std::cerr << "XC::ForceBeamColumn2d::addLoad -- load type unknown for element with tag: " <<
            this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

//! @brief Add the inertial loads to the unbalanced load vector.
int XC::ForceBeamColumn2d::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // Check for a XC::quick return
    if(rho == 0.0)
      return 0;

    // get R * accel from the nodes
    const Vector &Raccel1= theNodes[0]->getRV(accel);
    const Vector &Raccel2= theNodes[1]->getRV(accel);

    const double L= theCoordTransf->getInitialLength();
    //const double m= 0.5*rho*L;

    // Should be done through p0[0]
    /*
    load(0) -= m*Raccel1(0);
    load(1) -= m*Raccel1(1);
    load(3) -= m*Raccel2(0);
    load(4) -= m*Raccel2(1);
    */
    return 0;
  }

//! @brief Returns the acciones del elemento sobre los nodos incluidas
//! las de inercia.
const XC::Vector &XC::ForceBeamColumn2d::getResistingForceIncInertia(void) const
  {
    // Compute the current resisting force
    theVector = this->getResistingForce();

    // Check for a quick return
    if(rho != 0.0)
      {
        const XC::Vector &accel1 = theNodes[0]->getTrialAccel();
        const XC::Vector &accel2 = theNodes[1]->getTrialAccel();

        const double L = theCoordTransf->getInitialLength();
        const double m = 0.5*rho*L;

        theVector(0) += m*accel1(0);
        theVector(1) += m*accel1(1);
        theVector(3) += m*accel2(0);
        theVector(4) += m*accel2(1);

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
int XC::ForceBeamColumn2d::sendData(CommParameters &cp)
  {
    int res= NLForceBeamColumn2dBase::sendData(cp);
    res+= sendBeamIntegrationPtr(beamIntegr,28,29,getDbTagData(),cp);
    res+= v0.sendData(cp,getDbTagData(),CommMetaData(30));
    res+= cp.sendInt(maxSubdivisions,getDbTagData(),CommMetaData(31));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::ForceBeamColumn2d::recvData(const CommParameters &cp)
  {
    int res= NLForceBeamColumn2dBase::recvData(cp);
    beamIntegr= receiveBeamIntegrationPtr(beamIntegr,28,29,getDbTagData(),cp);
    res+= v0.receiveData(cp,getDbTagData(),CommMetaData(30));
    res+= cp.receiveInt(maxSubdivisions,getDbTagData(),CommMetaData(31));
    return res;
  }

//! @brief Envía el objeto.
int XC::ForceBeamColumn2d::sendSelf(CommParameters &cp)
  {
    inicComm(32);
    int res= sendData(cp);
    
    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ForceBeamColumn2d::sendSelf() - failed to send ID data.\n";
    return res;
  }


//! @brief Envía el objeto.
int XC::ForceBeamColumn2d::recvSelf(const CommParameters &cp)
  {
    inicComm(32);

    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ForceBeamColumn3d::recvSelf -- failed to receive ID data\n";
    else
      res+= recvData(cp);
    return res;
  }

int XC::ForceBeamColumn2d::getInitialFlexibility(Matrix &fe) const
  {
    fe.Zero();

    const double L = theCoordTransf->getInitialLength();
    const double oneOverL  = 1.0/L;

    // Flexibility from elastic interior
    beamIntegr->addElasticFlexibility(L, fe);

    const size_t numSections= getNumSections();
    std::vector<double> xi(section_matrices.getMaxNumSections());
    beamIntegr->getSectionLocations(numSections, L, &xi[0]);

    std::vector<double> wt(section_matrices.getMaxNumSections());
    beamIntegr->getSectionWeights(numSections, L, &wt[0]);

    for(size_t i = 0; i < numSections; i++)
      {
        int order      = theSections[i]->getOrder();
        const XC::ID &code = theSections[i]->getType();
    
        Matrix fb(workArea, order, NEBD);
    
        const double xL= xi[i];
        const double xL1= xL-1.0;
        const double wtL= wt[i]*L;
    
        const XC::Matrix &fSec = theSections[i]->getInitialFlexibility();
        fb.Zero();
        double tmp;
        int jj;
        for(int ii = 0; ii < order; ii++)
          {
            switch(code(ii))
              {
              case SECTION_RESPONSE_P:
                for(jj = 0; jj < order; jj++)
                  fb(jj,0) += fSec(jj,ii)*wtL;
                break;
              case SECTION_RESPONSE_MZ:
                for(jj = 0; jj < order; jj++)
                  {
                    tmp = fSec(jj,ii)*wtL;
                    fb(jj,1)+= xL1*tmp;
                    fb(jj,2)+= xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VY:
                for(jj = 0; jj < order; jj++)
                  {
                    tmp = oneOverL*fSec(jj,ii)*wtL;
                    fb(jj,1) += tmp;
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
              default:
                break;
              }
          }
      }
    return 0;
  }

//! @brief Computes displacements at section(s).
void XC::ForceBeamColumn2d::compSectionDisplacements(std::vector<Vector> &sectionCoords,std::vector<Vector> &sectionDispls) const
  {
    // get basic displacements and increments
    static Vector ub(NEBD);
    ub = theCoordTransf->getBasicTrialDisp();

    const double L = theCoordTransf->getInitialLength();

    // get integration point positions and weights
    //   const XC::Matrix &xi_pt  = quadRule.getIntegrPointCoords(numSections);
    // get integration point positions and weights
    const size_t numSections= getNumSections();
    static double pts[SectionMatrices::maxNumSections];
    beamIntegr->getSectionLocations(numSections, L, pts);

    // setup Vandermode and CBDI influence matrices
    double xi;

    // get CBDI influence matrix
    Matrix ls(numSections, numSections);
    getCBDIinfluenceMatrix(numSections, pts, L, ls);

    // get section curvatures
    Vector kappa(numSections);  // curvature
    static XC::Vector vs;              // section deformations

    for(size_t i=0; i<numSections; i++)
      {
        // THIS IS VERY INEFFICIENT ... CAN CHANGE LATER
        int sectionKey = 0;
        const XC::ID &code = theSections[i]->getType();
        int ii;
        for(ii = 0; ii < code.Size(); ii++)
          if(code(ii) == SECTION_RESPONSE_MZ)
            {
              sectionKey = ii;
              break;
            }
        if(ii == code.Size())
          {
            std::cerr << "FATAL NLBeamColumn2d::compSectionDispls - section does not provide Mz response\n";
            exit(-1);
          }

        // get section deformations
        vs = theSections[i]->getSectionDeformation();
        kappa(i) = vs(sectionKey);
      }

     Vector w(numSections);
     static XC::Vector xl(NDM), uxb(NDM);
     static XC::Vector xg(NDM), uxg(NDM);

     // w = ls * kappa;
     w.addMatrixVector (0.0, ls, kappa, 1.0);

     for(size_t i=0; i<numSections; i++)
       {
         xi= pts[i];

         xl(0)= xi * L;
         xl(1)= 0;

         // get section global coordinates
         sectionCoords[i] = theCoordTransf->getPointGlobalCoordFromLocal(xl);

         // compute section displacements
         uxb(0)= xi * ub(0); // consider linear variation for axial displacement. CHANGE LATER!!!!!!!!!!
         uxb(1)= w(i);

      // get section displacements in global system
      sectionDispls[i] = theCoordTransf->getPointGlobalDisplFromBasic(xi, uxb);
   }
   return;
}

//! @brief Imprime el elemento en el stream being passed as parameter.
void XC::ForceBeamColumn2d::Print(std::ostream &s, int flag)
  {
    const size_t numSections= getNumSections();
    if(flag == 2)
      {

        s << "#ForceBeamColumn2D\n";

        const Vector &node1Crd = theNodes[0]->getCrds();
        const Vector &node2Crd = theNodes[1]->getCrds();
        const Vector &node1Disp = theNodes[0]->getDisp();
        const Vector &node2Disp = theNodes[1]->getDisp();

        s << "#NODE " << node1Crd(0) << " " << node1Crd(1)
          << " " << node1Disp(0) << " " << node1Disp(1) << " " << node1Disp(2) << std::endl;

        s << "#NODE " << node2Crd(0) << " " << node2Crd(1)
          << " " << node2Disp(0) << " " << node2Disp(1) << " " << node2Disp(2) << std::endl;

        double P  = Secommit(0);
        double M1 = Secommit(1);
        double M2 = Secommit(2);
        double L = theCoordTransf->getInitialLength();
        double V = (M1+M2)/L;

        s << "#END_FORCES " << -P+p0[0] << " " << V+p0[1] << " " << M1 << std::endl;
        s << "#END_FORCES " << P << " " << -V+p0[2] << " " << M2 << std::endl;

        // plastic hinge rotation
        static Vector vp(3);
        static Matrix fe(3,3);
        this->getInitialFlexibility(fe);
        vp= theCoordTransf->getBasicTrialDisp();
        vp.addMatrixVector(1.0, fe, Se, -1.0);
        s << "#PLASTIC_HINGE_ROTATION " << vp[1] << " " << vp[2] << " " << 0.1*L << " " << 0.1*L << std::endl;

        // allocate array of vectors to store section coordinates and displacements
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
            s << "#SECTION " << (coords[i])(0) << " " << (coords[i])(1);
            s << " " << (displs[i])(0) << " " << (displs[i])(1) << std::endl;
            theSections[i]->Print(s, flag);
          }
      }
    else
      {
        s << "\nElement: " << this->getTag() << " Type: ForceBeamColumn2d ";
        s << "\tConnected Nodes: " << theNodes;
        s << "\tNumber of Sections: " << getNumSections();
        s << "\tMass density: " << rho << std::endl;
        beamIntegr->Print(s, flag);
        double P  = Secommit(0);
        double M1 = Secommit(1);
        double M2 = Secommit(2);
        double L = theCoordTransf->getInitialLength();
        double V = (M1+M2)/L;
        theVector(1) = V;
        theVector(4) = -V;
        s << "\tEnd 1 Forces (P V M): " << -P+p0[0] << " " << V+p0[1] << " " << M1 << std::endl;
        s << "\tEnd 2 Forces (P V M): " << P << " " << -V+p0[2] << " " << M2 << std::endl;

        if(flag == 1)
          {
            for(size_t i = 0; i < numSections; i++)
              s << "\numSections "<<i<<" :" << *theSections[i];
          }
      }
  }

//! @brief Imprime el elemento en el stream de salida.
std::ostream &XC::operator<<(std::ostream &s, ForceBeamColumn2d &E)
  {
    E.Print(s);
    return s;
  }

XC::Response *XC::ForceBeamColumn2d::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    const size_t numSections= getNumSections();
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
      return new ElementResponse(this, 3, Vector(3));

    // plastic rotation -
    else if(argv[0] == "plasticRotation" || argv[0] == "plasticDeformation")
      return new ElementResponse(this, 4, Vector(3));

    // point of inflection
    else if(argv[0] == "inflectionPoint")
      return new ElementResponse(this, 5, 0.0);

    // tangent drift
    else if(argv[0] == "tangentDrift")
      return new ElementResponse(this, 6, Vector(2));

    // section response -
    else if(argv[0] == "section")
      {
        const size_t argc= argv.size();
        if(argc <= 2)
          return 0;

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

int XC::ForceBeamColumn2d::getResponse(int responseID, Information &eleInfo)
  {
    static XC::Vector vp(3);
    static XC::Matrix fe(3,3);

    if(responseID == 1)
      return eleInfo.setVector(this->getResistingForce());
    else if(responseID == 2)
      {
        theVector(3) =  Se(0);
        theVector(0) = -Se(0)+p0[0];
        theVector(2) = Se(1);
        theVector(5) = Se(2);
        double V = (Se(1)+Se(2))/theCoordTransf->getInitialLength();
        theVector(1) =  V+p0[1];
        theVector(4) = -V+p0[2];
        return eleInfo.setVector(theVector);
      }
    // Chord rotation
    else if(responseID == 3)
      {
        vp = theCoordTransf->getBasicTrialDisp();
        return eleInfo.setVector(vp);
      }
    // Plastic rotation
    else if(responseID == 4)
      {
        this->getInitialFlexibility(fe);
        vp = theCoordTransf->getBasicTrialDisp();
        vp.addMatrixVector(1.0, fe, Se, -1.0);
        return eleInfo.setVector(vp);
      }
    // Point of inflection
    else if(responseID == 5)
      {
        double LI = 0.0;
        if(fabs(Se(1)+Se(2)) > DBL_EPSILON)
          {
            double L = theCoordTransf->getInitialLength();
            LI = Se(1)/(Se(1)+Se(2))*L;
          }
        return eleInfo.setDouble(LI);
      }
    // Tangent drift
    else if(responseID == 6)
      {
        double d2 = 0.0;
        double d3 = 0.0;

        double L = theCoordTransf->getInitialLength();

        // Location of inflection point from node I
        double LI = 0.0;
        if(fabs(Se(1)+Se(2)) > DBL_EPSILON)
          LI = Se(1)/(Se(1)+Se(2))*L;

	std::vector<double> wts(section_matrices.getMaxNumSections());
        const size_t numSections= getNumSections();
        beamIntegr->getSectionWeights(numSections, L, &wts[0]);

        std::vector<double> pts(section_matrices.getMaxNumSections());
        beamIntegr->getSectionLocations(numSections, L, &pts[0]);

        for(size_t i= 0;i<numSections;i++)
          {
            double x = pts[i]*L;
            if(x > LI)
              continue;
            const XC::ID &type = theSections[i]->getType();
            int order = theSections[i]->getOrder();
            double kappa = 0.0;
            for(int j = 0; j < order; j++)
              if(type(j) == SECTION_RESPONSE_MZ)
                kappa += vs[i](j);
            double b = -LI+x;
            d2 += (wts[i]*L)*kappa*b;
          }

        d2 += beamIntegr->getTangentDriftI(L, LI, Se(1), Se(2));

        for(size_t i= numSections-1;i>=0;i--)
          {
            double x = pts[i]*L;
            if(x < LI)
              continue;
            const XC::ID &type = theSections[i]->getType();
            int order = theSections[i]->getOrder();
            double kappa = 0.0;
            for(int j = 0; j < order; j++)
              if(type(j) == SECTION_RESPONSE_MZ)
                kappa += vs[i](j);
            double b = x-LI;
            d3+= (wts[i]*L)*kappa*b;
          }

        d3+= beamIntegr->getTangentDriftJ(L, LI, Se(1), Se(2));

        static XC::Vector d(2);
        d(0) = d2;
        d(1) = d3;
        return eleInfo.setVector(d);
      }
    else
      return -1;
  }

int XC::ForceBeamColumn2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
  if(argc < 1)
    return 0;

  int result = -1;

        const size_t numSections= getNumSections();
  if(argv[0] == "rho")
    return param.addObject(1, this);
  
  // section response -
  else if(strstr(argv[0].c_str(),"sectionX") != 0) {
    if(argc > 2) {
      float sectionLoc = atof(argv[1]);

      std::vector<double> xi(section_matrices.getMaxNumSections());
      double L = theCoordTransf->getInitialLength();
      beamIntegr->getSectionLocations(numSections, L, &xi[0]);
      
      sectionLoc /= L;

      float minDistance = fabs(xi[0]-sectionLoc);
      int sectionNum = 0;
      for(size_t i = 1; i < numSections; i++)
        {
	  if(fabs(xi[i]-sectionLoc) < minDistance)
            {
	      minDistance = fabs(xi[i]-sectionLoc);
	      sectionNum = i;
	    }
        }
      std::vector<std::string> argv2(argv);
      argv2.erase(argv2.begin(),argv2.begin()+2);
      return theSections[sectionNum]->setParameter(argv2, param);
    }
  }

  // If the parameter belongs to a section or lower
  else if(strstr(argv[0].c_str(),"section") != 0) {
    
    if(argc < 3)
      return -1;

    // Get section number: 1...Np
    size_t sectionNum = atoi(argv[1]);
    
    if(sectionNum > 0 && sectionNum <= numSections)
      return setSectionParameter(theSections[sectionNum-1],argv,2,param);
    else
      return -1;
  }
  
  else if(strstr(argv[0].c_str(),"integration") != 0) {
    
    if(argc < 2)
      return 0;

        std::vector<std::string> argv1(argv);
        argv1.erase(argv1.begin(),argv1.begin()+2);
    return beamIntegr->setParameter(argv1, param);
  }

  // Default, send to everything
  int ok;

  for(size_t i = 0; i < numSections; i++)
    {
      ok = theSections[i]->setParameter(argv, param);
      if(ok != -1)
        result = ok;
    }
  
  ok = beamIntegr->setParameter(argv, param);
  if(ok != -1)
    result = ok;
  return result;
  }

int XC::ForceBeamColumn2d::updateParameter (int parameterID, Information &info)
  {
    // If the parameterID value is not equal to 1 it belongs
    // to section or material further down in the hierarchy.

    if(parameterID == 1)
      {
        this->rho = info.theDouble;
        return 0;
      }
    else if(parameterID > 0 )
      {
        // Extract the section number
        int sectionNumber = (int)( floor((double)parameterID) / (100000) );

        int ok = -1;
        const size_t numSections= getNumSections();
        for(size_t i=0; i<numSections; i++)
          {
            if(sectionNumber == theSections[i]->getTag())
              { ok = theSections[i]->updateParameter(parameterID, info); }
          }
        if(ok < 0)
          {
            std::cerr << "XC::ForceBeamColumn2d::updateParameter() - could not update parameter. " << std::endl;
            return ok;
          }
        else
          { return ok; }
      }
    else
      {
        std::cerr << "XC::ForceBeamColumn2d::updateParameter() - could not update parameter. " << std::endl;
        return -1;
      }
  }

void XC::ForceBeamColumn2d::setSectionPointers(const std::vector<SeccionBarraPrismatica *> &secPtrs)
  {
    const size_t numSections= getNumSections();
    if(numSections > section_matrices.getMaxNumSections())
      { std::cerr << "Error: ForceBeamColumn2d::setSectionPointers -- max number of sections exceeded"; }
    setSections(secPtrs);
  }
