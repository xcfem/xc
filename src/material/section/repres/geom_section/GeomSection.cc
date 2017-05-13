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
//GeomSection.cpp

#include <material/section/repres/geom_section/GeomSection.h>

#include <material/section/repres/geom_section/SisRefScc.h>
#include <material/section/repres/geom_section/SisRefSccCartesianas2d.h>
#include <material/section/repres/geom_section/Spot.h>
#include <material/section/repres/geom_section/Segment.h>

#include "preprocessor/loaders/MaterialLoader.h"
#include "material/section/repres/CrossSectionProperties3d.h"
#include "material/section/repres/CrossSectionProperties2d.h"

#include <material/section/repres/geom_section/region/RegionSecc.h>

#include "material/section/repres/geom_section/reinfLayer/ReinfLayer.h"

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"
#include "xc_utils/src/geom/d2/Semiplano2d.h"
#include "xc_utils/src/geom/d1/Segmento2d.h"

#include "boost/lexical_cast.hpp"

XC::GeomSection::GeomSection(MaterialLoader *ml)
  : SectionMassProperties(),material_loader(ml), regiones(ml), capas_armado(this,ml), tag_sis_ref(0),tag_spot(0) {}

//! @brief Returns a geometry that contains only the regions
//! defined in this object.
XC::GeomSection XC::GeomSection::getGMRegiones(void) const
  {
    GeomSection retval(material_loader);
    retval.regiones= regiones;
    return retval;
  }

//! @brief Returns a geometry that contains only the regions
//! defined in this object.
XC::GeomSection XC::GeomSection::getGMCapasArmado(void) const
  {
    GeomSection retval(material_loader);
    retval.capas_armado= capas_armado;
    return retval;
  }

//! @brief Return a section with only the compressed regions of the section.
XC::GeomSection XC::GeomSection::getCrackedSection(const Semiplano2d &sp_compresiones) const
  {
    GeomSection retval(getGMCapasArmado());
    retval.regiones= regiones.Interseccion(sp_compresiones);
    return retval;
  }

size_t XC::GeomSection::getNumFiberData(void) const
  { return (regiones.getNumCells() + capas_armado.getNumReinfBars()); }

//! @brief Returns a pointer to the reference system which identifier is being passed as parameter.
XC::SisRefScc *XC::GeomSection::get_reference_system(const size_t &id)
  {
    SisRefScc *retval= nullptr;
    if(id>0) //El 0 se reserva para el sistema de coordenadas universal.
      {
        lst_sis_ref::iterator i= reference_systems.find(id);
        if(i!= reference_systems.end()) //Reference system exists.
        retval= (*i).second;
      }
    return retval;
  }

//! @brief Returns a const pointer to the reference system which identifier is being passed as parameter.
const XC::SisRefScc *XC::GeomSection::get_reference_system(const size_t &id) const
  {
    SisRefScc *retval= nullptr;
    if(id>0) //El 0 se reserva para el sistema de coordenadas universal.
      {
        lst_sis_ref::const_iterator i= reference_systems.find(id);
        if(i!= reference_systems.end()) //Reference system exists.
          retval= (*i).second;
      }
    return retval;
  }

//! @brief Returns a pointer to the punto cuyo identificador being passed as parameter.
XC::Spot *XC::GeomSection::busca_spot(const size_t &id)
  {
    Spot *retval= nullptr;
    lst_spots::iterator i= spots.find(id);
    if(i!= spots.end()) //Point exists.
      retval= (*i).second;
    return retval;
  }

//! @brief Returns a pointer to the punto cuyo identificador being passed as parameter.
const XC::Spot *XC::GeomSection::busca_spot(const size_t &id) const
  {
    Spot *retval= nullptr;
    lst_spots::const_iterator i= spots.find(id);
    if(i!= spots.end()) //Point exists.
      retval= (*i).second;
    return retval;
  }

//! @brief Returns a pointer to the eje cuyo nombre being passed as parameter.
XC::Eje *XC::GeomSection::busca_eje(const size_t &id)
  {
    Eje *retval= nullptr;
    lst_ejes::iterator i= ejes.find(id);
    if(i!= ejes.end()) //Point exists.
      retval= (*i).second;
    return retval;
  }

