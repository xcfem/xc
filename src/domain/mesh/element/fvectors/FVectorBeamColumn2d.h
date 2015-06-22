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
//FVectorBeamColumn2d.h

#ifndef FVectorBeamColumn2d_h
#define FVectorBeamColumn2d_h

#include "FVectorData.h"

namespace XC {


//! \ingroup ElemFV
//
//! @brief Esfuerzos en un elemento de tipo barra 2D.
class FVectorBeamColumn2d: public FVectorData<3>
  {
  public:
    FVectorBeamColumn2d(void);
    FVectorBeamColumn2d(const FVectorBeamColumn2d &otro);
    explicit FVectorBeamColumn2d(const Vector &);
    FVectorBeamColumn2d &operator=(const FVectorBeamColumn2d &otro);

    void zero(void);
    FVectorBeamColumn2d &operator*=(const double &d);
    FVectorBeamColumn2d &operator+=(const FVectorBeamColumn2d &otro);
    FVectorBeamColumn2d &operator-=(const FVectorBeamColumn2d &otro);

    void Print(std::ostream &os) const;
  };
} // fin namespace XC

#endif


