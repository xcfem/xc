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
//ListReinfLayer.h

#ifndef ListReinfLayer_h 
#define ListReinfLayer_h 

#include <list>
#include "material/section/repres/SeccionInerte.h"

class Pos2d;
class Poligono2d;
class Semiplano2d;

namespace XC {

class ReinfLayer;
class CircReinfLayer;
class StraightReinfLayer;
class BarraSuelta;
class MaterialLoader;
class Vector;
class Matrix;
class GeomSection;

//! @ingroup MATSCCArmaduras
//
//! @brief Contenedor (lista) de capas de armadura.
class ListReinfLayer: public std::list<ReinfLayer *>, public SeccionInerte
  {
  public:
    typedef std::list<ReinfLayer *> l_reg;
    typedef l_reg::reference reference;
    typedef l_reg::const_reference const_reference;
    typedef l_reg::iterator iterator;
    typedef l_reg::const_iterator const_iterator;
  private:
    void libera(void);
    void libera(const size_t i);
    void copia(const ListReinfLayer &otra);
  protected:

    MaterialLoader *material_loader; //!< Gestor de materiales (búsqueda,...).

    friend class GeomSection;
    ListReinfLayer(GeomSection *,MaterialLoader *ml);
    ListReinfLayer(const ListReinfLayer  &otro);
    ListReinfLayer &operator=(const ListReinfLayer  &otro);
  public:
   ~ListReinfLayer(void);

    ReinfLayer *push_back(const ReinfLayer &reg);

    void clear(void);

    const GeomSection *getGeomSection(void) const;
    double getRecubrimiento(void) const;

    StraightReinfLayer *newStraightReinfLayer(const std::string &);
    CircReinfLayer *newCircReinfLayer(const std::string &);
    BarraSuelta *newReinfBar(const std::string &);


    size_t getNumReinfBars(void) const;

    void Cumplen(const std::string &,ListReinfLayer &,bool );
    void getBarrasIn(const Poligono2d &,ListReinfLayer &,bool );
    void getBarrasIn(const Semiplano2d &,ListReinfLayer &,bool );

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

    void Print(std::ostream &s) const;
  };

} // end of XC namespace


#endif
