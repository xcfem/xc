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
//BilinearCyclic.cpp

#include "BilinearCyclic.h"
#include <cmath>

XC::BilinearCyclic::BilinearCyclic(int tag, double weight)
:CyclicModel(tag, -1), weightFactor(weight)
{

}


XC::CyclicModel *XC::BilinearCyclic::getCopy()
  { return new BilinearCyclic(*this); }

double XC::BilinearCyclic::getTaskFactor()
{
double tfactor;
	// redundant - only for print
	if(d_curr >= 0 && !initYieldPos)
		return 1.0;
	if(d_curr  < 0 && !initYieldNeg)
		return 1.0;
	// end redundant

	if(yielding /* && fabs(d_curr) >= fabs(d_end) */)
//		return resFactor; // will eventually unload
		tfactor = cycFactor_hist;
    else
    {
    	if(f_bgn*f_end < 0) // full-cycle
    	{
    		if(contains(0.0, f_bgn, f_curr))
    			tfactor=resFactor;
    		else
			  {
				  tfactor=rationalize(d_curr, f_curr, d_end, f_end); // actually with (x0, 0.0)
				  tfactor = weightFactor*tfactor + (1 - weightFactor)*resFactor;
			  }
				  
    	}
    	else // half-cycle
		 {
			 tfactor = rationalize(d_bgn, f_bgn, d_end, f_end);
			 tfactor = weightFactor*tfactor + (1 - weightFactor)*resFactor;
         }
    }

    // std::cerr << *this; std::cerr << "\a";
	// std::cerr << "tFactor = " << tfactor << std::endl;
	// if(yielding) {clog << "-yielding-" << endl;cin.get();}

	return tfactor;
}

void XC::BilinearCyclic::Print (std::ostream &s, int flag)
{
	this->XC::CyclicModel::Print (s, flag);
	s << "+BilinearCyclic\n";
	//s << "   taskFactor = " << getTaskFactor() << std::endl;
	s << "----------------------------------------"
      << "----------------------------------------"
	  << std::endl;
}



