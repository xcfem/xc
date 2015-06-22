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
// File: ~/system_of_eqn/eigenSOE/BandArpackSolver.h
//
// Written: Jun Peng
// Created: Feb. 11, 1999
// Revision: A
//
// Description: This file contains the class definition for 
// BandArpackSolver. It solves the BandArpackSOE object by calling
// Arpack routines.


#ifndef BandArpackSolver_h
#define BandArpackSolver_h

#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackSOE.h>

namespace XC {
//! @ingroup LinearSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de sistemas de ecuaciones lineales con matriz simétrica
//! empleando la biblioteca arcpack
class BandArpackSolver : public EigenSolver
  {
  private:
    BandArpackSOE *theSOE;
    Vector value;
    Vector eigenvector;
    double tol; //!< Tolerancia para los autovalores obtenidos.
    int maxitr; //!< Número máximo de iteraciones.
    ID iPiv;
    mutable Vector eigenV;
    
    void myMv(int n, double *v, double *result);
    void myCopy(int n, double *v, double *result);
    int getNCV(int n, int nev);

    void print_err_info(int);
  protected:
    bool procesa_comando(CmdStatus &status);

    friend class EigenSOE;
    BandArpackSolver(int numE = 0);   
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:

    virtual int solve(void);
    virtual int solve(int nModes);
    virtual int setSize(void);
    const int &getSize(void) const;
    virtual bool setEigenSOE(BandArpackSOE &theSOE);
    
    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline EigenSolver *BandArpackSolver::getCopy(void) const
   { return new BandArpackSolver(*this); }
} // fin namespace XC

#endif


