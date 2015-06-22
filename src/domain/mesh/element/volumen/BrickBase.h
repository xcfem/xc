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
//BrickBase.h
                                                                        
#ifndef BrickBase_h
#define BrickBase_h

#include <domain/mesh/element/ElemWithMaterial.h>
#include "domain/mesh/element/physical_properties/NDMaterialPhysicalProperties.h"

namespace XC {
class NDMaterial;

//! \ingroup Elem
//
//! @defgroup ElemVol Elementos para el análisis de sólidos.
//!
//! \ingroup ElemVol
//
//! @brief Clase base para los hexaedros.
class BrickBase : public ElemWithMaterial<8,NDMaterialPhysicalProperties>
  {
  protected:
    TritrizPtrElem coloca_en_malla(const TritrizPtrNod &,dir_mallado dm) const;
  public:
    BrickBase(int classTag);
    BrickBase(int tag,int classTag,const NDMaterialPhysicalProperties &);
    BrickBase(int tag, int classTag,int nd1, int nd2, int nd3, int nd4,int nd5,int nd6,int nd7,int nd8, const NDMaterialPhysicalProperties &);
    size_t getDimension(void) const;
  };

} // fin namespace XC
#endif
