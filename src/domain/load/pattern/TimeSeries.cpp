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


