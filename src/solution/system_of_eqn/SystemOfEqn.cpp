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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/SystemOfEqn.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/SystemOfEqn.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class implementation of XC::SystemOfEqn.
//
// What: "@(#) SystemOfEqn.C, revA"

#include <solution/system_of_eqn/SystemOfEqn.h>
#include <solution/analysis/model/AnalysisModel.h>
#include "solution/SoluMethod.h"
#include "xc_utils/src/base/CmdStatus.h"

XC::SystemOfEqn::SystemOfEqn(SoluMethod *owr,int clasTag)
  : MovableObject(clasTag), EntCmd(owr) {}

//! @brief Lee un objeto XC::SystemOfEqn desde archivo
bool XC::SystemOfEqn::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(SystemOfEqn) Procesando comando: " << cmd << std::endl;
    return EntCmd::procesa_comando(status);
  }


//! @brief Devuelve un puntero al modelo de solución al que pertenece.
XC::SoluMethod *XC::SystemOfEqn::getSoluMethod(void)
  { return dynamic_cast<SoluMethod *>(Owner()); }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
const XC::SoluMethod *XC::SystemOfEqn::getSoluMethod(void) const
  { return dynamic_cast<const SoluMethod *>(Owner()); }

//! @brief Devuelve un puntero al modelo de análisis.
const XC::AnalysisModel *XC::SystemOfEqn::getAnalysisModelPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Devuelve un puntero al modelo de análisis.
XC::AnalysisModel *XC::SystemOfEqn::getAnalysisModelPtr(void)
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }
