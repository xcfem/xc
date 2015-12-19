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
//ListRegiones.h

#ifndef ListRegiones_h 
#define ListRegiones_h 

#include <list>
#include "material/section/repres/SeccionInerte.h"

class Pos2d;
class BND2d;
class Poligono2d;
class Semiplano2d;

namespace XC {

class RegionSecc;
class RgSccCuadrilatero;
class RgSccCirc;
class MaterialLoader;
class Vector;
class Matrix;

//! @ingroup MATSCCRegiones
//
//! @brief Lista de regiones.
class ListRegiones: protected std::list<RegionSecc *>, public SeccionInerte
  {
  public:
    typedef std::list<RegionSecc *> l_reg;
    typedef l_reg::reference reference;
    typedef l_reg::const_reference const_reference;
    typedef l_reg::iterator iterator;
    typedef l_reg::const_iterator const_iterator;
  private:
    void libera(void);
    void libera(const size_t i);
    void copia(const ListRegiones &otra);
  protected:

    MaterialLoader *material_loader; //!< Gestor de materiales (búsqueda,...).
  public:

    ListRegiones(MaterialLoader *ml);
    ListRegiones(const ListRegiones  &otro);
    ListRegiones &operator=(const ListRegiones  &otro);
    ~ListRegiones(void);

    XC::RegionSecc *push_back(const RegionSecc &reg);
    inline size_t size(void) const
      { return l_reg::size(); }
    inline bool empty(void) const
      { return l_reg::empty(); }

    void clear(void);

    const_iterator begin(void) const;
    const_iterator end(void) const;
    iterator begin(void);
    iterator end(void);

    RgSccCuadrilatero *newQuadRegion(const std::string &);
    RgSccCirc *newCircularRegion(const std::string &);

    size_t getNumCells(void) const;

    std::list<Poligono2d> getContornosRegiones(void) const;
    std::list<Poligono2d> getContorno(void) const;
    BND2d getBnd(void) const;
    ListRegiones Interseccion(const Semiplano2d &) const;

    void Cumplen(const std::string &,ListRegiones &,bool );

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

 std::ostream &operator<<(std::ostream &os,const ListRegiones &lr);

} // end of XC namespace


#endif
