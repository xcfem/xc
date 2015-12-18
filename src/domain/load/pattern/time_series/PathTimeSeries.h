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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/10/20 21:58:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PathTimeSeries.h,v $
                                                                        
                                                                        
#ifndef PathTimeSeries_h
#define PathTimeSeries_h

// File: ~/domain/load/pattern/PathTimeSeries.h
// 
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Description: This file contains the class definition for PathTimeSeries.
// PathTimeSeries is a CFactorSeries class which linear interpolates the
// load factor using user specified control points provided in a vector object.
// the points in the vector are given at time points specified in another vector.
// object. 
//
// What: "@(#) PathTimeSeries.h, revA"

#include "domain/load/pattern/time_series/PathSeriesBase.h"

namespace XC {
class Vector;

//! @ingroup TSeries
//
//! @brief Función del tiempo expresada mediante una seríe de puntos (ti,fi).
class PathTimeSeries : public PathSeriesBase
  {
  private:
    Vector time; //!< vector containg the time values of data points
    mutable int currentTimeLoc; //!< current location in time
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors  
    PathTimeSeries(void);
    PathTimeSeries(const Vector &thePath,const Vector &theTime,  double cfactor = 1.0);
    PathTimeSeries(const std::string &fileNamePath, const std::string &fileNameTime, double cfactor = 1.0);    
    PathTimeSeries(const std::string &fileName,double cfactor = 1.0);
    TimeSeries *getCopy(void) const
      { return new PathTimeSeries(*this); }
    

    // method to get factor
    double getFactor(double pseudoTime) const;
    double getDuration(void) const;
    double getTimeIncr(double pseudoTime) const;

    inline Vector getTime(void) const
      { return time; }
    inline void setTime(const Vector &d)
      { time= d; }
    inline int getCurrentTimeLoc(void) const
      { return currentTimeLoc; }
    inline void setCurrentTimeLoc(const int &d)
      { currentTimeLoc= d; }

    void readFromFile(const std::string &fileName);
    void readFromFiles(const std::string &filePathName,const std::string &fileTimeName);

    // methods for output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

