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

//! @brief Devuelve el momento de inercia respecto al eje paralelo al y por el CDG.
double XC::ReinfBar::Iy(void) const
  { return M_PI/4.0*pow((diameter/2.0),4.0); }

//! @brief Devuelve el momento de inercia respecto al eje paralelo al z por el CDG.
double XC::ReinfBar::Iz(void) const
  { return Iy(); }

//! @brief Devuelve el producto de inercia respecto a los ejes paralelos por el CDG.
double XC::ReinfBar::Pyz(void) const
  { return 0.0; }

//! @brief Devuelve el momento de inercia polar respecto al CDG en ejes locales.
double XC::ReinfBar::Ix(void) const
  { return Iy()+Iz(); }

//! @brief Devuelve el ángulo que define un eje principal de inercia.
double XC::ReinfBar::Theta_p(void) const
  { return 0.0; }

//! @brief Devuelve la dirección de un eje principal de inercia (no sabemos si
//! el mayor o el menor
const XC::Vector &XC::ReinfBar::DirEjeI_a(void) const
  {
    static Vector retval(2);
    retval[0]= 1.0;
    retval[1]= 0.0;
    return retval;
  }

//! @brief Devuelve la dirección del otro eje principal de inercia (no sabemos si
//!     //el mayor o el menor
const XC::Vector &XC::ReinfBar::DirEjeI_b(void) const
  {
    static Vector retval(2);
    retval[0]= 0.0;
    retval[1]= 1.0;
    return retval;
  }

//     Recta2d EjeI_a(void) const;
//     //Devuelve un eje principal de inercia (no sabemos si
//     //el mayor o el menor).
//     Recta2d EjeI_b(void) const;
//     //Devuelve el otro eje principal de inercia (no sabemos si
//     //el mayor o el menor).
//     Ref2d2d EjesPrincipalesInercia(void) const;
//     //Devuelve los ejes principales de inercia.

//! @brief Devuelve el momento de inercia principal mayor.
double XC::ReinfBar::getI1(void) const
  { return Iy(); }

//! @brief Devuelve el momento de inercia principal menor.
double XC::ReinfBar::getI2(void) const
  { return Iz(); }

//     inline EjesPrincInercia2d Inercia(void)
//       { return EjesPrincInercia2d(Cdg(),Iy(),Iz(),Pyz()); }

//! @brief Devuelve la componente i,j del tensor de inercia calculado respecto al CDG.
double XC::ReinfBar::getI(const unsigned short int &i,const unsigned short int &j) const
  {
    if(i!=j)
      return 0.0;
    else
      return Iy();
  }

//! @brief Devuelve el momento de inercia respecto al eje que pasa por O con dirección la de e.
double XC::ReinfBar::getI(const Pos2d &O,const Vector &e) const
  {
    const Matrix Io(getI(O));
    return dot(e,Io*e)/e.Norm2();
  }

//! @brief Devuelve la componente i,j del tensor de inercia calculado respecto al punto "o".
double XC::ReinfBar::getI(const unsigned short int i,const unsigned short int j,const Pos2d &o) const
  {
    const double Iij= getI(i,j);
    const Vector cdg= getPosition();

    Pos2d pp(cdg[0],cdg[1]);
    Ref2d2d ejes(pp);
    Pos2d pos_local= ejes.GetPosLocal(o);
    return Iij + getArea() * pos_local(i) * pos_local(j);
  }

//! @brief Devuelve el momento polar de inercia respecto al punto o.
double XC::ReinfBar::getIO(const Pos2d &o) const
  { return (getI(1,1,o)+getI(2,2,o)+getI(3,3,o))/2; }

//! @brief Devuelve el tensor de inercia calculado desde el centro de gravedad del objeto.
XC::Matrix &XC::ReinfBar::getI(void) const
  {
    static Matrix i(2,2);
    i(0,0)= Iy(); i(0,1)= -Pyz();
    i(1,0)= i(0,1);   i(1,1)= Iz();
    return i;
  }

//! @brief Devuelve el tensor de inercia respector al punto o.
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


