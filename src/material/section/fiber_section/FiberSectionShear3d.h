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
//FiberSectionShear3d.h

#ifndef FiberSectionShear3d_h
#define FiberSectionShear3d_h

#include "FiberSection3d.h"

namespace XC {
//! \ingroup MATSCC
//
//! @brief FiberSectionShear3d decorates an MP
//! section (couple bending and axial) with an uncoupled shear
//! relation.
class FiberSectionShear3d : public FiberSection3d
  {
  private:
    UniaxialMaterial *respVy;
    UniaxialMaterial *respVz;
    UniaxialMaterial *respT;
    
    static Vector def; //!< Storage for section deformations
    static Vector defzero; //!< Storage for initial section deformations
    static Vector s; //!< Storage for stress resultants
    static Matrix ks;//!< Storage for section stiffness
    static Matrix fs;//!< Storage for section flexibility

    void setRespVy(const UniaxialMaterial *);
    void setRespVz(const UniaxialMaterial *);
    void setRespT(const UniaxialMaterial *);
    void setRespVyVzT(const UniaxialMaterial *,const UniaxialMaterial *,const UniaxialMaterial *);

    void freeRespVyVzT(void);
    void free_mem(void);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    FiberSectionShear3d(int tag= 0,MaterialLoader *mat_ldr= nullptr);
    FiberSectionShear3d(const FiberSectionShear3d &otro);
    XC::FiberSectionShear3d &operator=(const FiberSectionShear3d &otro);
    ~FiberSectionShear3d(void);

    void setRespVyByName(const std::string &);
    UniaxialMaterial *getRespVy(void);
    void setRespVzByName(const std::string &);
    UniaxialMaterial *getRespVz(void);
    void setRespTByName(const std::string &);
    UniaxialMaterial *getRespT(void);
    void setRespVyVzTByName(const std::string &,const std::string &,const std::string &);

    int setInitialSectionDeformation(const Vector &deforms); 
    int setTrialSectionDeformation(const Vector &deforms); 
    void zeroInitialSectionDeformation(void);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;
    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
    const Matrix &getInitialFlexibility(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);
 
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
 
    void Print(std::ostream &s, int flag =0) const;

    int setVariable(const std::string &argv);
    int getVariable(int variableID, double &info);

  };
} // end of XC namespace

#endif
