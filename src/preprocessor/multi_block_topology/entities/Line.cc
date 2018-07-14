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
//Line.cc

#include "Line.h"
#include "preprocessor/Preprocessor.h"
#include "Pnt.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Segment3d.h"
#include "xc_utils/src/geom/pos_vec/Pos3dArray.h"
#include "utility/matrix/util_matrix.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "med.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::Line::Line(Preprocessor *m,const size_t &nd)
  : LineBase(m,nd) {}

//! @brief Constructor.
XC::Line::Line(const std::string &nombre,Preprocessor *m,const size_t &nd)
  : LineBase(nombre,m,nd) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::Line::getCopy(void) const
  { return new Line(*this); }

//! @brief Return the line length.
double XC::Line::getLength(void) const
  {
    double retval= 0;
    if(check_points())
      retval= dist(p1->GetPos(),p2->GetPos());
    return retval;
  }

//! @brief Return the centroid of the line.
Pos3d XC::Line::getCentroid(void) const
  {
    Pos3d retval(0.0,0.0,0.0);
    if(check_points())
      retval= getLineSegment().getCenterOfMass();
    return retval;
  }

//! @brief Returns the parameter of the point in the line (distance to the line's first point measured over the line)
double XC::Line::getLambda(const Pos3d &p) const
  {
    double retval= 0;
    if(check_points())
      retval= getLineSegment().getLambda(p);
    return retval;
  }

//! @brief Returns the segment than links the line ends.
Segment3d XC::Line::getLineSegment(void) const
  {
    Segment3d retval;
    if(check_points())
      retval= Segment3d(p1->GetPos(),p2->GetPos());
    return retval;
  }

//! @brief Returns the squared distance to the
//! position being passed as parameter.
double XC::Line::getSquaredDistanceTo(const Pos3d &pt) const
  { return getLineSegment().dist2(pt); }

//! @brief Divides the line by the point being passed as parameter.
XC::Edge *XC::Line::split_at(Pnt *p,const double &lambda,const double &length)
  {
    MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
    Edge *tmp= nullptr;
    if(lambda<0)
      tmp= mbt.getLines().createLine(p,P1());
    else if(lambda>length)
      tmp= mbt.getLines().createLine(P2(),p);
    else
      {
        tmp= mbt.getLines().createLine(p,p2);
        SetVertices(p1,p);
      }
    Line *retval= dynamic_cast<Line *>(tmp);
    assert(retval);
    //Settint the number of divisions so
    //the element size remains almost constant.
    const double sz_elem= length/NDiv();
    SetNDiv(ceil(getLength()/sz_elem));
    retval->SetNDiv(ceil(retval->getLength()/sz_elem));
    //copying sets.
    std::set<SetBase *> sets= get_sets();
    retval->add_to_sets(sets);
    return retval;
  }

//! @brief Divides the line by the point being passed as parameter.
XC::Edge *XC::Line::splitAtPoint(Pnt *p)
  {
    Edge *retval= nullptr;
    if(p)
      {
        const Pos3d pN= p->GetPos();
        const Segment3d s= getLineSegment();
        const double lambda= s.getLambda(pN);
        const double l= s.getLength();
        retval= split_at(p,lambda,l);
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; pointer to point is null."
                  << std::endl;
      }
    return retval;
  }

//! @brief Divides the line by the point obtained by: p1+lambda*VDir().
XC::Edge *XC::Line::splitAtLambda(const double &lambda)
  {
    Edge *retval= nullptr;
    const Segment3d s= getLineSegment();
    const Pos3d pN= s.PtoParametricas(lambda);
    Pnt *p= create_point(pN);
    const double l= s.getLength();
    retval= split_at(p,lambda,l);
    return retval;
  }

//! @brief Divides the line by the point obtained by: p1+lambda*VDir().
XC::Edge *XC::Line::splitAtCooNatural(const double &chi)
  {
    Edge *retval= nullptr;
    const Segment3d s= getLineSegment();
    const Pos3d pN= s.PtoCooNatural(chi);
    Pnt *p= create_point(pN);
    retval= split_at(p,s.getParamCooNatural(chi),s.getLength());
    return retval;
  }

//! @brief Returns the line boundary.
BND3d XC::Line::Bnd(void) const
  {
    BND3d retval;
    if(p1) retval+= p1->GetPos();
    if(p2) retval+= p2->GetPos();
    return retval;
  }

//! @brief Returns ndiv+1 positions equally spaced along the line.
Pos3dArray XC::Line::get_positions(void) const
  {
    Pos3dArray retval;
    if(check_points())
      retval= Pos3dArray(p1->GetPos(),p2->GetPos(),NDiv());
    return retval;
  }

//! @brief Returns the vector that goes for back end to front end.
const XC::Vector &XC::Line::getVector(void) const
  {
    static XC::Vector retval(3);
    if(check_points())
      retval= (*p2)-(*p1);
    return retval;
  }

//! @brief Returns a unit vector in the tangent direction in the point
//! defined by s.
//! @param s: parameter that defines the point of tangency.
const XC::Vector &XC::Line::getTang(const double &s) const
  {
    static Vector retval(3);
    retval= getVector();
    retval.Normalize();
    return retval;
  }

//! @brief VTK interface.
int XC::Line::getVtkCellType(void) const
  { return VTK_LINE; }


//! @brief Salome MED interface.
int XC::Line::getMEDCellType(void) const
  { return MED_SEG2; }
