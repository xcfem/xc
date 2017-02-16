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
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d2/Triangulo3d.h"
#include "xc_basic/src/util/mchne_eps.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "utility/matrix/Vector.h"

#include "material/section/fiber_section/FiberSectionBase.h"
#include "material/section/interaction_diagram/InteractionDiagramData.h"



void XC::InteractionDiagram::clasifica_triedro(const Triedro3d &tdro)
  {
    //Clasificamos el triedro por cuadrantes.
    for(int i= 0;i<8;i++)
      if(tdro.TocaCuadrante(i+1)) triedros_cuadrante[i].insert(&tdro);
  }

void XC::InteractionDiagram::clasifica_triedros(void)
  {
    //Clasificamos los triedros por cuadrantes.
    for(XC::InteractionDiagram::const_iterator i= begin();i!=end();i++)
      clasifica_triedro(*i);
  }

//! @brief Default constructor.
XC::InteractionDiagram::InteractionDiagram(void)
  : ClosedTriangleMesh() {}

XC::InteractionDiagram::InteractionDiagram(const Pos3d &org,const Triang3dMesh &mll)
  : ClosedTriangleMesh(org,mll)
  {
    clasifica_triedros();
  }

//! @brief Constructor de copia.
XC::InteractionDiagram::InteractionDiagram(const InteractionDiagram &otro)
  : ClosedTriangleMesh(otro)
  {
    clasifica_triedros();
  }

//! @brief Operador de asignación.
XC::InteractionDiagram &XC::InteractionDiagram::operator=(const InteractionDiagram &otro)
  {
    ClosedTriangleMesh::operator=(otro);
    clasifica_triedros();
    return *this;
  }

//! @brief Virtual constructor.
XC::InteractionDiagram *XC::InteractionDiagram::clon(void) const
  { return new InteractionDiagram(*this); }

