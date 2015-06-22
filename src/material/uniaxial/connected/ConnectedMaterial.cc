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
                                                                        
// $Revision: 1.10 $
// $Date: 2004/11/24 00:48:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ConnectedMaterial.cpp,v $
                                                                        
                                                                        
// File: ~/material/ConnectedModel.C
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for 
// ConnectedModel. ConnectedModel is an aggregation
// of XC::UniaxialMaterial objects all considered acting in Connected.
//
// What: "@(#) ConnectedModel.C, revA"

#include "ConnectedMaterial.h"
#include "utility/matrix/ID.h"

XC::ConnectedMaterial::ConnectedMaterial(int tag, int classTag,const DqUniaxialMaterial &theMaterialModels)
  :UniaxialMaterial(tag,classTag), theModels(theMaterialModels)
  { theModels.set_owner(this); }


//! @brief Constructor por defecto.
XC::ConnectedMaterial::ConnectedMaterial(int tag, int classTag)
  :UniaxialMaterial(tag,classTag), theModels(this) {}

XC::ConnectedMaterial::ConnectedMaterial(const ConnectedMaterial &otro)
  :UniaxialMaterial(otro), theModels(otro.theModels)
  { theModels.set_owner(this); }

XC::ConnectedMaterial &XC::ConnectedMaterial::operator=(const ConnectedMaterial &otro)
  {
    UniaxialMaterial::operator=(otro);
    theModels= otro.theModels;
    theModels.set_owner(this);
    return *this;
  }


//! @brief Envía sus miembros a través del canal que se pasa como parámetro.
int XC::ConnectedMaterial::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendMovable(theModels,getDbTagData(),CommMetaData(2));
    if(res<0)
      std::cerr << "ConnectedMaterial::sendData -- failed to send.\n";
    return res;
  }

//! @brief Recibe sus miembros a través del canal que se pasa como parámetro.
int XC::ConnectedMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveMovable(theModels,getDbTagData(),CommMetaData(2));
    if(res<0)
      std::cerr << "ConnectedMaterial::recvData -- failed to receive.\n";
    return res;
  }






