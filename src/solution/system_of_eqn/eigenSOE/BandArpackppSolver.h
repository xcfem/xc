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
//BandArpackppSolver.h

#ifndef BandArpackppSolver_h
#define BandArpackppSolver_h

#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include "utility/matrix/Vector.h"

namespace XC {
class BandArpackppSOE;

//! @ingroup LinearSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de sistemas de ecuaciones lineales con matriz en banda
//! empleando la biblioteca arcpack++
class BandArpackppSolver : public EigenSolver
  {
  private:
    BandArpackppSOE *theSOE;
    std::vector<double> autovalores;
    std::vector<Vector> autovectores;

    void setup_autos(const size_t &nmodos,const size_t &n);

    friend class EigenSOE;
    BandArpackppSolver(void);
    BandArpackppSolver(const int &nModes);
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:
  
    virtual int solve(void);
    virtual int solve(int nModes);
    virtual int setSize(void);
    const int &getSize(void) const;
    virtual bool setEigenSOE(BandArpackppSOE &theSOE);
  
    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;
  
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline EigenSolver *BandArpackppSolver::getCopy(void) const
   { return new BandArpackppSolver(*this); }
} // fin namespace XC

#endif
