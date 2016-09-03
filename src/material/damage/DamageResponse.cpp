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
                                                                        
// $Revision: 1.1 $
// $Date: 2004/09/01 03:54:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/damage/DamageResponse.cpp,v $
                                                                        
// Written: Arash Altoontash, Gregory Deierlein
// Created: Aug 2002
//
// Description: This file contains the XC::DamageResponse class implementation

#include <material/damage/DamageResponse.h>
#include <material/damage/DamageModel.h>

XC::DamageResponse::DamageResponse(DamageModel *dmg, int id)
  : Response(), theDamage(dmg), responseID(id) {}

XC::DamageResponse::DamageResponse(DamageModel *dmg, int id, int val)
  : Response(val), theDamage(dmg), responseID(id) {}

XC::DamageResponse::DamageResponse(DamageModel *dmg, int id, double val)
  : Response(val), theDamage(dmg), responseID(id) {}

XC::DamageResponse::DamageResponse(DamageModel *dmg, int id, const XC::ID &val)
  : Response(val), theDamage(dmg), responseID(id) {}

XC::DamageResponse::DamageResponse(DamageModel *dmg, int id, const XC::Vector &val)
  : Response(val), theDamage(dmg), responseID(id) {}

XC::DamageResponse::DamageResponse(DamageModel *dmg, int id, const XC::Matrix &val)
  : Response(val), theDamage(dmg), responseID(id) {}

XC::DamageResponse::DamageResponse(DamageModel *dmg, int id, const BJtensor &val)
  : Response(val), theDamage(dmg), responseID(id) {}

int XC::DamageResponse::getResponse(void)
  { return theDamage->getResponse(responseID, myInfo); }
