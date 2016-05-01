//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
#include <domain/mesh/element/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/repres/section/FiberSectionRepr.h>

#include "material/section/ResponseId.h"
#include "xc_utils/src/geom/d1/Recta2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

// constructors:
XC::FiberSection2d::FiberSection2d(int tag,const contenedor_fibras &fibers,XC::MaterialLoader *mat_ldr)
  : FiberSectionBase(tag, SEC_TAG_FiberSection2d,fibers.size(),mat_ldr)
  {
    fibras.setup(*this,fibers,kr);
// AddingSensitivity:BEGIN ////////////////////////////////////
    parameterID= 0;
// AddingSensitivity:END //////////////////////////////////////
  }

//! @brief Constructor.
XC::FiberSection2d::FiberSection2d(int tag,XC::MaterialLoader *mat_ldr)
  : FiberSectionBase(tag, SEC_TAG_FiberSection2d,mat_ldr)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
    parameterID= 0;
// AddingSensitivity:END //////////////////////////////////////
  }

// constructor for blank object that recvSelf needs to be invoked upon
XC::FiberSection2d::FiberSection2d(XC::MaterialLoader *mat_ldr)
  : FiberSectionBase(0, SEC_TAG_FiberSection2d,mat_ldr)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
    parameterID= 0;
// AddingSensitivity:END //////////////////////////////////////
  }

//! @brief Añade una fibra a la sección.
XC::Fiber *XC::FiberSection2d::addFiber(Fiber &newFiber)
  { return fibras.addFiber(*this,newFiber,kr); }

//! @brief Añade una fibra a la sección.
XC::Fiber *XC::FiberSection2d::addFiber(int tag,const MaterialLoader &ldr,const std::string &nmbMat,const double &Area, const Vector &position)
  {
    Fiber *retval= fibras.buscaFibra(tag);
    if(retval)
      std::cerr << "(FiberSection2d::addFiber; fiber with tag: " << tag << " already exists." << std::endl;
    else
      {
        if(position.Size()<1)
          std::cerr << "(FiberSection2d::addFiber; el vector de posición debe ser de dimensión 1." << std::endl;
        UniaxialFiber2d f(tag,ldr,nmbMat,Area,position(0));
        retval= addFiber(f);
      }
    return retval;
  }

void XC::FiberSection2d::setupFibers(void)
  {
    if(section_repres)
      fibras.setup(*this,section_repres->getFibras2d(),kr);
    else
      fibras.updateKRCDG(*this,kr);
  }

double XC::FiberSection2d::get_strain(const double &y) const
  {
    const Vector &def= getSectionDeformation();
    return (def(0) + y*def(1));
  }

//! @brief Devuelve la deformación correspondiente a la posición que se pasa como parámetro.
double XC::FiberSection2d::getStrain(const double &y,const double &) const
  { return get_strain(-y); } //XXX El cambio de signo de la y se debe al lío que produce
                             //el que la coordenada este cambiada de signo
                             //ver constructor UniaxialFiber2d y otras líneas.

//! @brief Establece los valores de las deformaciones iniciales.
int XC::FiberSection2d::setInitialSectionDeformation(const Vector &deforms)
  {
    FiberSectionBase::setInitialSectionDeformation(deforms);
    return fibras.setInitialSectionDeformation(*this);
  }

//! @brief Establece los valores de las deformaciones de prueba.
int XC::FiberSection2d::setTrialSectionDeformation(const Vector &deforms)
  {
    FiberSectionBase::setTrialSectionDeformation(deforms);
    return fibras.setTrialSectionDeformation(*this,kr);
  }

//! @brief Devuelve la matriz de rigidez noval.
const XC::Matrix &XC::FiberSection2d::getInitialTangent(void) const
  { return fibras.getInitialTangent(*this); }

XC::SectionForceDeformation *XC::FiberSection2d::getCopy(void) const
  { return new FiberSection2d(*this); }

const XC::ResponseId &XC::FiberSection2d::getType(void) const
  { return RespElasticSection2d; }

int XC::FiberSection2d::getOrder(void) const
  { return getType().Size(); }

int XC::FiberSection2d::revertToLastCommit(void)
  {
    // Last committed section deformations
    FiberSectionBase::revertToLastCommit();
    return fibras.revertToLastCommit(*this,kr);
  }

//! @brief Devuelve la sección a su estado inicial.
int XC::FiberSection2d::revertToStart(void)
  {
    FiberSectionBase::revertToStart();
    return fibras.revertToStart(*this,kr);
  }

int XC::FiberSection2d::sendSelf(CommParameters &cp)
  {
    int res= 0;
    std::cerr <<  "XC::FiberSection2d::sendSelf - no implementada.\n";
    return res;
  }

int XC::FiberSection2d::recvSelf(const CommParameters &)
  {
    int res= 0;
    std::cerr <<  "XC::FiberSection2d::recvSelf - no implementada.\n";
    return res;
  }

void XC::FiberSection2d::Print(std::ostream &s, int flag)
  {
    s << "\nFiberSection2d, tag: " << this->getTag() << std::endl;
    s << "\tSection code: " << getType();
    if(flag == 1)
      fibras.Print(s,flag);
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
        parameterID= fibras.setParameter(materialTag,argv2, param);
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

        int ok= fibras.updateParameter(materialTag,parameterID,info);
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
    return fibras.activateParameter(passedParameterID);
  }

const XC::Vector &XC::FiberSection2d::getSectionDeformationSensitivity(int gradNumber)
  {
    static XC::Vector dummy(2);
    return dummy;
  }

const XC::Vector &XC::FiberSection2d::getStressResultantSensitivity(int gradNumber, bool conditional)
  { return fibras.getStressResultantSensitivity(gradNumber,conditional); }

const XC::Matrix &XC::FiberSection2d::getSectionTangentSensitivity(int gradNumber)
  {
    static XC::Matrix something(2,2);
    something.Zero();
    return something;
  }

int XC::FiberSection2d::commitSensitivity(const XC::Vector& defSens, int gradNumber, int numGrads)
  {
    fibras.commitSensitivity(defSens,gradNumber,numGrads);
    return 0;
  }

// AddingSensitivity:END ///////////////////////////////////

XC::FiberSection2d XC::FiberSectionReprToFiberSection2d(const int &tag,const XC::FiberSectionRepr &fiberSectionRepr)
  { return fiberSectionRepr.getFiberSection2d(tag); }
