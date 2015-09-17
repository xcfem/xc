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
//GeomSection.cpp

#include <material/section/repres/geom_section/GeomSection.h>

#include <material/section/repres/geom_section/SisRefScc.h>
#include <material/section/repres/geom_section/SisRefSccCartesianas2d.h>
#include <material/section/repres/geom_section/Spot.h>
#include <material/section/repres/geom_section/Segment.h>

#include "modelador/loaders/MaterialLoader.h"
#include "material/section/repres/ConstantesSecc3d.h"
#include "material/section/repres/ConstantesSecc2d.h"

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

#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"

#include "boost/lexical_cast.hpp"
#include "xc_utils/src/nucleo/InterpreteRPN.h"

XC::GeomSection::GeomSection(MaterialLoader *ml)
  : SeccionInerte(),material_loader(ml), regiones(ml), capas_armado(this,ml), tag_sis_ref(0),tag_spot(0) {}

//! @brief Devuelve una geometría que sólo contiene las regiones
//! definidas en ésta.
XC::GeomSection XC::GeomSection::getGMRegiones(void) const
  {
    GeomSection retval(material_loader);
    retval.regiones= regiones;
    return retval;
  }

//! @brief Devuelve una geometría que sólo contiene las capas de
//! armado definidas en ésta.
XC::GeomSection XC::GeomSection::getGMCapasArmado(void) const
  {
    GeomSection retval(material_loader);
    retval.capas_armado= capas_armado;
    return retval;
  }

//! @brief Devuelve la sección que tiene la misma armadura que ésta
//! pero sólo las regiones comprimidas.
XC::GeomSection XC::GeomSection::getSeccionFisurada(const Semiplano2d &sp_compresiones) const
  {
    GeomSection retval(getGMCapasArmado());
    retval.regiones= regiones.Interseccion(sp_compresiones);
    return retval;
  }

size_t XC::GeomSection::getNumFiberData(void) const
  { return (regiones.getNumCells() + capas_armado.getNumReinfBars()); }

//! @brief Devuelve un puntero al sistema de referencia cuyo identificador se pasa como parámetro.
XC::SisRefScc *XC::GeomSection::busca_sistema_referencia(const size_t &id)
  {
    SisRefScc *retval= nullptr;
    if(id>0) //El 0 se reserva para el sistema de coordenadas universal.
      {
        lst_sis_ref::iterator i= sistemas_referencia.find(id);
        if(i!= sistemas_referencia.end()) //Reference system exists.
        retval= (*i).second;
      }
    return retval;
  }

//! @brief Devuelve un puntero al sistema de referencia cuyo identificador se pasa como parámetro.
const XC::SisRefScc *XC::GeomSection::busca_sistema_referencia(const size_t &id) const
  {
    SisRefScc *retval= nullptr;
    if(id>0) //El 0 se reserva para el sistema de coordenadas universal.
      {
        lst_sis_ref::const_iterator i= sistemas_referencia.find(id);
        if(i!= sistemas_referencia.end()) //Reference system exists.
          retval= (*i).second;
      }
    return retval;
  }

//! @brief Devuelve un puntero al punto cuyo identificador se pasa como parámetro.
XC::Spot *XC::GeomSection::busca_spot(const size_t &id)
  {
    Spot *retval= nullptr;
    lst_spots::iterator i= spots.find(id);
    if(i!= spots.end()) //Point exists.
      retval= (*i).second;
    return retval;
  }

//! @brief Devuelve un puntero al punto cuyo identificador se pasa como parámetro.
const XC::Spot *XC::GeomSection::busca_spot(const size_t &id) const
  {
    Spot *retval= nullptr;
    lst_spots::const_iterator i= spots.find(id);
    if(i!= spots.end()) //Point exists.
      retval= (*i).second;
    return retval;
  }

//! @brief Devuelve un puntero al eje cuyo nombre se pasa como parámetro.
XC::Eje *XC::GeomSection::busca_eje(const size_t &id)
  {
    Eje *retval= nullptr;
    lst_ejes::iterator i= ejes.find(id);
    if(i!= ejes.end()) //Point exists..
      retval= (*i).second;
    return retval;
  }

//! @brief Devuelve un puntero al eje cuyo nombre se pasa como parámetro.
const XC::Eje *XC::GeomSection::busca_eje(const size_t &id) const
  {
    const Eje *retval= nullptr;
    lst_ejes::const_iterator i= ejes.find(id);
    if(i!= ejes.end()) //Point exists..
      retval= (*i).second;
    return retval;
  }

