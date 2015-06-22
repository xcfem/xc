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
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/LoadControl.h,v $
                                                                        
                                                                        
#ifndef LoadControl_h
#define LoadControl_h

// File: ~/analysis/integrator/LoadControl.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for LoadControl.
// LoadControl is an algorithmic class for perfroming a static analysis
// using a load control integration scheme.
//
// What: "@(#) LoadControl.h, revA"

#include "solution/analysis/integrator/static/BaseControl.h"

namespace XC {

//! @ingroup StaticIntegrator
//
//! @brief Especifica el factor incremental a aplicar a las cargas para cada
//! paso del análisis.
class LoadControl : public BaseControl
  {
  private:
    double deltaLambda;  //!< Valor de dLambda en el paso (i-1).
    double dLambdaMin, dLambdaMax; //!< Valores máximo y mínimo para dlambda en el paso (i).
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    LoadControl(SoluMethod *,double deltaLambda= 1.0, int numIncr= 1, double minLambda=1.0, double maxlambda=1.0);
    Integrator *getCopy(void) const;
  public:

    int newStep(void);    
    int update(const Vector &deltaU);
    void setDeltaLambda(const double &);
    inline double getDeltaLambda(void) const
      { return deltaLambda; }
    void setDeltaLambdaMin(const double &);
    inline double getDeltaLambdaMin(void) const
      { return dLambdaMin; }
    void setDeltaLambdaMax(const double &);
    inline double getDeltaLambdaMax(void) const
      { return dLambdaMax; }

    // Public methods for Output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);    
  };
inline Integrator *LoadControl::getCopy(void) const
  { return new LoadControl(*this); }
} // fin namespace XC

#endif

