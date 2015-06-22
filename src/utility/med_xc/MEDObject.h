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
//MEDObject.h
//Envoltorio para objetos de MED (para exportar archivos a «salome»).

#ifndef MEDOBJECT_H
#define MEDOBJECT_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_utils/src/utils/ObjMap.h"

namespace XC {
//! @defgroup MED Interfaz con la biblioteca MED.
//
//! @ingroup MED
//
//!  @brief Clase base para objetos MED.
class MEDObject: public EntCmd
  {
  public:
    typedef ObjMap<MEDObject> contenedor;
    typedef contenedor::iterator iterator;
    typedef contenedor::const_iterator const_iterator;
  protected:
    bool procesa_llamada(const std::string &cmd,CmdStatus &status);
    bool procesa_comando(CmdStatus &status);
  public:
    MEDObject(void);
    any_const_ptr GetProp(const std::string &) const;
  };
} // fin namespace XC
#endif
