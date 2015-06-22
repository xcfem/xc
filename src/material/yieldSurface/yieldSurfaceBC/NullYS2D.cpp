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
// NullYS2D.cpp: implementation of the YieldSurfaceBC class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/yieldSurfaceBC/NullYS2D.h"
#include <math.h>
#include <material/yieldSurface/evolution/NullEvolution.h>

 XC::NullEvolution XC::NullYS2D::evolModel(0, 1e+20, 1e+20);
 
#define nullptrYS2D_CLASS_TAG -1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::NullYS2D::NullYS2D(int tag)
:YieldSurface_BC2D(tag, nullptrYS2D_CLASS_TAG, 1, 1, evolModel){}


//////////////////////////////////////////////////////////////////////
// YS specific methods
//////////////////////////////////////////////////////////////////////
void XC::NullYS2D::setExtent()
{
	// Extent along the axis
	xPos =  1;
	xNeg = -1;
	yPos =  1;
	yNeg = -1;
}


void XC::NullYS2D::getGradient(double &gx, double &gy, double x, double y)
{
    	std::cerr << "ERROR - XC::NullYS2D::getGradient function should not be called\n";
		gx = 1;
		gy = 1;
}

double XC::NullYS2D::getSurfaceDrift(double x, double y)
{
	return -1;
}

 XC::YieldSurface_BC *XC::NullYS2D::getCopy(void)
{
    NullYS2D *theCopy = new XC::NullYS2D(this->getTag());
    return theCopy;
}

void XC::NullYS2D::Print(std::ostream &s, int flag)
{
    s << "\nYield Surface No: " << this->getTag() << " type: NullYS2D\n";
}