//! @brief Crea un nuevo sistema de referencia.
XC::SisRefScc *XC::GeomSection::creaSisRef(const std::string &tipo)
  {
    SisRefScc *retval= busca_sistema_referencia(tag_sis_ref);
    if(!retval) //El sistema de referencia es nuevo.
      {
        if(tipo == "cartesianas")
          {
            retval= new SisRefSccCartesianas2d("r"+boost::lexical_cast<std::string>(tag_sis_ref),this);
            sistemas_referencia[tag_sis_ref]= retval;
            tag_sis_ref++;
          }
        else
	  std::cerr << "XC::GeomSection::creaSisRef; el tipo de sistema de referencia: '" << tipo
                    << "' es desconocido." << std::endl;
      }
    return retval;
  }

//! @brief Crea un nuevo punto.
XC::Spot *XC::GeomSection::creaSpot(const Pos2d &p)
  {
    Spot *retval= busca_spot(tag_spot);
    if(!retval) //El punto es nuevo.
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
        SisRefScc *sr= busca_sistema_referencia(tag_sis_ref);
        if(sr)
          trfP= sr->GetPosGlobal(p); //Pasa a coordenadas globales.
        else
	  std::cerr << "No se encontró el sistema de referencia de índice: " << tag_sis_ref << ".\n";
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
      std::cerr << "GeomSection::newSegment; can't assign endpoints."
                << std::endl;
    return s;
  }


//! @brief Devuelve la distancia entre los puntos cuyos identificadores se pasan como parámetro.
double XC::GeomSection::DistSpots(const size_t &i,const size_t &j) const
  {
    double retval(-1.0);
    const Spot *pA= busca_spot(i);
    const Spot *pB= busca_spot(j);
    if(!pA)
      std::cerr << "XC::GeomSection::DistSpots; no se encontró el punto: " << i << std::endl;
    else if(!pB)
      std::cerr << "XC::GeomSection::DistSpots; no se encontró el punto: " << j << std::endl;
    else
      retval= pA->DistanciaA(pB->GetPos());
    return retval;
  }

//! @brief Devuelve el contorno de las regiones definidas.
Poligono2d XC::GeomSection::getContornoRegiones(void) const
  {
    Poligono2d retval;
    const std::list<Poligono2d> tmp= regiones.getContorno();
    if(!tmp.empty())
      {
        if(tmp.size()>1)
	  std::cerr << "GeomSection::getContornoRegiones; la sección no es simplemente conexa."
                    << std::endl;
        retval= *tmp.begin();
      }
    return retval;
  }

//! @brief Devuelve el contorno de la parte comprimida de las regiones definidas.
Poligono2d XC::GeomSection::getContornoZonaComprimida(const Semiplano2d &sp_compresiones) const
  {
    Poligono2d retval;
    Poligono2d tmp= getContornoRegiones();
    if(!tmp.empty())
      {
	std::list<Poligono2d> tmpList= tmp.Interseccion(sp_compresiones);
        if(tmpList.size()>1)
	  std::cerr << "GeomSection::getContornoZonaComprimida; la sección no es simplemente conexa."
                    << std::endl;
        retval= *tmpList.begin();
      }
    return retval;
  }

//! @brief Devuelve el canto con el que trabaja la sección a partir de la posición
//! del semiplano que se pasa como parámetro.
//! @param trazaPF: Intersección del plano de flexión con el plano de la sección.
double XC::GeomSection::getCantoMecanico(const Recta2d &trazaPF) const
  {
    Poligono2d contorno= getContornoRegiones();
    Pos2d C= contorno.Cdg();
    Semiplano2d sp(trazaPF.Perpendicular(C));
    const size_t num_vertices= contorno.GetNumVertices();
    double d= 0.0,dpos= 0.0,dneg=0.0;    
    for(register size_t i=1;i<=num_vertices;i++)
      {
        d= sp.DistSigno(contorno.Vertice(i));
        if(d<dneg) dneg= d;
        if(d>dpos) dpos= d;
      }
    assert(dneg<=0);
    assert(dpos>=0);
    return dpos-dneg;
  }

//! @brief Devuelve el canto desde el borde del semiplano que se pasa como parámetro
//! a la fibra más comprimida.
double XC::GeomSection::getCantoMecanicoZonaComprimida(const Semiplano2d &sp_compresiones) const
  {
    Poligono2d contorno= getContornoRegiones();
    const size_t num_vertices= contorno.GetNumVertices();
    double d= 0.0,dneg= 0.0;    
    for(register size_t i=1;i<=num_vertices;i++)
      {
        d= sp_compresiones.DistSigno(contorno.Vertice(i));
        if(d<dneg) dneg= d;
      }
    assert(dneg<=0);
    return dneg;
  }

//! @brief Devuelve el canto desde el borde del semiplano que se pasa como parámetro
//! a la fibra más traccionada.
double XC::GeomSection::getCantoMecanicoZonaTraccionada(const Semiplano2d &sp_compresiones) const
  {
    Poligono2d contorno= getContornoRegiones();
    const size_t num_vertices= contorno.GetNumVertices();
    double d= 0.0,dpos=0.0;    
    for(register size_t i=1;i<=num_vertices;i++)
      {
        d= sp_compresiones.DistSigno(contorno.Vertice(i));
        if(d>dpos) dpos= d;
      }
    assert(dpos>=0);
    return dpos;
  }

