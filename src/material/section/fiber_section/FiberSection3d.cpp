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

// $Revision: 1.20 $
// $Date: 2005/05/20 16:44:07 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSection3d.cpp,v $

// Written: fmk
// Created: 04/04
//
// Description: This file contains the class implementation of XC::FiberSection2d.

#include <cstdlib>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <material/section/fiber_section/fiber/Fiber.h>
#include "classTags.h"
#include <material/section/fiber_section/FiberSection3d.h>
#include <domain/mesh/element/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/repres/section/FiberSectionRepr.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "material/section/ResponseId.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

//! @brief Constructor (se usa en FiberSectionShear3d).
XC::FiberSection3d::FiberSection3d(int tag,int classTag,MaterialLoader *mat_ldr)
  : FiberSection3dBase(tag, classTag,3,mat_ldr) {}

//! @brief Constructor.
XC::FiberSection3d::FiberSection3d(int tag,const contenedor_fibras &fibers,XC::MaterialLoader *mat_ldr):
  FiberSection3dBase(tag, SEC_TAG_FiberSection3d, 3,fibers,mat_ldr)
  {fibras.setup(*this,fibers,kr);}

//! @brief Constructor.
XC::FiberSection3d::FiberSection3d(int tag,MaterialLoader *mat_ldr)
  : FiberSection3dBase(tag, SEC_TAG_FiberSection3d,3,mat_ldr) {}

//! @brief Constructor for blank object that recvSelf needs to be invoked upon
XC::FiberSection3d::FiberSection3d(XC::MaterialLoader *mat_ldr)
  : FiberSection3dBase(0, SEC_TAG_FiberSection3d,3,mat_ldr) {}

void XC::FiberSection3d::setupFibers(void)
  {
    if(section_repres)
      fibras.setup(*this,section_repres->getFibras3d(),kr);
    else
      fibras.updateKRCDG(*this,kr);
  }

//! @brief Añade una fibra a la sección.
XC::Fiber *XC::FiberSection3d::addFiber(Fiber &newFiber)
  { return fibras.addFiber(*this,newFiber,kr); }


//! @brief Establece los valores de las deformaciones iniciales.
int XC::FiberSection3d::setInitialSectionDeformation(const Vector &deforms)
  {
    FiberSectionBase::setInitialSectionDeformation(deforms);
    return fibras.setInitialSectionDeformation(*this);
  }

//! @brief Establece los valores de las deformaciones de prueba.
int XC::FiberSection3d::setTrialSectionDeformation(const Vector &deforms)
  {
    FiberSection3dBase::setTrialSectionDeformation(deforms);
    return fibras.setTrialSectionDeformation(*this,kr);
  }

//! @brief Devuelve la matriz de rigidez tangente inicial.
const XC::Matrix &XC::FiberSection3d::getInitialTangent(void) const
  { return fibras.getInitialTangent(*this); }

//! @brief Constructor virtual.
XC::SectionForceDeformation *XC::FiberSection3d::getCopy(void) const
  { return new FiberSection3d(*this); }

//! @brief Devuelve el tipo.
const XC::ResponseId &XC::FiberSection3d::getType(void) const
  { return RespFiberSection3d; }

int XC::FiberSection3d::getOrder(void) const
  { return 3; }

//! @brief Devuelve el estado de la sección al del último commit.
int XC::FiberSection3d::revertToLastCommit(void)
  {
    FiberSection3dBase::revertToLastCommit();
    return fibras.revertToLastCommit(*this,kr);
  }

//! @brief Devuelve la sección a su estado inicial.
int XC::FiberSection3d::revertToStart(void)
  { return fibras.revertToStart(*this,kr); }

int XC::FiberSection3d::sendSelf(CommParameters &cp)
  {
    int res= 0;
    std::cerr << "FiberSection3d::sendSelf - no implementada.\n";
    return res;
  }

int XC::FiberSection3d::recvSelf(const CommParameters &cp)
  {
    int res= 0;
    std::cerr << "FiberSection3d::recvSelf - no implementada.\n";
    return res;
  }

void XC::FiberSection3d::Print(std::ostream &s, int flag)
  {
    if(flag == 2)
      fibras.Print(s,flag);
    else
      {
        s << "\nFiberSection3d, tag: " << getTag() << std::endl;
        s << "\tSection code: " << getType();

        if(flag == 1)
          fibras.Print(s,flag);
      }
  }

XC::FiberSection3d XC::FiberSectionReprToFiberSection3d(const int &tag,const FiberSectionRepr &fiberSectionRepr)
  { return fiberSectionRepr.getFiberSection3d(tag); }
