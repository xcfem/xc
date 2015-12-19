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
//EntGeomSection.h

#ifndef ENTGEOMSECTION_H
#define ENTGEOMSECTION_H

#include "xc_utils/src/base/EntConNmb.h"

namespace XC {
class GeomSection;
class SisRefScc;
class Spot;
class Eje;

//! @ingroup MATSCCRepresGeom
//
//! @defgroup MATSCCEntGeom Entidades geométricas.
//
//! @ingroup MATSCCEntGeom
//
//! @brief Base de los objetos geométricos que se emplean
//! para representar la sección.
class EntGeomSection: public EntConNmb
  {
  private:
    GeomSection *sccGeom; //!< GeomSection.
  protected:
    const GeomSection *getGeomSection(void) const;
    GeomSection *getGeomSection(void);
    virtual bool procesa_comando(CmdStatus &status);
  public:
    EntGeomSection(const std::string &nmb="",GeomSection *sccGeom= NULL);
    EntGeomSection(const EntGeomSection &otro);
    EntGeomSection &operator=(const EntGeomSection &otro);
    inline const GeomSection *GetGeomSection(void) const
      { return getGeomSection(); }
    inline GeomSection *GetGeomSection(void)
      { return getGeomSection(); }
    int getVtkCellType(void) const;
    virtual int getMEDCellType(void) const;
    virtual any_const_ptr GetProp(const std::string &cod) const;

    SisRefScc *SisRefSccActual(void);    
    const SisRefScc *SisRefSccActual(void) const;

    Spot *BuscaSpot(const size_t &id_punto);    
    const Spot *BuscaSpot(const size_t &id_punto) const;
    Eje *BuscaEje(const size_t &id_edge);    
    const Eje *BuscaEje(const size_t &id_edge) const;
  };

} //end of XC namespace
#endif
