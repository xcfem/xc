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
#include "SingleBar.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/d2/2d_polygons/Polygon2d.h"
#include "xc_utils/src/geom/d2/HalfPlane2d.h"


#include "material/section/repres/geom_section/GeomSection.h"

//! @brief Constructor.
XC::ReinfLayer::ReinfLayer(ListReinfLayer *owr,Material *m)
  : DiscretBase(m), nReinfBars(0), barDiam(0.0),area(0.0) 
  { set_owner(owr); }

//! @brief Constructor.
XC::ReinfLayer::ReinfLayer(ListReinfLayer *owr,Material *m,const int &numReinfBars,const double &bDiam,const double &bArea)
  : DiscretBase(m), nReinfBars(numReinfBars), barDiam(bDiam),area(bArea) 
  { set_owner(owr); }


//! @brief Set the number or rebars in the layer.
void XC::ReinfLayer::setNumReinfBars(int numReinfBars)
  { nReinfBars= numReinfBars; }

//! @brief Returns a reference to the (GeomSection) owner object.
const XC::GeomSection *XC::ReinfLayer::getGeomSection(void) const
  {
    const EntCmd *owr= Owner();
    assert(owr);
    const ListReinfLayer *l= dynamic_cast<const ListReinfLayer *>(owr);
    assert(l);
    return l->getGeomSection();
  }

//! @brief Returns the minimum value for rebar cover.
double XC::ReinfLayer::getCover(void) const
  {
    const VectorReinfBar &barras= getReinfBars();
    double retval= 0;
    if(!barras.empty())
      {
        const GeomSection *geom= getGeomSection();
        if(geom)
          {
            VectorReinfBar::const_iterator i= barras.begin();
            retval= geom->getCover((*i)->getPos2d());
            i++;
            for(;i!= barras.end();i++)
              retval= std::min(retval,geom->getCover((*i)->getPos2d()));
          }
      }
    return retval;
  }

//! @brief Returns the reinforcement layer centroid.
XC::Vector XC::ReinfLayer::getCenterOfMass(void) const
  {
    const VectorReinfBar &barras= getReinfBars();
    return barras.getCenterOfMassGrossSection();
  }

//! @brief Returns the barras contenidas total o parcialmente en el polígono.
void XC::ReinfLayer::getBarrasIn(const Polygon2d &plg,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const VectorReinfBar &barras= getReinfBars();  

    VectorReinfBar::const_iterator i= barras.begin();
    for(;i!= barras.end();i++)
      if(plg.In((*i)->getPos2d()))
        retval.push_back(SingleBar(**i));
  }

//! @brief Returns the barras contenidas total o parcialmente en el half-plane.
void XC::ReinfLayer::getBarrasIn(const HalfPlane2d &sp,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const VectorReinfBar &barras= getReinfBars();

    VectorReinfBar::const_iterator i= barras.begin();
    for(;i!= barras.end();i++)
      if(sp.In((*i)->getPos2d()))
        retval.push_back(SingleBar(**i));
  }

//! @brief Return a vector containing the bars of the layer.
XC::VectorReinfBar &XC::ReinfLayer::getReinfBars(void)
  {
    //Evitamos duplicar el método.
    return const_cast<VectorReinfBar&>(const_cast<const ReinfLayer&>(*this).getReinfBars());
  } 

//! @brief Returns the number of bars of the layer.
int XC::ReinfLayer::getNumReinfBars(void) const
  { return nReinfBars; }

//! @brief Sets bars diameter.
void XC::ReinfLayer::setReinfBarDiameter(double reinfBarDiameter)
  {
    barDiam = reinfBarDiameter;
    area = M_PI * barDiam*barDiam/4.0;
  }

//! @brief Sets the bars area.
void XC::ReinfLayer::setReinfBarArea(double reinfBarArea)
  { area = reinfBarArea; }

//! @brief Returns the bars diameter.
const double &XC::ReinfLayer::getReinfBarDiameter(void) const
  { return barDiam; }

//! @brief Returns the bars area.
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
 
