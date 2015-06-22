//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

// $Revision: 1.25 $
// $Date: 2003/05/15 21:30:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/nonlinearBeamColumn/element/NLBeamColumn3d.cpp,v $


// Written: Remo Magalhaes de Souza (rmsouza.ce.berkeley.edu) on 03/99
// Revised: rms 06/99 (mass matrix)
//          rms 07/99 (using setDomain)
//          rms 08/99 (included P-Delta effect)
//            fmk 10/99 setResponse() & getResponse()
//          rms 11/99 (included rigid joint offsets)
//          rms 04/00 (using transformation class w/ linear or corotational transf)
//          rms 04/00 (generalized to iterative/non-iterative algorithm)
//          mhs 06/00 (using new_ section class w/ variable dimensions)
//          rms 06/00 (torsional stiffness considered at the section level)
//          rms 06/00 (making copy of the sections)
//          rms 06/00 (storing section history variables at the element level)
//          rms 07/00 (new_ state determination procedure, no need to store fscommit)
//
// Purpose: This file contains the implementation for the XC::NLBeamColumn3d class.
//          NLBeamColumn3d is a materially nonlinear flexibility based frame element.

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cfloat>

#include <domain/mesh/element/Information.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn3d.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>
#include <domain/mesh/element/coordTransformation/CrdTransf3d.h>
#include <domain/mesh/node/Node.h>

#include <material/section/SeccionBarraPrismatica.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussLobattoQuadRule1d01.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "domain/component/Parameter.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"

XC::GaussLobattoQuadRule1d01 XC::NLBeamColumn3d::quadRule;

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::NLBeamColumn3d::NLBeamColumn3d(int tag)
  : XC::NLForceBeamColumn3dBase(tag,ELE_TAG_NLBeamColumn3d) {load.reset(NEGD);}

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::NLBeamColumn3d::NLBeamColumn3d(int tag,int numSec,const Material *m,const CrdTransf *coordTransf)
  : XC::NLForceBeamColumn3dBase(tag,ELE_TAG_NLBeamColumn3d,numSec,m,coordTransf)
  {load.reset(NEGD);}

// constructor which takes the unique element tag, sections,
// and the node XC::ID's of it's nodal end points.
// allocates the necessary space needed by each object
XC::NLBeamColumn3d::NLBeamColumn3d(int tag, int nodeI, int nodeJ,
                                int numSections,const std::vector<SeccionBarraPrismatica *> &sectionPtrs,
                                CrdTransf3d &coordTransf, double massDensPerUnitLength,
                                int maxNumIters, double tolerance)
  : XC::NLForceBeamColumn3dBase(tag,ELE_TAG_NLBeamColumn3d,numSections,nullptr,&coordTransf)
  {
    load.reset(NEGD);
    theNodes.set_id_nodos(nodeI,nodeJ);

    // get copy of the sections
    isTorsion= setSections(sectionPtrs);
    if(!isTorsion)
      std::cerr << "XC::NLBeamColumn3d::NLBeamColumn3d -- no torsion detected in sections, " <<
       "continuing with element torsional stiffness GJ/L = " << 1.0/DefaultLoverGJ;

  }


//! @brief Constructor virtual.
XC::Element* XC::NLBeamColumn3d::getCopy(void) const
  { return new NLBeamColumn3d(*this); }


void XC::NLBeamColumn3d::setDomain(Domain *theDomain)
  {
   // check XC::Domain is not null - invoked when object removed from a domain
    NLForceBeamColumn3dBase::setDomain(theDomain);
    if(theDomain)
      {
        // ensure connected nodes have correct number of dof's
        int dofNode1 = theNodes[0]->getNumberDOF();
        int dofNode2 = theNodes[1]->getNumberDOF();

        if((dofNode1 != NND) || (dofNode2 != NND))
          {
            std::cerr << "XC::NLBeamColumn3d::setDomain(): Nd2 or Nd1 incorrect dof ";
            exit(0);
          }

        initialize_trf();

        // get element length
        const double L = theCoordTransf->getInitialLength();
        if(L == 0.0)
          {
            std::cerr << "XC::NLBeamColumn3d::setDomain(): Zero element length:" << this->getTag();
            exit(0);
          }
        if(initialFlag != 2)
          this->initializeSectionHistoryVariables();
      }
  }


int XC::NLBeamColumn3d::commitState(void)
  {

    int err = 0;

    // call element commitState to do any base class stuff
    if((err = this->NLForceBeamColumn3dBase::commitState()) != 0)
      {
        std::cerr << "XC::NLBeamColumn3d::commitState () - failed in base class";
        return err;
      }

    const size_t nSections= getNumSections();
    size_t i= 0;
    do
      { vscommit[i] = vs[i]; }
    while(err == 0 && i < nSections);

    if(err)
      return err;

    // commit the transformation between coord. systems
    if((err = theCoordTransf->commitState()) != 0)
      return err;

    // commit the element variables state
    kvcommit = kv;
    Secommit = Se;
    return err;
  }


