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
//MEDFieldInfo.h
//Información sobre las celdas de la malla (para exportar archivos a «salome»).

#ifndef MEDFIELDINFO_H
#define MEDFIELDINFO_H

#include "MEDGroupInfo.h"
#include "post_process/FieldInfo.h"

namespace XC {

class MEDGaussModel;
class GaussModel;

//! @ingroup MED
//
//! @brief Información sobre un campo definido sobre un
//! subconjunto de la malla.
class MEDFieldInfo: public MEDObject
  {
    const FieldInfo &field_info;
  protected:
    friend class MEDMeshing;
    MEDFieldInfo(const FieldInfo &,MEDGroupInfo *);

    const MEDGroupInfo &getGrupo(void) const;
    MEDMEM::GROUP *getGrupoMED(void) const;
    bool procesa_comando(CmdStatus &status);
  public:
    inline virtual ~MEDFieldInfo(void)
      {}
    inline const FieldInfo &getXCFieldInfo(void) const
      { return field_info; }
    inline const std::string *getComponentNamesPtr(void) const
      { return &(field_info.getComponentNames()[0]); }
    inline const std::string *getComponentDescriptionsPtr(void) const
      { return &(field_info.getComponentDescriptions()[0]); }
    inline const std::string *getComponentUnitsPtr(void) const
      { return &(field_info.getComponentUnits()[0]); }

    virtual void defineGaussModels(const Set &)= 0;

    virtual void write(const std::string &) const= 0;

    any_const_ptr GetProp(const std::string &) const;
  };
} // end of XC namespace
#endif
