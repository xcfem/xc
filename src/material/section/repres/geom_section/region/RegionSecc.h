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
//RegionSecc.h
// Written by Remo M. de Souza
// December 1998

#ifndef RegionSecc_h 
#define RegionSecc_h 


#include <iostream>
#include "material/section/repres/DiscretBase.h" 
#include <material/section/repres/cell/VectorCells.h>

class Poligono2d;
class Semiplano2d;
class Pos2d;

namespace XC {
class Cell;
class VectorCells;
class Vector;
class Matrix;
class RgSccPoligono;

//! @ingroup MATSCCRepresGeom
//
//! @defgroup MATSCCRegiones Regiones de la sección ocupadas por un material.
//
//! @ingroup MATSCCRegiones
//
//! @brief Región de una sección correspondiente a un material.
class RegionSecc: public DiscretBase
  {
  protected:
    mutable VectorCells cells;

  public:
    RegionSecc(Material *);
    virtual ~RegionSecc(void) {}

    // inquiring functions
    //! @brief Devuelve el número de celdas de la región.
    virtual int getNumCells(void) const= 0;
    //! @brief Devuelve el contenedor de celdas.
    virtual const VectorCells &getCells(void) const= 0;
    //! @brief Devuelve una copia de la region.
    virtual RegionSecc *getCopy(void) const= 0;

    virtual Poligono2d getPoligono(void) const;
    RgSccPoligono Interseccion(const Semiplano2d &sp) const;
    const Vector &Cdg(void) const;
    double Longitud(void) const;
    double Area(void) const;
    double Iy(void) const;
    double Iz(void) const;
    double Pyz(void) const;
    double Ix(void) const;
    double Theta_p(void) const;
    const Vector &DirEjeI_a(void) const;
    const Vector &DirEjeI_b(void) const;
    double getI1(void) const;
    double getI2(void) const;
    double getI(const unsigned short int &i,const unsigned short int &j) const;
    double getI(const unsigned short int i,const unsigned short int j,const Pos2d &o) const;
    double getI(const Pos2d &O,const Vector &e) const;
    double getIO(const Pos2d &o);
    Matrix &getI(void) const;
    Matrix &getI(const Pos2d &o) const;



    virtual void Print(std::ostream &s, int flag= 0) const =0;   
    friend std::ostream &operator<<(std::ostream &s, const RegionSecc &rg_scc);    
  };
} // end of XC namespace


#endif

