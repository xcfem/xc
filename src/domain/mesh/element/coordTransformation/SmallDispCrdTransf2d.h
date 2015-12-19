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
//SmallDispCrdTransf2d.h

#ifndef SmallDispCrdTransf2d_h
#define SmallDispCrdTransf2d_h

#include "CrdTransf2d.h"

namespace XC {

//! \ingroup ElemCT
//
//! @brief Clase base para las transformaciones de coordenadas en 2d.
class SmallDispCrdTransf2d: public CrdTransf2d
  {
  protected:
    Vector &basic_to_local_resisting_force(const XC::Vector &pb, const XC::Vector &p0) const;
    const Vector &local_to_global_resisting_force(const Vector &pl) const;
    DbTagData &getDbTagData(void) const;
  public:
    SmallDispCrdTransf2d(int tag, int classTag);
    const Vector &getPointGlobalCoordFromLocal(const Vector &) const;
    const Vector &getPointGlobalDisplFromBasic(double xi, const Vector &) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
