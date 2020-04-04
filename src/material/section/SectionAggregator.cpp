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
                                                                        
// $Revision: 1.11 $
// $Date: 2003/05/15 21:34:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/SectionAggregator.cpp,v $
                                                                        
                                                                        
// File: ~/section/SectionAggregator.C
//
// Written: MHS
// Created: June 2000
// Revision: A
//
// Purpose: This file contains the implementation for the XC::SectionAggregator class. 

#include <cstdlib>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <classTags.h>
#include <material/section/SectionAggregator.h>
#include <utility/recorder/response/MaterialResponse.h>
#include "preprocessor/prep_handlers/MaterialHandler.h"


#include "xc_utils/src/kernel/python_utils.h"
#include "classTags.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

const size_t maxOrder= 10;

// Assumes section order is less than or equal to maxOrder.
// Can increase if needed!!!
double XC::SectionAggregator::workArea[2*maxOrder*(maxOrder+1)];

//! @brief Allocate storage for pointers.
void XC::SectionAggregator::resize(void)
  {
    const size_t order= getOrder();
    if(order > maxOrder)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; order too big, need to modify the maxOrder value"
	          << " in SectionAggregator to: " << order << std::endl;
        exit(-1);
      }
    //theCode= new ResponseId(codeArea, order); Not sharing area anymore
    //                                          LCPT 19/09/2016
    if(order>0)
      {
        theCode= ResponseId(order);
        def= Vector(workArea, order);
        defzero= Vector(workArea, order);
        s= Vector(&workArea[maxOrder], order);
        ks= Matrix(&workArea[2*maxOrder], order, order);
        fs= Matrix(&workArea[maxOrder*(maxOrder+2)], order, order);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; 0 or negative order; order= " << order << std::endl;
  }

//! @brief Free memory.
void XC::SectionAggregator::free_mem(void)
  {
    if(theSection)
      {
        delete theSection;
        theSection= nullptr;
      }
  }

//! @brief Copy section from argument.
void XC::SectionAggregator::copy_section(const SectionForceDeformation *theSec)
  {
    if(theSection)
      {
        delete theSection;
        theSection= nullptr;
      }
    if(theSec)
      {
        SectionForceDeformation *tmp= theSec->getCopy();
        if(tmp)
          {
            theSection= dynamic_cast<PrismaticBarCrossSection *>(tmp);
            if(!theSection)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; not a suitable material.\n";
                delete(tmp);
                exit(-1);
              }
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to get copy of section.\n";
      }
  }

//! @brief Constructor.
XC::SectionAggregator::SectionAggregator(int tag, PrismaticBarCrossSection &theSec,const AggregatorAdditions &theAdds,MaterialHandler *mat_ldr)
  : PrismaticBarCrossSection(tag, SEC_TAG_Aggregator,mat_ldr),
    theSection(nullptr), theAdditions(theAdds,this)
  {
    copy_section(&theSec);
    resize();
  }

//! @brief Copy constructor.
XC::SectionAggregator::SectionAggregator(const SectionAggregator &other)
  : PrismaticBarCrossSection(other),
    theSection(nullptr), theAdditions(other.theAdditions)
   {
     copy_section(other.theSection);
     resize();
   }

//! @brief Constructor.
XC::SectionAggregator::SectionAggregator(int tag, const AggregatorAdditions &theAdds,MaterialHandler *mat_ldr)
  : PrismaticBarCrossSection(tag, SEC_TAG_Aggregator,mat_ldr),
    theSection(nullptr), theAdditions(theAdds,this)
  { resize(); }

XC::SectionAggregator::SectionAggregator(int tag, PrismaticBarCrossSection &theSec, UniaxialMaterial &theAddition, int c,MaterialHandler *mat_ldr)
  : PrismaticBarCrossSection(tag, SEC_TAG_Aggregator,mat_ldr),
    theSection(nullptr), theAdditions(this,theAddition,c)
  {
    copy_section(&theSec);
    resize();
  }

XC::SectionAggregator::SectionAggregator(int tag,MaterialHandler *mat_ldr)
  : PrismaticBarCrossSection(tag, SEC_TAG_Aggregator,mat_ldr),
    theSection(nullptr), theAdditions(this) {}

//! @brief Default constructor.
XC::SectionAggregator::SectionAggregator(MaterialHandler *mat_ldr)
  : PrismaticBarCrossSection(0, SEC_TAG_Aggregator,mat_ldr),
    theSection(nullptr), theAdditions(this) {}

