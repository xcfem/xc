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
//BeamColumnWithSectionFDs.h

#ifndef BeamColumnWithSectionFD_h
#define BeamColumnWithSectionFD_h

#include <domain/mesh/element/Element1D.h>
#include "material/section/PrismaticBarCrossSectionsVector.h"
#include "SectionMatrices.h"


namespace XC {

//! @ingroup BeamColumnElemGrp
//
//! @brief Beam-column element with PrismaticBarCrossSection material
class BeamColumnWithSectionFD: public Element1D
  {
  protected:
    PrismaticBarCrossSectionsVector theSections; //!< pointers to the PrismaticBarCrossSection objects


    SectionMatrices section_matrices;
    double rho;	//!< Density per unit length

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc= 1);
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const Material *m);
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const PrismaticBarCrossSection *sccModel);
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const PrismaticBarCrossSection *sccModel,int Nd1,int Nd2);

    const PrismaticBarCrossSection *getSectionPtr(const size_t &i) const;

    void setTrialSectionDeformations(const std::vector<Vector> &vs);

    virtual void setSection(const PrismaticBarCrossSection *sccModel);
    bool setSections(const std::vector<PrismaticBarCrossSection *> &sectionPtrs);
    inline size_t getNumSections(void) const
      { return theSections.size(); }
    inline PrismaticBarCrossSectionsVector &getSections(void)
      { return theSections; }
 
    Response *setSectionResponse(PrismaticBarCrossSection *,const std::vector<std::string> &,const size_t &,Information &);
    int setSectionParameter(PrismaticBarCrossSection *,const std::vector<std::string> &,const size_t &, Parameter &);
    
    virtual double getRho(void) const
      { return rho; }
    virtual void setRho(const double &r)
      { rho= r; }

    // public methods to set the state of the element
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void zeroLoad(void);
  };

} //end of XC namespace
#endif
