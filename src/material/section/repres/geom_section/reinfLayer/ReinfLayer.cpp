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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfLayer/ReinfLayer.cpp,v $
                                                                        
                                                                        
// File: ReinfLayer.C
// Written by Remo M. de Souza
// December 1998


#include "material/section/repres/geom_section/reinfLayer/ReinfLayer.h"
#include "material/section/repres/geom_section/reinfBar/VectorReinfBar.h"
#include "material/section/repres/geom_section/reinfBar/ReinfBar.h"


#include "xc_basic/src/matrices/m_double.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "ListReinfLayer.h"
#include "BarraSuelta.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"
#include "xc_utils/src/geom/d2/Semiplano2d.h"


#include "material/section/repres/geom_section/GeomSection.h"

//! @brief Constructor.
XC::ReinfLayer::ReinfLayer(ListReinfLayer *owr,Material *m)
  : DiscretBase(m), nReinfBars(0), barDiam(0.0),area(0.0) 
  { set_owner(owr); }

//! @brief Constructor.
XC::ReinfLayer::ReinfLayer(ListReinfLayer *owr,Material *m,const int &numReinfBars,const double &bDiam,const double &bArea)
  : DiscretBase(m), nReinfBars(numReinfBars), barDiam(bDiam),area(bArea) 
  { set_owner(owr); }


//! @brief Establece el número de barras de la capa.
void XC::ReinfLayer::setNumReinfBars(int numReinfBars)
  { nReinfBars= numReinfBars; }

//! @brief Devuelve una referencia al objeto GeomSection propietario.
const XC::GeomSection *XC::ReinfLayer::getGeomSection(void) const
  {
    const EntCmd *owr= Owner();
    assert(owr);
    const ListReinfLayer *l= dynamic_cast<const ListReinfLayer *>(owr);
    assert(l);
    return l->getGeomSection();
  }

//! @brief Devuelve el valor mínimo del recubrimiento de las barras.
double XC::ReinfLayer::getRecubrimiento(void) const
  {
    const VectorReinfBar &barras= getReinfBars();
    double retval= 0;
    if(!barras.empty())
      {
        const GeomSection *geom= getGeomSection();
        if(geom)
          {
            VectorReinfBar::const_iterator i= barras.begin();
            retval= geom->getRecubrimiento((*i)->getPos2d());
            i++;
            for(;i!= barras.end();i++)
              retval= std::min(retval,geom->getRecubrimiento((*i)->getPos2d()));
          }
      }
    return retval;
  }

//! @brief Devuelve el centro de gravedad de las armaduras.
XC::Vector XC::ReinfLayer::getCdg(void) const
  {
    const VectorReinfBar &barras= getReinfBars();
    return barras.getCdgSeccBruta();
  }

// //! @brief Devuelve el subconjunto de barras de ésta que cumplen la condición que se
// //! pasa como parámetro.
// void XC::ReinfLayer::Cumplen(const std::string &cond,ListReinfLayer &retval,bool clear)
//   {
//     if(clear) retval.clear();
//     const VectorReinfBar &barras= getReinfBars();  

//     VectorReinfBar::const_iterator i= barras.begin();
//     for(;i!= barras.end();i++)
//       if((*i)->verdadero(cond))
//         retval.push_back(BarraSuelta(**i));
//   }

//! @brief Devuelve las barras contenidas total o parcialmente en el polígono.
void XC::ReinfLayer::getBarrasIn(const Poligono2d &plg,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const VectorReinfBar &barras= getReinfBars();  

    VectorReinfBar::const_iterator i= barras.begin();
    for(;i!= barras.end();i++)
      if(plg.In((*i)->getPos2d()))
        retval.push_back(BarraSuelta(**i));
  }

//! @brief Devuelve las barras contenidas total o parcialmente en el semiplano.
void XC::ReinfLayer::getBarrasIn(const Semiplano2d &sp,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const VectorReinfBar &barras= getReinfBars();

    VectorReinfBar::const_iterator i= barras.begin();
    for(;i!= barras.end();i++)
      if(sp.In((*i)->getPos2d()))
        retval.push_back(BarraSuelta(**i));
  }

//! @brief Devuelve un vector con las barras de la capa de armadura.
XC::VectorReinfBar &XC::ReinfLayer::getReinfBars(void)
  {
    //Evitamos duplicar el método.
    return const_cast<VectorReinfBar&>(const_cast<const ReinfLayer&>(*this).getReinfBars());
  } 

//! @brief Devuelve el número de barras de la capa.
int XC::ReinfLayer::getNumReinfBars(void) const
  { return nReinfBars; }

//! @brief Establece el diámetro de las barras de refuerzo.
void XC::ReinfLayer::setReinfBarDiameter(double reinfBarDiameter)
  {
    barDiam = reinfBarDiameter;
    area = M_PI * barDiam*barDiam/4.0;
  }

//! @brief Establece el área de las barras de refuerzo.
void XC::ReinfLayer::setReinfBarArea(double reinfBarArea)
  { area = reinfBarArea; }

//! @brief Devuelve el diámetro de las barras.
const double &XC::ReinfLayer::getReinfBarDiameter(void) const
  { return barDiam; }

//! @brief Devuelve el área de las barras.
const double &XC::ReinfLayer::getReinfBarArea(void) const
  { return area; }

//! @brief Imprime.
void XC::ReinfLayer::Print(std::ostream &s, int flag) const
  {
     s << "\nReinf. bar diameter: " << getReinfBarDiameter();
     s << "\nReinf. bar area: " << getReinfBarArea();
  }

std::ostream &XC::operator<<(std::ostream &s, const XC::ReinfLayer &reinfLayer)
  {  
    reinfLayer.Print(s);
    return s;
  }
 
