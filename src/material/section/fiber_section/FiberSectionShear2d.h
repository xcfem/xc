// -*-c++-*-
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
//FiberSectionShear2d.h

#ifndef FiberSectionShear2d_h
#define FiberSectionShear2d_h

#include "FiberSection2d.h"

namespace XC {
//! @ingroup MATSCC
//
//! @brief FiberSectionShear2d decorates an MP
//! section (couple bending and axial) with an uncoupled shear
//! relation.
class FiberSectionShear2d: public FiberSection2d
  {
  private:
    UniaxialMaterial *respVy;
    
    static Vector def; //!< Storage for section deformations
    static Vector defzero; //!< Storage for initial section deformations
    static Vector s; //!< Storage for stress resultants
    static Matrix ks;//!< Storage for section stiffness
    static Matrix fs;//!< Storage for section flexibility

    void setRespVy(const UniaxialMaterial *);
    void freeRespVy(void);
    void free_mem(void);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    FiberSectionShear2d(int tag= 0,MaterialHandler *mat_ldr= nullptr);
    FiberSectionShear2d(const FiberSectionShear2d &);
    FiberSectionShear2d &operator=(const FiberSectionShear2d &);
    ~FiberSectionShear2d(void);

    void setRespVyByName(const std::string &);
    UniaxialMaterial *getRespVy(void);

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
    virtual const double &GAy(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);
 
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getResponseType(void) const;
    int getOrder(void) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
 
    void Print(std::ostream &s, int flag =0) const;

    int setVariable(const std::string &argv);
    int getVariable(int variableID, double &info);

  };
} // end of XC namespace

#endif
