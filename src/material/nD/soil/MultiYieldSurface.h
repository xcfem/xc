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
// $Revision: 1.7 $
// $Date: 2003/02/14 23:01:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/MultiYieldSurface.h,v $
                                                                        
// Written: ZHY
// Created: August 2000
//
// MultiYieldSurface.h
// -------------------
//

#ifndef _MultiYieldSurface_H_
#define _MultiYieldSurface_H_

#include <material/nD/soil/T2Vector.h>


#define LOCK_VALUE  1.0e+30

// global function to find the roots of a second order equation
double secondOrderEqn(double A, double B, double C, int i);

// define yield surface in stress space
namespace XC {
//! @ingroup SoilNDMat
//
//! @brief ??.
class MultiYieldSurface
  {
  private:
    double theSize;
    Vector theCenter;  
    double plastShearModulus;
  public:
    //constructors
    MultiYieldSurface();
    MultiYieldSurface(const Vector & center_init, double size_init, 
                    double plas_modul); 

  void setData(const Vector & center_init, double size_init,double plas_modul); 
  const Vector & center() const {return theCenter; }
  double size() const {return theSize; }
  double modulus() const {return plastShearModulus; }
  void  setCenter(const Vector & newCenter); 

  // friend ostream & operator<< (ostream & os, const MultiYieldSurface & );  
  // friend istream & operator>> (istream & is, MultiYieldSurface & );

};
} // fin namespace XC

#endif