//! @brief Busca el triedro que contiene al punto being passed as parameter.
const Triedro3d *XC::InteractionDiagram::BuscaPtrTriedro(const Pos3d &p) const
  {
    const Triedro3d *retval= nullptr;
    if(triedros.empty())
      {
	std::cerr << "InteractionDiagram::BuscaPtrTriedro; la lista de triedros está vacía."
                  << std::endl;
        return retval;
      }
    const int cuadrante= p.Cuadrante();
    const set_ptr_triedros &set_triedros= triedros_cuadrante[cuadrante-1];
    for(set_ptr_triedros::const_iterator i= set_triedros.begin();i!=set_triedros.end();i++)
      if((*i)->In(p,tol))
        {
          retval= *i;
          break;
        }
    if(!retval) //No lo encuentra, lo intentamos por fuerza bruta.
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
    // if(!retval) //No lo encuentra, lo intentamos por fuerza más bruta.
    //   {
    // 	InteractionDiagram::const_iterator i= begin();
    //     const Triedro3d *tr= &(*i);
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
    if(!retval) //Sigue sin encontrarlo, buscamos aquel cuyo eje está más próximo.
      {
	InteractionDiagram::const_iterator i= begin();
        const Triedro3d *tr= &(*i);
        SemiRecta3d rayo(tr->Cuspide(),p);
        Recta3d eje= tr->Eje();
        double angMin= angulo(eje,rayo);
        double angTmp= angMin;
        retval= tr;
        i++;
        for(;i!=end();i++)
          {
            tr= &(*i);
            rayo= SemiRecta3d(tr->Cuspide(),p);
            angTmp= angulo(tr->Eje(),rayo);
            if(angTmp<angMin)
              {
                angMin= angTmp;
                retval= tr;
              }
          }
      }
//     if(!retval) //Si sigue sin encontrarlo buscamos un punto en la misma línea.
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

//! @brief Return the intersección de la semirrecta que une el origen (0,0,0) y el
//! el punto p con el diagrama de interacción.
GeomObj::list_Pos3d XC::InteractionDiagram::get_interseccion(const Pos3d &p) const
  {
    GeomObj::list_Pos3d lst_intersec;
    const Pos3d O= Pos3d(0.0,0.0,0.0);
    //Buscamos el triedro que contiene a p.
    const Triedro3d *i= BuscaPtrTriedro(p);
    if(!i)
      {
	std::cerr << "InteractionDiagram::get_interseccion: no se encontró un triedro que contuviera a:"
                  << p << " cuadrante: " << p.Cuadrante() << std::endl;
      }
    else
      {
        Triangulo3d triang(i->Vertice(1),i->Vertice(2),i->Vertice(3));
        //Calculamos el plano que contiene al triangulo.
        const Plano3d plano= triang.GetPlano();
        SemiRecta3d Op(O,p);
        //Calculamos la intersección entre la semirrecta y dicho plano.
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
	    std::clog << "WARNING Interaction diagram; intersection for"
		      << " internal forces(N,My,Mz): " << p
		      << " not found, using approximation"
                      << " (sphere defined for the nearest point)."
		      << std::endl;
	    lst_intersec.push_back(pInt);
	  }
        if(lst_intersec.empty())
          {
            
	    std::cerr << "Doesn't intersect. " << std::endl
                      << " Triangle area: " << triang.Area() << std::endl
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
    const GeomObj::list_Pos3d lst_intersec= get_interseccion(esf_d);
    if(!lst_intersec.empty())
      retval= *(lst_intersec.begin());
    else
      {
	std::cerr << nombre_clase() << "::" << __FUNCTION__ << "; intersection for triplet (N,My,Mz): " << esf_d << " not found." << std::endl;
      }
    return retval;    
  }

//! @brief Returns the factor de capacidad para la terna de esfuerzos que se pasan como parámetro.
double XC::InteractionDiagram::FactorCapacidad(const Pos3d &esf_d) const
  {
    double retval= 1e6;
    assert(rMax>0.0);
    static const Pos3d O= Pos3d(0.0,0.0,0.0);
    const double d= dist(O,esf_d); //Distancia desde la terna de esfuerzos al origen.
    const double umbralMax= rMax*10.0;
    if(d<mchne_eps_dbl) //Si el punto está muy cerca del origen.
      retval= 0.0;//Devolvemos el máximo factor de capacidad que puede presentarse.
    else if(d>umbralMax) //Point is far from diagram surface.
      retval= d/rMax;
    else
      {
        const double umbralMin= rMin/10.0;
        if(d<umbralMin) //Point is inside the diagram.
          retval= d/rMin;
        const GeomObj::list_Pos3d lst_intersec= get_interseccion(esf_d);
        if(!lst_intersec.empty())
          {
            const Pos3d C= *(lst_intersec.begin());
            const Segmento3d sOC(O,C);
            retval= d/sOC.Longitud();
          }
        else
          {
	    std::cerr << "InteractionDiagram::FactorCapacidad; no se encontró la intersección para la terna: "
                      << esf_d << std::endl;
            retval= d/rMin; //No ha encontrado la intersección.
          }
      }
    return retval;
  }

XC::Vector XC::InteractionDiagram::FactorCapacidad(const GeomObj::list_Pos3d &lp) const
  {
    Vector retval(lp.size());
    int i= 0;
    for(GeomObj::list_Pos3d::const_iterator j= lp.begin();j!=lp.end(); j++, i++)
      retval[i]= FactorCapacidad(*j);
    return retval;
  }


void XC::InteractionDiagram::Print(std::ostream &os) const
  {
    std::cerr << "InteractionDiagram::Print no implementada." << std::endl;
  }

void XC::InteractionDiagram::setMatrizPosiciones(const Matrix &m)
  {
    ClosedTriangleMesh::setMatrizPosiciones(m);
    clasifica_triedros();   
  }

XC::InteractionDiagram XC::calc_interaction_diagram(const FiberSectionBase &scc,const InteractionDiagramData &datos= InteractionDiagramData())
  {
    InteractionDiagram retval;
    FiberSectionBase *tmp= dynamic_cast<FiberSectionBase *>(scc.getCopy());
    if(tmp)
      {
        retval= tmp->GetInteractionDiagram(datos);
        delete tmp;
      }
    else
      std::cerr << "XC::calcInteractionDiagram, no se pudo obtener una copia de la sección."
                << std::endl;
    return retval;
  }

