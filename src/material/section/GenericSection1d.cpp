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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/GenericSection1d.cpp,v $
                                                                        
                                                                        
// File: ~/material/GenericSection1d.C
//
// Written: MHS 
// Created: Apr 2000
// Revision: A
//
// Description: This file contains the class implementation for XC::GenericSection1d.
//
// What: "@(#) GenericSection1d.C, revA"

#include "GenericSection1d.h"
#include <material/uniaxial/UniaxialMaterial.h>
#include <domain/mesh/element/Information.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cstring>
#include "material/section/ResponseId.h"

XC::Vector XC::GenericSection1d::s(1);
XC::Matrix XC::GenericSection1d::ks(1,1);
XC::ResponseId XC::GenericSection1d::c(1);

//! @brief Constructor.
XC::GenericSection1d::GenericSection1d(int tag, UniaxialMaterial &m, int type)
  :SeccionBarraPrismatica(tag,SEC_TAG_Generic1d), code(type)
  {
    theModel = m.getCopy();

    if(!theModel)
      {
        std::cerr << "XC::GenericSection1d::GenericSection1d  -- failed to get copy of material model\n";
        exit(-1);
      }
  }

//! @brief Constructor.
XC::GenericSection1d::GenericSection1d(int tag)
:SeccionBarraPrismatica(0,SEC_TAG_Generic1d), theModel(0), code(0)
  {}

//! @brief Constructor.
XC::GenericSection1d::GenericSection1d()
:SeccionBarraPrismatica(0,SEC_TAG_Generic1d), theModel(0), code(0)
  {}

//! @brief Destructor.
XC::GenericSection1d::~GenericSection1d(void)
  { if (theModel) delete theModel; }

//! @brief Asigna la deformación inicial (axial y curvaturas) de la sección.
int XC::GenericSection1d::setInitialSectionDeformation(const Vector &def)
  { return theModel->setInitialStrain(def(0)); }

//! @brief Asigna la deformación de prueba (axial y curvaturas) de la sección.
int XC::GenericSection1d::setTrialSectionDeformation(const Vector &def)
  { return theModel->setTrialStrain(def(0)); }

//! @brief Devuelve la deformación inicial (axial y curvaturas) de la sección.
const XC::Vector &XC::GenericSection1d::getInitialSectionDeformation(void) const
  {
    static Vector e(1); // static for class-wide returns
    e(0) = theModel->getInitialStrain();
    return e;
  }

//! @brief Devuelve la deformación (axial y curvaturas) de la sección.
const XC::Vector &XC::GenericSection1d::getSectionDeformation(void) const
  {
    static Vector e(1); // static for class-wide returns
    e(0) = theModel->getStrain();
    return e;
  }

//! @brief Devuelve la resultante del campo de tensiones sobre la sección.
const XC::Vector &XC::GenericSection1d::getStressResultant(void) const
  {
    s(0) = theModel->getStress();
    return s;
  }

//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix &XC::GenericSection1d::getSectionTangent(void) const
  {
    ks(0,0) = theModel->getTangent();
    return ks;
  }

//! @brief Devuelve el valor inicial de la matriz de rigidez tangente.
const XC::Matrix &XC::GenericSection1d::getInitialTangent(void) const
  {
    ks(0,0) = theModel->getInitialTangent();
    return ks;
  }

//! @brief Devuelve la matriz de flexibilidad.
const XC::Matrix &XC::GenericSection1d::getSectionFlexibility(void) const
  {
    double tangent = theModel->getTangent();
    if(tangent != 0.0)
      ks(0,0) = 1.0/tangent;
    else
      ks(0,0) = 1.0e12;
    return ks;
  }

//! @brief Devuelve el valor inicial la matriz de flexibilidad.
const XC::Matrix &XC::GenericSection1d::getInitialFlexibility(void) const
  {
    double tangent = theModel->getInitialTangent();
    ks(0,0) = 1.0/tangent;
    return ks;
  }

//! @brief Devuelve el índice del estado consumado.
int XC::GenericSection1d::commitState(void)
  { return theModel->commitState(); }

//! @brief Devuelve el estado de la sección al último consumado.
int XC::GenericSection1d::revertToLastCommit ()
  { return theModel->revertToLastCommit(); }

//! @brief Devuelve el estado de la sección al inicial.
int XC::GenericSection1d::revertToStart ()
  { return theModel->revertToStart(); }

//! @brief Devuelve el tipo de respuesta.
const XC::ResponseId &XC::GenericSection1d::getType(void) const
  {
    c(0)= code;
    return c;
  }

int XC::GenericSection1d::getOrder(void) const
  { return 1; }

//! @brief Constructor virtual.
XC::SectionForceDeformation *XC::GenericSection1d::getCopy(void) const
  { return new GenericSection1d(*this); }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::GenericSection1d::sendData(CommParameters &cp)
  {
    int res= SeccionBarraPrismatica::sendData(cp);
    setDbTagDataPos(5,code);
    res+= cp.sendBrokedPtr(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::GenericSection1d::recvData(const CommParameters &cp)
  {
    int res= SeccionBarraPrismatica::recvData(cp);
    code= getDbTagDataPos(5);
    theModel= cp.getBrokedMaterial(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    return res;
  }

//! @brief Envia los miembros del objeto a través del canal que se pasa como parámetro.
int XC::GenericSection1d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::GenericSection1d::recvSelf(const CommParameters &cp)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::GenericSection1d::Print (std::ostream &s, int flag)
{
    s << "GenericSection1d (Uniaxial), tag: " << this->getTag() << std::endl;
    s << "\tResponse code: " << code << std::endl;
    s << "\tUniaxialMaterial: " << theModel->getTag() << std::endl;
}
