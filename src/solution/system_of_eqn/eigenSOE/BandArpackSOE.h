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
// File: ~/system_of_eqn/eigenSOE/BandArpackSOE.h
//
// Written: Jun Peng 
// Created: Febuary 1999
// Revision: A
//



#ifndef BandArpackSOE_h
#define BandArpackSOE_h

#include <solution/system_of_eqn/eigenSOE/ArpackSOE.h>
#include "utility/matrix/Vector.h"

namespace XC {

class BandArpackSolver;

//! @ingroup EigenSOE
//
//! @brief Sistema de ecuaciones correspondiente
//! a un problema de autovalores con matriz en banda
//! en cuya solución se empleará la biblioteca
//! ArPack (http://www.caam.rice.edu/software/ARPACK)
//! Description: This file contains the class definition for BandArpackSOE
//! BandArpackSOE is a subclass of ArpackSOE. It uses the LAPACK storage
//! scheme to store the components of the K, M matrix, which is a full matrix.
//! It uses the ARPACK to do eigenvalue analysis.
//! ARPACK is an eigen analysis package which was developed by 
//! R.B.Lehoucq, D.C.Sorensen and C.Yang at Rice University. ARPACK is a
//! collection of FORTRAN77 subroutines designed to solve large scale eigen
//! problems. ARPACK is capable of solving large scale non-Hermitian standard 
//! and generalized eigen problems. When the matrix <B>K</B> is symmetric, 
//! the method is a variant of the Lanczos process called Implicitly Restarted
//! Lanczos Method (IRLM).
class BandArpackSOE: public ArpackSOE
  {
  private:
    int numSuperD; //Numero de diagonales por encima de la principal.
    int numSubD; //Numero de diagonales por debajo de la principal.
    Vector A;
  protected:
    bool setSolver(EigenSolver *);
    bool procesa_comando(CmdStatus &status);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    BandArpackSOE(SoluMethod *, double shift = 0.0);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    virtual int addM(const Matrix &, const ID &, double fact = 1.0);
   
    virtual void zeroA(void);
    virtual void zeroM(void);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    virtual any_const_ptr GetProp(const std::string &cod) const;
    friend class BandArpackSolver;
  };
inline SystemOfEqn *BandArpackSOE::getCopy(void) const
  { return new BandArpackSOE(*this); }
} // end of XC namespace


#endif



