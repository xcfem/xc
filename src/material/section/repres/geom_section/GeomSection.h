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
//GeomSection.h

#ifndef GeomSection_h 
#define GeomSection_h 

#include "material/section/repres/geom_section/region/ListRegiones.h"
#include "material/section/repres/geom_section/reinfLayer/ListReinfLayer.h"
#include <list>
#include "boost/lexical_cast.hpp"
#include "Eje.h"
#include "material/section/repres/SeccionInerte.h"

class Semiplano2d;

namespace XC {
class SisRefScc;
class Spot;
class Segment;
class RegionSecc;
class ReinfLayer;
class MaterialLoader;
class Material;
class Vector;
class Matrix;
class CrossSectionProperties3d;
class CrossSectionProperties2d;

//! @ingroup MATSCCRepres
//
//! @defgroup MATSCCRepresGeom Representación de la geometría de una sección.
//
//! @ingroup MATSCCRepresGeom
//
//! @brief Representación de la geometría de una sección.
class GeomSection: public SeccionInerte
  {
  public:

    typedef std::map<size_t,SisRefScc *> lst_sis_ref;//!< Contenedor para sistemas de referencia.
    typedef std::map<size_t,Spot *> lst_spots; //!< Contenedor para lista de puntos.
    typedef std::map<size_t,Eje *> lst_ejes; //!< Contenedor de líneas. 

  protected:
    MaterialLoader *material_loader; //!< Gestor de materiales (búsqueda,...).

    ListRegiones regiones; //!< Contenedor de regiones.
    ListReinfLayer capas_armado; //!< Contenedor de capas de armadura.

    lst_sis_ref sistemas_referencia; //!< Lista de sistemas de referencia.
    size_t tag_sis_ref; //!< Identificador por defecto para el próximo sistema de referencia.

    lst_spots spots; //!< Lista de puntos.
    size_t tag_spot; //!< Identificador por defecto para el próximo punto.
    lst_ejes ejes; //!< Lista de ejes.
    size_t tag_eje; //!< Identificador por defecto para el próximo eje.

  public:
    //Constructores
    GeomSection(MaterialLoader *ml);    
    inline virtual ~GeomSection(void) {}

    inline int getTagSisRef(void) const
      { return tag_sis_ref; }
    void setTagSisRef(int i)
      { tag_sis_ref= i; }
    SisRefScc *busca_sistema_referencia(const size_t &id);
    const SisRefScc *busca_sistema_referencia(const size_t &id) const;
    inline SisRefScc *sistema_referencia_actual(void)
      { return busca_sistema_referencia(tag_sis_ref); }
    const SisRefScc *sistema_referencia_actual(void) const
      { return busca_sistema_referencia(tag_sis_ref); }
    SisRefScc *creaSisRef(const std::string &); 

    // Section edition functions
    inline int getTagSpot(void) const
      { return tag_spot; }
    void setTagSpot(int i)
      { tag_spot= i; }
    Spot *busca_spot(const size_t &);
    const Spot *busca_spot(const size_t &) const;
    Spot *creaSpot(const Pos2d &);
    Spot *newSpot(const Pos2d &);

    inline int getTagEje(void) const
      { return tag_eje; }
    void setTagEje(int i)
      { tag_eje= i; }
    Eje *busca_eje(const size_t &);
    const Eje *busca_eje(const size_t &) const;
    template <class E>
    Eje *creaEje(void);
    Segment *newSegment(size_t, size_t);
    
    // Section inquiring functions
    double DistSpots(const size_t &i,const size_t &j) const;
    Poligono2d getContornoRegiones(void) const;
    Poligono2d getContornoZonaComprimida(const Semiplano2d &) const;
    double getLongCorte(const Recta2d &r) const;
    std::vector<double> getLongsCorte(const std::list<Recta2d> &lr) const;
    double getCantoMecanicoZonaComprimida(const Semiplano2d &) const;
    double getCantoMecanicoZonaTraccionada(const Semiplano2d &) const;
    double getCantoMecanico(const Recta2d &) const;
    double getAnchoMecanico(const Recta2d &) const;
    double getRecubrimiento(const Pos2d &) const;
    double getAnchoBielaComprimida(const Segmento2d &) const;

    //Acceso contenedores.
    inline const ListRegiones &getRegiones(void) const
      { return regiones; }
    inline ListRegiones &getRegiones(void)
      { return regiones; }
    inline const ListReinfLayer &getCapasArmadura(void) const
      { return capas_armado; }
    inline ListReinfLayer &getCapasArmadura(void)
      { return capas_armado; }
    GeomSection getGMRegiones(void) const;
    GeomSection getGMCapasArmado(void) const;
    GeomSection getSeccionFisurada(const Semiplano2d &) const;
    size_t getNumFiberData(void) const;

    double getAreaSeccBruta(void) const;
    Vector getCdgSeccBruta(void) const;
    double getIySeccBruta(void) const;
    double getIzSeccBruta(void) const;
    double getPyzSeccBruta(void) const;

    Vector getCdgSeccHomogeneizada(const double &E0) const;
    double getAreaSeccHomogeneizada(const double &E0) const;
    double getIySeccHomogeneizada(const double &E0) const;
    double getIzSeccHomogeneizada(const double &E0) const;
    double getPyzSeccHomogeneizada(const double &E0) const;

    void Print(std::ostream &s, int flag =0);
    friend std::ostream &operator<<(std::ostream &s, GeomSection &fiberSectionRepr);    
  };

//! @brief Crea un nuevo Eje.
template <class E>
Eje *XC::GeomSection::creaEje(void)
  {
    Eje *retval= busca_eje(tag_eje);
    if(!retval) //El Eje es nuevo.
      {
        retval= new E(this);
        if(retval)
          {
            retval->Nombre()= "l"+boost::lexical_cast<std::string>(tag_eje);
            ejes[tag_eje]= retval;
            tag_eje++;
	  }
        else
	  std::cerr << "No se pudo crear el eje de tag: "
                    << tag_eje << ".\n";
      }
    return retval;
  }

} // end of XC namespace


#endif

