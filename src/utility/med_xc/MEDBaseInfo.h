//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//MEDBaseInfo.h
//Envoltorio para el objeto MESHING de MED (para exportar archivos a «salome»).

#ifndef MEDBASEINFO_H
#define MEDBASEINFO_H

#include "MEDObject.h"
#include "MEDMapIndices.h"

namespace XC {

class Mesh;
//! @ingroup MED
//
//!  @brief Información sobre los vértices (nodos) de la malla.
class MEDBaseInfo: public MEDObject
  {
  protected:
    MEDMapIndices indices; //!< Correspondencia de índices XC y MED.
    void nuevo_tag(size_t tag);
    bool procesa_comando(CmdStatus &status);
  public:
    const MEDMapIndices &getMapIndices(void) const;
    virtual void clear(void);

    any_const_ptr GetProp(const std::string &) const;
  };
} // fin namespace XC
#endif