int XC::NLBeamColumn3d::revertToLastCommit(void)
  {
    int err;

    // call element revertToLastCommit to do any base class stuff
    if((err = this->NLForceBeamColumn3dBase::revertToLastCommit()) != 0)
      {
        std::cerr << "XC::NLBeamColumn3d::revertToLastCommit() - failed in base class";
        return err;
      }

    setTrialSectionDeformations(vscommit);
    const size_t nSections= getNumSections();
    size_t i = 0;
    do
      {
        vs[i] = vscommit[i];
        Ssr[i] = theSections[i]->getStressResultant();
        fs[i]  = theSections[i]->getSectionFlexibility();
        i++;
      }
    while(err == 0 && i < nSections);

    if(err)
      return err;

    // revert the transformation to last commit
    if((err = theCoordTransf->revertToLastCommit()) != 0)
      return err;

    // revert the element state to last commit
    Se= Secommit;
    kv= kvcommit;

    initialFlag = 0;
    // return this->update();
    return err;
  }


int XC::NLBeamColumn3d::revertToStart(void)
  {
    // revert the sections state to start
    int err;

    if((err = this->NLForceBeamColumn3dBase::revertToStart()) != 0)
      {
        std::cerr << "XC::NLBeamColumn3d::revertToLastCommit() - failed in base class";
        return err;
      }

    const size_t nSections= getNumSections();
    size_t i= 0;
    do
      {
        fs[i].Zero();
        vs[i].Zero();
        Ssr[i].Zero();

      }
    while (err == 0 && i < nSections);

    if(err) return err;

    // revert the transformation to start
    if((err = theCoordTransf->revertToStart()) != 0)
       return err;

    // revert the element state to start
    Secommit.Zero();
    kvcommit.Zero();

    Se.Zero();
    kv.Zero();

    initialFlag = 0;
    // this->update();
    return err;
  }



