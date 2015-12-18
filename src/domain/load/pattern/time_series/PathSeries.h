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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/25 23:32:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PathSeries.h,v $
                                                                        
                                                                        
#ifndef PathSeries_h
#define PathSeries_h

// File: ~/analysis/integrator/PathSeries.h
// 
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Description: This file contains the class definition for PathSeries.
// PathSeries is a PathSeriesBase class which linear interpolates the
// load factor using user specified control points provided in a vector object.
// the points in the vector are given at regular time increments pathTimeIncr
// apart. (could be provided in another vector if different)
//
// What: "@(#) PathSeries.h, revA"

#include "PathSeriesBase.h"
#include <string>

namespace XC {
class Vector;

//! @ingroup TSeries
//
//! @brief Función del tiempo expresada mediante una seríe de puntos (ti,fi) en
//! los que la diferencia entre abcisas consecutivas (incremento de tiempo)
//! ti - t(i-1) es una cantidad constante.
class PathSeries : public PathSeriesBase
  {
  private:
    double pathTimeIncr; //!< Incremento de tiempo.
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors
    PathSeries(const Vector &thePath,double pathTimeIncr = 1.0, double cfactor = 1.0);
    PathSeries(const std::string &fileName, double pathTimeIncr = 1.0, double cfactor = 1.0);
    PathSeries(void);
    TimeSeries *getCopy(void) const
      { return new PathSeries(*this); }

    // method to get factor
    double getFactor(double pseudoTime) const;
    double getDuration(void) const;
    double getTimeIncr(double pseudoTime) const {return pathTimeIncr;}

    inline double getPathTimeIncr(void) const
      { return pathTimeIncr; }
    inline void setPathTimeIncr(const double &d)
      { pathTimeIncr= d; }

    void readFromFile(const std::string &fileName);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