//! @brief Returns a pointer to the eje cuyo nombre being passed as parameter.
const XC::Eje *XC::GeomSection::busca_eje(const size_t &id) const
  {
    const Eje *retval= nullptr;
    lst_ejes::const_iterator i= ejes.find(id);
    if(i!= ejes.end()) //Point exists..
      retval= (*i).second;
    return retval;
  }

//! @brief Creates a new reference system of the type being passed as parameter.
XC::SisRefScc *XC::GeomSection::creaSisRef(const std::string &tipo)
  {
    SisRefScc *retval= get_reference_system(tag_sis_ref);
    if(!retval) //New reference system.
      {
        if(tipo == "cartesianas")
          {
            retval= new SisRefSccCartesianas2d("r"+boost::lexical_cast<std::string>(tag_sis_ref),this);
            reference_systems[tag_sis_ref]= retval;
            tag_sis_ref++;
          }
        else
	  std::cerr << "XC::GeomSection::creaSisRef; type: '" << tipo
                    << "' unknown." << std::endl;
      }
    return retval;
  }

//! @brief Creates a new point.
XC::Spot *XC::GeomSection::creaSpot(const Pos2d &p)
  {
    Spot *retval= busca_spot(tag_spot);
    if(!retval) //New point.
      {
        retval= new Spot("p"+boost::lexical_cast<std::string>(tag_spot),this);
        spots[tag_spot]= retval;
        retval->setPos(p);
        tag_spot++;
      }
    return retval;
  }

//! @brief New spot.
XC::Spot *XC::GeomSection::newSpot(const Pos2d &p)
  {
    Pos2d trfP(p);
    if(tag_sis_ref != 0) //El sistema de coordenadas no es el global.
      {
        SisRefScc *sr= get_reference_system(tag_sis_ref);
        if(sr)
          trfP= sr->GetPosGlobal(p); //Pasa a coordenadas globales.
        else
	  std::cerr << "Reference system with identifier: " << tag_sis_ref << " not found.\n";
       }
    Spot *retval= creaSpot(trfP);
    return retval;
  }

//! @brief New segment.
XC::Segment *XC::GeomSection::newSegment(size_t p1,size_t p2)
  {
    Eje *e= creaEje<Segment>();
    Segment *s= dynamic_cast<Segment *>(e);
    if(s)
      s->setEndPoints(p1,p2);
    else
      std::cerr << nombre_clase() << "::" << __FUNCTION__ << "; can't assign endpoints."
                << std::endl;
    return s;
  }


//! @brief Return the distancia entre los puntos cuyos identificadores being passed as parameters.
double XC::GeomSection::DistSpots(const size_t &i,const size_t &j) const
  {
    double retval(-1.0);
    const Spot *pA= busca_spot(i);
    const Spot *pB= busca_spot(j);
    if(!pA)
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; point: " << i << " not found. " << std::endl;
    else if(!pB)
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; point: " << j << " not found. " << std::endl;
    else
      retval= pA->DistanciaA(pB->GetPos());
    return retval;
  }

//! @brief Return the contour of the regions.
Poligono2d XC::GeomSection::getRegionsContour(void) const
  {
    Poligono2d retval;
    const std::list<Poligono2d> tmp= regiones.getContours();
    if(!tmp.empty())
      {
        if(tmp.size()>1)
	  std::cerr << nombre_clase() << "::" << __FUNCTION__
		    << "; cross section is not a simply connected region."
                    << std::endl;
        retval= *tmp.begin();
      }
    return retval;
  }

//! @brief Returns the contour of the compressed part of the regions.
Poligono2d XC::GeomSection::getCompressedZoneContour(const Semiplano2d &sp_compresiones) const
  {
    Poligono2d retval;
    Poligono2d tmp= getRegionsContour();
    if(!tmp.empty())
      {
	std::list<Poligono2d> tmpList= tmp.Interseccion(sp_compresiones);
        if(tmpList.size()>1)
	  std::cerr << nombre_clase() << "::" << __FUNCTION__
		    << "; is not a simply connected region."
                    << std::endl;
        retval= *tmpList.begin();
      }
    return retval;
  }

