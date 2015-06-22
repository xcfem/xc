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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/ModifiedNewton.h,v $
                                                                        
                                                                        
#ifndef ModifiedNewton_h
#define ModifiedNewton_h

// File: ~/OOP/analysis/algorithm/ModifiedNewton.h 
// 
// Written: fmk 
// Created: 11/96 
// Revision: A 
//
// Description: This file contains the class definition for 
// ModifiedNewton. ModifiedNewton is a class which performs a modified 
// Newton-Raphson  solution algorihm in solving the equations.
// No member functions are declared as virtual as 
// it is not expected that this class will be subclassed.
// 
// What: "@(#)ModifiedNewton.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/NewtonBased.h>

namespace XC {
class ConvergenceTest;

//! @ingroup EQSolAlgo
//
//! @brief Emplea la matriz de rigidez tangente en la
//! primera iteración para iterar hasta alcanzar la convengencia.
class ModifiedNewton: public NewtonBased
  {
    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    ModifiedNewton(SoluMethod *,int tangent = CURRENT_TANGENT);
    virtual SolutionAlgorithm *getCopy(void) const;
  public:
    int solveCurrentStep(void);    
    void Print(std::ostream &s, int flag =0);    
  };

inline SolutionAlgorithm *ModifiedNewton::getCopy(void) const
  { return new ModifiedNewton(*this); }
} // fin namespace XC

#endif


