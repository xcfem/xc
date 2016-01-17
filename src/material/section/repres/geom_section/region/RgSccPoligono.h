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
//RgSccPoligono.h

#ifndef RgSccPoligono_h 
#define RgSccPoligono_h 

#include <material/section/repres/geom_section/region/RgQuadCell.h>
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

class Poligono2d;

namespace XC {

//! @ingroup MATSCCRegiones
//
//! Region poligonal para la discretización.
class RgSccPoligono: public RgQuadCell
  {
  private:
    Poligono2d plg;
  protected:

  public:
    RgSccPoligono(Material *);
    RgSccPoligono(Material *, int numSubdivIJ, int numSubdivJK, const Poligono2d &);
    
    const Rejilla2d &Malla(void) const;
    const VectorCells &getCells(void) const;
    RegionSecc *getCopy(void) const;

    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;
    Poligono2d getPoligono(void) const;


    void Print(std::ostream &s, int flag =0) const;   
    friend std::ostream &operator<<(std::ostream &s, RgSccPoligono &rg_scc_plg);    
  };
} // end of XC namespace


#endif

 
