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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/03/04 00:48:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/TimeSeries.h,v $
                                                                        
                                                                        
#ifndef TimeSeries_h
#define TimeSeries_h

// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for TimeSeries.
// TimeSeries is an abstract class. A TimeSeries object is used to
// determine the load factor to be applied to the loads in a pettern.
// to the model. 
//
// What: "@(#) TimeSeries.h, revA"

#include "utility/actor/actor/MovableObject.h"
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
  class Information;
  class Vector;

//! \ingroup Loads
//!
//!
//! @defgroup TSeries Serie temporal.
//
//! @ingroup TSeries
//
//! @brief Función que modula en el tiempo el valor de otra.
class TimeSeries: public MovableObject, public EntCmd
  {
  protected:
  public:
    TimeSeries(int classTag);
    inline virtual ~TimeSeries(void)
      {}

    virtual TimeSeries *getCopy(void) const= 0;

    // pure virtual functions
    virtual double getFactor(double pseudoTime) const= 0;
    virtual double getDuration(void) const= 0;
    virtual double getPeakFactor(void) const= 0;

    virtual double getTimeIncr (double pseudoTime) const= 0;
    // This is defined to be the time increment from the argument
    // 'pseudoTime' to the NEXT point in the time series path
    // THIS MAY CHANGE -- MAY BE BETTER TO GET THE TIME INCREMENT
    // FROM THE PREVIOUS POINT IN THE PATH UP TO 'pseudoTime', WILL
    // DECIDE ONCE GroundMotionIntegrator IS IMPLEMENTED

    virtual void Print(std::ostream &s, int flag = 0) const= 0;        

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);
    virtual double getFactorSensitivity(double pseudoTime);
    // AddingSensitivity:BEGIN //////////////////////////////////////////
  };

int sendTimeSeriesPtr(TimeSeries *,int posClassTag, int posDbTag,DbTagData &,CommParameters &cp);
TimeSeries *receiveTimeSeriesPtr(TimeSeries *,int posClassTag, int posDbTag,DbTagData &,const CommParameters &cp);

} // end of XC namespace

#endif
