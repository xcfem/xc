// -*-c++-*-
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
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfLayer/ReinfLayer.h,v $
                                                                        
                                                                        
// File: ReinfLayer.h
// Written by Remo M. de Souza
// December 1998

#ifndef ReinfLayer_h 
#define ReinfLayer_h 

#include "material/section/repres/DiscretBase.h" 
#include <material/section/repres/geom_section/reinfBar/VectorReinfBar.h>

class HalfPlane2d;
class Polygon2d;

namespace XC {
class ReinfBar;
class ListReinfLayer;
class GeomSection;

//! @ingroup MATSCCReinforcement
//
//! @brief Reinforcement bars layer.
class ReinfLayer: public DiscretBase
  {
  private:
    int nReinfBars; //!< Number of bars in the layer.
    double barDiam; //!< Bar diameter.
    double area; //!< Bar area.
  protected:
    mutable VectorReinfBar reinfBars;



    friend class ListReinfLayer;
    ReinfLayer(ListReinfLayer *,Material *m);
    ReinfLayer(ListReinfLayer *,Material *mat,const int &numReinfBars,const double &bDiam=0.0,const double &bArea= 0.0);
    virtual ReinfLayer *getCopy(void) const= 0;
  public:
    virtual ~ReinfLayer(void) {}

    // edition functions
    virtual void setNumReinfBars(int numReinfBars);
    virtual int getNumReinfBars(void) const;
    virtual void setReinfBarDiameter(double reinfBarDiameter);
    virtual const double &getReinfBarDiameter(void) const;
    virtual void setReinfBarArea(double reinfBarArea);
    virtual const double &getReinfBarArea(void) const;

    // reinforcing layer inquiring functions
    void getBarrasIn(const Polygon2d &,ListReinfLayer &,bool );
    void getBarrasIn(const HalfPlane2d &,ListReinfLayer &,bool );
    
    const GeomSection *getSectionGeometry(void) const;
    double getCover(void) const;
    Vector getCenterOfMass(void) const;

    //! @brief Return the area of the bars.
    inline double getArea(void) const
      { return area*nReinfBars; }
    VectorReinfBar &getReinfBars(void);  
    virtual const VectorReinfBar &getReinfBars(void) const= 0;  


    virtual void Print(std::ostream &s, int flag =0) const;   
    friend std::ostream &operator<<(std::ostream &, const ReinfLayer &);    
  };

std::ostream &operator<<(std::ostream &s, const ReinfLayer &);
   
} // end of XC namespace


#endif

