// -*-c++-*-
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
//BaseElasticSection.h

#ifndef BaseElasticSection_h
#define BaseElasticSection_h

#include <material/section/PrismaticBarCrossSection.h>
#include <utility/matrix/Vector.h>

namespace XC {

//! @ingroup MATSCC
//!
//! @defgroup MATSCCElastica Elastic material cross sections.
//
//! @ingroup MATSCCElastica
//
//! @brief Base class for cross sections with linear elastic material.
class BaseElasticSection: public PrismaticBarCrossSection
  {
  protected:
    Vector eTrial; //!< section trial deformations
    Vector eInic; //!< section initial deformations
    Vector eCommit; //!< section committed deformations
    
    int sendData(Communicator &);
    int recvData(const Communicator &);


  public:
    BaseElasticSection(int tag,int classTag,const size_t &dim,MaterialHandler *mat_ldr= nullptr);    

    int commitState(void);
    int revertToLastCommit (void);
    int revertToStart (void);

    virtual void sectionGeometry(const std::string &)= 0;

    int setTrialSectionDeformation(const Vector&);
    inline Vector &getTrialSectionDeformation(void)
      { return eTrial; }
    int setInitialSectionDeformation(const Vector &);
    inline const Vector &getInitialSectionDeformation(void) const
      { return eInic; }
    void zeroInitialSectionDeformation(void)
      { eInic.Zero(); }
    const Vector &getSectionDeformation(void) const;
    virtual double getRho(void) const= 0;
    virtual void setRho(const double &)= 0;

    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