//! @brief Returns the working cross-section lever arm from the position
//! of the half-plane being passed as parameter.
//! @param trazaPF: Intersection of the bending plane with the plane that
//! contains the cross section.
double XC::GeomSection::getLeverArm(const Recta2d &trazaPF) const
  {
    Poligono2d contour= getRegionsContour();
    Pos2d C= contour.Cdg();
    Semiplano2d sp(trazaPF.Perpendicular(C));
    const size_t num_vertices= contour.GetNumVertices();
    double d= 0.0,dpos= 0.0,dneg=0.0;    
    for(register size_t i=1;i<=num_vertices;i++)
      {
        d= sp.DistSigno(contour.Vertice(i));
        if(d<dneg) dneg= d;
        if(d>dpos) dpos= d;
      }
    assert(dneg<=0);
    assert(dpos>=0);
    return dpos-dneg;
  }

//! @brief Returns the section depth from the border of the half-plane
//! being passed as parameter to the most compressed fiber.
double XC::GeomSection::getCompressedZoneLeverArm(const Semiplano2d &sp_compresiones) const
  {
    Poligono2d contour= getRegionsContour();
    const size_t num_vertices= contour.GetNumVertices();
    double d= 0.0,dneg= 0.0;    
    for(register size_t i=1;i<=num_vertices;i++)
      {
        d= sp_compresiones.DistSigno(contour.Vertice(i));
        if(d<dneg) dneg= d;
      }
    assert(dneg<=0);
    return dneg;
  }

//! @brief Returns the section depth from the border of the half-plane
//! being passed as parameter to the most tensioned fiber.
double XC::GeomSection::getTensionedZoneLeverArm(const Semiplano2d &sp_compresiones) const
  {
    Poligono2d contour= getRegionsContour();
    const size_t num_vertices= contour.GetNumVertices();
    double d= 0.0,dpos=0.0;    
    for(register size_t i=1;i<=num_vertices;i++)
      {
        d= sp_compresiones.DistSigno(contour.Vertice(i));
        if(d>dpos) dpos= d;
      }
    assert(dpos>=0);
    return dpos;
  }

//! @brief Returns the lengths of the segments that results of
//! cutting the line being passed as parameter with the section
//! contour.
double XC::GeomSection::getLongCorte(const Recta2d &r) const
  {
    double retval= 0.0;
    Poligono2d contour= agrega_puntos_medios(getRegionsContour());
    if(contour.Overlap(r))
      retval= contour.Clip(r).Longitud();
    return retval;
  }

//! @brief Returns the lengths of the segments that results of
//! cutting the line being passed as parameter with the section
//! contour.
std::vector<double> XC::GeomSection::getLongsCorte(const std::list<Recta2d> &lr) const
  {
    const size_t sz= lr.size();
    std::vector<double> retval;
    if(sz>0)
      {
        Poligono2d contour= agrega_puntos_medios(getRegionsContour());
        int conta= 0;
        for(std::list<Recta2d>::const_iterator i= lr.begin();i!=lr.end();i++,conta++)
          {
            const Recta2d &r= *i;
            if(contour.Overlap(r))
              retval[conta]= contour.Clip(r).Longitud();
          }
      }
    return retval;
  }

//! @brief Returns the section width for the bending plane intersect
//! being passed as parameter.
double XC::GeomSection::getAnchoMecanico(const Recta2d &traza_plano_flexion) const
  {
    const Poligono2d contour= agrega_puntos_medios(getRegionsContour());
    const size_t num_vertices= contour.GetNumVertices();
    double d= 0.0,dmax= 0.0;
    Recta2d perp;
    Segmento2d ancho;
    for(register size_t i=1;i<=num_vertices;i++)
      {
        perp= traza_plano_flexion.Perpendicular(contour.Vertice(i));
        ancho= contour.Clip(perp);
        d= ancho.Longitud();
        if(d>dmax)
          dmax= d;
      }
    assert(dmax>=0);
    return dmax;
  }

