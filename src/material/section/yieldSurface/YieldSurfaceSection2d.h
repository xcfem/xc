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
// @ rkaul@stanford.edu
// @ ggd@stanford.edu

#ifndef YieldSurfaceSection2d_h
#define YieldSurfaceSection2d_h

#include "material/section/SeccionBarraPrismatica.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Channel;
class FEM_ObjectBroker;
class Information;
class YieldSurface_BC;

//! \ingroup MATSCC
//
//! @defroup MATSCCYS Cross sections with yield surface.
//
//! \ingroup MATSCCYS
//
//! @brief Base class for cross sections with yield surface
//! on a bi-dimensional space (three DOFs for each section).
class YieldSurfaceSection2d: public SeccionBarraPrismatica
  {
  protected:
    virtual void getSectionStiffness(Matrix &Ks)=0;
    const   bool use_Kr_orig;
    YieldSurface_BC *ys;
    Vector eTrial; //!< section trial deformations
    Vector eInic; //!< section initial deformations.
    Vector eCommit;
    Vector s;
    Vector sCommit;
    mutable Matrix ks;
  
  private:
    //    int algo;
    bool use_Kr, split_step;
    static Vector dele;
    static Vector surfaceForce;
    static Matrix G;
    static Matrix Ktp;
  public:
    YieldSurfaceSection2d( int tag, int classtag,YieldSurface_BC *ptrys, bool use_kr=true);
    YieldSurfaceSection2d(void);    
    ~YieldSurfaceSection2d(void);
  
    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);
  
    virtual int setInitialSectionDeformation (const Vector&);
    virtual int setTrialSectionDeformation (const Vector&);
    void zeroInitialSectionDeformation(void);
    virtual const Vector &getInitialSectionDeformation(void) const;
    virtual const Vector &getSectionDeformation(void) const;
  
    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
  
    const ResponseId &getType(void) const;
    int getOrder(void) const;
  
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  
    virtual void Print(std::ostream &s, int flag =0);
    virtual SectionForceDeformation *getCopy(void) const=0;
  };
} // end of XC namespace

#endif
