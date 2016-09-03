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
// $Date: 2003/02/14 23:00:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/ArcLength1.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/ArcLength1.C
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::ArcLength1.
// ArcLength1 is an algorithmic class for perfroming a static analysis
// using the arc length scheme, that is within a load step the follwing
// constraint is enforced: dU^TdU + alpha^2*dLambda^2 = ArcLength1^2
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and XC::ArcLength1 is a control parameter.
//
// What: "@(#) ArcLength1.C, revA"


#include <solution/analysis/integrator/static/ArcLength1.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>

XC::ArcLength1::ArcLength1(SoluMethod *owr,double arcLength, double alpha)
  :ArcLengthBase(owr,INTEGRATOR_TAGS_ArcLength1,arcLength,alpha) {}

//! @brief Devuelve el valor de dLambda.
double XC::ArcLength1::getDLambdaUpdate(void) const
  {
    // determine delta lambda(i)
    const double a= vectores.getDeltaUstep()^vectores.getDeltaUbar();
    const double b= (vectores.getDeltaUstep()^vectores.getDeltaUhat())
                    + alpha2*vectores.getDeltaLambdaStep();
    if(b == 0)
      {
        std::cerr << "ArcLength1::update() - zero denominator,";
        std::cerr << " alpha was set to 0.0 and zero reference load\n";
        return -1;
      }
    return -a/b;
  }

void XC::ArcLength1::Print(std::ostream &s, int flag)
  {
    ArcLengthBase::Print(s,flag);
    s << "  ArcLength1: " << sqrt(arcLength2) <<  "  alpha: ";
    s << sqrt(alpha2) << std::endl;
  }