//! @brief Returns the ancho «b0» de la biela comprimida
//! correspondiente al brazo mecánico being passed as parameter.
double XC::GeomSection::getAnchoBielaComprimida(const Segmento2d &brazo_mecanico) const
  {
    const Poligono2d contour= agrega_puntos_medios(getRegionsContour());
    const size_t num_vertices= contour.GetNumVertices();
    Recta2d perp= brazo_mecanico.Mediatriz();
    Segmento2d ancho= contour.Clip(perp);
    Pos2d p= punto_interseccion(ancho,brazo_mecanico);
    assert(p.exists());
    double b2= std::min(dist2(p,ancho.Origen()),dist2(p,ancho.Destino()));
    double bmin2= b2;
    bool intersecaBrazo= false;
    for(register size_t i=1;i<=num_vertices;i++)
      {
        perp= brazo_mecanico.Perpendicular(contour.Vertice(i));
        intersecaBrazo= brazo_mecanico.Interseca(perp);
        if(intersecaBrazo)
          {
            ancho= contour.Clip(perp);
            p= punto_interseccion(ancho,brazo_mecanico);
            if(p.exists())
              {
                b2= std::min(dist2(p,ancho.Origen()),dist2(p,ancho.Destino()));
                if(b2<bmin2)
                  bmin2= b2;
              }
          }
      }
    assert(bmin2>=0);
    return 2*sqrt(bmin2);
  }

//! @brief Returns the cover for the position being passed as parameter.
double XC::GeomSection::getRecubrimiento(const Pos2d &p) const
  {
    const double retval= -getRegionsContour().DistSigno(p);
    if(retval<0)
      std::clog << "Warning! position: " << p
                << " is outside the section." << std::endl;
    return retval;
  }

//! @brief Returns the homogenized area of the regions.
double XC::GeomSection::getAreaHomogenizedSection(const double &E0) const
  {
    double retval= 0.0;
    if(!regiones.empty())
      retval+= regiones.getAreaHomogenizedSection(E0);
    if(!capas_armado.empty())
      retval+= capas_armado.getAreaHomogenizedSection(E0);
    return retval;
  }

XC::Vector XC::GeomSection::getCdgHomogenizedSection(const double &E0) const
  {
    Vector retval(2);
    double weight= 0.0;
    double divisor= 0.0;
    weight= regiones.getAreaHomogenizedSection(E0);
    retval+= weight*regiones.getCdgHomogenizedSection(E0);
    divisor+= weight;
    weight= capas_armado.getAreaHomogenizedSection(E0);
    retval+= weight*capas_armado.getCdgHomogenizedSection(E0);
    divisor+= weight;
    retval/= divisor;
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respecto to the axis parallel to y passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::GeomSection::getIyHomogenizedSection(const double &E0) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgHomogenizedSection(E0)[1];

    if(!regiones.empty())
      {
        d= regiones.getCdgHomogenizedSection(E0)[1]-zCdg;
        retval+= regiones.getIyHomogenizedSection(E0)+regiones.getAreaHomogenizedSection(E0)*sqr(d);
      }
    if(!capas_armado.empty())
      {
        d= capas_armado.getCdgHomogenizedSection(E0)[1]-zCdg;
        retval+= capas_armado.getIyHomogenizedSection(E0)+capas_armado.getAreaHomogenizedSection(E0)*sqr(d);
      }
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respecto to the axis parallel to z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::GeomSection::getIzHomogenizedSection(const double &E0) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgHomogenizedSection(E0)[0];

    if(!regiones.empty())
      {
        d= regiones.getCdgHomogenizedSection(E0)[0]-yCdg;
        retval+= regiones.getIzHomogenizedSection(E0)+regiones.getAreaHomogenizedSection(E0)*sqr(d);
      }
    if(!capas_armado.empty())
      {
        d= capas_armado.getCdgHomogenizedSection(E0)[0]-yCdg;
        retval+= capas_armado.getIzHomogenizedSection(E0)+capas_armado.getAreaHomogenizedSection(E0)*sqr(d);
      }
    return retval;
  }