//! @brief Devuelve la longitud del segmento que resulta
//! de cortar la recta que se pasa como parámetro con el
//! contorno de la sección.
double XC::GeomSection::getLongCorte(const Recta2d &r) const
  {
    double retval= 0.0;
    Poligono2d contorno= agrega_puntos_medios(getContornoRegiones());
    if(contorno.Overlap(r))
      retval= contorno.Clip(r).Longitud();
    return retval;
  }

//! @brief Devuelve las longitudes de los segmentos que resultan
//! de cortar la recta que se pasa como parámetro con el
//! contorno de la sección.
std::vector<double> XC::GeomSection::getLongsCorte(const std::list<Recta2d> &lr) const
  {
    const size_t sz= lr.size();
    std::vector<double> retval;
    if(sz>0)
      {
        Poligono2d contorno= agrega_puntos_medios(getContornoRegiones());
        int conta= 0;
        for(std::list<Recta2d>::const_iterator i= lr.begin();i!=lr.end();i++,conta++)
          {
            const Recta2d &r= *i;
            if(contorno.Overlap(r))
              retval[conta]= contorno.Clip(r).Longitud();
          }
      }
    return retval;
  }

//! @brief Devuelve el ancho de la sección para el plano de flexión.
double XC::GeomSection::getAnchoMecanico(const Recta2d &traza_plano_flexion) const
  {
    const Poligono2d contorno= agrega_puntos_medios(getContornoRegiones());
    const size_t num_vertices= contorno.GetNumVertices();
    double d= 0.0,dmax= 0.0;
    Recta2d perp;
    Segmento2d ancho;
    for(register size_t i=1;i<=num_vertices;i++)
      {
        perp= traza_plano_flexion.Perpendicular(contorno.Vertice(i));
        ancho= contorno.Clip(perp);
        d= ancho.Longitud();
        if(d>dmax)
          dmax= d;
      }
    assert(dmax>=0);
    return dmax;
  }

