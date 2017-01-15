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


//! @brief Envía sus miembros through the channel being passed as parameter.
int XC::ConnectedMaterial::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendMovable(theModels,getDbTagData(),CommMetaData(2));
    if(res<0)
      std::cerr << "ConnectedMaterial::sendData -- failed to send.\n";
    return res;
  }

//! @brief Recibe sus miembros through the channel being passed as parameter.
int XC::ConnectedMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveMovable(theModels,getDbTagData(),CommMetaData(2));
    if(res<0)
      std::cerr << "ConnectedMaterial::recvData -- failed to receive.\n";
    return res;
  }






