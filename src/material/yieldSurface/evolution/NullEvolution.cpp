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
/***************************************************************************
                          NullEvolution.cpp  -  description
                             -------------------
    begin                : Thu Aug 1 2002
    email                : rkaul@ce-blume215-pent-2.stanford.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#include "material/yieldSurface/evolution/NullEvolution.h"
#define nullptr_EVOL_CLASS_TAG -1

XC::Vector XC::NullEvolution::vec_dim_1(1);
XC::Vector XC::NullEvolution::vec_dim_2(2);
XC::Vector XC::NullEvolution::vec_dim_3(3);

XC::NullEvolution::NullEvolution(int tag, double isox)
:YS_Evolution(tag, nullptr_EVOL_CLASS_TAG, 0.0, 0.0, 1, 0.0, 0.0)
{
	isotropicFactor(0) = isox;
	isotropicFactor_hist(0) = isox;
}


XC::NullEvolution::NullEvolution(int tag, double isox, double isoy)
:YS_Evolution(tag, nullptr_EVOL_CLASS_TAG, 0.0, 0.0, 2, 0.0, 0.0)
{
	isotropicFactor(0) = isox;
	isotropicFactor(1) = isoy;
	isotropicFactor_hist(0) = isox;
	isotropicFactor_hist(1) = isoy;

}


XC::NullEvolution::NullEvolution(int tag, double isox, double isoy, double isoz)
:YS_Evolution(tag, nullptr_EVOL_CLASS_TAG, 0.0, 0.0, 3, 0.0, 0.0)
{
	isotropicFactor(0) = isox;
	isotropicFactor(1) = isoy;
	isotropicFactor(2) = isoz;
	isotropicFactor_hist(0) = isox;
	isotropicFactor_hist(1) = isoy;
	isotropicFactor_hist(2) = isoz;
}

/** No descriptions */
int XC::NullEvolution::evolveSurface(YieldSurface_BC * ys, double magPlasticDefo,
                                XC::Vector & G, XC::Vector & F_Surface, int flag)
{
	// does nothing
	return 0;
}

/** No descriptions */
XC::YS_Evolution * XC::NullEvolution::getCopy()
  {
 XC::NullEvolution *copy=0;

	if(dimension == 1)
		copy = new XC::NullEvolution(getTag(), isotropicFactor(0));
	else if(dimension == 2)
		copy = new XC::NullEvolution(getTag(), isotropicFactor(0), isotropicFactor(1));
	else if(dimension == 3)
		copy = new XC::NullEvolution(getTag(), isotropicFactor(0), isotropicFactor(1), isotropicFactor(2));
	else
		copy = 0;
	return copy;
}

/** No descriptions */
const XC::Vector & XC::NullEvolution::getEquiPlasticStiffness()
{
	if(dimension == 1)
		return vec_dim_1;
	else if (dimension == 2)
		return vec_dim_2;
	else if (dimension == 3)
		return vec_dim_3;
	else
		std::cerr << "XC::NullEvolution::getEquiPlasticStiffness() - error dimension\n";

	return vec_dim_3;
}

double XC::NullEvolution::getTrialPlasticStrains(int dof)
{
	return 0;
}

double XC::NullEvolution::getCommitPlasticStrains(int dof)
{
	return 0;
}


/** No descriptions */
int XC::NullEvolution::getResponse(int responseID, Information & info)
{
	return 0;
}

/** No descriptions */
XC::Response * XC::NullEvolution::setResponse(const std::vector<std::string> &argv, Information & info)
  { return 0; }

/** No descriptions */
int XC::NullEvolution::commitState(int status)
  { return 0; }
/** No descriptions */
int XC::NullEvolution::revertToLastCommit(void)
  { return 0; }

/*
// implement these or else the base class will complain
void	XC::NullEvolution::toDeformedCoord(double &x){
	}
void	XC::NullEvolution::toDeformedCoord(double &x, double &y){
	}
void	XC::NullEvolution::toDeformedCoord(double &x, double &y, double &z){
	}

void	XC::NullEvolution::toOriginalCoord(double &x){
	}
void	XC::NullEvolution::toOriginalCoord(double &x, double &y){
	}
void	XC::NullEvolution::toOriginalCoord(double &x, double &y, double &z){
	}
*/




