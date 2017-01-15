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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/03/04 00:48:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/TimeSeries.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for XC::TimeSeries.
// TimeSeries is an abstract class. A XC::TimeSeries object is used to
// determine the load factor to be applied to the loads in a pettern.
// to the model. 
//
// What: "@(#) TimeSeries.C, revA"

#include <domain/load/pattern/TimeSeries.h>
#include <domain/mesh/element/Information.h>


#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"


//! @brief Constructor.
XC::TimeSeries::TimeSeries(int classTag)
  :MovableObject(classTag){}

// AddingSensitivity:BEGIN //////////////////////////////////////////
int XC::TimeSeries::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return 0; }

int XC::TimeSeries::updateParameter(int parameterID, Information &info)
  { return 0; }

int XC::TimeSeries::activateParameter(int parameterID)
  { return 0; }

double XC::TimeSeries::getFactorSensitivity(double pseudoTime)
  { return 0.0; }

// AddingSensitivity:END ////////////////////////////////////////////

//! @brief Envía un puntero a la serie through the channel being passed as parameter.
//! @param posClassTag: Posición de ID del identificador de la clase de la serie.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
int XC::sendTimeSeriesPtr(TimeSeries *ptr,int posClassTag, int posDbTag,DbTagData &dt,CommParameters &cp)
  {
    int res= 0;
    if(ptr)
      {
        dt.setDbTagDataPos(posClassTag,ptr->getClassTag());
        res= cp.sendMovable(*ptr,dt,CommMetaData(posDbTag));
      }
    if(res < 0)
      std::cerr <<"WARNING sendTimeSeriesPtr - "
                << " failed to send time serie.\n";
    return res;
  }

//! @brief Recibe un puntero a la serie through the channel being passed as parameter.
//! @param posClassTag: Posición de ID del identificador de la clase de la serie.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
XC::TimeSeries *XC::receiveTimeSeriesPtr(TimeSeries* ptr,int posClassTag, int posDbTag,DbTagData &dt,const CommParameters &cp)
  {
    TimeSeries *retval= nullptr;
    const int tsClass= dt.getDbTagDataPos(posClassTag);
    if(ptr && (ptr->getClassTag() == tsClass))
      retval= ptr;
    else 
      {
        if(ptr)
          delete ptr;
        retval= cp.getNewTimeSeries(tsClass);
      }
    if(retval)
      {
        int res= cp.receiveMovable(*retval,dt,CommMetaData(posDbTag));
        if(res<0)
          std::cerr <<"WARNING - receiveTimeSeriesPtr "
                    << "failed to receive material.\n";
      }
    else
      std::cerr <<"WARNING  - receiveTimeSeriesPtr "
                << " failed to get a blank material of type "
                << tsClass << std::endl; 
    return retval;
  }


