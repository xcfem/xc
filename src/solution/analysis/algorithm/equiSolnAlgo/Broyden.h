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
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/Broyden.h,v $
                                                                        
#ifndef Broyden_h
#define Broyden_h

// File: ~/OOP/analysis/algorithm/Broyden.h 
// 
// Written: Ed C++ Love
// Created: 04/01

// Description: This file contains the class definition for
// Broyden.  Broyden is a class which performs the Broyden
// solution algorihm in solving the equations.
// No member functions are declared as virtual as 
// it is not expected that this class will be subclassed.
// 
// What: "@(#)Broyden.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/BFBRoydenBase.h>

namespace XC {

//! @ingroup EQSolAlgo
//
//! @brief Algoritmo de Broyden.
class Broyden: public BFBRoydenBase
  {
  private:
    void BroydenUpdate(IncrementalIntegrator *theIntegrator, LinearSOE *theSOE,
			Vector &du, int count);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    Broyden(SoluMethod *,int tangent = CURRENT_TANGENT, int n= 10);    
    Broyden(SoluMethod *,ConvergenceTest &theTest,int tangent = CURRENT_TANGENT, int n= 10);
    virtual SolutionAlgorithm *getCopy(void) const;
  public:

    int solveCurrentStep(void);

    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);    
  };
inline SolutionAlgorithm *Broyden::getCopy(void) const
  { return new Broyden(*this); }
} // fin namespace XC

#endif


