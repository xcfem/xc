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
                                                                        
#ifndef ElementEdge2dUniformLoad_h
#define ElementEdge2dUniformLoad_h

#include "ElementEdgeUniformLoad.h"

namespace XC {
class FVector;

//! @ingroup ElemLoads
//
//! @brief Carga uniforme sobre elementos barra de dos dimensiones.
class ElementEdge2dUniformLoad : public ElementEdgeUniformLoad
  {
  private:
    static Vector data;
  public:
    ElementEdge2dUniformLoad(int tag,const double &,const double &,const ID &,const ID &);
    ElementEdge2dUniformLoad(int tag);
    ElementEdge2dUniformLoad(void);

    int getType(void);
    inline const double &W2(void) const
      { return Trans; }
    inline const double &W1(void) const
      { return Axial; }

    size_t getDimVectorFuerza(void) const;
    size_t getDimVectorMomento(void) const;
    const Matrix &getLocalForces(void) const;
    const Matrix &getLocalMoments(void) const;

    void addReactionsInBasicSystem(const ID &,const Vector &,const double &,const double &,FVector &);
    void addFixedEndForcesInBasicSystem(const ID &,const Vector &,const double &,const double &,FVector &);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // fin namespace XC

#endif