//! @brief Returns homogenized product of inertia of the cross-section with respecto to the axis parallel to y and z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::GeomSection::getPyzHomogenizedSection(const double &E0) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const double zCdg= getCdgHomogenizedSection(E0)[0];
    const double yCdg= getCdgHomogenizedSection(E0)[0];

    if(!regiones.empty())
      {
        d2= (regiones.getCdgHomogenizedSection(E0)[0]-yCdg)*(regiones.getCdgHomogenizedSection(E0)[1]-zCdg);
        retval+= regiones.getPyzHomogenizedSection(E0)+regiones.getAreaHomogenizedSection(E0)*d2;
      }
    if(!capas_armado.empty())
      {
        d2= (capas_armado.getCdgHomogenizedSection(E0)[0]-yCdg)*(capas_armado.getCdgHomogenizedSection(E0)[1]-zCdg);
        retval+= capas_armado.getPyzHomogenizedSection(E0)+capas_armado.getAreaHomogenizedSection(E0)*d2;
      }
    return retval;
  }

//! @brief Returns region's gross section area.
double XC::GeomSection::getAreaGrossSection(void) const
  {
    double retval= 0.0;
    if(!regiones.empty())
      retval+= regiones.getAreaGrossSection();
    // if(!capas_armado.empty())
    //   retval+= capas_armado.getAreaGrossSection();
    return retval;
  }

//! @brief Returns gross section centroid position.
XC::Vector XC::GeomSection::getCdgGrossSection(void) const
  { return regiones.getCdgGrossSection(); }

//! @brief Inertia of the gross section about an axis parallel to y through its centroid.
double XC::GeomSection::getIyGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgGrossSection()[1];

    if(!regiones.empty())
      {
        d= regiones.getCdgGrossSection()[1]-zCdg;
        retval+= regiones.getIyGrossSection()+regiones.getAreaGrossSection()*sqr(d);
      }
    // if(!capas_armado.empty())
    //   {
    //     d= capas_armado.getCdgGrossSection()[1]-zCdg;
    //     retval+= capas_armado.getIyGrossSection()+capas_armado.getAreaGrossSection()*sqr(d);
    //   }
    return retval;
  }

//! @brief Inertia of the gross section about an axis parallel to z through its centroid
double XC::GeomSection::getIzGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgGrossSection()[0];

    if(!regiones.empty())
      {
        d= regiones.getCdgGrossSection()[0]-yCdg;
        retval+= regiones.getIzGrossSection()+regiones.getAreaGrossSection()*sqr(d);
      }
    // if(!capas_armado.empty())
    //   {
    //     d= capas_armado.getCdgGrossSection()[0]-yCdg;
    //     retval+= capas_armado.getIzGrossSection()+capas_armado.getAreaGrossSection()*sqr(d);
    //   }
    return retval;
  }

//! @brief Product of inertia of the gross section about y and z axis through its centroid.
double XC::GeomSection::getPyzGrossSection(void) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const Vector posCdg= getCdgGrossSection();
    const double zCdg= posCdg[1];
    const double yCdg= posCdg[0];

    if(!regiones.empty())
      {
        d2= (regiones.getCdgGrossSection()[0]-yCdg)*(regiones.getCdgGrossSection()[1]-zCdg);
        retval+= regiones.getPyzGrossSection()+regiones.getAreaGrossSection()*d2;
      }
    // if(!capas_armado.empty())
    //   {
    //     d2= (capas_armado.getCdgGrossSection()[0]-yCdg)*(capas_armado.getCdgGrossSection()[1]-zCdg);
    //     retval+= capas_armado.getPyzGrossSection()+capas_armado.getAreaGrossSection()*d2;
    //   }
    return retval;
  }


//! @brief Printing.
void XC::GeomSection::Print(std::ostream &s, int flag)
  {
    s << "\nCurrent Número de regiones: "       << regiones.size();
    s << "\nCurrent Número de capas de refuerzo: " << capas_armado.size();
  }


std::ostream &XC::operator<<(std::ostream &s, XC::GeomSection &sectionRepr)
  {
    sectionRepr.Print(s);
    return s;
  }
