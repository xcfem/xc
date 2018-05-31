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
//InteractionDiagram.cc

#include "InteractionDiagram.h"
#include "xc_utils/src/geom/d2/Triang3dMesh.h"
#include "xc_utils/src/geom/d2/Plane.h"
#include "xc_utils/src/geom/d2/Triangulo3d.h"
#include "xc_basic/src/util/mchne_eps.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "utility/matrix/Vector.h"

#include "material/section/fiber_section/FiberSectionBase.h"
#include "material/section/interaction_diagram/InteractionDiagramData.h"


//! @brier We classify the trihedron by its quadrant.
void XC::InteractionDiagram::classify_trihedron(const Trihedron &tdro)
  {
    for(int i= 0;i<8;i++)
      if(tdro.TocaCuadrante(i+1)) quadrant_trihedrons[i].insert(&tdro);
  }

//! @brier We classify the trihedrons by its quadrants.
void XC::InteractionDiagram::classify_trihedrons(void)
  {
    //Clasificamos los trihedrons por cuadrantes.
    for(XC::InteractionDiagram::const_iterator i= begin();i!=end();i++)
      classify_trihedron(*i);
  }

//! @brief Default constructor.
XC::InteractionDiagram::InteractionDiagram(void)
  : ClosedTriangleMesh() {}

XC::InteractionDiagram::InteractionDiagram(const Pos3d &org,const Triang3dMesh &mll)
  : ClosedTriangleMesh(org,mll)
  {
    classify_trihedrons();
  }

//! @brief Copy constructor.
XC::InteractionDiagram::InteractionDiagram(const InteractionDiagram &otro)
  : ClosedTriangleMesh(otro)
  {
    classify_trihedrons();
  }

//! @brief Assignment operator.
XC::InteractionDiagram &XC::InteractionDiagram::operator=(const InteractionDiagram &otro)
  {
    ClosedTriangleMesh::operator=(otro);
    classify_trihedrons();
    return *this;
  }

//! @brief Virtual constructor.
XC::InteractionDiagram *XC::InteractionDiagram::clon(void) const
  { return new InteractionDiagram(*this); }

//! @brief Search for the trihedron that contains the point being passed as parameter.
const Trihedron *XC::InteractionDiagram::findTrihedronPtr(const Pos3d &p) const
  {
    const Trihedron *retval= nullptr;
    if(trihedrons.empty())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; trihedron list empty."
                  << std::endl;
        return retval;
      }
    const int cuadrante= p.Cuadrante();
    const set_ptr_trihedrons &set_trihedrons= quadrant_trihedrons[cuadrante-1];
    for(set_ptr_trihedrons::const_iterator i= set_trihedrons.begin();i!=set_trihedrons.end();i++)
      if((*i)->In(p,tol))
        {
          retval= *i;
          break;
        }
    if(!retval) //Not found, so brute-force search.
      {
        for(XC::InteractionDiagram::const_iterator i= begin();i!=end();i++)
          {
            if((*i).In(p,tol))
              {
                retval= &(*i);
                break;
              }
          }
      }
    // if(!retval) //Not found, we try even harder.
    //   {
    // 	InteractionDiagram::const_iterator i= begin();
    //     const Trihedron *tr= &(*i);
    //     retval= tr;
    //     double distMin= tr->PseudoDist(p);
    //     double dist= distMin;
    //     i++;
    //     for(XC::InteractionDiagram::const_iterator i= begin();i!=end();i++)
    //       {
    //         tr= &(*i);
    //         dist= tr->PseudoDist(p);
    //         if(dist<distMin)
    //           {
    //             distMin= dist;
    //             retval= tr;
    //           }
    //       }
    //     if(distMin>tol)
    //       retval= nullptr;
    //   }
    if(!retval) //Not found, we search the one with the nearest axis.
      {
	InteractionDiagram::const_iterator i= begin();
        const Trihedron *tr= &(*i);
        SemiRecta3d rayo(tr->Cuspide(),p);
        Recta3d axis= tr->Axis();
        double angMin= angulo(axis,rayo);
        double angTmp= angMin;
        retval= tr;
        i++;
        for(;i!=end();i++)
          {
            tr= &(*i);
            rayo= SemiRecta3d(tr->Cuspide(),p);
            angTmp= angulo(tr->Axis(),rayo);
            if(angTmp<angMin)
              {
                angMin= angTmp;
                retval= tr;
              }
          }
      }
//     if(!retval) //Not found yet, we search a point in the same line.
//       {
//         const GEOM_FT denom= 100.0; //Abs(p.VectorPos())/100;
//         const Pos3d perturb= Pos3d(p.x()/denom,p.y()/denom,p.z()/denom); // + Vector3d(tol/1e2,tol/1e2,tol/1e2);
//         for(XC::InteractionDiagram::const_iterator i= begin();i!=end();i++)
//           {
//             if((*i).In(perturb,tol))
//               {
//                 retval= &(*i);
//                 break;
//               }
//           }
//       }
    return retval;
  }

