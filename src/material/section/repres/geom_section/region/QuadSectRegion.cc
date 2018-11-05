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
//QuadSectRegion.cpp                                                                        
                                                                        
// File: QuadSectRegion.C
// Written by Remo M. de Souza
// December 1998

#include <material/section/repres/geom_section/region/QuadSectRegion.h>
#include <material/section/repres/cell/QuadCell.h>

#include "xc_utils/src/utils/text/text_string.h"
#include "xc_utils/src/geom/d1/Segment2d.h"
#include "xc_utils/src/geom/d2/Grid2d.h"
#include "xc_utils/src/geom/d2/2d_polygons/Quadrilateral2d.h"
#include <utility/matrix/Matrix.h>

// L +-----------------------+ K
//   |                       |
//   |                       |
//   |                       |
// I +-----------------------+ J

const int I= 0, J=1, K= 2, L= 3;
const int Y= 0, Z=1;

XC::QuadSectRegion::QuadSectRegion(Material *mat)
  : QuadCellRegion(mat), vertCoord(4,2) {}


XC::QuadSectRegion::QuadSectRegion(Material *mat, int numSubdivIJ, int numSubdivJK,const XC::Matrix &vertexCoords)
  : QuadCellRegion(mat,numSubdivIJ,numSubdivJK), vertCoord(vertexCoords)
  {}
  
//! @brief Sets numbers of divisions to get a tile IJ side smaller than size.
int XC::QuadSectRegion::setTileSizeIJ(const double &size)
  {
    const double longIJ= Segment2d(getIVertex(),getJVertex()).getLength();
    const double longKL= Segment2d(getKVertex(),getLVertex()).getLength();
    setNDivIJ(ceil(std::max(longIJ,longKL)/size));
    return getNDivIJ();
  }

//! @brief Sets numbers of divisions to get a tile JK side smaller than size.
int XC::QuadSectRegion::setTileSizeJK(const double &size)
  {
    const double longJK= Segment2d(getJVertex(),getKVertex()).getLength();
    const double longIL= Segment2d(getIVertex(),getLVertex()).getLength();
    setNDivJK(ceil(std::max(longJK,longIL)/size));
    return getNDivJK();
  }

//! @brief Sets numbers of divisions to get tile sizes smaller than sizeIJ and sizeJK.
int XC::QuadSectRegion::setTileSize(const double &sizeIJ, const double &sizeJK)
  {
    const int ndIJ= setTileSizeIJ(sizeIJ);
    const int ndJK= setTileSizeJK(sizeJK);
    return ndIJ*ndJK;
  }

double XC::QuadSectRegion::getMaxY(void) const
  {
    double retval= vertCoord(I,Y);
    retval= std::max(retval,vertCoord(J,Y));
    retval= std::max(retval,vertCoord(K,Y));
    retval= std::max(retval,vertCoord(L,Y));
    return retval;
  }
double XC::QuadSectRegion::getMaxZ(void) const
  {
    double retval= vertCoord(I,Z);
    retval= std::max(retval,vertCoord(J,Z));
    retval= std::max(retval,vertCoord(K,Z));
    retval= std::max(retval,vertCoord(L,Z));
    return retval;
  }
double XC::QuadSectRegion::getMinY(void) const
  {
    double retval= vertCoord(I,Y);
    retval= std::min(retval,vertCoord(J,Y));
    retval= std::min(retval,vertCoord(K,Y));
    retval= std::min(retval,vertCoord(L,Y));
    return retval;
  }
double XC::QuadSectRegion::getMinZ(void) const
  {
    double retval= vertCoord(I,Z);
    retval= std::min(retval,vertCoord(J,Z));
    retval= std::min(retval,vertCoord(K,Z));
    retval= std::min(retval,vertCoord(L,Z));
    return retval;
  }


//! @brief Returns upper rigth corner.
Pos2d XC::QuadSectRegion::getPMax(void) const
  { return Pos2d(getMaxY(),getMaxZ()); }

//! @brief Sets upper rigth corner.
void XC::QuadSectRegion::setPMax(const Pos2d &p)
  {
    const double y= p.x();
    vertCoord(J,Y)= y;
    vertCoord(K,Y)= y;
    const double z= p.y();
    vertCoord(L,Z)= z;
    vertCoord(K,Z)= z;
  }

//! @brief Returns lower left corner.
Pos2d XC::QuadSectRegion::getPMin(void) const
  { return Pos2d(getMinY(),getMinZ()); }

//! @brief Sets upper rigth corner.
void XC::QuadSectRegion::setPMin(const Pos2d &p)
  {
    const double y= p.x();
    vertCoord(I,Y)= y;
    vertCoord(L,Y)= y;
    const double z= p.y();
    vertCoord(I,Z)= z;
    vertCoord(J,Z)= z;
  }

//! @brief Returns position of I vertex.
Pos2d XC::QuadSectRegion::getIVertex(void) const
  { return Pos2d(vertCoord(I,Y),vertCoord(I,Z)); }

//! @brief Returns position of J vertex.
Pos2d XC::QuadSectRegion::getJVertex(void) const
  { return Pos2d(vertCoord(J,Y),vertCoord(J,Z)); }

//! @brief Returns position of K vertex.
Pos2d XC::QuadSectRegion::getKVertex(void) const
  { return Pos2d(vertCoord(K,Y),vertCoord(K,Z)); }