//! @brief Devuelve el ancho «b0» de la biela comprimida
//! correspondiente al brazo mecánico que se pasa como parámetro.
double XC::GeomSection::getAnchoBielaComprimida(const Segmento2d &brazo_mecanico) const
  {
    const Poligono2d contorno= agrega_puntos_medios(getContornoRegiones());
    const size_t num_vertices= contorno.GetNumVertices();
    Recta2d perp= brazo_mecanico.Mediatriz();
    Segmento2d ancho= contorno.Clip(perp);
    Pos2d p= punto_interseccion(ancho,brazo_mecanico);
    assert(p.exists());
    double b2= std::min(dist2(p,ancho.Origen()),dist2(p,ancho.Destino()));
    double bmin2= b2;
    bool intersecaBrazo= false;
    for(register size_t i=1;i<=num_vertices;i++)
      {
        perp= brazo_mecanico.Perpendicular(contorno.Vertice(i));
        intersecaBrazo= brazo_mecanico.Interseca(perp);
        if(intersecaBrazo)
          {
            ancho= contorno.Clip(perp);
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

//! @brief Devuelve el recubrimiento de la posición que se pasa como parámetro.
double XC::GeomSection::getRecubrimiento(const Pos2d &p) const
  {
    const double retval= -getContornoRegiones().DistSigno(p);
    if(retval<0)
      std::clog << "¡Ojo! la posición: " << p
                << " está fuera de la sección." << std::endl;
    return retval;
  }

//! @brief Devuelve el área homogeneizada de las regiones.
double XC::GeomSection::getAreaSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    if(!regiones.empty())
      retval+= regiones.getAreaSeccHomogeneizada(E0);
    if(!capas_armado.empty())
      retval+= capas_armado.getAreaSeccHomogeneizada(E0);
    return retval;
  }

XC::Vector XC::GeomSection::getCdgSeccHomogeneizada(const double &E0) const
  {
    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    peso= regiones.getAreaSeccHomogeneizada(E0);
    retval+= peso*regiones.getCdgSeccHomogeneizada(E0);
    divisor+= peso;
    peso= capas_armado.getAreaSeccHomogeneizada(E0);
    retval+= peso*capas_armado.getCdgSeccHomogeneizada(E0);
    divisor+= peso;
    retval/= divisor;
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al y por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::GeomSection::getIySeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgSeccHomogeneizada(E0)[1];

    if(!regiones.empty())
      {
        d= regiones.getCdgSeccHomogeneizada(E0)[1]-zCdg;
        retval+= regiones.getIySeccHomogeneizada(E0)+regiones.getAreaSeccHomogeneizada(E0)*sqr(d);
      }
    if(!capas_armado.empty())
      {
        d= capas_armado.getCdgSeccHomogeneizada(E0)[1]-zCdg;
        retval+= capas_armado.getIySeccHomogeneizada(E0)+capas_armado.getAreaSeccHomogeneizada(E0)*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::GeomSection::getIzSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgSeccHomogeneizada(E0)[0];

    if(!regiones.empty())
      {
        d= regiones.getCdgSeccHomogeneizada(E0)[0]-yCdg;
        retval+= regiones.getIzSeccHomogeneizada(E0)+regiones.getAreaSeccHomogeneizada(E0)*sqr(d);
      }
    if(!capas_armado.empty())
      {
        d= capas_armado.getCdgSeccHomogeneizada(E0)[0]-yCdg;
        retval+= capas_armado.getIzSeccHomogeneizada(E0)+capas_armado.getAreaSeccHomogeneizada(E0)*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el producto de inercia de la sección homogeneizada respecto a los ejes paralelos al y y al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::GeomSection::getPyzSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const double zCdg= getCdgSeccHomogeneizada(E0)[0];
    const double yCdg= getCdgSeccHomogeneizada(E0)[0];

    if(!regiones.empty())
      {
        d2= (regiones.getCdgSeccHomogeneizada(E0)[0]-yCdg)*(regiones.getCdgSeccHomogeneizada(E0)[1]-zCdg);
        retval+= regiones.getPyzSeccHomogeneizada(E0)+regiones.getAreaSeccHomogeneizada(E0)*d2;
      }
    if(!capas_armado.empty())
      {
        d2= (capas_armado.getCdgSeccHomogeneizada(E0)[0]-yCdg)*(capas_armado.getCdgSeccHomogeneizada(E0)[1]-zCdg);
        retval+= capas_armado.getPyzSeccHomogeneizada(E0)+capas_armado.getAreaSeccHomogeneizada(E0)*d2;
      }
    return retval;
  }

//! @brief Devuelve el área homogeneizada de las regiones.
double XC::GeomSection::getAreaSeccBruta(void) const
  {
    double retval= 0.0;
    if(!regiones.empty())
      retval+= regiones.getAreaSeccBruta();
    if(!capas_armado.empty())
      retval+= capas_armado.getAreaSeccBruta();
    return retval;
  }

XC::Vector XC::GeomSection::getCdgSeccBruta(void) const
  {
    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    peso= regiones.getAreaSeccBruta();
    retval+= peso*regiones.getCdgSeccBruta();
    divisor+= peso;
    peso= capas_armado.getAreaSeccBruta();
    retval+= peso*capas_armado.getCdgSeccBruta();
    divisor+= peso;
    retval/= divisor;
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al y por el CDG.
double XC::GeomSection::getIySeccBruta(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgSeccBruta()[1];

    if(!regiones.empty())
      {
        d= regiones.getCdgSeccBruta()[1]-zCdg;
        retval+= regiones.getIySeccBruta()+regiones.getAreaSeccBruta()*sqr(d);
      }
    if(!capas_armado.empty())
      {
        d= capas_armado.getCdgSeccBruta()[1]-zCdg;
        retval+= capas_armado.getIySeccBruta()+capas_armado.getAreaSeccBruta()*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al z por el CDG.
double XC::GeomSection::getIzSeccBruta(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgSeccBruta()[0];

    if(!regiones.empty())
      {
        d= regiones.getCdgSeccBruta()[0]-yCdg;
        retval+= regiones.getIzSeccBruta()+regiones.getAreaSeccBruta()*sqr(d);
      }
    if(!capas_armado.empty())
      {
        d= capas_armado.getCdgSeccBruta()[0]-yCdg;
        retval+= capas_armado.getIzSeccBruta()+capas_armado.getAreaSeccBruta()*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el producto de inercia de la sección homogeneizada respecto a los ejes paralelos al y y al z por el CDG.
double XC::GeomSection::getPyzSeccBruta(void) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const Vector posCdg= getCdgSeccBruta();
    const double zCdg= posCdg[1];
    const double yCdg= posCdg[0];

    if(!regiones.empty())
      {
        d2= (regiones.getCdgSeccBruta()[0]-yCdg)*(regiones.getCdgSeccBruta()[1]-zCdg);
        retval+= regiones.getPyzSeccBruta()+regiones.getAreaSeccBruta()*d2;
      }
    if(!capas_armado.empty())
      {
        d2= (capas_armado.getCdgSeccBruta()[0]-yCdg)*(capas_armado.getCdgSeccBruta()[1]-zCdg);
        retval+= capas_armado.getPyzSeccBruta()+capas_armado.getAreaSeccBruta()*d2;
      }
    return retval;
  }


//! @brief Imprime información sobre el objeto.
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
