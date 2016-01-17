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
//RgSccCuadrilatero.h
// Written by Remo M. de Souza
// December 1998

#ifndef RgSccCuadrilatero_h 
#define RgSccCuadrilatero_h 

#include <material/section/repres/geom_section/region/RgQuadCell.h>
#include "utility/matrix/Matrix.h"

class Cuadrilatero2d;

namespace XC {

// Orden de los vértices I->J->K->L.
//
//         nDivIJ= 4
// L +---+---+---+---+ K
//   |   |   |   |   |
//   +---+---+---+---+ nDivKL= 2
//   |   |   |   |   |
// I +---+---+---+---+ J

//! @ingroup MATSCCRegiones
//
//! Region cuadrilátera para la discretización.
class RgSccCuadrilatero: public RgQuadCell
  {
  private:
    Matrix vertCoord; //!< Coordenadas de los vértices.
  protected:

  public:
    RgSccCuadrilatero(Material *);
    RgSccCuadrilatero(Material *, int numSubdivIJ, int numSubdivJK, const Matrix &vertexCoords);

    // edition functions
    void setVertCoords(const Matrix &vertexCoords);

    
    const Rejilla2d &Malla(void) const;
    const VectorCells &getCells(void) const;
    RegionSecc *getCopy(void) const;

    const Matrix &getVertCoords(void) const;
    double getMaxY(void) const;
    double getMaxZ(void) const;
    Pos2d getPMax(void) const;
    void setPMax(const Pos2d &);
    double getMinY(void) const;
    double getMinZ(void) const;
    Pos2d getPMin(void) const;
    void setPMin(const Pos2d &);
    Cuadrilatero2d getCuadrilatero(void) const;
    void setQuad(const Cuadrilatero2d &);
    Poligono2d getPoligono(void) const;
    void swap(void);


    void Print(std::ostream &s, int flag =0) const;   
    friend std::ostream &operator<<(std::ostream &s, RgSccCuadrilatero &rg_scc_cuad);    
  };
} // end of XC namespace


#endif

 
