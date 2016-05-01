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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfLayer/CircReinfLayer.cpp,v $
                                                                        
                                                                        
// File: CircReinfLayer.C 
// Written by Remo M. de Souza 
// December 1998

#include <cmath>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

#include <material/section/repres/geom_section/reinfBar/ReinfBar.h>
#include <material/section/repres/geom_section/reinfLayer/CircReinfLayer.h>
#include "xc_basic/src/util/matem.h"


//! @brief Constructor.
XC::CircReinfLayer::CircReinfLayer(ListReinfLayer *owr,Material *mat)
  : ReinfLayer(owr,mat,0,0.0,0.0),
    centerPosit(), arcRad(0.0),
    initAng(0.0), finalAng(0.0)          
  {
    // Figure out final angle so that complete circle does not put
    // two bars at the same location
    const int num_barras= getNumReinfBars();
    if(num_barras > 0)
      finalAng = 360.0 - 360.0/num_barras;
  }


//! @brief Constructor.
XC::CircReinfLayer::CircReinfLayer(ListReinfLayer *owr,Material *mat, int numReinfBars, 
                                   double reinfBarArea,
                                   const Pos2d &centerPosition,
                                   double arcRadius, double initialAngle,
                                   double finalAngle)
  : ReinfLayer(owr,mat,numReinfBars,0.0,reinfBarArea),
    centerPosit(centerPosition), arcRad(arcRadius),
    initAng(initialAngle), finalAng(finalAngle)
  {}

//! @brief Constructor.
XC::CircReinfLayer::CircReinfLayer(ListReinfLayer *owr,Material *mat, int numReinfBars, double  reinfBarArea,
						  const Pos2d &centerPosition, double radius)
  : ReinfLayer(owr,mat,numReinfBars,0.0,reinfBarArea),
    centerPosit(centerPosition), arcRad(radius),
    initAng(0.0), finalAng(0.0)
  {
    // Figure out final angle so that complete circle does not put
    // two bars at the same location
    const int num_barras= getNumReinfBars();
    if(num_barras > 0)
      finalAng = 360.0 - 360.0/num_barras;
  }

double XC::CircReinfLayer::getMaxY(void) const
  {
    double retval= centerPosit(0) + arcRad*cos(initAng);
    const int num_barras= getNumReinfBars();
    if(num_barras>1)
      {
        const double dtheta= (finalAng-initAng)/(num_barras - 1);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta= initAng + dtheta * i;
            retval= std::max(retval,centerPosit(0) + arcRad*cos(theta));
          }
      }
    return retval;
  }
double XC::CircReinfLayer::getMaxZ(void) const
  {
    double retval= centerPosit(1) + arcRad*sin(initAng);
    const int num_barras= getNumReinfBars();
    if(num_barras>1)
      {
        const double dtheta= (finalAng-initAng)/(num_barras - 1);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta= initAng + dtheta * i;
            retval= std::max(retval,centerPosit(1) + arcRad*sin(theta));
          }
      }
    return retval;
  }
double XC::CircReinfLayer::getMinY(void) const
  {
    double retval= centerPosit(0) + arcRad*cos(initAng);
    const int num_barras= getNumReinfBars();
    if(num_barras>1)
      {
        const double dtheta= (finalAng-initAng)/(num_barras - 1);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta= initAng + dtheta * i;
            retval= std::min(retval,centerPosit(0) + arcRad*cos(theta));
          }
      }
    return retval;
  }
double XC::CircReinfLayer::getMinZ(void) const
  {
    double retval= centerPosit(1) + arcRad*sin(initAng);
    const int num_barras= getNumReinfBars();
    if(num_barras>1)
      {
        const double dtheta= (finalAng-initAng)/(num_barras - 1);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta= initAng + dtheta * i;
            retval= std::min(retval,centerPosit(1) + arcRad*sin(theta));
          }
      }
    return retval;
  }

//! @brief Devuelve un vector con las barras de la capa de armadura.
const XC::VectorReinfBar &XC::CircReinfLayer::getReinfBars(void) const
  {
    Vector barPosit(2);

    const int num_barras= getNumReinfBars();
    const double area_barra= getReinfBarArea();

    if(num_barras > 1)
      {
        const double dtheta = (finalAng-initAng) /(num_barras - 1);

        reinfBars.resize(num_barras);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta = initAng + dtheta * i;
            barPosit(0) = centerPosit(0) + arcRad*cos(theta);
            barPosit(1) = centerPosit(1) + arcRad*sin(theta);
            reinfBars.put(i,ReinfBar(area_barra,getMaterialPtr(),barPosit));
          }
      }
    return reinfBars;
  }


Pos2d XC::CircReinfLayer::getCenterPosition(void) const
  { return centerPosit; }

double XC::CircReinfLayer::getArcRadius(void) const 
  { return arcRad; }

double XC::CircReinfLayer::getInitAngle(void) const 
  { return initAng; }

double XC::CircReinfLayer::getFinalAngle(void) const 
  { return finalAng; }

void XC::CircReinfLayer::setCenterPosition(const Pos2d &p)
  { centerPosit= p; }

void XC::CircReinfLayer::setArcRadius(const double &d)
  { arcRad= d; }

void XC::CircReinfLayer::setInitAngle(const double &d)
  { initAng= d; }

void XC::CircReinfLayer::setFinalAngle(const double &d)
  { finalAng= d; }


XC::ReinfLayer *XC::CircReinfLayer::getCopy(void) const
  { return new CircReinfLayer(*this); }


void XC::CircReinfLayer::Print(std::ostream &s, int flag) const
  {
     s << "\nReinforcing Layer type:  Circ";
     ReinfLayer::Print(s,flag);
     s << "\nCenter Position: " << centerPosit;
     s << "\nArc Radius: " << arcRad;
     s << "\nInitial angle: " << initAng;
     s << "\nFinal angle: " << finalAng;
  }


 
