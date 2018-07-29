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
*                                                                    **
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

// $Revision: 1.23 $
// $Date: 2005/03/31 19:04:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSection2d.cpp,v $

// Written: fmk
// Created: 04/04
//
// Description: This file contains the class implementation of XC::FiberSection2d.

#include "FiberSection2d.h"
#include <cstdlib>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include "material/section/fiber_section/fiber/UniaxialFiber2d.h"
#include <classTags.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/repres/section/FiberSectionRepr.h>

#include "material/section/ResponseId.h"
#include "xc_utils/src/geom/d1/Line2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

// constructors:
XC::FiberSection2d::FiberSection2d(int tag,const fiber_list &fiberList,MaterialHandler *mat_ldr)
  : FiberSectionBase(tag, SEC_TAG_FiberSection2d,fibers.size(),mat_ldr)
  {
    fibers.setup(*this,fiberList,kr);
// AddingSensitivity:BEGIN ////////////////////////////////////
    parameterID= 0;
// AddingSensitivity:END //////////////////////////////////////
  }

//! @brief Constructor.
XC::FiberSection2d::FiberSection2d(int tag,XC::MaterialHandler *mat_ldr)
  : FiberSectionBase(tag, SEC_TAG_FiberSection2d,mat_ldr)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
    parameterID= 0;
// AddingSensitivity:END //////////////////////////////////////
  }

// constructor for blank object that recvSelf needs to be invoked upon
XC::FiberSection2d::FiberSection2d(XC::MaterialHandler *mat_ldr)
  : FiberSectionBase(0, SEC_TAG_FiberSection2d,mat_ldr)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
    parameterID= 0;
// AddingSensitivity:END //////////////////////////////////////
  }

//! @brief Adds a fiber to the section.
XC::Fiber *XC::FiberSection2d::addFiber(Fiber &newFiber)
  { return fibers.addFiber(*this,newFiber,kr); }

//! @brief Adds a fiber to the section.
XC::Fiber *XC::FiberSection2d::addFiber(int tag,const MaterialHandler &ldr,const std::string &nmbMat,const double &Area, const Vector &position)
  {
    Fiber *retval= fibers.findFiber(tag);
    if(retval)
      std::cerr << "(FiberSection2d::addFiber; fiber with tag: " << tag << " already exists." << std::endl;
    else
      {
        if(position.Size()<1)
          std::cerr << "(FiberSection2d::addFiber; position vector must be of dimension 2." << std::endl;
        UniaxialFiber2d f(tag,ldr,nmbMat,Area,position(0));
        retval= addFiber(f);
      }
    return retval;
  }

void XC::FiberSection2d::setupFibers(void)
  {
    if(section_repres)
      fibers.setup(*this,section_repres->get2DFibers(),kr);
    else
      fibers.updateKRCenterOfMass(*this,kr);
  }

double XC::FiberSection2d::get_strain(const double &y) const
  {
    const Vector &def= getSectionDeformation();
    return (def(0) + y*def(1));
  }

//! @brief Returns the strains in the position being passed as parameter.
double XC::FiberSection2d::getStrain(const double &y,const double &) const
  { return get_strain(-y); } //XXX We must change sign of the y coordinate
                               //because of the mess due to the change of
                               //the coordinate sign in the private member
                               //of the object. See, for example,
                               //UniaxialFiber2d constructor

//! @brief Sets values for initial strains.
int XC::FiberSection2d::setInitialSectionDeformation(const Vector &deforms)
  {
    FiberSectionBase::setInitialSectionDeformation(deforms);
    return fibers.setInitialSectionDeformation(*this);
  }

//! @brief Sets values for trial strains.
int XC::FiberSection2d::setTrialSectionDeformation(const Vector &deforms)
  {
    FiberSectionBase::setTrialSectionDeformation(deforms);
    return fibers.setTrialSectionDeformation(*this,kr);
  }

//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix &XC::FiberSection2d::getInitialTangent(void) const
  { return fibers.getInitialTangent(*this); }

XC::SectionForceDeformation *XC::FiberSection2d::getCopy(void) const
  { return new FiberSection2d(*this); }

const XC::ResponseId &XC::FiberSection2d::getType(void) const
  { return RespElasticSection2d; }

int XC::FiberSection2d::getOrder(void) const
  { return getType().Size(); }

