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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/05/08 22:56:41 $
// $Source: SRC/material/section/fiber_section/fiber/UniaxialFiber.cpp,v $
                                                                        
                                                                        
// File: ~/section/UniaxialFiber.C
//
// Written: Remo Magalhaes de Souza
// Created: 10/98
// Revision: 
//
// Description: This file contains the implementation for the
// UniaxialFiber class. UniaxialFiber provides the abstraction of a
// uniaXial XC::Fiber whose position  is defined with only one coordinate.
// The XC::UniaxialFiber is subjected to a stress state with 
// only one nonzero axial stress and corresponding axial strain.
//
// What: "@(#) UniaxialFiber.h, revA"

#include "UniaxialFiber.h"
#include <material/uniaxial/UniaxialMaterial.h>
#include "preprocessor/loaders/MaterialLoader.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/actor/actor/MovableVector.h"

void XC::UniaxialFiber::libera(void)
  {
    if(theMaterial)
      {
        delete theMaterial;
        theMaterial= nullptr;
      }
  }

void XC::UniaxialFiber::alloc(const UniaxialMaterial &theMat)
  {
    libera();
    theMaterial = theMat.getCopy();// get a copy of the MaterialModel
    if(!theMaterial)
      {
        std::cerr <<"XC::UniaxialFiber::UniaxialFiber  -- failed to get copy of XC::UniaxialMaterial\n";
        exit(-1);
      }    
  }

//! @brief Constructor for blank object that recvSelf needs to be invoked upon
XC::UniaxialFiber::UniaxialFiber(int classTag)
  : Fiber(0, classTag), theMaterial(nullptr), area(0.0) {}

//! @brief Constructor.
XC::UniaxialFiber::UniaxialFiber(int tag, int classTag, double Area)
  : Fiber(tag, classTag), theMaterial(nullptr), area(Area) {}

//! @brief Constructor.
XC::UniaxialFiber::UniaxialFiber(int tag, int classTag,const UniaxialMaterial &theMat,const double &Area)
  : Fiber(tag, classTag), theMaterial(nullptr), area(Area)
  { alloc(theMat); }

//! @brief Constructor.
XC::UniaxialFiber::UniaxialFiber(int tag, int classTag,const MaterialLoader &ldr,const std::string &nmbMat,const double &Area)
  : Fiber(tag, classTag), theMaterial(nullptr), area(Area)
  { setMaterial(ldr,nmbMat); }

//! @brief Constructor de copia.
XC::UniaxialFiber::UniaxialFiber(const UniaxialFiber &otra)
  : Fiber(otra),theMaterial(nullptr), area(otra.area)
  { setMaterial(otra.theMaterial); }

//! @brief Operador asignación.
XC::UniaxialFiber &XC::UniaxialFiber::operator=(const UniaxialFiber &otra)
  {
    Fiber::operator=(otra);
    area= otra.area;
    setMaterial(otra.theMaterial);
    return *this;
  }

//! @brief Lee un objeto UniaxialFiber desde archivo
bool XC::UniaxialFiber::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(UniaxialFiber) Procesando comando: " << cmd << std::endl;
    if(cmd == "area")
      {
        area= interpretaDouble(status.GetString());
        return true;
      }
    else
      return Fiber::procesa_comando(status);
  }

// Destructor: 
XC::UniaxialFiber::~UniaxialFiber(void)
  { libera(); }

//! @brief Asigna a la fibra el material que se pasa como parámetro.
void XC::UniaxialFiber::setMaterial(const UniaxialMaterial *theMat)
  {
    if(theMat)
      alloc(*theMat);
    else
      libera();
  }

//! @brief Asigna a la fibra el material cuyo nombre se pasa como parámetro.
void XC::UniaxialFiber::setMaterial(const MaterialLoader &ldr,const std::string &nmbMat)
  {
    const UniaxialMaterial *theMat= dynamic_cast<const UniaxialMaterial *>(ldr.find_ptr(nmbMat));
    setMaterial(theMat);
  }

//! @brief Consuma el estado del material.
int XC::UniaxialFiber::commitState(void)
  { return theMaterial->commitState(); }

//! @brief Devuelve el estado del material al último consumado.
int XC::UniaxialFiber::revertToLastCommit(void)
  { return theMaterial->revertToLastCommit(); }

//! @brief Devuelve el material de la fibra al estado inicial.
int XC::UniaxialFiber::revertToStart(void)
  { return theMaterial->revertToStart(); }


//! @brief Envia los datos a través del canal que se pasa como parámetro.
int XC::UniaxialFiber::sendData(CommParameters &cp)
  {
    int res= Fiber::sendData(cp);
    res+= cp.sendBrokedPtr(theMaterial,getDbTagData(),BrokedPtrCommMetaData(2,3,4));
    res+= cp.sendDouble(area,getDbTagData(),CommMetaData(5));

    return res;
  }


//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::UniaxialFiber::recvData(const CommParameters &cp)
  {    
    int res= Fiber::recvData(cp);
    theMaterial= cp.getBrokedMaterial(theMaterial,getDbTagData(),BrokedPtrCommMetaData(2,3,4));
    res+= cp.receiveDouble(area,getDbTagData(),CommMetaData(5));
    return res;
  }
