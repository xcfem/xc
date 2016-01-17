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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/25 23:33:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSectionGJ.cpp,v $
                                                                        
// Written: fmk
// Created: 04/04
//
// Description: This file contains the class implementation of XC::FiberSection2d.

#include <cstdlib>

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <material/section/fiber_section/fiber/Fiber.h>
#include <classTags.h>
#include <material/section/fiber_section/FiberSectionGJ.h>
#include <domain/mesh/element/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/repres/section/FiberSectionRepr.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "material/section/ResponseId.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

// constructors:
XC::FiberSectionGJ::FiberSectionGJ(int tag,const contenedor_fibras &fibers, double gj,XC::MaterialLoader *mat_ldr): 
  FiberSection3dBase(tag, SEC_TAG_FiberSectionGJ,4,fibers,mat_ldr), GJ(gj)
  {fibras.setup(*this,fibers,kr);}

XC::FiberSectionGJ::FiberSectionGJ(int tag,MaterialLoader *mat_ldr): 
  FiberSection3dBase(tag, SEC_TAG_FiberSectionGJ,4,mat_ldr),GJ(1.0) {}

// constructor for blank object that recvSelf needs to be invoked upon
XC::FiberSectionGJ::FiberSectionGJ(MaterialLoader *mat_ldr):
  FiberSection3dBase(0,SEC_TAG_FiberSectionGJ,4,mat_ldr), GJ(1.0) {}

void XC::FiberSectionGJ::setupFibers(void)
  {
    if(section_repres)
      fibras.setup(*this,section_repres->getFibras3d(),kr);
    else
      fibras.updateKRCDG(*this,kr);
  }

//! @brief Añade una fibra a la sección.
XC::Fiber *XC::FiberSectionGJ::addFiber(Fiber &newFiber)
  { return fibras.addFiber(*this,newFiber,kr); }

//! @brief Establece los valores de las deformaciones iniciales.
int XC::FiberSectionGJ::setInitialSectionDeformation(const Vector &deforms)
  {
    FiberSection3dBase::setInitialSectionDeformation(deforms);
    return fibras.setInitialSectionDeformation(*this);
  }

//! @brief Establece los valores de las deformaciones de prueba.
int XC::FiberSectionGJ::setTrialSectionDeformation(const XC::Vector &deforms)
  {
    FiberSection3dBase::setTrialSectionDeformation(deforms);
    return fibras.setTrialSectionDeformation(*this,kr);
  }

//! @brief Devuelve la matriz de rigidez noval.
const XC::Matrix &XC::FiberSectionGJ::getInitialTangent(void) const
  { return fibras.getInitialTangent(*this);  }

XC::SectionForceDeformation *XC::FiberSectionGJ::getCopy(void) const
  { return new FiberSectionGJ(*this); }

const XC::ResponseId &XC::FiberSectionGJ::getType(void) const
  { return RespElasticSection3d; }

int XC::FiberSectionGJ::getOrder(void) const
  { return kr.dim(); }

//! @brief Devuelve el estado de la sección al del último commit.
int XC::FiberSectionGJ::revertToLastCommit(void)
  {
    FiberSection3dBase::revertToLastCommit();
    return fibras.revertToLastCommit(*this,kr);
  }

//! @brief Devuelve la sección a su estado inicial.
int XC::FiberSectionGJ::revertToStart(void)
  { return fibras.revertToStart(*this,kr); }

int XC::FiberSectionGJ::sendSelf(CommParameters &cp)
  {
    int res= 0;
    return res;
  }

int XC::FiberSectionGJ::recvSelf(const CommParameters &)
  {
    int res= 0;
    return res;
  }

void XC::FiberSectionGJ::Print(std::ostream &s, int flag)
  {
    s << "\nFiberSectionGJ, tag: " << this->getTag() << std::endl;
    s << "\tSection code: " << getType();
    s << "\tTorsional Stiffness: " << GJ << std::endl;

    if(flag == 1)
      fibras.Print(s,flag);
  }

XC::FiberSectionGJ XC::FiberSectionReprToFiberSectionGJ(const int &tag,const XC::FiberSectionRepr &fiberSectionRepr,const double &GJ)
  { return fiberSectionRepr.getFiberSectionGJ(tag,GJ); }
