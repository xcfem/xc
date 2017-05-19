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
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfBar/ReinfBar.cpp,v $
                                                                        
                                                                        
// File: ReinfBar.C
// Written by Remo M. de Souza
// December 1998

#include "material/section/repres/geom_section/reinfBar/ReinfBar.h"
#include <cmath>
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"


//! @brief Constructor.
XC::ReinfBar::ReinfBar(Material *mat)
  : DiscretBase(mat), diameter(0.0), area(0.0), posit(2) {}


//! @brief Constructor.
XC::ReinfBar::ReinfBar(const double &barArea, Material *mat, const Vector &position)
  : DiscretBase(mat), diameter(2*sqrt(barArea/M_PI)), area(barArea), posit(position){}

//! @brief Constructor.
XC::ReinfBar::ReinfBar(const double &barArea,const double &barDiameter, Material *mat, const Vector &position)
  : DiscretBase(mat), diameter(barDiameter), area(barArea), posit(position){}


//! @brief Constructor de copia.
XC::ReinfBar *XC::ReinfBar::getCopy(void) const
  { return new ReinfBar(*this); }

void XC::ReinfBar::setDiameter(const double &barDiameter)
  {
    diameter = barDiameter;
    area = M_PI * diameter*diameter/4.0;     
  }

void XC::ReinfBar::setArea(const double &barArea)
  { area = barArea; }

void XC::ReinfBar::setPosition (const Vector &position)
  { posit = position; }

double XC::ReinfBar::getDiameter(void) const
  { return diameter; }

double XC::ReinfBar::getArea (void) const
  { return area; }

//! @brief Returns the moment of inertia with respect to the axis paralelo al y por el centroid.
double XC::ReinfBar::Iy(void) const
  { return M_PI/4.0*pow((diameter/2.0),4.0); }

//! @brief Returns the moment of inertia with respect to the axis paralelo al z por el centroid.
double XC::ReinfBar::Iz(void) const
  { return Iy(); }

//! @brief Returns the producto de inercia respecto a los ejes paralelos por el centroid.
double XC::ReinfBar::Pyz(void) const
  { return 0.0; }

//! @brief Returns the moment of inertia polar with respect to centroid en ejes locales.
double XC::ReinfBar::Ix(void) const
  { return Iy()+Iz(); }

//! @brief Returns the ángulo que define un principal axis of inertia.
double XC::ReinfBar::Theta_p(void) const
  { return 0.0; }

//! @brief Return the direcction of one of the principal axis of inertia
//! (we don't know yet if it's the major one or the minor one).
const XC::Vector &XC::ReinfBar::DirEjeI_a(void) const
  {
    static Vector retval(2);
    retval[0]= 1.0;
    retval[1]= 0.0;
    return retval;
  }

//! @brief Return the direcction of the other (with respect to DirEjeI_a)
//! principal axis of inertia (we don't know yet if it's the major
//! one or the minor one).
const XC::Vector &XC::ReinfBar::DirEjeI_b(void) const
  {
    static Vector retval(2);
    retval[0]= 0.0;
    retval[1]= 1.0;
    return retval;
  }

//     Recta2d EjeI_a(void) const;
//     //Returns a principal axis of inertia (we don't know yet if it's
//     //the major one or the minor one).
//     Recta2d EjeI_b(void) const;
//     //Returns the otro principal axis of inertia (we don't know yet if it's
//     //the major one or the minor one).
//     Ref2d2d EjesPrincipalesInercia(void) const;
//     //Returns the ejes principales de inercia.

//! @brief Returns the principal major axis of inertia.
double XC::ReinfBar::getI1(void) const
  { return Iy(); }

//! @brief Returns the principal minor axis of inertia.
double XC::ReinfBar::getI2(void) const
  { return Iz(); }

//     inline PrincipalAxesOfInertia2D Inercia(void)
//       { return PrincipalAxesOfInertia2D(Cdg(),Iy(),Iz(),Pyz()); }

//! @brief Return the (i,j) components of the inertia tensor computed with respect to the centroid.
double XC::ReinfBar::getI(const unsigned short int &i,const unsigned short int &j) const
  {
    if(i!=j)
      return 0.0;
    else
      return Iy();
  }

//! @brief Returns the moment of inertia with respect to the axis
//! that passes through O with the direction of e.
double XC::ReinfBar::getI(const Pos2d &O,const Vector &e) const
  {
    const Matrix Io(getI(O));
    return dot(e,Io*e)/e.Norm2();
  }

//! @brief Return the (i,j) components of the inertia tensor computed with respect to the point "o".
double XC::ReinfBar::getI(const unsigned short int i,const unsigned short int j,const Pos2d &o) const
  {
    const double Iij= getI(i,j);
    const Vector cdg= getPosition();

    Pos2d pp(cdg[0],cdg[1]);
    Ref2d2d ejes(pp);
    Pos2d pos_local= ejes.GetPosLocal(o);
    return Iij + getArea() * pos_local(i) * pos_local(j);
  }

//! @brief Returns the polar moment of inertia with respect to the point o.
double XC::ReinfBar::getIO(const Pos2d &o) const
  { return (getI(1,1,o)+getI(2,2,o)+getI(3,3,o))/2; }

//! @brief Returns the inertia tensor computed with respect to the object centroid.
XC::Matrix &XC::ReinfBar::getI(void) const
  {
    static Matrix i(2,2);
    i(0,0)= Iy(); i(0,1)= -Pyz();
    i(1,0)= i(0,1);   i(1,1)= Iz();
    return i;
  }

//! @brief Returns the inertia tensor respector al punto o.
XC::Matrix &XC::ReinfBar::getI(const Pos2d &o) const
  {
    static Matrix retval(2,2);
    Matrix Ig= getI();
    Vector O(2); O[0]= o.x(); O[1]= o.y();
    Vector og= getPosition() - O;
    double m= getArea();
    retval= Ig+m*(og.Norm2()*identity(Ig)-(og & og));
    return retval;
  }

const XC::Vector &XC::ReinfBar::getPosition(void) const
  { return posit; }

Pos2d XC::ReinfBar::getPos2d(void) const
  { return Pos2d(posit[0],posit[1]); }

double XC::ReinfBar::getMaxY(void) const
  { return posit(0)+(diameter/2); }
double XC::ReinfBar::getMaxZ(void) const
  { return posit(1)+(diameter/2); }
double XC::ReinfBar::getMinY(void) const
  { return posit(0)-(diameter/2); }
double XC::ReinfBar::getMinZ(void) const
  { return posit(1)-(diameter/2); }

void XC::ReinfBar::Print(std::ostream &s, int flag) const
  {
    s << "\nReinforcing Bar area: " << area;
    //s << "\nMaterial ID: " << getMaterialID();
    s << "\nDiameter: " << diameter;
    s << "\nArea: " << area;
    s << "\nPosition: " << posit;
  }