//! @brief Returns position of L vertex.
Pos2d XC::QuadSectRegion::getLVertex(void) const
  { return Pos2d(vertCoord(L,Y),vertCoord(L,Z)); }

//! @brief Returns a quadrilateral object (geometry).
Quadrilateral2d XC::QuadSectRegion::getQuad(void) const
  { return Quadrilateral2d( getIVertex(),getJVertex(), getKVertex(),getLVertex()); }

void XC::QuadSectRegion::swap(void)
  {
    Matrix tmp(vertCoord);
    tmp(I,Y)= vertCoord(L,Y); tmp(I,Z)= vertCoord(L,Z);
    tmp(J,Y)= vertCoord(K,Y); tmp(J,Z)= vertCoord(K,Z);
    tmp(K,Y)= vertCoord(J,Y); tmp(K,Z)= vertCoord(J,Z);
    tmp(L,Y)= vertCoord(I,Y); tmp(L,Z)= vertCoord(I,Z);
    vertCoord= tmp;
  }

//! @brief Assigns vertex coordinates.
void XC::QuadSectRegion::setQuad(const Quadrilateral2d &quad)
  {
    Pos2d vI= quad.Vertice0(I); Pos2d vJ= quad.Vertice0(J);
    Pos2d vK= quad.Vertice0(K); Pos2d vL= quad.Vertice0(L);
    vertCoord(I,Y)= vI.x(); vertCoord(I,Z)= vI.y();
    vertCoord(J,Y)= vJ.x(); vertCoord(J,Z)= vJ.y();
    vertCoord(K,Y)= vK.x(); vertCoord(K,Z)= vK.y();
    vertCoord(L,Y)= vL.x(); vertCoord(L,Z)= vL.y();
  }

Polygon2d XC::QuadSectRegion::getPolygon(void) const
  { return getQuad(); }

void XC::QuadSectRegion::setVertCoords(const XC::Matrix &vertexCoords)
  {
    vertCoord = vertexCoords;
  }

const XC::Matrix &XC::QuadSectRegion::getVertCoords(void) const
  { return vertCoord; }

const Grid2d &XC::QuadSectRegion::getMesh(void) const
  { return alloc(Grid2d(getQuad().genBilinMesh(nDivIJ,nDivJK))); }

const XC::VectorCells &XC::QuadSectRegion::getCells(void) const
  {
    if(nDivIJ > 0  && nDivJK > 0)
      {
        const int numCells= this->getNumCells();

        cells.resize(numCells);

        const double deltaXi= 2.0 / nDivIJ;
        const double deltaEta= 2.0 / nDivJK;

        Matrix cellVertCoord(4,2);
        double xi, eta;
        Vector N(4);
        int k = 0;
        for(int j= 0;j<nDivJK;j++)
          for(int i= 0;i< nDivIJ;i++)
            {
              // compute natural coordinates
              cellVertCoord(I,Y) = -1.0 + deltaXi  * i;
              cellVertCoord(I,Z) = -1.0 + deltaEta * j;
              cellVertCoord(J,Y) = -1.0 + deltaXi  * (i+1);
              cellVertCoord(J,Z) = cellVertCoord(I,Z);
              cellVertCoord(K,Y) = cellVertCoord(J,Y);
              cellVertCoord(K,Z) = -1.0 + deltaEta * (j+1);
              cellVertCoord(L,Y) = cellVertCoord(I,Y);
              cellVertCoord(L,Z) = cellVertCoord(K,Z);

              // map to cartesian coordinates using bilinear
              // shape functions

              for(int r= 0;r<4;r++)
                {
                  xi= cellVertCoord(r,Y);
                  eta= cellVertCoord(r,Z);
 
                  N(0) = (1.0 - xi)*(1.0 - eta)/4.0;
                  N(1) = (1.0 + xi)*(1.0 - eta)/4.0;
                  N(2) = (1.0 + xi)*(1.0 + eta)/4.0;
                  N(3) = (1.0 - xi)*(1.0 + eta)/4.0;

                  cellVertCoord(r,Y) = 0.0;
                  cellVertCoord(r,Z) = 0.0;

                  for(int s=0;s<4;s++)
                    {
                      cellVertCoord(r,Y) += N(s) * vertCoord(s,Y);
                      cellVertCoord(r,Z) += N(s) * vertCoord(s,Z);
                    }
                }
              cells.put(k,QuadCell(cellVertCoord)); 
              //std::cerr << "\ncreating cells Cell " << k << " :" << cells[k];
              k++;
             }
      }
    return cells;
  }


XC::SectRegion *XC::QuadSectRegion::getCopy (void) const
  { return new QuadSectRegion(*this); }
 
void XC::QuadSectRegion::Print(std::ostream &s, int flag) const
  {
    s << "\nQuadCellRegion Type: QuadSectRegion";
    //s << "\nMaterial Id: " << getMaterialID();
    s << "\nNumber of subdivisions in the IJ direction: " << nDivIJ;
    s << "\nNumber of subdivisions in the JK direction: " << nDivJK;
    s << "\nVertex Coordinates: " << vertCoord;
  }


std::ostream &XC::operator<<(std::ostream &s, XC::QuadSectRegion &rg_scc_quad)
  {
    rg_scc_quad.Print(s);
    return s;
  }
