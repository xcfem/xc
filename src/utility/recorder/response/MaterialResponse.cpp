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
                                                                        
// $Revision: 1.3 $
// $Date: 2001/07/31 18:26:59 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/response/MaterialResponse.cpp,v $
                                                                        
// Written: MHS 
// Created: Oct 2000
//
// Description: This file contains the XC::MaterialResponse class implementation

#include <utility/recorder/response/MaterialResponse.h>
#include "domain/mesh/element/Information.h"
#include <material/Material.h>

XC::MaterialResponse::MaterialResponse(Material *mat, int id)
  : XC::Response(), theMaterial(mat), responseID(id) {}

XC::MaterialResponse::MaterialResponse(Material *mat, int id, int val)
  : XC::Response(val), theMaterial(mat), responseID(id) {}

XC::MaterialResponse::MaterialResponse(Material *mat, int id, double val)
  : XC::Response(val), theMaterial(mat), responseID(id) {}

XC::MaterialResponse::MaterialResponse(Material *mat, int id, const XC::ID &val)
  : XC::Response(val), theMaterial(mat), responseID(id) {}

XC::MaterialResponse::MaterialResponse(Material *mat, int id, const XC::Vector &val)
  : XC::Response(val), theMaterial(mat), responseID(id) {}

XC::MaterialResponse::MaterialResponse(Material *mat, int id, const XC::Matrix &val)
  : XC::Response(val), theMaterial(mat), responseID(id) {}

XC::MaterialResponse::MaterialResponse(Material *mat, int id, const BJtensor &val)
  : XC::Response(val), theMaterial(mat), responseID(id) {}

int XC::MaterialResponse::getResponse(void)
  { return theMaterial->getResponse(responseID, myInfo); }
