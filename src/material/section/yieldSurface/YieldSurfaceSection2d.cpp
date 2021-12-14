//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// @ rkaul@stanford.edu
// @ ggd@stanford.edu

#include "YieldSurfaceSection2d.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <cstdlib>
#include "material/section/ResponseId.h"
#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h"

#define SEC_TAG_YS2d -1

XC::Vector  XC::YieldSurfaceSection2d::dele(2);
XC::Vector  XC::YieldSurfaceSection2d::surfaceForce(2);
XC::Matrix  XC::YieldSurfaceSection2d::G(2,1);
XC::Matrix  XC::YieldSurfaceSection2d::Ktp(2,2);

XC::YieldSurfaceSection2d::YieldSurfaceSection2d(void)
  :PrismaticBarCrossSection(0, SEC_TAG_YS2d),
   use_Kr_orig(true), ys(0), eTrial(2), eInic(2), eCommit(2), s(2), sCommit(2), ks(2,2),
   use_Kr(true), split_step(false)
  {}

XC::YieldSurfaceSection2d::YieldSurfaceSection2d(int tag, int classtag, YieldSurface_BC *ptrys, bool use_kr)
  :PrismaticBarCrossSection(tag, classtag), use_Kr_orig(use_kr), ys(0), eTrial(2), eInic(2), eCommit(2),s(2),
    sCommit(2), ks(2,2), use_Kr(use_kr), split_step(false)
  {
    if(ptrys==0)
      {
        std::cerr << "WARNING - InelasticYS2DGNL(): ys1 = 0" << std::endl;
      }
    else
      {
        ys =  ptrys->getCopy();
        ys->setTransformation(1, 0, 1, -1);   // x-axis is Mz, y-axis is P
        //                ys->setEleInfo(getTag(), 1);
      }
  }


XC::YieldSurfaceSection2d::~YieldSurfaceSection2d(void)
  { if(ys) delete ys; }

//! @brief Commit the state of the material.
int XC::YieldSurfaceSection2d::commitState(void)
  {
    eCommit = eTrial;
    sCommit = s;
    ys->commitState(s);
    split_step = false;
    return 0;
  }

int XC::YieldSurfaceSection2d::revertToLastCommit(void)
  {
    eTrial = eCommit;
    s = sCommit;
    ys->revertToLastCommit();
    return 0;
  }

int XC::YieldSurfaceSection2d::revertToStart(void)
  {
    eCommit.Zero();
    sCommit.Zero();
    //        ys->revertToStart();
    return 0;
  }

int XC::YieldSurfaceSection2d::setInitialSectionDeformation(const XC::Vector &def)
  {
    eInic = def;
    return 0;
  }

int XC::YieldSurfaceSection2d::setTrialSectionDeformation(const XC::Vector &def)
  {
    ys->update(); // important
    use_Kr = use_Kr_orig;
    //        split_step = false;
    //  once determined, leave it till convergence
  
    eTrial = def;
    dele = eTrial - eCommit;

    getSectionStiffness(ks);
    double EA = ks(0,0);
    double EI = ks(1,1);
  
    s(0) = sCommit(0) + EA*dele(0);
    s(1) = sCommit(1) + EI*dele(1);
  
    if(ys->getTrialForceLocation(s) <= 0)
      return 0;
  
  
    // case: if it shoots through
    //         use dF return to surface
    int driftOld = ys->getCommitForceLocation();
  
    if(driftOld < 0) // Inside
      {
        use_Kr = false;
        split_step = true;
      
        surfaceForce = s;
        ys->setToSurface(surfaceForce, ys->dFReturn);  //dFReturn, ConstantYReturn, RadialReturn
        ys->getTrialGradient(G, surfaceForce);
      }
    // Now we know that force point has drifted from the surface
    else if(driftOld == 0) // On surface
      {
        ys->getCommitGradient(G);
        surfaceForce =  sCommit;
      }
    else // driftOld outside - bug or bad constraints or continued from not converged state
      {
        std::cerr << "WARNING: XC::YieldSurfaceSection2d::setTrialSectionDeformation, driftOld outside [" << getTag()<<"]\n";
      }
  
    double dF_in0 = s(0) - surfaceForce(0);
    double dF_in1 = s(1) - surfaceForce(1);
  
    double g0 = G(0,0);
    double g1 = G(1,0);
  
    Ktp(0,0) = EA;
    Ktp(1,1) = EI;
    ys->addPlasticStiffness(Ktp);
  
    double inv = 1/(Ktp(0,0)*g0*g0 + Ktp(1,1)*g1*g1);
  
    double lamda = inv*(g0*dF_in0 + g1*dF_in1);
    if(fabs(lamda) < 1e-8) lamda = 0.0; // to get rid of -1e-15 etc
  
    if(lamda < 0)
      {
        use_Kr = false;
        lamda = 0.0;
      }
  
    //int grow= ys->modifySurface(lamda, surfaceForce, G);
    ys->modifySurface(lamda, surfaceForce, G);

    // used to do: (not tested shrinking yet)
    //        if(grow < 0)
    //                algo = ys->ConstantYReturn;
    //        else
    //                algo = algo_orig;
  
    if(use_Kr)
      {
        ks(0,0) = EA - EA*EA*g0*g0*inv;
        ks(0,1) = -1*EA*g0*g1*EI*inv;
        ks(1,0) = ks(0,1);
        ks(1,1) = EI - EI*EI*g1*g1*inv;
      }
    if(split_step)
      {
        s(0) = s(0) - EA*g0*lamda;
        s(1) = s(1) - EI*g1*lamda;
      }
    else
      {
        if(use_Kr)
          s= surfaceForce + ks*dele;
      }
  
    ys->setToSurface(s, ys->ConstantYReturn);
    // used to do centroid return
    // then force-balance using ConstantYReturn
    // comp/tension issue: always use constantP
  
    return 0;
  }

//! @brief Zeroes initial strains.
void XC::YieldSurfaceSection2d::zeroInitialSectionDeformation(void)
  { return eInic.Zero(); }

const XC::Vector &XC::YieldSurfaceSection2d::getInitialSectionDeformation(void) const
  { return eInic; }

//! @brief Returns generalized trial deformation.
const XC::Vector &XC::YieldSurfaceSection2d::getSectionDeformation(void) const
  {
    static Vector retval;
    retval= eTrial-eInic;
    return retval;
  }

const XC::Vector &XC::YieldSurfaceSection2d::getStressResultant(void) const
  { return s; }

const XC::Matrix &XC::YieldSurfaceSection2d::getSectionTangent(void) const
  { return ks; }

const XC::Matrix &XC::YieldSurfaceSection2d::getSectionFlexibility(void) const
  {
    static XC::Matrix fs(2,2);
    invert2by2Matrix(ks, fs);
    return fs;
  }

const XC::ResponseId &XC::YieldSurfaceSection2d::getType(void) const
  { return RespElasticSection2d; }

int XC::YieldSurfaceSection2d::getOrder(void) const
  { return 2; }

int XC::YieldSurfaceSection2d::sendSelf(Communicator &comm)
  { return -1; }

int XC::YieldSurfaceSection2d::recvSelf(const Communicator &)
  { return -1; }

void XC::YieldSurfaceSection2d::Print(std::ostream &s, int flag) const
  {
    s << "YieldSurfaceSection2d, tag: " << this->getTag() << std::endl;
    s << "\tYield Surface:" << *ys << std::endl;
    s << "\tSection Force:" << sCommit;
    s << "\tSection Defom:" << eCommit;
  }
