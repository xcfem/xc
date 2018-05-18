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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfBar/ReinfBar.h,v $
                                                                        
                                                                        
// File: ReinfBar.h
//
// Written by Remo M. de Souza
// November 1998


#ifndef ReinfBar_h 
#define ReinfBar_h 

#include "utility/matrix/Vector.h"
#include "material/section/repres/DiscretBase.h" 

class Pos2d;

namespace XC {
class Matrix;

//! @ingroup MATSCCRepresGeom
//
//! @defgroup MATSCCReinforcement Reinforcing layers representation (rebars).
//
//! @ingroup MATSCCReinforcement
//
//! @brief Reinforcing bar.
class ReinfBar: public DiscretBase
  {
  private:
    double diameter; //!< bar diameter.
    double area; //!< bar area.
    Vector posit;  //!< bar position.
  public:
    ReinfBar(Material *mat= nullptr);
    ReinfBar(const double &barArea, Material *, const Vector &position);
    ReinfBar(const double &barArea,const double &barDiameter, Material *, const Vector &);
    virtual ReinfBar *getCopy(void) const;
    virtual ~ReinfBar(void) {}
    
    // edition functions

    void setDiameter(const double &barDiameter);
    void setArea(const double &barArea);
    void setPosition(const Vector &position);

    // reinforcing bar inquiring functions
    
    double getDiameter(void) const;
    double getArea(void) const;
    double Iy(void) const;
    double Iz(void) const;
    double Pyz(void) const;
    double Ix(void) const;
    double Theta_p(void) const;
    const Vector &IAxisDir_a(void) const;
    const Vector &IAxisDir_b(void) const;
    double getI1(void) const;
    double getI2(void) const;
    double getI(const unsigned short int &i,const unsigned short int &j) const;
    double getI(const unsigned short int i,const unsigned short int j,const Pos2d &o) const;
    double getI(const Pos2d &O,const Vector &e) const;
    double getIO(const Pos2d &o) const;
    Matrix &getI(void) const;
    Matrix &getI(const Pos2d &o) const;

    const Vector &getPosition(void) const;
    Pos2d getPos2d(void) const;

    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;

    virtual void Print(std::ostream &s, int flag =0) const;   

  };
} // end of XC namespace


#endif

