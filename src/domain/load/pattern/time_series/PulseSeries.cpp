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

// $Revision: 1.1 $
// $Date: 2005/12/15 00:35:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PulseSeries.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/04
// Revision: A
//
// Purpose: This file contains the class implementation of XC::PulseSeries.
//
// What: "@(#) PulseSeries.cpp, revA"


#include "PulseSeries.h"
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cmath>
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"


//! @brief Constructor.
XC::PulseSeries::PulseSeries(double startTime, double finishTime,
    double T, double pulseWidth, double phi, double theFactor)
  : PeriodSeries(TSERIES_TAG_PulseSeries,startTime,finishTime,T,phi,theFactor), pWidth(pulseWidth) {}


//! @brief Constructor por defecto.
XC::PulseSeries::PulseSeries(void)
  : PeriodSeries(TSERIES_TAG_PulseSeries),pWidth(0.5) {}

//! @brief Lee un objeto XC::PulseSeries desde archivo
bool XC::PulseSeries::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(PulseSeries) Procesando comando: " << cmd << std::endl;
    return PeriodSeries::procesa_comando(status);
  }

//! @brief Devuelve el valor del factor para el instante que se pasa como parámetro.
double XC::PulseSeries::getFactor(double pseudoTime) const
  {
    if(tStart <= pseudoTime && pseudoTime <= tFinish)
      {
        const double k = (pseudoTime+shift)/period - floor((pseudoTime+shift)/period);
        if(k < pWidth)
          return cFactor;
        else if(k < 1.00)
          return 0;
        else
          return 0;
      }
    else
      return 0;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::PulseSeries::sendData(CommParameters &cp)
  {
    int res= PeriodSeries::sendData(cp);
    res+= cp.sendDouble(pWidth,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::PulseSeries::recvData(const CommParameters &cp)
  {
    int res= PeriodSeries::recvData(cp);
    res+= cp.receiveDouble(pWidth,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::PulseSeries::sendSelf(CommParameters &cp)
  {
    static ID data(4);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "PulseSeries::sendSelf() - ch failed to send data\n";
    return result;
  }



//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::PulseSeries::recvSelf(const CommParameters &cp)
  {
    static ID data(4);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "PulseSeries::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;    
  }

//! @brief Imprime el objeto.
void XC::PulseSeries::Print(std::ostream &s, int flag) const
  {
    s << "Pulse Series" << std::endl;
    s << "\tFactor: " << cFactor << std::endl;
    s << "\ttStart: " << tStart << std::endl;
    s << "\ttFinish: " << tFinish << std::endl;
    s << "\tPeriod: " << period << std::endl;
    s << "\tPulse Width: " << pWidth << std::endl;
    s << "\tPhase Shift: " << shift << std::endl;
  }
