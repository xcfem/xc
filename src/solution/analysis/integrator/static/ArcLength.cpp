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

// $Revision: 1.7 $
// $Date: 2005/10/19 21:53:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/ArcLength.cpp,v $


// File: ~/analysis/integrator/ArcLength.C
//
// Written: fmk
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::ArcLength.
// ArcLength is an algorithmic class for perfroming a static analysis
// using the arc length scheme, that is within a load step the follwing
// constraint is enforced: dU^TdU + alpha^2*dLambda^2 = arcLength^2
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and arcLength is a control parameter.
//
// What: "@(#) ArcLength.C, revA"


#include <solution/analysis/integrator/static/ArcLength.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>

//! @brief Constructor.
XC::ArcLength::ArcLength(SoluMethod *owr,double arcLength, double alpha)
  :ArcLengthBase(owr,INTEGRATOR_TAGS_ArcLength,arcLength,alpha) {}

//! @brief Devuelve el valor de dLambda para el método update.
double XC::ArcLength::getDLambdaUpdate(void) const
  {
    const double &dLStep= vectores.getDeltaLambdaStep();
    const Vector &dUhat= vectores.getDeltaUhat();
    const Vector &dUstep= vectores.getDeltaUstep();
    const Vector &dUbar= vectores.getDeltaUbar();
    // determine the coeeficients of our quadratic equation
    const double a = alpha2 + (dUhat^dUhat);
    double b = alpha2*dLStep + (dUhat^dUbar) + (dUstep^dUhat);
    b *= 2.0;
    const double c = 2*(dUstep^dUbar) + (dUbar^dUbar);
    // check for a solution to quadratic
    double b24ac = b*b - 4.0*a*c;
    if(b24ac < 0)
      {
        std::cerr << "ArcLength::getDLambda() - imaginary roots due to multiple instability";
        std::cerr << " directions - initial load increment was too large\n";
        std::cerr << "a: " << a << " b: " << b << " c: " << c << " b24ac: " << b24ac << std::endl;
        return -1;
      }
    const double a2 = 2.0*a;
    if(a2 == 0.0)
      {
        std::cerr << "ArcLength::getDLambda() - zero denominator";
        std::cerr << " alpha was set to 0.0 and zero reference load\n";
        return -2;
      }

    // determine the roots of the quadratic
    double sqrtb24ac = sqrt(b24ac);
    double dlambda1 = (-b + sqrtb24ac)/a2;
    double dlambda2 = (-b - sqrtb24ac)/a2;

    double val = dUhat^dUstep;
    double theta1 = (dUstep^dUstep) + (dUbar^dUstep);
    //    double theta2 = theta1 + dlambda2*val;
    theta1 += dlambda1*val;

    // choose dLambda based on angle between incremental displacement before
    // and after this step -- want positive
    double retval= 0;
    if(theta1 > 0)
      retval= dlambda1;
    else
      retval= dlambda2;
    return retval;
  }

//! @brief Imprime el objeto.
void XC::ArcLength::Print(std::ostream &s, int flag)
  {
    ArcLengthBase::Print(s,flag);
    s << "  arcLength: " << sqrt(arcLength2) <<  "  alpha: ";
    s << sqrt(alpha2) << std::endl;
  }