//! @brief Return the intersection of the half-line thats links the
//! origin (0,0,0) and p and the interaction diagram.
GeomObj::list_Pos3d XC::InteractionDiagram::get_intersection(const Pos3d &p) const
  {
    GeomObj::list_Pos3d lst_intersec;
    const Pos3d O= Pos3d(0.0,0.0,0.0);
    //Search for the trihedron thant contains p.
    const Trihedron *i= findTrihedronPtr(p);
    if(!i)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; bounding trihedron for: "
                  << p << " not found. Quadrant: "
		  << p.Cuadrante() << std::endl;
      }
    else
      {
        Triangulo3d triang(i->Vertice(1),i->Vertice(2),i->Vertice(3));
        //Calculamos el plano que contiene al triangulo.
        const Plane plano= triang.GetPlano();
        SemiRecta3d Op(O,p);
        //Intersection of the half-line with the plane.
        lst_intersec= interseccion(plano,Op);
        if(lst_intersec.empty())
          {
	    Pos3d nearest= i->Vertice(1);
	    GEOM_FT dmin= Op.dist2(nearest);
	    GEOM_FT d= Op.dist2(i->Vertice(2));
	    if(d<dmin)
	      {
		dmin= d;
	        nearest= i->Vertice(2);
	      }
	    d= Op.dist2(i->Vertice(3));
	    if(d<dmin)
	      {
		dmin= d;
	        nearest= i->Vertice(3);
	      }
	    GEOM_FT sphereRadius= nearest.dist(O);
	    Pos3d pInt= Op.PtoParametricas(sphereRadius);
	    std::clog << getClassName() << "::" << __FUNCTION__
		      << "; WARNING: intersection for"
		      << " internal forces(N,My,Mz): " << p
		      << " not found, using approximation"
                      << " (sphere defined for the nearest point)."
		      << std::endl;
	    lst_intersec.push_back(pInt);
	  }
        if(lst_intersec.empty())
          {
            
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; doesn't intersect. " << std::endl
                      << " Triangle area: " << triang.getArea() << std::endl
                      << " vertex 1: " << i->Vertice(1)
                      << " vertex 2: " << i->Vertice(2)
                      << " vertex 3: " << i->Vertice(3) << std::endl
                      << " internal forces (N,My,Mz): " << p << std::endl;
            exit(-1);
          }
      }
    return lst_intersec;
  }

//! @brief Returns the intersection of the ray O->esf_d with the
//! interaction diagram.
Pos3d XC::InteractionDiagram::getIntersection(const Pos3d &esf_d) const
  {
    Pos3d retval;
    const GeomObj::list_Pos3d lst_intersec= get_intersection(esf_d);
    if(!lst_intersec.empty())
      retval= *(lst_intersec.begin());
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; intersection for triplet (N,My,Mz): "
		  << esf_d << " not found." << std::endl;
      }
    return retval;    
  }

//! @brief Return the capacity factor for the internal forces triplet being passed as parameters.
double XC::InteractionDiagram::getCapacityFactor(const Pos3d &esf_d) const
  {
    double retval= 1e6;
    assert(rMax>0.0);
    static const Pos3d O= Pos3d(0.0,0.0,0.0);
    const double d= dist(O,esf_d); //Distance from the internal force triplet to origin.
    const double umbralMax= rMax*10.0;
    if(d<mchne_eps_dbl) //Point is almost at origin.
      retval= 0.0;//Return maximum possible capacity factor.
    else if(d>umbralMax) //Point is far from diagram surface.
      retval= d/rMax;
    else
      {
        const double umbralMin= rMin/10.0;
        if(d<umbralMin) //Point is inside the diagram.
          retval= d/rMin;
        const GeomObj::list_Pos3d lst_intersec= get_intersection(esf_d);
        if(!lst_intersec.empty())
          {
            const Pos3d C= *(lst_intersec.begin());
            const Segmento3d sOC(O,C);
            retval= d/sOC.getLength();
          }
        else
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; intersection for the triplet: "
                      << esf_d << " not found." << std::endl;
            retval= d/rMin; //Intersection not found.
          }
      }
    return retval;
  }

XC::Vector XC::InteractionDiagram::getCapacityFactor(const GeomObj::list_Pos3d &lp) const
  {
    Vector retval(lp.size());
    int i= 0;
    for(GeomObj::list_Pos3d::const_iterator j= lp.begin();j!=lp.end(); j++, i++)
      retval[i]= getCapacityFactor(*j);
    return retval;
  }


void XC::InteractionDiagram::Print(std::ostream &os) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

void XC::InteractionDiagram::setPositionsMatrix(const Matrix &m)
  {
    ClosedTriangleMesh::setPositionsMatrix(m);
    classify_trihedrons();   
  }

XC::InteractionDiagram XC::calc_interaction_diagram(const FiberSectionBase &scc,const InteractionDiagramData &data= InteractionDiagramData())
  {
    InteractionDiagram retval;
    FiberSectionBase *tmp= dynamic_cast<FiberSectionBase *>(scc.getCopy());
    if(tmp)
      {
        retval= tmp->GetInteractionDiagram(data);
        delete tmp;
      }
    else
      std::cerr << __FUNCTION__
		<< "; can't get a copy of the section."
                << std::endl;
    return retval;
  }

