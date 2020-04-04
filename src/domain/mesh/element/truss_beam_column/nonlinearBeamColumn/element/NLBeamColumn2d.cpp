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

// $Revision: 1.33 $
// $Date: 2003/05/15 21:30:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/nonlinearBeamColumn/element/NLBeamColumn2d.cpp,v $


//
// Written by Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu) on 01/99
// Revised: rms 01/99 (distributed loads)
//          rms 06/99 (mass matrix)
//          rms 07/99 (using setDomain)
//          rms 08/99 (included P-Delta effect)
//            fmk 10/99 setResponse() & getResponse()
//          rms 11/99 (included rigid joint offsets)
//          rms 04/00 (using transformation class w/ linear or corotational transf)
//          rms 04/00 (generalized to iterative/non-iterative algorithm)
//          mhs 06/00 (using new section class w/ variable dimensions)
//          rms 06/00 (torsional stiffness considered at the section level)
//          rms 06/00 (making copy of the sections)
//          rms 06/00 (storing section history variables at the element level)
//          rms 07/00 (new state determination procedure, no need to store fscommit)
//
// Purpose: This file contains the implementation for the XC::NLBeamColumn2d class.
//          NLBeamColumn2d.C is a materially nonlinear flexibility based frame element.

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cfloat>

#include <domain/mesh/element/utils/Information.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn2d.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include <domain/mesh/element/utils/coordTransformation/CrdTransf2d.h>
#include <domain/mesh/node/Node.h>
#include "domain/component/Parameter.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"

XC::GaussLobattoQuadRule1d01 XC::NLBeamColumn2d::quadRule;

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::NLBeamColumn2d::NLBeamColumn2d(int tag)
  : NLForceBeamColumn2dBase(tag,ELE_TAG_NLBeamColumn2d,0),
 maxSubdivisions(0), cosTheta(0.0), sinTheta(0.0)
  {
    load.reset(NEGD);
    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;
  }

//! @brief Constructor.
XC::NLBeamColumn2d::NLBeamColumn2d(int tag,int numSec, const Material *m,const CrdTransf *coordTransf)
  : NLForceBeamColumn2dBase(tag,ELE_TAG_NLBeamColumn2d,numSec,m,coordTransf),
  maxSubdivisions(0), cosTheta(0.0), sinTheta(0.0)
  {
    load.reset(NEGD);
    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;
  }

// constructor which takes the unique element tag, sections,
// and the node XC::ID's of it's nodal end points.
// allocates the necessary space needed by each object
XC::NLBeamColumn2d::NLBeamColumn2d (int tag, int nodeI, int nodeJ,
                                int numSections,const std::vector<PrismaticBarCrossSection *> &sectionPtrs,
                                CrdTransf2d &coordTransf, double massDensPerUnitLength,
                                int maxNumIters, double tolerance, int maxSub)
  : NLForceBeamColumn2dBase(tag,ELE_TAG_NLBeamColumn2d,numSections),
 maxSubdivisions(maxSub), cosTheta(0.0), sinTheta(0.0)
  {
    load.reset(NEGD);
    theNodes.set_id_nodes(nodeI,nodeJ);

    // get copy of the sections
    setSections(sectionPtrs);

    // get copy of the transformation object
    set_transf(&coordTransf);


    p0[0] = 0.0;
    p0[1] = 0.0;
    p0[2] = 0.0;

  }

//! @brief Virtual constructor.
XC::Element* XC::NLBeamColumn2d::getCopy(void) const
  { return new NLBeamColumn2d(*this); }


