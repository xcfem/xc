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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/TrigSeries.cpp,v $
                                                                        
                                                                        
// File: ~/domain/load/pattern/TrigSeries.C
//
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class implementation of XC::TrigSeries.
//
// What: "@(#) TrigSeries.C, revA"


#include "TrigSeries.h"
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cmath>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"

XC::TrigSeries::TrigSeries(double startTime, double finishTime, double T, double phi, double theFactor)
  :PeriodSeries(TSERIES_TAG_TrigSeries,startTime,finishTime,T,phi,theFactor) {}

XC::TrigSeries::TrigSeries(void)
  :PeriodSeries(TSERIES_TAG_TrigSeries) {}

//! @brief Lee un objeto XC::TrigSeries desde archivo
bool XC::TrigSeries::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(TrigSeries) Procesando comando: " << cmd << std::endl;

    return PeriodSeries::procesa_comando(status);
  }

double XC::TrigSeries::getFactor(double pseudoTime) const
  {
    static double twopi = 4*asin(1.0);
    if(pseudoTime >= tStart && pseudoTime <= tFinish)
      return cFactor*sin(twopi*(pseudoTime-tStart)/period + shift);
    else
      return 0.0;
  }

int XC::TrigSeries::sendSelf(CommParameters &cp)
  {
    static ID data(3);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "TrigSeries::sendSelf() - ch failed to send data\n";
    return result;
  }


int XC::TrigSeries::recvSelf(const CommParameters &cp)
  {
    static ID data(3);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "TrigSeries::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;    
  }

void XC::TrigSeries::Print(std::ostream &s, int flag) const
  {
    s << "Trig Series" << std::endl;
    s << "\tFactor: " << cFactor << std::endl;
    s << "\ttStart: " << tStart << std::endl;
    s << "\ttFinish: " << tFinish << std::endl;
    s << "\tPeriod: " << period << std::endl;
    s << "\tPhase Shift: " << shift << std::endl;
  }

//! @brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::TrigSeries::GetProp(const std::string &cod) const
  {
    if(cod=="start")
      return any_const_ptr(tStart);
    else if(cod=="finish")
      return any_const_ptr(tFinish);
    else if(cod=="period")
      return any_const_ptr(period);
    else if(cod=="phase_shift")
      return any_const_ptr(shift);
    else if(cod=="factor")
      return any_const_ptr(cFactor);
    else if(cod=="frec")
      {
        tmp_gp_dbl= 1.0/period;
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="puls")
      {
        tmp_gp_dbl= (2.0*M_PI/period);
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return PeriodSeries::GetProp(cod);
  }
