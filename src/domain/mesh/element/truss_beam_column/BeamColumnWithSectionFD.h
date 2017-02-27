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
//BeamColumnWithSectionFDs.h

#ifndef BeamColumnWithSectionFD_h
#define BeamColumnWithSectionFD_h

#include <domain/mesh/element/Element1D.h>
#include "material/section/VectorSeccionesBarraPrismatica.h"
#include "SectionMatrices.h"


namespace XC {

//! \ingroup OneDimensionalElem
//
//! @brief Beam-column element with SeccionBarraPrismatica material
class BeamColumnWithSectionFD : public Element1D
  {
  protected:
    VectorSeccionesBarraPrismatica theSections; //!< pointers to the SeccionBarraPrismatica objects


    SectionMatrices section_matrices;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc= 1);
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const Material *m);
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const SeccionBarraPrismatica *sccModel);
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const SeccionBarraPrismatica *sccModel,int Nd1,int Nd2);

    const SeccionBarraPrismatica *getSectionPtr(const size_t &i) const;

    void setTrialSectionDeformations(const std::vector<Vector> &vs);

    virtual void setSection(const SeccionBarraPrismatica *sccModel);
    bool setSections(const std::vector<SeccionBarraPrismatica *> &sectionPtrs);
    inline size_t getNumSections(void) const
      { return theSections.size(); }
    inline VectorSeccionesBarraPrismatica &getSections(void)
      { return theSections; }
 
    Response *setSectionResponse(SeccionBarraPrismatica *,const std::vector<std::string> &,const size_t &,Information &);
    int setSectionParameter(SeccionBarraPrismatica *,const std::vector<std::string> &,const size_t &, Parameter &);

    // public methods to set the state of the element
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void zeroLoad(void);
  };

} //end of XC namespace
#endif
