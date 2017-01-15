//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//RgSccCirc.h
// Written by Remo M. de Souza
// December 1998

#ifndef RgSccCirc_h 
#define RgSccCirc_h 

#include <material/section/repres/geom_section/region/RgQuadCell.h>
#include <utility/matrix/Vector.h>

class SectorAnilloCircular2d;

namespace XC {
class Matrix;

//! @ingroup MATSCCRegiones
//
//! @brief Region en forma de sector de anillo empleada
//! para la discretización.
class RgSccCirc: public RgQuadCell
  {
  private:
    Vector centerPosit;
    double intRad, extRad;
    double initAng, finalAng;
  protected:

  public:
    RgSccCirc(Material *mat);
    RgSccCirc(Material *, int numSubdivCircunf, int numSubdivRadial,
              const Vector &centerPosition, double internRadius, 
              double externRadius, double initialAngle, double finalAngle);
    
    //edition functions
    void setCenterPosition(const Vector &);
    void setCenterPos(const Pos2d &);
    Pos2d getCenterPos(void) const;
    inline double getInteriorRadius(void) const
      { return intRad; }
    inline double getExteriorRadius(void) const
      { return extRad; }
    inline double getInitialAngle(void) const
      { return initAng; }
    inline double getFinalAngle(void) const
      { return finalAng; }
    void setInteriorRadius(const double &d)
      { intRad= d; }
    void setExteriorRadius(const double &d)
      { extRad= d; }
    void setInitialAngle(const double &d)
      { initAng= d; }
    void setFinalAngle(const double &d)
      { finalAng= d; }
    void setRadii(double internRadius, double externRadius);
    void setAngles(double initialAngle, double finalAngle);

    //inquiring functions
    const Rejilla2d &Malla(void) const;
    const VectorCells &getCells(void) const;
    RegionSecc *getCopy(void) const;

    void getRadii(double &internRadius, double &externRadius)  const;
    void getAngles(double &initialAngle, double &finalAngle)    const;
    const Vector &getCenterPosition(void) const;
    const Matrix &getVertCoords(void) const;

    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;

    Poligono2d getPoligono(void) const;
    SectorAnilloCircular2d &getSector(void) const;


    void Print(std::ostream &s, int flag =0) const;   
    friend std::ostream &operator<<(std::ostream &s, RgSccCirc &rg_scc_circ);    
  };
} // end of XC namespace


#endif

 
