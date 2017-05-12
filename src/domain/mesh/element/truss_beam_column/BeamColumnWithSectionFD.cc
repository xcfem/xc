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
//BeamColumnWithSectionFD.cc

#include "BeamColumnWithSectionFD.h"


#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include <material/section/PrismaticBarCrossSection.h>
#include <utility/recorder/response/ElementResponse.h>


XC::BeamColumnWithSectionFD::BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc)
  : Element1D(tag,classTag), theSections(numSecc)
  {}

XC::BeamColumnWithSectionFD::BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc= 1,const Material *m= nullptr)
  : Element1D(tag,classTag), theSections(numSecc,m)
  {}

XC::BeamColumnWithSectionFD::BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const PrismaticBarCrossSection *matModel)
  : Element1D(tag,classTag), theSections(numSecc,matModel)
  {}

XC::BeamColumnWithSectionFD::BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const PrismaticBarCrossSection *sccModel,int Nd1,int Nd2)
  : Element1D(tag,classTag,Nd1,Nd2), theSections(numSecc,sccModel)
  {}

//! @brief Zeroes loads on element.
void XC::BeamColumnWithSectionFD::zeroLoad(void)
  {
    Element1D::zeroLoad();
    theSections.zeroInitialSectionDeformations(); //Removes initial strains.
  }

int XC::BeamColumnWithSectionFD::commitState(void)
  {
    int retVal = 0;

    if((retVal= Element1D::commitState()) != 0)
      {
        std::cerr << "EightNodeBrick-XC::u_p_U::commitState () - failed in base class";
        return (-1);
      }
    retVal+= theSections.commitState();
    return retVal;
  }

int XC::BeamColumnWithSectionFD::revertToLastCommit(void)
  {
    int retval= theSections.revertToLastCommit();
    return retval;
  }

int XC::BeamColumnWithSectionFD::revertToStart(void)
  {
    int retval= Element1D::revertToStart();
    retval+= theSections.revertToStart();
    return retval;
  }

void XC::BeamColumnWithSectionFD::setSection(const PrismaticBarCrossSection *matModel)
  { theSections.setSection(matModel); }

bool XC::BeamColumnWithSectionFD::setSections(const std::vector<PrismaticBarCrossSection *> &sectionPtrs)
  { return theSections.setSections(sectionPtrs); }
void XC::BeamColumnWithSectionFD::setTrialSectionDeformations(const std::vector<Vector> &vs)
  { return theSections.setTrialSectionDeformations(vs); }

//! @brief Returns a pointer to the i-th section of the element.
const XC::PrismaticBarCrossSection *XC::BeamColumnWithSectionFD::getSectionPtr(const size_t &i) const
  { return theSections[i]; }

XC::Response* XC::BeamColumnWithSectionFD::setSectionResponse(PrismaticBarCrossSection *theSection,const std::vector<std::string> &argv,const size_t &offset, Information &info)
  {
    Response *retval= nullptr;
    if(theSection)
      {
        std::vector<std::string> argvOffset(argv);
        argvOffset.erase(argvOffset.begin(),argvOffset.begin()+offset);
        retval= theSection->setResponse(argvOffset,info);
      }
    return retval;
  }

int XC::BeamColumnWithSectionFD::setSectionParameter(PrismaticBarCrossSection *theSection,const std::vector<std::string> &argv,const size_t &offset, Parameter &param)
  {
    int retval= -1;
    if(theSection)
      {
        std::vector<std::string> argvOffset(argv);
        argvOffset.erase(argvOffset.begin(),argvOffset.begin()+offset);
        retval= theSection->setParameter(argvOffset,param);
      }
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::BeamColumnWithSectionFD::sendData(CommParameters &cp)
  {
    int res= Element1D::sendData(cp);
    res+= cp.sendMovable(theSections,getDbTagData(),CommMetaData(7));
    res+= cp.sendMovable(section_matrices,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::BeamColumnWithSectionFD::recvData(const CommParameters &cp)
  {
    int res= Element1D::recvData(cp);
    res+= cp.receiveMovable(theSections,getDbTagData(),CommMetaData(7));
    res+= cp.receiveMovable(section_matrices,getDbTagData(),CommMetaData(8));
    return res;
  }


