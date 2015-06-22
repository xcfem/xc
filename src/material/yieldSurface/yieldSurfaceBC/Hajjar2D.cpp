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
// Hajjar2D.cpp: implementation of the YieldSurfaceBC class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/yieldSurfaceBC/Hajjar2D.h"
#include "utility/matrix/Vector.h"
#include "material/yieldSurface/evolution/YS_Evolution.h"
#include <cmath>

#define coefDebug 1
#define HAJJAR_CLASS_TAG -1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
XC::Hajjar2D::Hajjar2D(int tag, double xmax, double ymax,
			  YS_Evolution &model,
			  double centroid_y, double c1_, double c2_, double c3_)
:YieldSurface_BC2D(tag, HAJJAR_CLASS_TAG, xmax, ymax, model),
	centroidY(centroid_y),c1(c1_), c2(c2_), c3(c3_)
{
//!!	translateY = translateY_hist = centroidY;   // fix!
}


XC::Hajjar2D::Hajjar2D(	 int tag,
                     YS_Evolution &model,
					 double D, double b, double t,
					 double fc_, double fy_)
:YieldSurface_BC2D(tag, HAJJAR_CLASS_TAG, 0, 0, model),//would blow up if not..
					 depth(D), width(b), thick(t), fc(fc_), fy(fy_)
{
double fr  = 0.623*sqrt(fc);
double Ast = D*b - (D - 2*t)*(b - 2*t);
double Ac  = (D - 2*t)*(b - 2*t);

double x = D/t;
double y = fc/fy;

	c1 = 1.08 - 0.00265*x + 0.000023*x*x - 1.13*1e-7*x*x*x +
		 0.374*y - 1.3*y*y - 0.0419*y*y*y - 0.0691*x*y +
		 0.000234*x*x*y + 0.0754*x*y*y;

	c2 = 0.628 + 0.0259*x - 0.000367*x*x + 1.99*1e-6*x*x*x +
		 4.5*y - 14.9*y*y + 22.4*y*y*y + 0.164*x*y +
		 -0.000756*x*x*y - 0.126*x*y*y;

	c3 = 0.42 + 0.0892*x - 0.00122*x*x + 5.13*1e-6*x*x*x +
		 4.9*y - 16.5*y*y + 16.2*y*y*y - 0.165*x*y +
		 0.000713*x*x*y + 0.12*x*y*y;

	capY = Ast*fy + Ac*fc;

double num   = fc*(b*t - 2*t*t) + 0.5*fr*(D - t)*(b - 2*t) + fy*(2*D*t);
double denom = fc*(b - 2*t) + 0.5*fr*(b - 2*t) + fy*(4*t);

double xn = num/denom;
	capX  = 	  fc*(0.5*(b - 2*t)*(xn - t)*(xn - t))
				+ 0.5*fr*(0.5*(b - 2*t)*(D - xn - t)*(D - xn - t))
				+ fy*((2*t)*(D*D/2 + xn*xn + t*t - D*t - D*xn) + (b*t)*(D - t));

	centroidY = (Ac*fc - Ac*fr)/2;
	centroidY = centroidY/capY;

//!!	translateY = translateY_hist = centroidY;
    XC::Vector tt(2);
	tt(0) = 0;
	tt(1) = centroidY;
	hModel->setInitTranslation(tt);

	if (coefDebug)
	{
		std::cerr << " c1 = " << c1 << ", c2 = " << c2 << ", c3 = " << c3 << "\n";
		std::cerr << " centroidY = " << centroidY << "\n";
		std::cerr << " capX = " << capX << ", capY = " << capY << "\n";
	}
	// bad:
	capX_orig = capX;
	capY_orig = capY;
	capXdim = capX;
	capYdim = capY;

}

//////////////////////////////////////////////////////////////////////
// YS specific methods
//////////////////////////////////////////////////////////////////////

void XC::Hajjar2D::setExtent()
{
	// Extent along the axis
	xPos = sqrt(1/fabs(c1));
	xNeg = -xPos;
	yPos = sqrt(1/fabs(c2));
	yNeg = -yPos;

}

void XC::Hajjar2D::getGradient(double &gx, double &gy, double xi, double yi)
{
// check if the point is on the surface
    double drift =  getDrift(xi, yi);
    //!! why is capXdim not here??
    
    /*if(drift < -error)
    {
        std::cerr << "ERROR - XC::Hajjar2D::getGradient(double &gx, double &gy, double x, double y)\n";
        std::cerr << "Point inside the yield surface\n";
		std::cerr << " fx = " << xi << ", fy = " << yi  << " drift = " << drift << "\n";
        std::cerr << "\a";
    }
    else if(drift > error)
    {
        std::cerr << "ERROR - XC::Hajjar2D::getGradient(double &gx, double &gy, double x, double y)\n";
        std::cerr << "Point outside the yield surface\n";
		std::cerr << " fx = " << xi << ", fy = " << yi  << " drift = " << drift << "\n";
        std::cerr << "\a";
    }*/
    if(forceLocation(drift)!=0)
    {
     	std::cerr << "ERROR - XC::Hajjar2D::getGradient(double &gx, double &gy, double x, double y)\n";
        std::cerr << "Force point not on the yield surface\n";
		std::cerr << " fx = " << xi << ", fy = " << yi  << " drift = " << drift << "\n";
        std::cerr << "\a";
    }
    else
    {
		double x = xi;
		double y = yi /*- centroidY*/;

    	gx = 2*c1*x + 2*c3*pow(y, 2)*(x);
        gy = 2*c2*y + 2*c3*pow(x, 2)*(y);
    }

}

double XC::Hajjar2D::getSurfaceDrift(double xi, double yi)
{
double x = xi;
double y = yi/* - centroidY*/;

double phi = c1*x*x + c2*y*y + c3*y*y*x*x;
double drift = phi - 1;
	return drift;
}

XC::YieldSurface_BC *XC::Hajjar2D::getCopy(void)
  {
    Hajjar2D *theCopy = new XC::Hajjar2D(this->getTag(), *hModel,
									 depth, width, thick, fc, fy);
    //later  copy all the state variables
    return theCopy;
  }

void XC::Hajjar2D::Print(std::ostream &s, int flag)
  {
    s << "\nYield Surface No: " << this->getTag() << " type: Attalla2D\n";
  }