const XC::Matrix &XC::NLBeamColumn3d::getInitialStiff(void) const
  {

    // check for XC::quick return
    if(!Ki.Nula())
      return Ki;

    // get integration point positions and weights
    const size_t nSections= getNumSections();
    const Matrix &xi_pt= quadRule.getIntegrPointCoords(nSections);
    const Vector &weight= quadRule.getIntegrPointWeights(nSections);

    static Matrix f(NEBD,NEBD);   // element flexibility matrix
    static Matrix I(NEBD,NEBD);   // an identity matrix for matrix inverse

    I.Zero();
    for(size_t i=0; i<NEBD; i++)
      I(i,i) = 1.0;

    const double L= theCoordTransf->getInitialLength();
    const double oneOverL= 1.0/L;

    // initialize f and vr for integration
    f.Zero();

    for(size_t i=0; i<nSections; i++)
      {
        int order= theSections[i]->getOrder();
        const XC::ID &code= theSections[i]->getType();

        Vector Ss(workArea, order);
        Vector dSs(&workArea[order], order);
        Vector dvs(&workArea[2*order], order);

        Matrix fb(&workArea[3*order], order, NEBD);

        const double xL  = xi_pt(i,0);
        const double xL1 = xL-1.0;

        // get section flexibility matrix
        const XC::Matrix &fSec = theSections[i]->getInitialFlexibility();

        // f = f + (b^ fs * b) * weight(i);
        //f.addMatrixTripleProduct(1.0, b[i], fs[i], weight(i));
        int jj, ii;
        fb.Zero();
        double tmp;
        for(ii = 0; ii < order; ii++)
          {
            switch(code(ii))
              {
              case SECTION_RESPONSE_P:
                for(jj = 0; jj < order; jj++)
                  fb(jj,0)+= fSec(jj,ii)*weight(i);
                break;
              case SECTION_RESPONSE_MZ:
                for(jj = 0; jj < order; jj++)
                  {
                    tmp = fSec(jj,ii)*weight(i);
                    fb(jj,1)+= xL1*tmp;
                    fb(jj,2)+= xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VY:
                for(jj = 0; jj < order; jj++)
                  {
                    tmp = oneOverL*fSec(jj,ii)*weight(i);
                    fb(jj,1)+= tmp;
                    fb(jj,2)+= tmp;
                  }
                break;
              case SECTION_RESPONSE_MY:
                for(jj = 0; jj < order; jj++)
                  {
                    tmp= fSec(jj,ii)*weight(i);
                    fb(jj,3)+= xL1*tmp;
                    fb(jj,4)+= xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VZ:
                for(jj = 0; jj < order; jj++)
                  {
                    tmp = oneOverL*fSec(jj,ii)*weight(i);
                    fb(jj,3)+= tmp;
                    fb(jj,4)+= tmp;
                  }
                break;
              case SECTION_RESPONSE_T:
                for(jj = 0; jj < order; jj++)
                  fb(jj,5)+= fSec(jj,ii)*weight(i);
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
                  f(0,jj)+= fb(ii,jj);
                break;
              case SECTION_RESPONSE_MZ:
                for(jj = 0; jj < 6; jj++)
                  {
                    tmp = fb(ii,jj);
                    f(1,jj)+= xL1*tmp;
                    f(2,jj)+= xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VY:
                for(jj = 0; jj < 6; jj++)
                  {
                    tmp = oneOverL*fb(ii,jj);
                    f(1,jj)+= tmp;
                    f(2,jj)+= tmp;
                  }
                break;
              case SECTION_RESPONSE_MY:
                for(jj = 0; jj < 6; jj++)
                  {
                    tmp = fb(ii,jj);
                    f(3,jj)+= xL1*tmp;
                    f(4,jj)+= xL*tmp;
                  }
                break;
              case SECTION_RESPONSE_VZ:
                for(jj = 0; jj < 6; jj++)
                  {
                    tmp = oneOverL*fb(ii,jj);
                    f(3,jj)+= tmp;
                    f(4,jj)+= tmp;
                  }
                break;
              case SECTION_RESPONSE_T:
                for(jj = 0; jj < 6; jj++)
                  f(5,jj)+= fb(ii,jj);
                  break;
              default:
                break;
              }
           }
      }

    f*= L;
    if(!isTorsion)
      f(5,5) = DefaultLoverGJ;

    // calculate element stiffness matrix
    static XC::Matrix kvInit(NEBD, NEBD);
    if(f.Solve(I,kvInit) < 0)
      std::cerr << "XC::NLBeamColumn3d::updateElementState() - could not invert flexibility\n";

    // set Ki
    Ki= Matrix(theCoordTransf->getInitialGlobalStiffMatrix(kvInit));
    return Ki;
  }

int XC::NLBeamColumn3d::update(void)
  {

    // if have completed a recvSelf() - do a revertToLastCommit
    // to get Ssr, etc. set correctly
    if(initialFlag == 2)
      this->revertToLastCommit();

    // update the transformation
    theCoordTransf->update();

    // get basic displacements and increments
    static Vector v(NEBD);
    static Vector dv(NEBD);

    v = theCoordTransf->getBasicTrialDisp();
    dv = theCoordTransf->getBasicIncrDeltaDisp();

    // get integration point positions and weights
    const size_t nSections= getNumSections();
    const Matrix &xi_pt = quadRule.getIntegrPointCoords(nSections);
    const Vector &weight = quadRule.getIntegrPointWeights(nSections);

    static Vector vr(NEBD);       // element residual displacements
    static Matrix f(NEBD,NEBD);   // element flexibility matrix

    static Matrix I(NEBD,NEBD);   // an identity matrix for matrix inverse
    double dW;                    // section strain energy (work) norm

    I.Zero();
    for(size_t i=0; i<NEBD; i++)
      I(i,i) = 1.0;

    // calculate nodal force increments and update nodal forces
    static XC::Vector dSe(NEBD);

    // dSe = kv * dv;
    dSe.addMatrixVector(0.0, kv, dv, 1.0);

    const double L= theCoordTransf->getInitialLength();
    const double oneOverL= 1.0/L;

    for(int j=0; j < maxIters; j++)
      {
        Se+= dSe;

        // initialize f and vr for integration
        f.Zero();
        vr.Zero();

        const size_t nSections= getNumSections();
        for(size_t i=0; i<nSections; i++)
          {
            int order= theSections[i]->getOrder();
            const XC::ID &code = theSections[i]->getType();

            Vector Ss(workArea, order);
            Vector dSs(&workArea[order], order);
            Vector dvs(&workArea[2*order], order);

            Matrix fb(&workArea[3*order], order, NEBD);

            double xL  = xi_pt(i,0);
            double xL1 = xL-1.0;

            // calculate total section forces
            // Ss = b*Se + bp*currDistrLoad;
            //Ss.addMatrixVector(0.0, b[i], Se, 1.0);
            int ii;
            for(ii = 0; ii < order; ii++)
              {
                switch(code(ii))
                  {
                  case SECTION_RESPONSE_P:
                    Ss(ii) = Se(0);
                    break;
                  case SECTION_RESPONSE_MZ:
                    Ss(ii) = xL1*Se(1) + xL*Se(2);
                    break;
                  case SECTION_RESPONSE_VY:
                    Ss(ii) = oneOverL*(Se(1)+Se(2));
                    break;
                  case SECTION_RESPONSE_MY:
                    Ss(ii) = xL1*Se(3) + xL*Se(4);
                    break;
                  case SECTION_RESPONSE_VZ:
                    Ss(ii) = oneOverL*(Se(3)+Se(4));
                    break;
                  case SECTION_RESPONSE_T:
                    Ss(ii) = Se(5);
                    break;
                  default:
                    Ss(ii) = 0.0;
                    break;
                  }
              }

            // Add the effects of element loads, if present
	    if(!sp.Nula())
             {
               const XC::Matrix &s_p= sp;
               for(ii = 0; ii < order; ii++)
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
            dSs = Ss;
            dSs.addVector(1.0, Ssr[i], -1.0);  // dSs = Ss - Ssr[i];

            // compute section deformation increments
            //       vs+= fs * dSs;
            dvs.addMatrixVector(0.0, fs[i], dSs, 1.0);

            if(initialFlag != 0)
              vs[i]+= dvs;

            theSections[i]->setTrialSectionDeformation(vs[i]);

            // get section resisting forces
            Ssr[i] = theSections[i]->getStressResultant();

            // get section flexibility matrix
            fs[i] = theSections[i]->getSectionFlexibility();

            // calculate section residual deformations
            // dvs = fs * (Ss - Ssr);
            dSs = Ss;
            dSs.addVector(1.0, Ssr[i], -1.0);  // dSs = Ss - Ssr[i];

            dvs.addMatrixVector(0.0, fs[i], dSs, 1.0);

            // integrate element flexibility matrix
            // f = f + (b^ fs * b) * weight(i);
                //f.addMatrixTripleProduct(1.0, b[i], fs[i], weight(i));
            int jj;
            const XC::Matrix &fSec = fs[i];
            fb.Zero();
            double tmp;
            for(ii = 0; ii < order; ii++)
              {
                switch(code(ii))
                  {
                  case SECTION_RESPONSE_P:
                    for(jj = 0; jj < order; jj++)
                      fb(jj,0)+= fSec(jj,ii)*weight(i);
                    break;
                  case SECTION_RESPONSE_MZ:
                    for(jj = 0; jj < order; jj++)
                      {
                        tmp = fSec(jj,ii)*weight(i);
                        fb(jj,1)+= xL1*tmp;
                        fb(jj,2)+= xL*tmp;
                      }
                    break;
                  case SECTION_RESPONSE_VY:
                    for(jj = 0; jj < order; jj++)
                      {
                        tmp = oneOverL*fSec(jj,ii)*weight(i);
                        fb(jj,1)+= tmp;
                        fb(jj,2)+= tmp;
                      }
                    break;
                  case SECTION_RESPONSE_MY:
                    for(jj = 0; jj < order; jj++)
                      {
                        tmp = fSec(jj,ii)*weight(i);
                        fb(jj,3)+= xL1*tmp;
                        fb(jj,4)+= xL*tmp;
                      }
                    break;
                  case SECTION_RESPONSE_VZ:
                    for(jj = 0; jj < order; jj++)
                      {
                        tmp = oneOverL*fSec(jj,ii)*weight(i);
                        fb(jj,3)+= tmp;
                        fb(jj,4)+= tmp;
                      }
                    break;
                  case SECTION_RESPONSE_T:
                    for(jj = 0; jj < order; jj++)
                      fb(jj,5)+= fSec(jj,ii)*weight(i);
                    break;
                  default:
                    break;
                }
            }
          for(ii = 0; ii < order; ii++)
            {
              switch (code(ii)) {
            case SECTION_RESPONSE_P:
              for(jj = 0; jj < 6; jj++)
                f(0,jj)+= fb(ii,jj);
              break;
            case SECTION_RESPONSE_MZ:
              for(jj = 0; jj < 6; jj++) {
                tmp = fb(ii,jj);
                f(1,jj)+= xL1*tmp;
                f(2,jj)+= xL*tmp;
              }
              break;
            case SECTION_RESPONSE_VY:
              for(jj = 0; jj < 6; jj++) {
                tmp = oneOverL*fb(ii,jj);
                f(1,jj)+= tmp;
                f(2,jj)+= tmp;
              }
              break;
            case SECTION_RESPONSE_MY:
              for(jj = 0; jj < 6; jj++) {
                tmp = fb(ii,jj);
                f(3,jj)+= xL1*tmp;
                f(4,jj)+= xL*tmp;
              }
              break;
            case SECTION_RESPONSE_VZ:
              for(jj = 0; jj < 6; jj++) {
                tmp = oneOverL*fb(ii,jj);
                f(3,jj)+= tmp;
                f(4,jj)+= tmp;
              }
              break;
            case SECTION_RESPONSE_T:
              for(jj = 0; jj < 6; jj++)
                f(5,jj)+= fb(ii,jj);
              break;
            default:
              break;
            }
          }

          // integrate residual deformations
          // vr+= (b^ (vs + dvs)) * weight(i);
          //vr.addMatrixTransposeVector(1.0, b[i], vs[i] + dvs, weight(i));
          dvs.addVector(1.0, vs[i], 1.0);
          double dei;
          for(ii = 0; ii < order; ii++) {
            dei = dvs(ii)*weight(i);
            switch(code(ii)) {
            case SECTION_RESPONSE_P:
              vr(0)+= dei; break;
            case SECTION_RESPONSE_MZ:
              vr(1)+= xL1*dei; vr(2)+= xL*dei; break;
            case SECTION_RESPONSE_VY:
              tmp = oneOverL*dei;
              vr(1)+= tmp; vr(2)+= tmp; break;
            case SECTION_RESPONSE_MY:
              vr(3)+= xL1*dei; vr(4)+= xL*dei; break;
            case SECTION_RESPONSE_VZ:
              tmp = oneOverL*dei;
              vr(3)+= tmp; vr(4)+= tmp; break;
            case SECTION_RESPONSE_T:
              vr(5)+= dei; break;
            default:
              break;
            }
          }
      }

      f  *= L;
      vr *= L;

      if(!isTorsion)
        {
          f(5,5) = DefaultLoverGJ;
          vr(5) = Se(5)*DefaultLoverGJ;
        }

      // calculate element stiffness matrix
      if(f.Invert(kv) < 0)
        std::cerr << "XC::NLBeamColumn3d::updateElementState() - could not invert flexibility\n";

      // dv = v - vr;
      dv = v;
      dv.addVector(1.0, vr, -1.0);

      // dSe = kv * dv;
      dSe.addMatrixVector(0.0, kv, dv, 1.0);

      dW = dv^ dSe;
      if(fabs(dW) < tol)
        break;

      if(maxIters == 1)
        {
          std::cerr << "XC::NLBeamColumn3d::updateElementState() - element: " << this->getTag() << " failed to converge but going on\n";
          break;
        }
      if(j == (maxIters-1))
        {
          std::cerr << "XC::NLBeamColumn3d::updateElementState() - element: " << this->getTag() << " failed to converge\n";
          std::cerr << "dW: " << dW  << "\n dv: " << dv << " dSe: " << dSe << std::endl;
          return -1;
        }
    }

  // determine resisting forces
    Se+= dSe;

    initialFlag = 1;

    return 0;
}



void XC::NLBeamColumn3d::getGlobalDispls(Vector &dg) const
  {
     // determine global displacements
     const XC::Vector &disp1 = theNodes[0]->getTrialDisp();
     const XC::Vector &disp2 = theNodes[1]->getTrialDisp();
     for(int i = 0; i < NND; i++)
       {
         dg(i)     = disp1(i);
         dg(i+NND) = disp2(i);
       }
  }



void XC::NLBeamColumn3d::getGlobalAccels(Vector &ag) const
  {
     // determine global displacements
     const XC::Vector &accel1 = theNodes[0]->getTrialAccel();
     const XC::Vector &accel2 = theNodes[1]->getTrialAccel();
     for(int i = 0; i < NND; i++)
       {
         ag(i)= accel1(i);
         ag(i+NND)= accel2(i);
       }
  }


void XC::NLBeamColumn3d::getForceInterpolatMatrix(double xi, Matrix &b, const XC::ID &code)
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
         case SECTION_RESPONSE_MY:                // Moment, My, interpolation
            b(i,3) = xi - 1.0;
            b(i,4) = xi;
            break;
         case SECTION_RESPONSE_VZ:                // Shear, Vz, interpolation
            b(i,3) = b(i,4) = 1.0/L;
            break;
         case SECTION_RESPONSE_T:                // Torque, T, interpolation
            b(i,5) = 1.0;
            break;
         default:
            break;
        }
     }
  }


void XC::NLBeamColumn3d::getDistrLoadInterpolatMatrix(double xi, Matrix &bp, const XC::ID &code)
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
         case SECTION_RESPONSE_MY:                // Moment, My, interpolation
            bp(i,2) = xi*(1-xi)*L*L/2;
            break;
         case SECTION_RESPONSE_VZ:                // Shear, Vz, interpolation
            bp(i,2) = (0.5-xi)*L;
            break;
         case SECTION_RESPONSE_T:                // Torsion, T, interpolation
            break;
         default:
            break;
         }
       }
  }


