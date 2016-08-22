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
//FiberData.h
		    
#ifndef FiberData_h 
#define FiberData_h 

#include <vector>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <material/section/repres/section/contenedor_fibras.h>

namespace XC {

class Material;
class VectorCells;
class VectorReinfBar;
class ListRegiones;
class ListReinfLayer;
 class GeomSection;

//! @ingroup MATSCCRepresGeom
//
//! @brief Datos de las fibras.
class FiberData
  {
    std::vector<Material *> fibersMaterial; //!< Material pointers for cells and rebars.
    Matrix fibersPosition; //! Cell and rebar positions.
    Vector fibersArea; //! Cell and rebar areas.
    size_t put_cells(const size_t &,const VectorCells &,Material *);
    size_t put_reinf_bars(const size_t &,const VectorReinfBar &,Material *);

    inline Material *getPtrMaterial(const int &i) const
      { return fibersMaterial[i]; }
    inline const double &Position(const int &i,const int &j) const
      { return fibersPosition(i,j); }
    inline double &Position(const int &i,const int &j)
      { return fibersPosition(i,j); }
    inline const double &Area(const int &i) const
      { return fibersArea(i); }
    inline double &Area(const int &i)
      { return fibersArea(i); }

  public:
    FiberData(const int &sz);
    FiberData(const GeomSection &);

    inline void setPtrMaterial(const int &i,Material *mat)
      { fibersMaterial[i]= mat; }

    inline size_t size(void) const
      { return fibersArea.Size(); }

    size_t PutCells(const size_t &,const ListRegiones &);
    size_t PutCapasArmadura(const size_t &,const ListReinfLayer &);
    void getFibras2d(contenedor_fibras &) const;
    void getFibras3d(contenedor_fibras &) const;
  };

} // end of XC namespace
#endif