void XC::NLBeamColumn2d::setDomain(Domain *theDomain)
  {
    NLForceBeamColumn2dBase::setDomain(theDomain);
    // check XC::Domain is not null - invoked when object removed from a domain
    if(theDomain)
      {
        // ensure connected nodes have correct number of dof's
        int dofNode1 = theNodes[0]->getNumberDOF();
        int dofNode2 = theNodes[1]->getNumberDOF();

        if((dofNode1 !=3 ) || (dofNode2 != 3))
          {
            std::cerr << "XC::NLBeamColumn2d::setDomain(): Nd2 or Nd1 incorrect dof ";
            exit(0);
          }

        initialize_trf(); // initialize the transformation

        // get element length
        const double L = theCoordTransf->getInitialLength();
        if(L == 0.0)
          {
            std::cerr << "XC::NLBeamColumn2d::setDomain(): Zero element length:" << this->getTag();
            exit(0);
          }
        if(initialFlag == 0)
          this->initializeSectionHistoryVariables();
      }
  }



int XC::NLBeamColumn2d::commitState(void)
{
   int err = 0;

   // call element commitState to do any base class stuff
   if((err = this->XC::NLForceBeamColumn2dBase::commitState()) != 0) {
     std::cerr << "XC::NLBeamColumn2d::commitState () - failed in base class";
     return err;
   }

   const size_t nSections= getNumSections();
   size_t i = 0;
   do
     {
       vscommit[i] = vs[i];
     }
   while (err == 0 && i < nSections);

   if(err)
      return err;

   // commit the transformation between coord. systems
   if((err = theCoordTransf->commitState()) != 0)
      return err;

   // commit the element variables state
   kvcommit = kv;
   Secommit = Se;

   /***
   if(this->getTag() == 75) {
    for(size_t i=0; i<nSections; i++) {
      std::cerr << "stress: " << theSections[i]->getStressResultant();
      std::cerr << "strain: " << theSections[i]->getSectionDeformation();
      std::cerr << "flexibility: " << theSections[i]->getSectionFlexibility();
    }
   }
   ***/

   //   initialFlag = 0;  fmk - commented out, see what happens to Example3.1.tcl if uncommented
   //                         - i have not a clue why, ask remo if he ever gets in contact with us again!

   return err;
}


int XC::NLBeamColumn2d::revertToLastCommit()
{
   int err;
   size_t i = 0;

   const size_t nSections= getNumSections();
   do {
      vs[i] = vscommit[i];
      err = theSections[i]->revertToLastCommit();

      theSections[i]->setTrialSectionDeformation(vs[i]);
      Ssr[i] = theSections[i]->getStressResultant();
      fs[i]  = theSections[i]->getSectionFlexibility();

      i++;
   } while (err == 0 && i < nSections);


   if(err)
      return err;

   // revert the transformation to last commit
   if((err = theCoordTransf->revertToLastCommit()) != 0)
      return err;

   // revert the element state to last commit
   Se   = Secommit;
   kv   = kvcommit;

   initialFlag = 0;
   // this->update();

   return err;
}


int XC::NLBeamColumn2d::revertToStart()
{
   // revert the sections state to start
   int err;
   size_t i = 0;
   const size_t nSections= getNumSections();
   do {
       fs[i].Zero();
       vs[i].Zero();
       Ssr[i].Zero();
       err = theSections[i++]->revertToStart();

   }while (err == 0 && i < nSections);

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

   initialFlag = 0;
   // this->update();
   return err;
}




