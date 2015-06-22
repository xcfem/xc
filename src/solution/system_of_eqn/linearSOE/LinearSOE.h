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
// $Date: 2005/12/01 01:12:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/LinearSOE.h,v $
                                                                        
                                                                        
#ifndef LinearSOE_h
#define LinearSOE_h

// File: ~/system_of_eqn/LinearSOE.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
//
// What: "@(#) LinearSOE.h, revA"

#include <solution/system_of_eqn/SystemOfEqn.h>

namespace XC {
class LinearSOESolver;
class Graph;
class Matrix;
class Vector;
class ID;

//!  @ingroup SOE
//! 
//! @defgroup LinearSOE Sistemas de ecuaciones lineales.
//
//! @ingroup LinearSOE
//
//! @brief Objetos que se encargan de obtener la solución
//! de problemas de autovalores.
//! @brief Sistema de ecuaciones lineal.
//!
//! This is the class definition for LinearSOE.
//! LinearSOE is an abstract base class and thus no objects of it's type
//! can be instantiated. It has pure virtual functions which must be
//! implemented in it's derived classes. LinearSystemOfEqn is an abstraction 
//! of the linear system of equation given by : [A]{X} = {B} - {C},
//! where A is a matrix and B,C and X are vectors. To solve the equation means
//! given A, B and C to find the unknown X such that the equation is satisfied.
class LinearSOE : public SystemOfEqn
  {
  private:
    LinearSOESolver *theSolver;
  protected:
    void libera(void);
    void copia(const LinearSOESolver *);
    friend class FEM_ObjectBroker;
    virtual bool setSolver(LinearSOESolver *);
    int setSolverSize(void);
    bool procesa_comando(CmdStatus &status);

    LinearSOE(SoluMethod *,int classTag);
  public:
    virtual ~LinearSOE(void);

    virtual int solve(void);    

    // pure virtual functions
    virtual int setSize(Graph &theGraph) =0;    
    virtual int getNumEqn(void) const =0;
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0) =0;
    virtual int addB(const Vector &, const ID &,const double &fact= 1.0) =0;    
    virtual int setB(const Vector &, const double &fact= 1.0) =0;        

    virtual void zeroA(void) =0;
    virtual void zeroB(void) =0;

    virtual const Vector &getX(void) const= 0;
    virtual const Vector &getB(void) const= 0;    
    virtual double getDeterminant(void);
    virtual double normRHS(void) const= 0;

    virtual void setX(int loc, double value) =0;
    virtual void setX(const Vector &X) =0;
    
    LinearSOESolver *getSolver(void);
    LinearSOESolver &newSolver(const std::string &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC


#endif
