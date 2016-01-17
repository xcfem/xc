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
//RgSccPoligono.cpp                                                                        

#include <material/section/repres/geom_section/region/RgSccPoligono.h>
#include <material/section/repres/cell/QuadCell.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_basic/src/texto/cadena_carac.h"
#include "xc_utils/src/geom/d2/Rejilla2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"
#include <utility/matrix/Matrix.h>

XC::RgSccPoligono::RgSccPoligono(Material *mat)
  : RgQuadCell(mat), plg() {}


XC::RgSccPoligono::RgSccPoligono(Material *mat, int numSubdivIJ, int numSubdivJK,const Poligono2d &p)
  : RgQuadCell(mat,numSubdivIJ,numSubdivJK), plg(p)
  {}  

double XC::RgSccPoligono::getMaxY(void) const
  { return plg.GetXMax(); }

double XC::RgSccPoligono::getMaxZ(void) const
  { return plg.GetYMax(); }

double XC::RgSccPoligono::getMinY(void) const
  { return plg.GetXMin(); }

double XC::RgSccPoligono::getMinZ(void) const
  { return plg.GetYMin(); }

Poligono2d XC::RgSccPoligono::getPoligono(void) const
  { return plg; }

const Rejilla2d &XC::RgSccPoligono::Malla(void) const
  {
    std::cerr << "RgSccPoligono::Malla no implementada." << std::endl;
    static Rejilla2d retval;
    return retval;
  }

const XC::VectorCells &XC::RgSccPoligono::getCells(void) const
  {
    std::cerr << "RgSccPoligono::getCells no implementada." << std::endl;
    return cells;
  }


XC::RegionSecc *XC::RgSccPoligono::getCopy (void) const
  { return new RgSccPoligono(*this); }
 
void XC::RgSccPoligono::Print(std::ostream &s, int flag) const
  {
    s << "\nRgQuadCell Type: RgSccPoligono";
    //s << "\nMaterial Id: " << getMaterialID();
    s << "\nNumber of subdivisions in the IJ direction: " << nDivIJ;
    s << "\nNumber of subdivisions in the JK direction: " << nDivJK;
    s << "\nVertex Polígono: " << plg;
  }


std::ostream &XC::operator<<(std::ostream &s, XC::RgSccPoligono &rg_scc_cuad)
  {
    rg_scc_cuad.Print(s);
    return s;
  }