const XC::Matrix &XC::NLBeamColumn2d::getInitialStiff(void) const
  {
    // check for quick return
    if(Ki.isEmpty())
      {
        const size_t nSections= getNumSections();
        const Matrix &xi_pt  = quadRule.getIntegrPointCoords(nSections);
        const Vector &weight = quadRule.getIntegrPointWeights(nSections);

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
            const ID &code = theSections[i]->getType();
            Matrix fb(workArea, order, NEBD);
            double xL  = xi_pt(i,0);
            double xL1 = xL-1.0;

            // get section flexibility matrix
            const Matrix &fSec = theSections[i]->getInitialFlexibility();

            // integrate element flexibility matrix
            // f = f + (b^ fs * b) * weight(i);
            //f.addMatrixTripleProduct(1.0, b[i], fs[i], weight(i));
            fb.Zero();
            double tmp;
            for(int ii = 0; ii < order; ii++)
              {
                switch(code(ii))
                  {
                  case SECTION_RESPONSE_P:
                    for(int jj = 0; jj < order; jj++)
                      fb(jj,0) += fSec(jj,ii)*weight(i);
                    break;
                  case SECTION_RESPONSE_MZ:
                    for(int jj = 0; jj < order; jj++)
                      {
                        tmp = fSec(jj,ii)*weight(i);
                        fb(jj,1) += xL1*tmp;
                        fb(jj,2) += xL*tmp;
                      }
                    break;
                  case SECTION_RESPONSE_VY:
                    for(int jj = 0; jj < order; jj++)
                      {
                        tmp= oneOverL*fSec(jj,ii)*weight(i);
                        fb(jj,1)+= tmp;
                        fb(jj,2)+= tmp;
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
                        tmp = oneOverL*fb(ii,jj);
                        f(1,jj) += tmp;
                        f(2,jj) += tmp;
                      }
                    break;
                  default:
                    break;
		}
              }
	  }
        f*= L;
        // calculate element stiffness matrix
        // invert3by3Matrix(f, kv);
        static Matrix kvInit(NEBD, NEBD);
        if(f.Solve(I, kvInit) < 0)
          std::cerr << "XC::NLBeamColumn2d::getInitialStiff() -- could not invert flexibility\n";
        Ki= Matrix(theCoordTransf->getInitialGlobalStiffMatrix(kvInit));
      }
    return Ki;
  }


/********* NEWTON , SUBDIVIDE AND INITIAL ITERATIONS ********************
 */
