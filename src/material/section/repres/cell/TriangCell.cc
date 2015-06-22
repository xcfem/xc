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
//TriangCell.cpp

#include <material/section/repres/cell/TriangCell.h>

//! @brief Constructor
XC::TriangCell::TriangCell(void)
  : Cell(3) {}


//! @brief Constructor virtual.
XC::TriangCell::TriangCell(const XC::Matrix &vertexCoords)
  : Cell(vertexCoords) {}

//! @brief Constructor de copia.
XC::Cell *XC::TriangCell::getCopy(void) const
  { return new TriangCell(*this); }

//! @brief Devuelve el área de la celda.
double XC::TriangCell::getArea(void) const
  {
    const double x0= vertCoord(0,0);
    const double y0= vertCoord(0,1);
    const double x1= vertCoord(1,0);
    const double y1= vertCoord(1,1);
    const double x2= vertCoord(2,0);
    const double y2= vertCoord(2,1);

    return (((x2-x1)*(y0-y1) - (x0-x1)*(y2-y1)) / 2.0);
  }

//! @brief Imprime la celda.
void XC::TriangCell::Print(std::ostream &s, int flag) const
  {
    s << "\nCell Type: TriangCell";
    s << "\nVertex Coordinates: " << vertCoord;
  }



