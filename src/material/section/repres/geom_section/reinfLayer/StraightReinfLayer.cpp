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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfLayer/StraightReinfLayer.cpp,v $
                                                                        
                                                                        
// File: StraightReinfLayer.C 
// Written by Remo M. de Souza 
// December 1998

#include <cmath>
#include <utility/matrix/Matrix.h>

#include <material/section/repres/geom_section/reinfLayer/StraightReinfLayer.h>
#include <material/section/repres/geom_section/reinfBar/ReinfBar.h>

#include "xc_basic/src/texto/cadena_carac.h"
#include "xc_utils/src/geom/d1/Segmento2d.h"


XC::StraightReinfLayer::StraightReinfLayer(ListReinfLayer *owr,Material *mat)
  : ReinfLayer(owr,mat,0,0.0,0.0),
    initPosit(2), finalPosit(2) {}

XC::StraightReinfLayer::StraightReinfLayer(ListReinfLayer *owr,Material *mat, int numReinfBars, 
                                       double reinfBarArea,
                                       const XC::Vector &InitialPosition,
                                       const XC::Vector &FinalPosition)
  : ReinfLayer(owr,mat,numReinfBars,0.0,reinfBarArea),
    initPosit(InitialPosition), finalPosit(FinalPosition) {}


void XC::StraightReinfLayer::setInitialPosition (const XC::Vector &initialPosition)
  { initPosit = initialPosition; }

void XC::StraightReinfLayer::setFinalPosition (const XC::Vector &finalPosition)
  { finalPosit = finalPosition; }

void XC::StraightReinfLayer::setInitialPos(const Pos2d &p)
  {
    initPosit[0]= p.x();
    initPosit[1]= p.y();
  }

Pos2d XC::StraightReinfLayer::getInitialPos(void) const
  { return Pos2d(initPosit(0),initPosit(1)); }

void XC::StraightReinfLayer::setFinalPos(const Pos2d &p)
  {
    finalPosit[0]= p.x();
    finalPosit[1]= p.y();
  }

Pos2d XC::StraightReinfLayer::getFinalPos(void) const
  { return Pos2d(finalPosit(0),finalPosit(1)); }


double XC::StraightReinfLayer::getMaxY(void) const
  {
    double retval= initPosit(0);
    retval= std::max(retval,finalPosit(0));
    return retval;
  }
double XC::StraightReinfLayer::getMaxZ(void) const
  {
    double retval= initPosit(1);
    retval= std::max(retval,finalPosit(1));
    return retval;
  }
double XC::StraightReinfLayer::getMinY(void) const
  {
    double retval= initPosit(0);
    retval= std::min(retval,finalPosit(0));
    return retval;
  }
double XC::StraightReinfLayer::getMinZ(void) const
  {
    double retval= initPosit(1);
    retval= std::min(retval,finalPosit(1));
    return retval;
  }

//! @brief Returns an array of reinforcement bars.
const XC::VectorReinfBar &XC::StraightReinfLayer::getReinfBars(void) const
  {
    Vector barPosit(2);
    const int num_barras= getNumReinfBars();
    const double area_barra= getReinfBarArea();

    if(num_barras == 1)
      {
        barPosit(0) = (initPosit(0) + finalPosit(0)) / 2;
        barPosit(1) = (initPosit(1) + finalPosit(1)) / 2;
    
        reinfBars.resize(1);
        reinfBars.put(0,ReinfBar(area_barra,getMaterialPtr(),barPosit));
      }
    else if(num_barras > 1)
      {
        const double dy = (finalPosit(0) - initPosit(0))/(num_barras - 1);
        const double dz = (finalPosit(1) - initPosit(1))/(num_barras - 1);

        reinfBars.resize(num_barras);

        for(int i= 0;i<num_barras;i++)
          {
            barPosit(0)= initPosit(0) + dy * i;
            barPosit(1)= initPosit(1) + dz * i;
            reinfBars.put(i,ReinfBar(area_barra,getMaterialPtr(),barPosit));
          }
      }
    return reinfBars;
  }

const XC::Vector &XC::StraightReinfLayer::getInitialPosition(void) const
  { return initPosit; }

const XC::Vector &XC::StraightReinfLayer::getFinalPosition(void) const
  { return finalPosit; }

//! @brief Devuelve el segmento que une los extremos.
Segmento2d XC::StraightReinfLayer::getSegmento(void) const
  { return Segmento2d(Pos2d(initPosit(0),initPosit(1)),Pos2d(finalPosit(0),finalPosit(1))); }

double XC::StraightReinfLayer::getLongitud(void) const
  { return getSegmento().Longitud(); }

double XC::StraightReinfLayer::getSeparacion(void) const
  { return getLongitud()/getNumReinfBars(); }

XC::ReinfLayer *XC::StraightReinfLayer::getCopy (void) const
  { return new XC::StraightReinfLayer(*this); }

void XC::StraightReinfLayer::Print(std::ostream &s, int flag) const
  {
     s << "\nReinforcing Layer type:  Straight";
     ReinfLayer::Print(s,flag);
     s << "\nInitial Position: " << initPosit;
     s << "\nFinal Position: " << finalPosit;
  }

