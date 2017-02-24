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
//InteractionDiagram2d.cc

#include "InteractionDiagram2d.h"
#include "xc_utils/src/geom/d1/Segmento2d.h"
#include "utility/matrix/Vector.h"

#include "material/section/fiber_section/FiberSectionBase.h"
#include "material/section/interaction_diagram/InteractionDiagramData.h"


inline double angle(const Pos2d &p)
  {
    return atan2(p.y(),p.x());
  }

inline bool comp(const Pos2d &a, const Pos2d &b)
  { return (angle(a) < angle(b)); }

GeomObj::list_Pos2d sort_points(const GeomObj::list_Pos2d &pts)
  {
    GeomObj::list_Pos2d tmp(pts);
    std::sort(tmp.begin(),tmp.end(),comp);
    return tmp;
  }

//! @brief Default constructor.
XC::InteractionDiagram2d::InteractionDiagram2d(void)
  : Poligono2d() {}

XC::InteractionDiagram2d::InteractionDiagram2d(const Poligono2d &pts)
  : Poligono2d(pts) //sort_points(pts))
  { }

//! @brief Virtual constructor.
XC::InteractionDiagram2d *XC::InteractionDiagram2d::clon(void) const
  { return new InteractionDiagram2d(*this); }

//! @brief Return the intersección de la semirrecta que une el origen (0,0,0) y el
//! el punto p con el diagrama de interacción.
Pos2d XC::InteractionDiagram2d::get_interseccion(const Pos2d &p) const
  {
    const Pos2d O= Pos2d(0.0,0.0);
    //Buscamos el triedro que contiene a p.
    SemiRecta2d Op(O,p);
    const Segmento2d sg= Clip(Op);
    const Pos2d p1= sg.Origen();
    const Pos2d p2= sg.Destino();
    if(p1!=O)
      return p1;
    else
      return p2;
  }

//! @brief Convierte el diagrama en un rombo con vértices en los
//! extremos del diagrama de interacción.
void XC::InteractionDiagram2d::Simplify(void)
  {
    const double xMax= GetMax(1);
    const double xMin= GetMin(1);
    const double x0= (xMax+xMin)/2.0;
    const double yMax= GetMax(2);
    const double yMin= GetMin(2);
    const double y0= (yMax+yMin)/2.0;
    const Pos2d p1= Pos2d(x0,yMin);
    const Pos2d p2= Pos2d(xMax,y0);
    const Pos2d p3= Pos2d(x0,yMax);
    const Pos2d p4= Pos2d(xMin,y0);
    erase();
    push_back(p1);
    push_back(p2);
    push_back(p3);
    push_back(p4);
  }

//! @brief Returns the intersection of the ray O->esf_d with the
//! interaction diagram.
Pos2d XC::InteractionDiagram2d::getIntersection(const Pos2d &esf_d) const
  { return get_interseccion(esf_d); }

//! @brief Returns the factor de capacidad para la terna de esfuerzos que se pasan como parámetro.
double XC::InteractionDiagram2d::FactorCapacidad(const Pos2d &esf_d) const
  {
    double retval= 1e6;
    static const Pos2d O= Pos2d(0.0,0.0);
    const double d= dist(O,esf_d); //Distancia desde la terna de esfuerzos al origen.
    if(d<mchne_eps_dbl) //Si el punto está muy cerca del origen.
      retval= 0.0;//Devolvemos el máximo factor de capacidad que puede presentarse.
    else
      {
        const Pos2d C= get_interseccion(esf_d);
        const Segmento2d sOC(O,C);
        retval= d/sOC.Longitud();
      }
    return retval;
  }

XC::Vector XC::InteractionDiagram2d::FactorCapacidad(const GeomObj::list_Pos2d &lp) const
  {
    Vector retval(lp.size());
    int i= 0;
    for(GeomObj::list_Pos2d::const_iterator j= lp.begin();j!=lp.end(); j++, i++)
      retval[i]= FactorCapacidad(*j);
    return retval;
  }


void XC::InteractionDiagram2d::Print(std::ostream &os) const
  {
    std::cerr << "InteractionDiagram2d::Print not implemented." << std::endl;
  }

XC::InteractionDiagram2d XC::calcPlaneInteractionDiagram(const FiberSectionBase &scc,const InteractionDiagramData &data, const double &theta)
  {
    InteractionDiagram2d retval;
    FiberSectionBase *tmp= dynamic_cast<FiberSectionBase *>(scc.getCopy());
    if(tmp)
      {
        retval= tmp->GetInteractionDiagramForPlane(data,theta);
        delete tmp;
      }
    else
      std::cerr << "XC::calcPlaneInteractionDiagram, can't get a copy of the section."
                << std::endl;
    return retval;
  }

XC::InteractionDiagram2d XC::calcNMyInteractionDiagram(const FiberSectionBase &scc,const InteractionDiagramData &data= InteractionDiagramData())
  {
    InteractionDiagram2d retval;
    FiberSectionBase *tmp= dynamic_cast<FiberSectionBase *>(scc.getCopy());
    if(tmp)
      {
        retval= tmp->GetNMyInteractionDiagram(data);
        delete tmp;
      }
    else
      std::cerr << "XC::calcNMyInteractionDiagram, can't get a copy of the section."
                << std::endl;
    return retval;
  }

XC::InteractionDiagram2d XC::calcNMzInteractionDiagram(const FiberSectionBase &scc,const InteractionDiagramData &data= InteractionDiagramData())
  {
    InteractionDiagram2d retval;
    FiberSectionBase *tmp= dynamic_cast<FiberSectionBase *>(scc.getCopy());
    if(tmp)
      {
        retval= tmp->GetNMzInteractionDiagram(data);
        delete tmp;
      }
    else
      std::cerr << "XC::calcNMzInteractionDiagram, can't get a copy of the section."
                << std::endl;
    return retval;
  }