const XC::Matrix &XC::NLBeamColumn3d::getMass(void) const
  {
    const double L = theCoordTransf->getInitialLength();
    theMatrix(0,0) = theMatrix(1,1) = theMatrix(2,2) =
      theMatrix(6,6) = theMatrix(7,7) = theMatrix(8,8) = 0.5*rho*L;
    return theMatrix;
  }


void XC::NLBeamColumn3d::zeroLoad(void)
  {
    sp.Zero();
    p0.zero();
    NLForceBeamColumn3dBase::zeroLoad();
  }

int XC::NLBeamColumn3d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << nombre_clase() 
                << "; se intentó cargar el elemento "
                << getTag() << " que está desactivado." 
                << std::endl;
    else
      {
        if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
          {
            const size_t nSections= getNumSections();
            const XC::Matrix &xi_pt = quadRule.getIntegrPointCoords(nSections);
            const double L = theCoordTransf->getInitialLength();

            if(sp.Nula())
              sp= Matrix(5,nSections);
            sp+= beamMecLoad->getAppliedSectionForces(L,xi_pt,loadFactor); // Accumulate applied section forces due to element loads
            beamMecLoad->addReactionsInBasicSystem(L,loadFactor,p0); // Accumulate reactions in basic system
          }
        else
          {
            std::cerr << "XC::NLBeamColumn3d::addLoad()  -- load type unknown for element with tag: " << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

int XC::NLBeamColumn3d::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    // Check for a XC::quick return
    if(rho == 0.0)
      return 0;

    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);

    const double L = theCoordTransf->getInitialLength();
    const double m = 0.5*rho*L;

    load(0)-= m*Raccel1(0);
    load(1)-= m*Raccel1(1);
    load(2)-= m*Raccel1(2);
    load(6)-= m*Raccel2(0);
    load(7)-= m*Raccel2(1);
    load(8)-= m*Raccel2(2);

    return 0;
  }