int XC::NLBeamColumn2d::update()
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

    static Vector vin(NEBD);
    vin = v;
    vin -= dv;

    if(initialFlag != 0 && dv.Norm() <= DBL_EPSILON && sp.isEmpty())
      return 0;

    const size_t nSections= getNumSections();
    const Matrix &xi_pt  = quadRule.getIntegrPointCoords(nSections);
    const Vector &weight = quadRule.getIntegrPointWeights(nSections);

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

    dvToDo = dv;
    dvTrial = dvToDo;

    static double factor = 10;
    const double L= theCoordTransf->getInitialLength();
    const double oneOverL= 1.0/L;

    maxSubdivisions = 10;


    // fmk - modification to get compatible ele forces and deformations
    //   for a change in deformation dV we try first a newton iteration, if
    //   that fails we try an initial flexibility iteration on first iteration
    //   and then regular newton, if that fails we use the initial flexiblity
    //   for all iterations.
    //
    //   if they both fail we subdivide dV & try to get compatible forces
    //   and deformations. if they work and we have subdivided we apply
    //   the remaining dV.

    while (converged == false && numSubdivide <= maxSubdivisions)
      {

	// try regular newton (if l==0), or
	// initial tangent on first iteration then regular newton (if l==1), or
	// initial tangent iterations (if l==2)

	for(int l=0; l<3; l++)
	  {
	    //      if(l == 1) l = 2;
	    SeTrial = Se;
	    kvTrial = kv;
	    for(size_t i=0; i<nSections; i++)
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
		int numIters= maxIters;
		if(l == 1)
		  numIters*= 10; // allow 10 times more iterations for initial tangent

	      for(int j=0;j<numIters;j++)
		{
		  // initialize f and vr for integration
		  f.Zero();
		  vr.Zero();

		  for(size_t i=0; i<nSections; i++)
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

		      const double xL= xi_pt(i,0);
		      const double xL1= xL-1.0;

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
			      Ss(ii) =  xL1*SeTrial(1) + xL*SeTrial(2);
			      break;
			    case SECTION_RESPONSE_VY:
			      Ss(ii) = oneOverL*(SeTrial(1)+SeTrial(2));
			      break;
			    default:
			      Ss(ii)= 0.0;
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
		      dSs = Ss;
		      dSs.addVector(1.0, section_matrices.getSsrSubdivide()[i], -1.0);


		      // compute section deformation increments
		      if(l == 0)
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

			  const Matrix &fs0= theSections[i]->getInitialFlexibility();
			  dvs.addMatrixVector(0.0, fs0, dSs, 1.0);
			}

		      // set section deformations
		      if(initialFlag != 0)
			section_matrices.getVsSubdivide()[i] += dvs;

		      theSections[i]->setTrialSectionDeformation(section_matrices.getVsSubdivide()[i]);

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
		      // f = f + (b^ fs * b) * weight(i);
		      //f.addMatrixTripleProduct(1.0, b[i], fs[i], weight(i));
		      const Matrix &fSec = section_matrices.getFsSubdivide()[i];
		      fb.Zero();
		      double tmp;
		      for(int ii = 0; ii < order; ii++)
			{
			  switch(code(ii))
			    {
			    case SECTION_RESPONSE_P:
			      for(int jj = 0; jj < order; jj++)
				fb(jj,0) += fSec(jj,ii)*weight(i);
			      break;
			    case SECTION_RESPONSE_MZ:
			      for(int jj = 0; jj < order; jj++)
				{
				  tmp= fSec(jj,ii)*weight(i);
				  fb(jj,1)+= xL1*tmp;
				  fb(jj,2) += xL*tmp;
				}
			      break;
			    case SECTION_RESPONSE_VY:
			      for(int jj = 0; jj < order; jj++)
				{
				  tmp= oneOverL*fSec(jj,ii)*weight(i);
				  fb(jj,1)+= tmp;
				  fb(jj,2)+= tmp;
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
				  tmp= fb(ii,jj);
				  f(1,jj)+= xL1*tmp;
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
		      // integrate residual deformations
		      // vr += (b^ (vs + dvs)) * weight(i);
		      //vr.addMatrixTransposeVector(1.0, b[i], vs[i] + dvs, weight(i));
		      dvs.addVector(1.0, section_matrices.getVsSubdivide()[i], 1.0);
		      double dei;
		      for(int ii = 0; ii < order; ii++)
			{
			  dei= dvs(ii)*weight(i);
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
			    default:
			      break;
			    }
			}
		    }

		  f  *= L;
		  vr *= L;

		  // calculate element stiffness matrix
		  // invert3by3Matrix(f, kv);

		  if(f.Solve(I, kvTrial) < 0)
		    std::cerr << getClassName() << "::" << __FUNCTION__
			      << "; could not invert flexibility\n";

		  // dv = vin + dvTrial  - vr
		  dv= vin;
		  dv+= dvTrial;
		  dv-= vr;

		  //dv.addVector(1.0, vr, -1.0);

		  // dSe = kv * dv;
		  dSe.addMatrixVector(0.0, kvTrial, dv, 1.0);

		  dW= dv ^ dSe;

		  SeTrial += dSe;

		  // check for convergence of this interval
		  if(fabs(dW) < tol)
		    {
		      // set the target displacement
		      dvToDo -= dvTrial;
		      vin += dvTrial;

		      // check if we have got to where we wanted
		      if(dvToDo.Norm() <= DBL_EPSILON)
			{ converged = true; }
		      else
			{  // we converged but we have more to do
			   // reset variables for start of next subdivision
			   dvTrial = dvToDo;
			   numSubdivide = 1;  // NOTE setting subdivide to 1 again maybe too much
			}

		      // set kv, vs and Se values
		      kv= kvTrial;
		      Se= SeTrial;

		      for(size_t k=0; k<nSections; k++)
			{
			  vs[k]= section_matrices.getVsSubdivide()[k];
			  fs[k]= section_matrices.getFsSubdivide()[k];
			  Ssr[k]= section_matrices.getSsrSubdivide()[k];
			}

		      // break out of j & l loops
		      j= numIters+1;
		      l= 4;
		    }
		  else
		    { //  if(fabs(dW) < tol) {
		      // if we have failed to convrege for all of our newton schemes
		      // - reduce step size by the factor specified
		      if(j == (numIters-1) && (l == 2))
			{
			  dvTrial/= factor;
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
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - failed to get compatible "
		  << "element forces & deformations for element: "
		  << this->getTag() << "(dW: << " << dW << ")\n";
                  return -1;
      }
    initialFlag = 1;
    return 0;
  }


void XC::NLBeamColumn2d::getGlobalDispls(Vector &dg) const
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



void XC::NLBeamColumn2d::getGlobalAccels(Vector &ag) const
{
   // determine global displacements
   const XC::Vector &accel1 = theNodes[0]->getTrialAccel();
   const XC::Vector &accel2 = theNodes[1]->getTrialAccel();

   for(int i = 0; i < NND; i++)
   {
      ag(i)     = accel1(i);
      ag(i+NND) = accel2(i);
   }
}



void XC::NLBeamColumn2d::getForceInterpolatMatrix(double xi, Matrix &b, const XC::ID &code)
{
   b.Zero();

   double L = theCoordTransf->getInitialLength();
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


void XC::NLBeamColumn2d::getDistrLoadInterpolatMatrix(double xi, Matrix &bp, const XC::ID &code)
{
   bp.Zero();

   double L = theCoordTransf->getInitialLength();
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


const XC::Matrix &XC::NLBeamColumn2d::getMass(void) const
  {
    theMatrix.Zero();
    const double L= theCoordTransf->getInitialLength();
    if(rho != 0.0)
      theMatrix(0,0) = theMatrix(1,1) = theMatrix(3,3) = theMatrix(4,4) = 0.5*L*rho;
    return theMatrix;
  }



void XC::NLBeamColumn2d::zeroLoad(void)
  {
    NLForceBeamColumn2dBase::zeroLoad();
    sp.Zero();
    p0.zero();
  }

int XC::NLBeamColumn2d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() 
                << "; load over inactive element: "
                << getTag()  
                << std::endl;
    else
      {
        if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
          {
            const size_t nSections= getNumSections();
            const XC::Matrix &xi_pt = quadRule.getIntegrPointCoords(nSections);
            const double L = theCoordTransf->getInitialLength();
            if(sp.isEmpty())
              sp= Matrix(3,nSections);
            sp+= beamMecLoad->getAppliedSectionForces(L,xi_pt,loadFactor); // Accumulate applied section forces due to element loads
            beamMecLoad->addReactionsInBasicSystem(L,loadFactor,p0); // Accumulate reactions in basic system
          }
        else
          {
            std::cerr << "XC::NLBeamColumn2d::addLoad() -- load type unknown for element with tag: " << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }


int XC::NLBeamColumn2d::addInertiaLoadToUnbalance(const XC::Vector &accel)
{
  // Check for a quick return
  if(rho == 0.0)
    return 0;

  // get R * accel from the nodes
  const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
  const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

  double L = theCoordTransf->getInitialLength();
  double m = 0.5*rho*L;

  load(0) -= m*Raccel1(0);
  load(1) -= m*Raccel1(1);
  load(3) -= m*Raccel2(0);
  load(4) -= m*Raccel2(1);

  return 0;
}

const XC::Vector &XC::NLBeamColumn2d::getResistingForceIncInertia(void) const
  {
    // Compute the current resisting force
    theVector = this->getResistingForce();

    // Check for a quick return
    if(rho != 0.0)
      {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();

	const double L = theCoordTransf->getInitialLength();
	const double m = 0.5*rho*L;

	theVector(0) += m*accel1(0);
	theVector(1) += m*accel1(1);
	theVector(3) += m*accel2(0);
	theVector(4) += m*accel2(1);

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



bool XC::NLBeamColumn2d::isSubdomain(void)
  { return false; }


//! @brief Send members through the communicator argument.
int XC::NLBeamColumn2d::sendData(Communicator &comm)
  {
    int res= NLForceBeamColumn2dBase::sendData(comm);
    res+= comm.sendInt(maxSubdivisions,getDbTagData(),CommMetaData(25));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::NLBeamColumn2d::recvData(const Communicator &comm)
  {
    int res= NLForceBeamColumn2dBase::recvData(comm);
    res+= comm.receiveInt(maxSubdivisions,getDbTagData(),CommMetaData(25));
    return res;
  }

int XC::NLBeamColumn2d::sendSelf(Communicator &comm)
  {
    inicComm(26);
    int res= sendData(comm);

    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "NLBeamColumn2d::sendSelf -- failed to send ID data\n";
    return res;
  }


int XC::NLBeamColumn2d::recvSelf(const Communicator &comm)
  {
    inicComm(26);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "NLBeamColumn2d::recvSelf() - failed to recv ID data";
    else
      res+= recvData(comm);
    return res;
  }


void XC::NLBeamColumn2d::compSectionDisplacements(std::vector<Vector> &sectionCoords,std::vector<Vector> &sectionDispls) const
  {
   // update the transformation
   theCoordTransf->update();

   // get basic displacements and increments
   static Vector ub(NEBD);
   ub = theCoordTransf->getBasicTrialDisp();

   const size_t nSections= getNumSections();
   // get integration point positions and weights
   const XC::Matrix &xi_pt  = quadRule.getIntegrPointCoords(nSections);

   // setup Vandermode and CBDI influence matrices
   double xi;

   // get CBDI influence matrix
   Matrix ls(nSections, nSections);
   double L = theCoordTransf->getInitialLength();
   getCBDIinfluenceMatrix(nSections, xi_pt, L, ls);

   // get section curvatures
   Vector kappa(nSections);  // curvature
   Vector vs;              // section deformations

   for(size_t i=0; i<nSections; i++)
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

       if(ii == code.Size()) {
         std::cerr << "FATAL XC::NLBeamColumn2d::compSectionDispls - section does not provide Mz response\n";
         exit(-1);
       }

       // get section deformations
       vs = theSections[i]->getSectionDeformation();
       kappa(i) = vs(sectionKey);
   }

       //std::cerr << "kappa: " << kappa;

   Vector w(nSections);
   static XC::Vector xl(NDM), uxb(NDM);
   static XC::Vector xg(NDM), uxg(NDM);

   // w = ls * kappa;
   w.addMatrixVector (0.0, ls, kappa, 1.0);

   for(size_t i=0; i<nSections; i++)
   {
      xi = xi_pt(i,0);

      xl(0) = xi * L;
      xl(1) = 0;

      // get section global coordinates
      sectionCoords[i] = theCoordTransf->getPointGlobalCoordFromLocal(xl);

      // compute section displacements
      uxb(0) = xi * ub(0); // consider linear variation for axial displacement. CHANGE LATER!!!!!!!!!!
      uxb(1) = w(i);

      // get section displacements in global system
      sectionDispls[i] = theCoordTransf->getPointGlobalDisplFromBasic(xi, uxb);
   }
}





void XC::NLBeamColumn2d::Print(std::ostream &s, int flag) const
{
   const size_t nSections= getNumSections();
   if(flag == -1) {
    int eleTag = this->getTag();
    s << "NL_BEAM\t" << eleTag << "\t";
    s << theSections[0]->getTag() << "\t" << theSections[nSections-1]->getTag();
    s  << "\t" << theNodes.getTagNode(0) << "\t" << theNodes.getTagNode(1);
    s << "0\t0.0000000\n";
   }

   else if(flag == 1) {
      s << "\nElement: " << this->getTag() << " Type: NLBeamColumn2d ";
      s << "\tConnected Nodes: " << theNodes;
      s << "\tNumber of Sections: " << nSections;
      s << "\tMass density: " << rho;
      for(size_t i = 0; i < nSections; i++)
         s << "\nSection "<<i<<" :" << *theSections[i];
   }

   else {
      s << "\nElement: " << this->getTag() << " Type: NLBeamColumn2d ";
      s << "\tConnected Nodes: " << theNodes;
      s << "\tNumber of Sections: " << nSections;
      s << "\tMass density: " << rho << std::endl;
      double P  = Secommit(0);
      double M1 = Secommit(1);
      double M2 = Secommit(2);
      double L = theCoordTransf->getInitialLength();
      double V = (Secommit(1)+Secommit(2))/L;
      theVector(1) = V;
      theVector(4) = -V;
      s << "\tEnd 1 Forces (P V M): " << -P+p0[0] << " "
        << V+p0[1] << " " << M1 << std::endl;
      s << "\tEnd 2 Forces (P V M): " << P << " "
        << -V+p0[2] << " " << M2 << std::endl;
   }
}


std::ostream &XC::operator<<(std::ostream &s, const NLBeamColumn2d &E)
  {
    E.Print(s);
    return s;
  }


XC::Response *XC::NLBeamColumn2d::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
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

    // section response -
    else if(argv[0] == "section") {
      if(argv.size() <= 2)
        return 0;

      size_t sectionNum = atoi(argv[1]);
      if(sectionNum > 0 && sectionNum <= getNumSections())
        return setSectionResponse(theSections[sectionNum-1],argv,2,eleInformation);
      else
        return 0;
    }

    else
      return 0;
}

int XC::NLBeamColumn2d::getResponse(int responseID, Information &eleInfo)
  {
    double V= 0;
    switch (responseID)
      {
      case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());

      case 2:
        theVector(3) =  Se(0);
        theVector(0) = -Se(0)+p0[0];
        theVector(2) = Se(1);
        theVector(5) = Se(2);
        V= (Se(1)+Se(2))/theCoordTransf->getInitialLength();
        theVector(1) =  V+p0[1];
        theVector(4) = -V+p0[2];
        return eleInfo.setVector(theVector);
      default:
        return -1;
      }
  }

int XC::NLBeamColumn2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
{
        //
        // From the parameterID value it should be possible to extract
        // information about:
        //  1) Which parameter is in question. The parameter could
        //     be at element, section, or material level.
        //  2) Which section and material number (tag) it belongs to.
        //
        // To accomplish this the parameterID is given the following value:
        //     parameterID = type + 1000*matrTag + 100000*sectionTag
        // ...where 'type' is an integer in the range (1-99) and added 100
        // for each level (from material to section to element).
        //
        // Example:
        //    If 'E0' (case 2) is random in material #3 of section #5
        //    the value of the parameterID at this (element) level would be:
        //    parameterID = 2 + 1000*3 + 100000*5 = 503002
        //    As seen, all given information can be extracted from this number.
        //

        // Initial declarations
        int parameterID = 0;

        // If the parameter belongs to the element itself
        if(argv[0] == "rho")
          return param.addObject(1, this);

        // If the parameter is belonging to a section or lower
        else if(argv[0] == "section") {

                // For now, no parameters of the section itself:
	  if(argv.size()<5) {
                        std::cerr << "For now: cannot handle parameters of the section itself." << std::endl;
                        return -1;
                }

                // Get section and material tag numbers from user input
                int paramSectionTag = atoi(argv[1]);

                // Find the right section and call its setParameter method
                const size_t nSections= getNumSections();
                for(size_t i=0; i<nSections; i++) {
                        if(paramSectionTag == theSections[i]->getTag()) {
                                parameterID = setSectionParameter(theSections[i],argv,2,param);
                        }
                }

                // Check if the parameterID is valid
                if(parameterID < 0) {
                        std::cerr << "XC::NLBeamColumn2d::setParameter() - could not set parameter. " << std::endl;
                        return -1;
                }
                else {
                        // Return the parameterID value (according to the above comments)
                        return parameterID;
                }
        }

        // Otherwise parameter is unknown for this class
        else {
                return -1;
        }
}

int XC::NLBeamColumn2d::updateParameter(int parameterID, Information &info)
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
                const size_t nSections= getNumSections();
                for(size_t i=0; i<nSections; i++) {
                        if(sectionNumber == theSections[i]->getTag()) {
                                ok = theSections[i]->updateParameter(parameterID, info);
                        }
                }

                if(ok < 0) {
                        std::cerr << "XC::NLBeamColumn2d::updateParameter() - could not update parameter. " << std::endl;
                        return ok;
                }
                else {
                        return ok;
                }
        }
        else {
                std::cerr << "XC::NLBeamColumn2d::updateParameter() - could not update parameter. " << std::endl;
                return -1;
        }
}
