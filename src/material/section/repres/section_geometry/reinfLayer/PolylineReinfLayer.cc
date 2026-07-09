//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

#include <material/section/repres/section_geometry/reinfLayer/PolylineReinfLayer.h>
#include <material/section/repres/section_geometry/reinfBar/ReinfBar.h>

#include "utility/utils/text/text_string.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/pos_vec/VectorPos2d.h"


XC::PolylineReinfLayer::PolylineReinfLayer(ListReinfLayer *owr,Material *mat)
  : ReinfLayer(owr,mat, 0, 0.0, 0.0),
    positions() {}

XC::PolylineReinfLayer::PolylineReinfLayer(ListReinfLayer *owr,Material *mat,
					   int numReinfBars, 
					   double reinfBarArea,
					   const std::deque<Vector> &vertices)
  : ReinfLayer(owr,mat, numReinfBars, 0.0, reinfBarArea), positions(vertices) {}


void XC::PolylineReinfLayer::setPositions(const std::deque<Vector> &vertices)
  { positions= vertices; }

const std::deque<XC::Vector> &XC::PolylineReinfLayer::getPositions(void) const
  { return this->positions; }

void XC::PolylineReinfLayer::setPolyline(const Polyline2d &pol)
  {
    const size_t sz= pol.size();
    positions.resize(sz, Vector(2, 0.0));
    GeomObj::list_Pos2d lst= pol.getVertexList();
    GeomObj::list_Pos2d::const_iterator i= lst.begin();
    size_t count= 0;
    for(;i!=lst.end();i++, count++)
      {
	const Pos2d &p= *i;
	Vector &v= positions[count];
	v[0]= p.x();
	v[1]= p.y();
      }
  }

Polyline2d XC::PolylineReinfLayer::getPolyline(void) const
  {
    Polyline2d retval;
    for(std::deque<Vector>::const_iterator i= positions.begin();
	i!= positions.end();
	i++)
      {
	const Vector &v= *i;
	const Pos2d p(v[0], v[1]);
	retval.appendVertex(p);
      }
    return retval;
  }

//! @brief Return the maximum value of the y coordinate.
double XC::PolylineReinfLayer::getMaxY(void) const
  {
    std::deque<Vector>::const_iterator i= positions.begin();
    double retval= (*i)[0];
    i++;
    for(;i!= positions.end();
	i++)
      {
	const Vector &v= *i;
	retval= std::max(retval,v[0]);
      }
    return retval;
  }

//! @brief Return the maximum value of the z coordinate.
double XC::PolylineReinfLayer::getMaxZ(void) const
  {
    std::deque<Vector>::const_iterator i= positions.begin();
    double retval= (*i)[1];
    i++;
    for(;i!= positions.end();
	i++)
      {
	const Vector &v= *i;
	retval= std::max(retval,v[1]);
      }
    return retval;
  }

//! @brief Return the minimum value of the y coordinate.
double XC::PolylineReinfLayer::getMinY(void) const
  {
    std::deque<Vector>::const_iterator i= positions.begin();
    double retval= (*i)[0];
    i++;
    for(;i!= positions.end();
	i++)
      {
	const Vector &v= *i;
	retval= std::min(retval,v[0]);
      }
    return retval;
  }

//! @brief Return the minimum value of the z coordinate.
double XC::PolylineReinfLayer::getMinZ(void) const
  {
    std::deque<Vector>::const_iterator i= positions.begin();
    double retval= (*i)[1];
    i++;
    for(;i!= positions.end();
	i++)
      {
	const Vector &v= *i;
	retval= std::min(retval,v[1]);
      }
    return retval;
  }

//! @brief Returns an array of reinforcement bars.
const XC::VectorReinfBar &XC::PolylineReinfLayer::getReinfBars(void) const
  {
    const int num_bars= this->getNumReinfBars();
    Vector barPosit(2);
    reinfBars.resize(num_bars);
    const double area_barra= this->getReinfBarArea();
    const Polyline2d polyline= this->getPolyline();

    if(num_bars == 1)
      {
	const Pos2d centroid= polyline.getCenterOfMass();
        barPosit(0)= centroid.x();
	barPosit(1)= centroid.y();
    
        reinfBars.put(0,ReinfBar(area_barra,getMaterialPtr(),barPosit));
      }
    else if(num_bars > 1)
      {
	VectorPos2d points= polyline.Divide(num_bars-1);
        for(int i= 0;i<num_bars;i++)
          {
	    const Pos2d &p= points[i];
            barPosit(0)= p.x();
            barPosit(1)= p.y();
            reinfBars.put(i,ReinfBar(area_barra,getMaterialPtr(),barPosit));
          }
      }
    return reinfBars;
  }

double XC::PolylineReinfLayer::getLength(void) const
  { return this->getPolyline().getLength(); }

//! @brier Returns spacing between rebars.
double XC::PolylineReinfLayer::getSpacing(void) const
  {
    double retval= std::numeric_limits<double>::infinity();
    const int &num_bars= this->getNumReinfBars();
    if(num_bars>1)
      retval= this->getLength()/(num_bars-1);
    return retval;
  }

//! @brief Set spacing between rebars (and return the resulting number of
//! them).
int XC::PolylineReinfLayer::setSpacing(const double &spacing)
  {
    const double length= this->getLength();
    int num_bars= int(length/spacing)+1;
    this->setNumReinfBars(num_bars);
    return num_bars;
  }

XC::ReinfLayer *XC::PolylineReinfLayer::getCopy(void) const
  { return new PolylineReinfLayer(*this); }

//! @ brief Create a PolylineReinfLayer object whose rebars are placed between
//! those of the given layer.
XC::PolylineReinfLayer XC::PolylineReinfLayer::_reinforce_mid_points(const double &diameter) const
  {
    // Remove half the spacing from both extremities of the polyline.
    const Polyline2d originalPolyline= this->getPolyline();
    const int nRebars= this->getNumReinfBars();
    const double length= originalPolyline.getLength();
    const double spacing= length/(nRebars-1);
    const double halfSpacing= spacing/2.0;
    const Pos2d p1= originalPolyline.getPointAtLength(halfSpacing);
    const Polyline2d tmpPolyline= originalPolyline.getRightChunk(p1, 0.0);
    const Pos2d p2= originalPolyline.getPointAtLength(length-halfSpacing);
    const Polyline2d newPolyline= tmpPolyline.getLeftChunk(p2, 0.0);
    // Create the new reinforcement layer.
    PolylineReinfLayer tmp(*this);
    tmp.setNumReinfBars(nRebars-1);
    tmp.setReinfBarDiameter(diameter);
    tmp.setPolyline(newPolyline);
    return tmp;
  }

void XC::PolylineReinfLayer::Print(std::ostream &s, int flag) const
  {
     s << "\nReinforcing Layer type:  Polyline";
     ReinfLayer::Print(s,flag);
     s << "\nPositions: " << this->getPolyline();
  }

