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
** (C) Copyright 2001, The Regents of the University of California    **
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
// $Date: 2003/02/14 23:00:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/LineSearch.h,v $
                                                                        
#ifndef LineSearch_h
#define LineSearch_h

// Written: fmk 
// Created: 11/01

// Description: This file contains the class definition for 
// LineSearch. LineSearch is an abstract base class, 
// i.e. no objects of it's type can be created.  Its subclasses seek
// to find a better solution to R(U)=0 than the solution Ui-1 + delta Ui
// would give, typically Ui = Ui-1 + factor * delta Ui.
// 
// What: "@(#)LineSearch.h, revA"

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/nucleo/EntCmd.h"
#include <iostream>
#include "utility/matrix/Vector.h"

namespace XC {
class SolutionAlgorithm;
class IncrementalIntegrator;
class LinearSOE;

//! @ingroup AnalAlgo
//
//! @defgroup EQSolAlgo Algoritmos para la solución del sistema de ecuaciones.
//
//! @ingroup EQSolAlgo
//
//! @brief  LineSearch is an abstract base class, 
//! i.e. no objects of it's type can be created.  Its subclasses seek
//! to find a better solution to R(U)=0 than the solution Ui-1 + delta Ui
//! would give, typically Ui = Ui-1 + factor * delta Ui.
class LineSearch: public MovableObject, public EntCmd
  {
  protected:
    Vector x;
    double tolerance;
    int    maxIter;
    double minEta;
    double maxEta;
    int    printFlag;

    bool procesa_comando(CmdStatus &status);

    friend class NewtonLineSearch;
    LineSearch(int classTag,const double &tol= 0.8, const int &mi= 10,const double &mneta= 0.1,const double &mxeta= 10,const int &flag= 1);
    virtual LineSearch *getCopy(void) const= 0;
  public:
    inline virtual ~LineSearch(void) {}
    // virtual functions
    virtual int newStep(LinearSOE &theSOE);
    virtual int search(double s0, 
		       double s1, 
		       LinearSOE &theSOE, 
		       IncrementalIntegrator &theIntegrator) =0;
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);    
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif


