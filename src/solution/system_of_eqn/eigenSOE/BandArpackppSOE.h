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
//BandArpackppSOE.h

#ifndef BandArpackppSOE_h
#define BandArpackppSOE_h

#include <solution/system_of_eqn/eigenSOE/ArpackSOE.h>
#include "xc_basic/src/matrices/matdispZ.h"

typedef matdispZ<double> msp_double;

namespace XC {
class BandArpackppSolver;

//! @ingroup EigenSOE
//
//! @brief Sistema de ecuaciones correspondiente
//! a un problema de autovalores con matriz en banda
//! en cuya solución se empleará la biblioteca
//! ArPack++.
class BandArpackppSOE : public ArpackSOE
  {
  private:
    msp_double A;
    msp_double M;
    
    int addToMatrix(msp_double &,const Matrix &, const ID &,const double &);
  protected:
    bool setSolver(EigenSolver *);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    BandArpackppSOE(SoluMethod *,const double &shift = -1.0);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    virtual int addM(const Matrix &, const ID &, double fact = 1.0);    
   
    virtual void zeroA(void);
    virtual void zeroM(void);

    inline const msp_double &getA(void) const
      { return A; }
    inline const msp_double &getM(void) const
      { return M; }
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    virtual any_const_ptr GetProp(const std::string &cod) const;
    friend class BandArpackppSolver;
  };
inline SystemOfEqn *BandArpackppSOE::getCopy(void) const
  { return new BandArpackppSOE(*this); }
} // fin namespace XC

#endif
