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
// $Revision: 1.8 $
// $Date: 2003/02/14 23:01:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/MultiYieldSurface.cpp,v $
                                                                        
// Written: ZHY
// Created: August 2000
//
// MultiYieldSurface.cpp
// ---------------------
//

#include <cmath>
#include <cstdlib>
#include <material/nD/soil/MultiYieldSurface.h>


// YieldSurface class methods
XC::MultiYieldSurface::MultiYieldSurface():
theSize(0.0), theCenter(6), plastShearModulus(0.0)
  {}

XC::MultiYieldSurface::MultiYieldSurface(const XC::Vector & theCenter_init, 
                                     double theSize_init, double plas_modul):
theSize(theSize_init), theCenter(theCenter_init), plastShearModulus(plas_modul)
  {}

void XC::MultiYieldSurface::setData(const XC::Vector & theCenter_init, 
                                double theSize_init, double plas_modul)
  {
    theSize = theSize_init;
    theCenter = theCenter_init;
    plastShearModulus = plas_modul;
  }

void XC::MultiYieldSurface::setCenter(const XC::Vector & newCenter)
  {
    if(newCenter.Size() != 6)
      {
	std::cerr << "FATAL:XC::MultiYieldSurface::setCenter(XC::Vector &): vector size not equal 6" << std::endl;
        exit(-1);
      }
    theCenter = newCenter;
  }


/**********************************************
ostream & operator<< (ostream & os, const XC::MultiYieldSurface & a)
{
  os << "  theSize = " << a.theSize << std::endl 
     << "  theCenter = " << a.theCenter << std::endl
     << "  plastShearModulus = " << a.plastShearModulus << std::endl;
  
  return os;
}


istream & operator>> (istream & is, MultiYieldSurface & a)
{
  is >> a.theSize >> a.theCenter >> a.plastShearModulus;

  return is;
}
*********************************************/

double secondOrderEqn(double A, double B, double C, int i)
  {
    if(A == 0)
      {
	std::cerr << "FATAL:second_order_eqn: A=0." << std::endl;
	if(i==0) std::cerr << " when finding reference point on outer surface."                           << std::endl;
	else
	  std::cerr << " when moving active surface." << std::endl;
        exit(-1);
      }
    if(C == 0) return 0;
    if(B == 0)
      {
        if(C/A > 0)
          {
	    std::cerr << "FATAL:second_order_eqn: Complex roots.\n";
            exit(-1);
          }
        return sqrt(-C/A);
      }

    double determ, val1, val2, val;
    determ = B*B - 4.*A*C; 
    if(determ < 0)
      {
        std::cerr << "FATAL:second_order_eqn: Complex roots.\n";
        if(i==0)
          std::cerr << " when finding reference point on outer surface." <<std::endl;
        else
          std::cerr << " when moving active surface." <<std::endl;
        std::cerr << "B2=" << B*B << " 4AC=" << 4.*A*C <<std::endl; 
        exit(-1);
      }
  
    if(B > 0)
      val1 = (-B - sqrt(determ)) / (2.*A);
    else
      val1 = (-B + sqrt(determ)) / (2.*A);
    val2 = C / (A * val1);

    if(val1 < 0 && val2 < 0)
      {
	if(fabs(val1) < LOW_LIMIT)
          val1 = 0.;
        else
          if(fabs(val2) < LOW_LIMIT) val2 = 0.;
      }
    if(val1 < 0 && val2 < 0)
      {
        std::cerr << "FATAL:second_order_eqn: Negative roots.\n";
        if(i==0)
          std::cerr << " when finding reference point on outer surface." <<std::endl;
        else
          std::cerr << " when moving active surface." <<std::endl;
	std::cerr << "A=" << A << " B=" << B << " C=" << C << " det=" << determ << 
			" x1=" << val1 << " x2=" << val2 << std::endl;  
        exit(-1);   
      }
    if(val1 < 0)
      return  val2;
    else if(val2 < 0)
      return  val1;
    else
      {
        val = val1;
        if(val > val2)
          val = val2;
	return val;
      }
  }