//! @brief Returns the section to its last commited state.
int XC::FiberSection2d::revertToLastCommit(void)
  {
    // Last committed section deformations
    FiberSectionBase::revertToLastCommit();
    return fibers.revertToLastCommit(*this,kr);
  }

//! @brief Returns the section to its initial state.
int XC::FiberSection2d::revertToStart(void)
  {
    FiberSectionBase::revertToStart();
    return fibers.revertToStart(*this,kr);
  }

int XC::FiberSection2d::sendSelf(CommParameters &cp)
  {
    int res= 0;
    std::cerr <<  "XC::FiberSection2d::sendSelf - not implemented.\n";
    return res;
  }

int XC::FiberSection2d::recvSelf(const CommParameters &)
  {
    int res= 0;
    std::cerr <<  "XC::FiberSection2d::recvSelf - not implemented.\n";
    return res;
  }

void XC::FiberSection2d::Print(std::ostream &s, int flag)
  {
    s << "\nFiberSection2d, tag: " << this->getTag() << std::endl;
    s << "\tSection code: " << getType();
    if(flag == 1)
      fibers.Print(s,flag);
  }

int XC::FiberSection2d::getResponse(int responseID, Information &sectInfo)
  {
    // Just call the base class method ... don't need to define
    // this function, but keeping it here just for clarity
    return FiberSectionBase::getResponse(responseID, sectInfo);
  }



// AddingSensitivity:BEGIN ////////////////////////////////////
int XC::FiberSection2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // Initial declarations
    int parameterID= -1;

    // Check if the parameter belongs to the material (only option for now)
    if(argv[0] == "-material" || argv[0] == "material")
      {
        // Get the tag of the material
        int materialTag= atoi(argv[1]);
        std::vector<std::string> argv2(argv);
        argv2.erase(argv2.begin(),argv2.begin()+2);
        parameterID= fibers.setParameter(materialTag,argv2, param);
        // Check that the parameterID is valid
        if(parameterID < 0)
          {
            std::cerr << "XC::FiberSection2d::setParameter() - could not set parameter. " << std::endl;
            return -1;
          }
        else
          { return (parameterID + 100000*this->getTag() + 1000*materialTag); }
      }
    else
      {
        std::cerr << "XC::FiberSection2d::setParameter() - could not set parameter. " << std::endl;
        return -1;
      }
  }

int XC::FiberSection2d::updateParameter (int parameterID, Information &info)
  {
    // Check if it is a material parameter (only option for now)
    if(parameterID > 1000)
      {
        // Get section number and material number
        int sectionTag= this->getTag();
        parameterID= parameterID - sectionTag*100000;
        int materialTag= (int)( floor((double)parameterID) / (1000) );
        parameterID= parameterID - materialTag*1000;

        int ok= fibers.updateParameter(materialTag,parameterID,info);
        if(ok < 0)
          {
            std::cerr << "XC::FiberSection2d::updateParameter() - could not update parameter. " << std::endl;
            return ok;
          }
        else
          { return ok; }
      }
    else
      {
        std::cerr << "XC::FiberSection2d::updateParameter() - could not update parameter. " << std::endl;
        return -1;
      }
  }

int XC::FiberSection2d::activateParameter(int passedParameterID)
  {
    // Note that the parameteID that is stored here at the
    // section level contains all information about section
    // and material tag number:
    parameterID= passedParameterID;
    return fibers.activateParameter(passedParameterID);
  }

const XC::Vector &XC::FiberSection2d::getSectionDeformationSensitivity(int gradNumber)
  {
    static XC::Vector dummy(2);
    return dummy;
  }

const XC::Vector &XC::FiberSection2d::getStressResultantSensitivity(int gradNumber, bool conditional)
  { return fibers.getStressResultantSensitivity(gradNumber,conditional); }

const XC::Matrix &XC::FiberSection2d::getSectionTangentSensitivity(int gradNumber)
  {
    static XC::Matrix something(2,2);
    something.Zero();
    return something;
  }

int XC::FiberSection2d::commitSensitivity(const XC::Vector& defSens, int gradNumber, int numGrads)
  {
    fibers.commitSensitivity(defSens,gradNumber,numGrads);
    return 0;
  }

// AddingSensitivity:END ///////////////////////////////////

XC::FiberSection2d XC::FiberSectionReprToFiberSection2d(const int &tag,const XC::FiberSectionRepr &fiberSectionRepr)
  { return fiberSectionRepr.getFiberSection2d(tag); }