const XC::Vector &XC::NLBeamColumn3d::getResistingForceIncInertia(void) const
  {
    // Check for a XC::quick return
    if(rho == 0.0)
      theVector = this->getResistingForce();

    if(rho != 0.0)
      {
        const XC::Vector &accel1 = theNodes[0]->getTrialAccel();
        const XC::Vector &accel2 = theNodes[1]->getTrialAccel();

       // Compute the current resisting force
       theVector = this->getResistingForce();

       const double L = theCoordTransf->getInitialLength();
       const double m = 0.5*rho*L;

       theVector(0)+= m*accel1(0);
       theVector(1)+= m*accel1(1);
       theVector(2)+= m*accel1(2);
       theVector(6)+= m*accel2(0);
       theVector(7)+= m*accel2(1);
       theVector(8)+= m*accel2(2);

      // add the damping forces if rayleigh damping
      if(!rayFactors.Nulos())
        theVector+= this->getRayleighDampingForces();
      }
    else
      {
        // add the damping forces if rayleigh damping
        if(!rayFactors.KNulos())
          theVector+= this->getRayleighDampingForces();
      }
    if(isDead())
      theVector*=dead_srf;
    return theVector;
  }

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::NLBeamColumn3d::sendData(CommParameters &cp)
  {
    int res= NLForceBeamColumn3dBase::sendData(cp);
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::NLBeamColumn3d::recvData(const CommParameters &cp)
  {
    int res= NLForceBeamColumn3dBase::recvData(cp);
    return res;
  }

int XC::NLBeamColumn3d::sendSelf(CommParameters &cp)
  {
    inicComm(29);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "NLBeamColumn3d::sendSelf -- failed to send ID data\n";
    return res;
  }


int XC::NLBeamColumn3d::recvSelf(const CommParameters &cp)
  {
    inicComm(29);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "NLBeamColumn3d::recvSelf() - failed to recv ID data";
    else
      res+= recvData(cp);
    return res;
  }


void XC::NLBeamColumn3d::compSectionDisplacements(std::vector<Vector> &sectionCoords,std::vector<Vector> &sectionDispls) const
  {
    // update the transformation
    theCoordTransf->update();

    // get basic displacements and increments
    static XC::Vector ub(NEBD);
    ub = theCoordTransf->getBasicTrialDisp();

    // get integration point positions and weights
    const size_t nSections= getNumSections();
    const XC::Matrix &xi_pt  = quadRule.getIntegrPointCoords(nSections);

    // setup Vandermode and CBDI influence matrices
    double xi;

    // get CBDI influence matrix
    Matrix ls(nSections, nSections);
    const double L = theCoordTransf->getInitialLength();
    getCBDIinfluenceMatrix(nSections, xi_pt, L, ls);

    // get section curvatures
    Vector kappa_y(nSections);  // curvature
    Vector kappa_z(nSections);  // curvature
    static XC::Vector vs;                // section deformations

    for(size_t i=0; i<nSections; i++)
      {
       // THIS IS VERY INEFFICIENT ... CAN CHANGE IF RUNS TOO SLOW
        int sectionKey1 = 0;
        int sectionKey2 = 0;
        const XC::ID &code = theSections[i]->getType();
        int j;
        for(j = 0; j < code.Size(); j++)
          {
            if(code(j) == SECTION_RESPONSE_MZ)
                sectionKey1 = j;
            if(code(j) == SECTION_RESPONSE_MY)
                sectionKey2 = j;
          }
        if(sectionKey1 == 0)
          {
            std::cerr << "FATAL XC::NLBeamColumn3d::compSectionResponse - section does not provide Mz response\n";
            exit(-1);
          }
        if(sectionKey2 == 0)
          {
            std::cerr << "FATAL XC::NLBeamColumn3d::compSectionResponse - section does not provide My response\n";
            exit(-1);
          }

        // get section deformations
        vs = theSections[i]->getSectionDeformation();

        kappa_z(i) = vs(sectionKey1);
        kappa_y(i) = vs(sectionKey2);
      }

    Vector v(nSections), w(nSections);
    static XC::Vector xl(NDM), uxb(NDM);
    static XC::Vector xg(NDM), uxg(NDM);
    // double theta;                             // angle of twist of the sections

    // v = ls * kappa_z;
    v.addMatrixVector (0.0, ls, kappa_z, 1.0);
    // w = ls * kappa_y *  (-1);
    w.addMatrixVector (0.0, ls, kappa_y, -1.0);

    for(size_t i=0; i<nSections; i++)
      {
        xi = xi_pt(i,0);

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
  }


void XC::NLBeamColumn3d::Print(std::ostream &s, int flag)
  {

    const size_t nSections= getNumSections();
    // flags with negative values are used by GSA
    if(flag == -1)
      {
        int eleTag = this->getTag();
        s << "NL_BEAM\t" << eleTag << "\t";
        s << theSections[0]->getTag() << "\t" << theSections[nSections-1]->getTag();
        s  << "\t" << theNodes.getTagNode(0) << "\t" << theNodes.getTagNode(1);
        s << "\t0\t0.0000000\n";
      }
    else if(flag < -1)
      {
      int eleTag = this->getTag();
      int counter = (flag +1) * -1;
      int i;
      const XC::Vector &force = this->getResistingForce();
      s << "FORCE\t" << eleTag << "\t" << counter << "\t0";
      for(i=0; i<3; i++)
        s << "\t" << force(i);
      s << std::endl;
      s << "FORCE\t" << eleTag << "\t" << counter << "\t1";
      for(i=0; i<3; i++)
        s << "\t" << force(i+6);
      s << std::endl;
      s << "MOMENT\t" << eleTag << "\t" << counter << "\t0";
      for(i=3; i<6; i++)
        s << "\t" << force(i);
      s << std::endl;
      s << "MOMENT\t" << eleTag << "\t" << counter << "\t1";
      for(i=3; i<6; i++)
        s << "\t" << force(i+6);
      s << std::endl;
   }

   else if(flag == 1) {
     static XC::Vector xAxis(3);
     static XC::Vector yAxis(3);
     static XC::Vector zAxis(3);

     theCoordTransf->getLocalAxes(xAxis, yAxis, zAxis);

     s << "#NLBeamColumn3D\n";
     s << "#LocalAxis " << xAxis(0) << " " << xAxis(1) << " " << xAxis(2)
       << " " << zAxis(0) << " " << zAxis(1) << " " << zAxis(2) << std::endl;

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

     // allocate array of vectors to store section coordinates and displacements
	static std::vector<Vector> coords;
	static std::vector<Vector> displs;
        coords.resize(nSections);
        displs.resize(nSections);
        for(size_t i= 0;i<nSections;i++)
          coords[i] = Vector(NDM);
        for(size_t i= 0;i<nSections;i++)
          displs[i] = Vector(NDM);

     // compute section location & displacements
     this->compSectionDisplacements(coords, displs);

     // spit out the section location & invoke print on the scetion
     for(size_t i=0; i<nSections; i++) {
       s << "#SECTION " << (coords[i])(0) << " " << (coords[i])(1) << " " << (coords[i])(2);       s << " " << (displs[i])(0) << " " << (displs[i])(1) << " " << (displs[i])(2) << std::endl;
       theSections[i]->Print(s, flag);
     }
   }

   else {
      s << "\nElement: " << this->getTag() << " Type: NLBeamColumn3d ";
      s << "\tConnected Nodes: " << theNodes;
      s << "\tNumber of Sections: " << nSections << std::endl;
      s << "\tElement End Forces (P MZ1 MZ2 MY1 MY2 T): " << Secommit;
      s << "\tResisting Force: " << this->getResistingForce();
   }
}


std::ostream &XC::operator<<(std::ostream &s, NLBeamColumn3d &E)
  {
    E.Print(s);
    return s;
  }


XC::Response *XC::NLBeamColumn3d::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
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
    else if((argv[0] == "defoANDforce") ||
             (argv[0] == "deformationANDforce") ||
             (argv[0] == "deformationsANDforces"))
      return new ElementResponse(this, 4, Vector(24));
    // section response -
    else if(argv[0] == "section")
      {
        if(argv.size() <= 2)
          return 0;

        size_t sectionNum = atoi(argv[1]);
        const size_t nSections= getNumSections();
        if(sectionNum > 0 && sectionNum <= nSections)
          return setSectionResponse(theSections[sectionNum-1],argv,2,eleInformation);
        else
          return 0;
      }
    else
      return 0;
  }

int XC::NLBeamColumn3d::getResponse(int responseID, Information &eleInfo)
{
  static XC::Vector force(12);
  static XC::Vector defoAndForce(24);
  double V, N, T, M1, M2;
  int i;
  const double L = theCoordTransf->getInitialLength();

  switch (responseID) {
  case 1:  // forces
    return eleInfo.setVector(this->getResistingForce());


  case 4:
    this->getGlobalDispls(force);
    this->getResistingForce();
    for(i = 0; i < 12; i++) {
      defoAndForce(i) = force(i);
      defoAndForce(i+12) = theVector(i);
    }
    return eleInfo.setVector(defoAndForce);

  case 2:
    // Axial
    N = Se(0);
    force(6) =  N;
    force(0) = -N+p0[0];

    // Torsion
    T = Se(5);
    force(9) =  T;
    force(3) = -T;

    // Moments about z and shears along y
    M1 = Se(1);
    M2 = Se(2);
    force(5)  = M1;
    force(11) = M2;
    V = (M1+M2)/L;
    force(1) =  V+p0[1];
    force(7) = -V+p0[2];

    // Moments about y and shears along z
    M1 = Se(3);
    M2 = Se(4);
    force(4)  = M1;
    force(10) = M2;
    V = -(M1+M2)/L;
    force(2) = -V+p0[3];
    force(8) =  V+p0[4];

    return eleInfo.setVector(force);

  default:
    return -1;
  }
}

int XC::NLBeamColumn3d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
        int ok = -1;

        // If the parameter belongs to the element itself
        if(argv[0] == "rho")
          return param.addObject(1, this);

        // If the parameter is belonging to a section or lower
        if(argv[0] == "section")
          {

          // For now, no parameters of the section itself:
	    if(argv.size()<5) {
            std::cerr << "For now: cannot handle parameters of the section itself." << std::endl;
            return -1;
          }

          // Reveal section and material tag numbers
          int paramSectionTag = atoi(argv[1]);
          int paramMatTag     = atoi(argv[3]);

          // Store section and material tag in theInfo
          ID theID(2);
          theID(0) = paramSectionTag;
          theID(1) = paramMatTag;
          //info.theID = theID; LCPT 22/04/2009

          // Find the right section and call its setParameter method
          const size_t nSections= getNumSections();
          for(size_t i=0; i<nSections; i++) {
            if(paramSectionTag == theSections[i]->getTag()) {
              ok = setSectionParameter(theSections[i],argv,2,param);
            }
          }

          if(ok < 0) {
            std::cerr << "XC::NLBeamColumn2d::setParameter() - could not set parameter. " << std::endl;
            return -1;
          }
          else {
            return ok + 100;
          }
        }

        // otherwise parameter is unknown for the XC::NLBeamColumn2d class
        else
          return -1;
  }

int XC::NLBeamColumn3d::updateParameter(int parameterID, Information &info)
  {
    ID *paramIDPtr;
    int ok = -1;
    const size_t nSections= getNumSections();
    switch (parameterID)
      {
      case 1:
        this->rho = info.theDouble;
        return 0;
      default:
        if(parameterID >= 100)
          {
             paramIDPtr = info.theID;
             ID paramID = (*paramIDPtr);
             int paramSectionTag = paramID(0);
             for(size_t i=0; i<nSections; i++)
               {
                 if(paramSectionTag == theSections[i]->getTag())
                   { ok = theSections[i]->updateParameter(parameterID-100, info); }
               }
             if(ok < 0)
               {
                 std::cerr << "XC::NLBeamColumn2d::updateParameter() - could not update parameter. " << std::endl;
                 return ok;
               }
             else
               { return ok; }
           }
         else
           return -1;
      }
  }