//! @brief Assignment operator.
XC::SectionAggregator &XC::SectionAggregator::operator=(const SectionAggregator &other)
  {
    free_mem();
    PrismaticBarCrossSection::operator=(other);
    copy_section(other.theSection);
    theAdditions= other.theAdditions;
    theAdditions.set_owner(this);
    resize();
    def= other.def;
    defzero= other.defzero;
    s= other.s;
    ks= other.ks;
    fs= other.fs;
    theCode= other.theCode;
    return *this;
  }

//! @brief Assigns the section.
void XC::SectionAggregator::setSection(const std::string &sectionName)
  {
    assert(material_handler);
    const Material *ptr_mat= material_handler->find_ptr(sectionName);
    if(ptr_mat)
      {
        const PrismaticBarCrossSection *tmp= dynamic_cast<const PrismaticBarCrossSection *>(ptr_mat);
        if(tmp)
	  theSection= dynamic_cast<PrismaticBarCrossSection *>(tmp->getCopy());
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; material identified by: '" << sectionName
                    << "' is not a prismatic bar cross-section material.\n";
        resize();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; material identified by: '" << sectionName
                << "' not found.\n";
  }

void XC::SectionAggregator::setAddtions(const std::vector<std::string> &responseCodes,const std::vector<std::string> &nmbMats)
  {
    ResponseId codes(responseCodes);
    theAdditions.putMatCodes(codes);
    const size_t n= nmbMats.size();
    if(n!= responseCodes.size())
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; error in number of materials: index number: "
		<< responseCodes.size()
                << " number of materials: " << n << std::endl;
    for(size_t i= 0;i<n;i++)
      {
        const Material *ptr_mat= material_handler->find_ptr(nmbMats[i]);
        if(ptr_mat)
          {
            const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
            if(tmp)
              theAdditions.push_back(tmp);
            else
              std::cerr << getClassName() << "::" << __FUNCTION__
                        << "; material with code: '" << nmbMats[i]
                        << "' is not an uniaxial material.\n";
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; material: '" << nmbMats[i]
                    << "' not found.\n";
      }
    resize();
  }

void XC::SectionAggregator::setAddtionsPyList(const boost::python::list &responseCodes,const boost::python::list &nmbMats)
  { setAddtions(vector_string_from_py_list(responseCodes),vector_string_from_py_list(nmbMats)); }


//! @brief destructor:
XC::SectionAggregator::~SectionAggregator(void)
  { free_mem(); }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::SectionAggregator::getCopy(void) const
  { return new SectionAggregator(*this); }

//! @brief Sets initial strain.
int XC::SectionAggregator::setInitialSectionDeformation(const Vector &def)
  {
    int ret= 0;
    int theSectionOrder= 0;

    if(theSection)
      {
        theSectionOrder= theSection->getOrder();
        Vector v(workArea, theSectionOrder);
        for(register int i= 0;i<theSectionOrder;i++)
          v(i)= def(i);
        ret= theSection->setInitialSectionDeformation(v);
      }
    theAdditions.setInitialStrain(def,theSectionOrder);
    return ret;
  }

//! @brief Sets trial strain.
int XC::SectionAggregator::setTrialSectionDeformation(const Vector &def)
  {
    int ret= 0;
    int theSectionOrder= 0;

    if(theSection)
      {
        theSectionOrder= theSection->getOrder();
        Vector v(workArea, theSectionOrder);
        for(register int i= 0;i<theSectionOrder;i++)
          v(i)= def(i);
        ret= theSection->setTrialSectionDeformation(v);
      }
    theAdditions.setTrialStrain(def,theSectionOrder);
    return ret;
  }

//! @brief Returns strain at position being passed as parameter.
double XC::SectionAggregator::getStrain(const double &y,const double &z) const
  {
    double retval= 0;
    if(theSection)
      retval= theSection->getStrain(y,z);
    return retval;
  }

//! @brief Zeroes material initial generalized strain.
void XC::SectionAggregator::zeroInitialSectionDeformation(void)
  {
    if(theSection)
      theSection->zeroInitialSectionDeformation();
    theAdditions.zeroInitialStrain();
  }


//! @brief Returns material initial generalized strain.
const XC::Vector &XC::SectionAggregator::getInitialSectionDeformation(void) const
  {
    int theSectionOrder= 0;
    if(theSection)
      {
        const Vector &eSec= theSection->getInitialSectionDeformation();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          defzero(i)= eSec(i);
      }
    theAdditions.getInitialStrain(defzero,theSectionOrder);
    return defzero;
  }

//! @brief Returns material trial generalized strain.
const XC::Vector &XC::SectionAggregator::getSectionDeformation(void) const
  {
    int theSectionOrder= 0;
    if(theSection)
      {
        const Vector &eSec= theSection->getSectionDeformation();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          def(i)= eSec(i);
      }
    theAdditions.getStrain(def,theSectionOrder);
    static Vector retval;
    retval= def-defzero;
    return retval;
  }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::SectionAggregator::getSectionTangent(void) const
  {
    int theSectionOrder= 0;

    // Zero before assembly
    ks.Zero();

    if(theSection)
      {
        const XC::Matrix &kSec= theSection->getSectionTangent();
        theSectionOrder= theSection->getOrder();

        for(register int i= 0; i < theSectionOrder; i++)
          for(register int j= 0; j < theSectionOrder; j++)
            ks(i,j)= kSec(i,j);
      }
    theAdditions.getTangent(ks,theSectionOrder);
    return ks;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::SectionAggregator::getInitialTangent(void) const
  {
    int theSectionOrder= 0;

    // Zero before assembly
    ks.Zero();

    if(theSection)
      {
        const Matrix &kSec= theSection->getInitialTangent();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          for(register int j= 0; j < theSectionOrder; j++)
            ks(i,j)= kSec(i,j);
      }
    theAdditions.getInitialTangent(ks,theSectionOrder);
    return ks;
  }

//! @brief Returns the flexibility matrix.
const XC::Matrix &XC::SectionAggregator::getSectionFlexibility(void) const
  {
    int theSectionOrder= 0;
    // Zero before assembly
    fs.Zero();

    if(theSection)
      {
        const XC::Matrix &fSec= theSection->getSectionFlexibility();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          for(register int j= 0; j < theSectionOrder; j++)
            fs(i,j)= fSec(i,j);
      }
    theAdditions.getFlexibility(fs,theSectionOrder);
    return fs;
  }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::SectionAggregator::getInitialFlexibility(void) const
  {
    int theSectionOrder= 0;

    // Zero before assembly
    fs.Zero();

    if(theSection)
      {
        const Matrix &fSec= theSection->getInitialFlexibility();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0;i<theSectionOrder;i++)
          for(register int j= 0;j<theSectionOrder;j++)
            fs(i,j)= fSec(i,j);
      }
    theAdditions.getInitialFlexibility(fs,theSectionOrder);
    return fs;
  }

//! @brief Returns the stress resultant.
const XC::Vector &XC::SectionAggregator::getStressResultant(void) const
  {
    int theSectionOrder= 0;
    
    if(theSection)
      {
        const Vector &sSec= theSection->getStressResultant();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          s(i)= sSec(i);
      }
    theAdditions.getStress(s,theSectionOrder);
    return s;
  }

//! @brief Section stiffness contribution response identifiers.
const XC::ResponseId &XC::SectionAggregator::getType(void) const
  {
    int theSectionOrder= 0;
    if(theSection)
      {
        const XC::ResponseId &secType= theSection->getType();
        theSectionOrder= theSection->getOrder();
        for(register int i= 0; i < theSectionOrder; i++)
          theCode(i)= secType(i);
      }
    theAdditions.getType(theCode,theSectionOrder);
    return theCode;
  }

//! @brief Returns the order of the section.
int XC::SectionAggregator::getOrder(void) const
  {
    int order= theAdditions.size();
    if(theSection)
      order+= theSection->getOrder();
    return order;
  }

//! @brief Commits material state after convergence.
int XC::SectionAggregator::commitState(void)
  {
    int err= 0;
    if(theSection)
      err+= theSection->commitState();
    err+= theAdditions.commitState();
    return err;
  }

//! @brief Returns the material to the las committed state.
int XC::SectionAggregator::revertToLastCommit(void)
  {
    int err= 0;
    // Revert the section
    if(theSection)
      err += theSection->revertToLastCommit();
    // Do the same for the uniaxial materials
    err+= theAdditions.revertToLastCommit();
    return err;
  }        

//! @brief Reverts the material to its initial state. 
int XC::SectionAggregator::revertToStart(void)
  {
    int err= 0;
    // Revert the section
    if(theSection)
      err += theSection->revertToStart();
    // Do the same for the uniaxial materials
    err+= theAdditions.revertToStart();
    return err;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::SectionAggregator::sendData(Communicator &comm)
  {
    int res= PrismaticBarCrossSection::sendData(comm);
    res+= comm.sendBrokedPtr(theSection,getDbTagData(),BrokedPtrCommMetaData(5,6,7));
    res+= comm.sendMovable(theAdditions,getDbTagData(),CommMetaData(8));
    res+= comm.sendVector(def,getDbTagData(),CommMetaData(9));
    res+= comm.sendVector(defzero,getDbTagData(),CommMetaData(10));
    res+= comm.sendVector(s,getDbTagData(),CommMetaData(11));
    res+= comm.sendMatrix(ks,getDbTagData(),CommMetaData(12));
    res+= comm.sendMatrix(fs,getDbTagData(),CommMetaData(13));
    res+= comm.sendID(theCode,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::SectionAggregator::recvData(const Communicator &comm)
  {
    int res= PrismaticBarCrossSection::recvData(comm);
    theSection= comm.getBrokedMaterial(theSection,getDbTagData(),BrokedPtrCommMetaData(5,6,7));
    res+= comm.receiveMovable(theAdditions,getDbTagData(),CommMetaData(8));
    res+= comm.receiveVector(def,getDbTagData(),CommMetaData(9));
    res+= comm.receiveVector(defzero,getDbTagData(),CommMetaData(10));
    res+= comm.receiveVector(s,getDbTagData(),CommMetaData(11));
    res+= comm.receiveMatrix(ks,getDbTagData(),CommMetaData(12));
    res+= comm.receiveMatrix(fs,getDbTagData(),CommMetaData(13));
    res+= comm.receiveID(theCode,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::SectionAggregator::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(15);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::SectionAggregator::recvSelf(const Communicator &comm)
  {
    inicComm(15);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::SectionAggregator::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      { theSection->Print(s, flag); }
    else
      {
        s << "\nSection Aggregator, tag: " << this->getTag() << std::endl;
        if(theSection)
          {
            s << "\tSection, tag: " << theSection->getTag() << std::endl;
            theSection->Print(s, flag);
          }
        s << "\tUniaxial Additions" << std::endl;
        theAdditions.Print(s,flag);
      }
  }

XC::Response *XC::SectionAggregator::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    // See if the response is one of the defaults
    Response *res= XC::PrismaticBarCrossSection::setResponse(argv,info);
    if(res != 0)
      return res;
  
    // If not, forward the request to the section (need to do this to get fiber response)
    // CURRENTLY NOT SENDING ANYTHING OFF TO THE UniaxialMaterials ... Probably
    // don't need anything more from them than stress, strain, and stiffness, 
    // which are covered in base class method ... can change if need arises
    else if(theSection)
      return theSection->setResponse(argv, info);
    else
      return 0;
  }

int XC::SectionAggregator::getResponse(int responseID, Information &info)
  {
    // Just call the base class method ... don't need to define
    // this function, but keeping it here just for clarity
    return XC::PrismaticBarCrossSection::getResponse(responseID, info);
  }

//! @brief Returns the identifier of the variable which name
//! is being passed as parameter.
int XC::SectionAggregator::setVariable(const std::string &argv)
  {
    // Axial strain
    if(argv =="axialStrain")
      return 1;
    else if (argv == "curvatureZ") //Curvature about the section z-axis
      return 2;
    else if(argv == "curvatureY") // Curvature about the section y-axis
      return 3;
    else
      return -1;
  }

//! @brief Returns the value of the variable which identifier
//! is being passed as parameter.
int XC::SectionAggregator::getVariable(int variableID, double &info)
  {
    int i;
    info= 0.0;

    const int order= getOrder();

    const Vector &e= getSectionDeformation();
    const ResponseId &code= this->getType();

    switch (variableID)
      {
      case 1: // Axial strain
        // Series model ... add all sources of deformation
        for(i= 0; i < order; i++)
          if(code(i) == SECTION_RESPONSE_P)
            info+= e(i);
        return 0;
      case 2: // Curvature about the section z-axis
        for(i= 0; i < order; i++)
          if(code(i) == SECTION_RESPONSE_MZ)
            info += e(i);
          return 0;
      case 3:// Curvature about the section y-axis
        for(i= 0;i<order;i++)
          if(code(i) == SECTION_RESPONSE_MY)
            info += e(i);
        return 0;
      default:
        return -1;
      }
  }
