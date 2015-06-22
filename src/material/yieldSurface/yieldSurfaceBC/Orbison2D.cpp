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
// Orbison2D.cpp: implementation of the YieldSurfaceBC class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/yieldSurfaceBC/Orbison2D.h"
#include <cmath>
#define ORBISON_CLASS_TAG -1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::Orbison2D::Orbison2D(int tag, double xcap, double ycap,
                     YS_Evolution &model)
:YieldSurface_BC2D(tag, ORBISON_CLASS_TAG, xcap, ycap, model)
  {}

//////////////////////////////////////////////////////////////////////
// YS specific methods
//////////////////////////////////////////////////////////////////////

void XC::Orbison2D::setExtent()
  {
	// Extent along the axis
	xPos =  1;
	xNeg = -1;
	yPos =  1;
	yNeg = -1;
}


void XC::Orbison2D::getGradient(double &gx, double &gy, double x, double y)
{
// check if the point is on the surface
    double drift =  getDrift(x, y);
    double capx = capXdim;
    double capy = capYdim;
    
    if(forceLocation(drift)!=0)
    {
     	std::cerr << "ERROR - XC::Orbison2D::getGradient(double &gx, double &gy, double x, double y)\n";
        std::cerr << "Force point not on the yield surface\n";
		std::cerr << " fx = " << x << ", fy = " << y  << " drift = " << drift << "\n";
        std::cerr << "\a";
    }
    else
    {
    	gx = 2*x/(capx) + 7.34*pow(y, 2)*(x/(capx));
    	gy = 2.3*y/(capy) - 0.9*pow(y, 5)/(capy) + 7.34*pow(x, 2)*(y/(capy));
//  	p1 = 2.3*p - 0.9*pow(p, 5) + 7.34*pow(m, 2)*(p);
//  	m1 = 2*m + 7.34*pow(p, 2)*(m);

//      gx = 2*x + 7.34*pow(y, 2)*x;
//      gy = 2.3*y - 0.9*pow(y, 5) + 7.34*pow(x, 2)*y;

    }

}

double XC::Orbison2D::getSurfaceDrift(double x, double y)
{
double phi = 1.15*y*y - 0.15*pow(y, 6) + x*x + 3.67*y*y*x*x;
double drift = phi - 1;
	return drift;
}

 XC::YieldSurface_BC *XC::Orbison2D::getCopy(void)
{
    Orbison2D *theCopy = new XC::Orbison2D(this->getTag(), capX, capY, *hModel);
    if(theCopy==0)
    {
     	std::cerr << "XC::Orbison2D::getCopy(void) - unable to make copy\n";
     	std::cerr << "\a";
    }
    //later  copy all the state variables
    return theCopy;
}

void XC::Orbison2D::Print(std::ostream &s, int flag)
  { s << "\nYield Surface No: " << this->getTag() << " type: Attalla2D\n"; }
