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
//FedeasBondMaterial.cpp

#include <cstdlib>
#include "FedeasBondMaterial.h"
#include "utility/matrix/Vector.h"

XC::FedeasBondMaterial::FedeasBondMaterial(int tag, int classTag, int nhv, int ndata,
	double u1p, double q1p, double u2p, double u3p, double q3p,
	double u1n, double q1n, double u2n, double u3n, double q3n,
	double s0, double bb):
 XC::FedeasMaterial(tag, classTag, nhv, ndata)
  {
    // Fill in material parameters
    data[0]= u1p;
    data[1]= q1p;
    data[2]= u2p;
    data[3]= u3p;
    data[4]= q3p;
    data[5]= u1n;
    data[6]= q1n;
    data[7]= u2n;
    data[8]= u3n;
    data[9]= q3n;
    data[10]= s0;
    data[11]= bb;
  
    trial.Tangent()= q1p/u1p;
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasBondMaterial::FedeasBondMaterial(int tag, int classTag, int nhv, int ndata, const Vector &d):
 XC::FedeasMaterial(tag, classTag, nhv, ndata)
  {
    if(d.Size() != numData)
      {
        std::cerr << "FedeasBondMaterial::FedeasBondMaterial -- not enough input arguments\n";
        exit(-1);
      }
		
    for(int i= 0; i < numData; i++)
      data[i]= d(i);

    trial.Tangent()=  data[1]/data[0];
    converged.Tangent()= trial.getTangent();
  }

//! @brief Constructor.
XC::FedeasBondMaterial::FedeasBondMaterial(int tag, int classTag, int nhv, int ndata)
  : XC::FedeasMaterial(tag, classTag, nhv, ndata) {}

//! @brief Devuelve el valor noval del módulo de deformación.
double XC::FedeasBondMaterial::getInitialTangent(void) const
  {
    //return q1p/u1p;
    return data[1]/data[0];
  }
