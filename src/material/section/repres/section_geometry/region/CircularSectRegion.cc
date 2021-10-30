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
//CircularSectRegion.cpp
// Written by Remo M. de Souza
// December 1998

#include <cmath>
#include <utility/matrix/Matrix.h>
#include <material/section/repres/section_geometry/region/CircularSectRegion.h>
#include <material/section/repres/cell/QuadCell.h>

#include "utility/utils/text/text_string.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"
#include "utility/geom/d2/AnnulusSector2d.h"
#include "utility/geom/d2/Grid2d.h"

const int I= 0, J=1, K= 2, L= 3; //Index of vertices.
const int Y= 0, Z=1; //Index of Y and Z components.

//! @brief Default constructor.
XC::CircularSectRegion::CircularSectRegion(Material *mat)
  : QuadCellRegion(mat), centerPosit(2),intRad(0.0), extRad(0.0), initAng(0.0), finalAng(2*M_PI)
  {}


XC::CircularSectRegion::CircularSectRegion(Material *mat, int numSubdivCircunf, int numSubdivRadial,
                     const XC::Vector &centerPosition, double internRadius, 
                     double externRadius, double initialAngle, double finalAngle)
  : QuadCellRegion(mat,numSubdivCircunf,numSubdivRadial),
    centerPosit(centerPosition), intRad(internRadius), extRad(externRadius), 
    initAng(initialAngle), finalAng(finalAngle)
  {}

void XC::CircularSectRegion::setCenterPosition(const Vector &centerPosition)
  { centerPosit= centerPosition; }

void XC::CircularSectRegion::setCenterPos(const Pos2d &p)
  {
    centerPosit(0)= p.x();
    centerPosit(1)= p.y();
  }

Pos2d XC::CircularSectRegion::getCenterPos(void) const
  { return Pos2d(centerPosit(0),centerPosit(1)); }

void XC::CircularSectRegion::setRadii(double internRadius, double externRadius)
  {
    intRad= internRadius;
    extRad= externRadius;
  }

void XC::CircularSectRegion::setAngles(double initialAngle, double finalAngle)
  {
    initAng= initialAngle;
    finalAng= finalAngle;
  }

void XC::CircularSectRegion::getRadii(double &internRadius, double &externRadius) const
  {
    internRadius= intRad;
    externRadius= extRad;
  }

void XC::CircularSectRegion::getAngles(double &initialAngle, double &finalAngle) const
  {
    initialAngle= initAng;
    finalAngle  = finalAng;
  }

const XC::Matrix &XC::CircularSectRegion::getVertCoords(void) const
  {
    static Matrix vertCoord(4,2);
    const double cosI= cos(initAng);
    const double cosF= cos(finalAng);
    const double sinI= sin(initAng);
    const double sinF= sin(finalAng);
    vertCoord(I,Y)= centerPosit(0) + intRad*cosI;
    vertCoord(I,Z)= centerPosit(1) + intRad*sinI;
    vertCoord(J,Y)= centerPosit(0) + extRad*cosI;
    vertCoord(J,Z)= centerPosit(1) + extRad*sinI;
    vertCoord(K,Y)= centerPosit(0) + extRad*cosF;
    vertCoord(K,Z)= centerPosit(1) + extRad*sinF;
    vertCoord(L,Y)= centerPosit(0) + intRad*cosF;
    vertCoord(L,Z)= centerPosit(1) + intRad*sinF;
    return vertCoord;
  }

double XC::CircularSectRegion::getMaxY(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }
double XC::CircularSectRegion::getMaxZ(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }
double XC::CircularSectRegion::getMinY(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }
double XC::CircularSectRegion::getMinZ(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }

const XC::Vector &XC::CircularSectRegion::getCenterPosition(void) const
  { return centerPosit; }

//! @brief Returns a polygon inscribed in the annulus sector.
Polygon2d XC::CircularSectRegion::getPolygon(void) const
  {
    Polygon2d retval;
    if(!isHollow())
      {
	const double angle= getIncludedAngle();
	if(abs(angle-2.0*M_PI)<1e-3)
          {
	    Circle2d circle(getCenterPos(),extRad);
	    retval= circle.getInscribedPolygon(nDivCirc()+1);
	  }
	else
          retval= getSector().getPolygon2d(nDivCirc());
      }
    else
      retval= getSector().getPolygon2d(nDivCirc());
    return retval;
  }

AnnulusSector2d &XC::CircularSectRegion::getSector(void) const
  {
    static AnnulusSector2d retval;
    Pos2d O(centerPosit(0),centerPosit(1));
    Circle2d c(O,extRad);
    CircularSector2d sc(c,initAng,finalAng);
    retval= AnnulusSector2d(sc,intRad);
    return retval;
  }

//! @brief Return the discretization grid.
const Grid2d &XC::CircularSectRegion::getMesh(void) const
  { return alloc(Grid2d(getSector().genMesh(nDivRad(),nDivCirc()))); }

const XC::VectorCells &XC::CircularSectRegion::getCells(void) const
  {
    if(nDivRad() > 0  && nDivCirc() > 0)
      {
        getMesh();
        int numCells= this->getNumCells();

        cells.resize(numCells);

        int k= 0;
        XC::Matrix cellVertCoord(4,2);
        for(int j= 1;j<nDivRad()+1;j++)
          {
            for(int i= 1;i<nDivCirc()+1;i++)
              {
                cellVertCoord= getCellVertCoords(i,j);   //centerPosit(0) + rad_j  * cosTh1;
                cells.put(k,QuadCell(cellVertCoord)); 
                //std::cerr << getClassName() << "::" << __FUNCTION__ << "\ncreating cells XC::Cell " << k << " :" << cells[k];
                k++; 
              }
           }
       }
     return cells;
  }

//! @brief Virtual constructor.
XC::SectRegion *XC::CircularSectRegion::getCopy(void) const
  { return new CircularSectRegion(*this); }
 
void XC::CircularSectRegion::Print(std::ostream &s, int flag) const
  {
    s << "\nQuadCellRegion Type: CircularSectRegion";
    //s << "\nMaterial Id: " << getMaterialID();
    s << "\nNumber of subdivisions in the radial direction: " << nDivRad();
    s << "\nNumber of subdivisions in the circunferential direction: " << nDivCirc();
    s << "\nCenter Position: " << centerPosit;
    s << "\nInternal Radius: " << intRad << "\tExternal Radius: " << extRad;
    s << "\nInitial Angle: " << initAng << "\tFinal Angle: " << finalAng;
  }

std::ostream &XC::operator<<(std::ostream &s, XC::CircularSectRegion &CircularSectRegion)
  {
    CircularSectRegion.Print(s);
    return s;
  }
