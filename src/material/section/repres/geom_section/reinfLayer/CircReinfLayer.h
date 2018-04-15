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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfLayer/CircReinfLayer.h,v $
                                                                        
                                                                        
// File: CircReinfLayer.h
// Written by Remo M. de Souza
// December 1998


#ifndef CircReinfLayer_h 
#define CircReinfLayer_h 

#include <material/section/repres/geom_section/reinfLayer/ReinfLayer.h>
#include "xc_utils/src/geom/pos_vec/Pos2d.h"


namespace XC {
class ReinfBar;

//! @ingroup MATSCCArmaduras
//
//! @brief Arc of a circle shaped reinforcement layer.
class CircReinfLayer : public ReinfLayer
  {
  private:
    Pos2d centerPosit;
    double arcRad;
    double initAng;
    double finalAng;
  protected:


    friend class ListReinfLayer;
    CircReinfLayer(ListReinfLayer *,Material *);
    // Constructor for an arc
    CircReinfLayer(ListReinfLayer *,Material *, int numReinfBars, double  reinfBarArea,
                   const Pos2d &centerPosition, double arcRadius, double
                   initialAngle, double finalAngle);
    // Constructor for full circle
    CircReinfLayer(ListReinfLayer *,Material *, int numReinfBars, double  reinfBarArea,
                   const Pos2d &centerPosition, double radius);
    ReinfLayer *getCopy(void) const;
  public:
    
    // edition functions
    void setCenterPosition(const Pos2d &centerPosition);
    void setArcRadius(const double &);
    void setInitAngle(const double &);
    void setFinalAngle(const double &);  

    // inquiring functions
    const VectorReinfBar &getReinfBars(void) const;
  
    Pos2d getCenterPosition(void) const;
    double getArcRadius(void) const;    
    double getInitAngle(void) const;
    double getFinalAngle(void) const;  
    
    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;


    void Print(std::ostream &s, int flag =0) const;   
  };
} // end of XC namespace


#endif




