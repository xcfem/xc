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
//FedeasConcrMaterial.cpp

#include <cstdlib>
#include "FedeasConcrMaterial.h"
#include "utility/matrix/Vector.h"

XC::FedeasConcrMaterial::FedeasConcrMaterial(int tag, int classTag, int nhv, int ndata, double fc, double ec, double fu, double eu)
  : XC::FedeasMaterial(tag, classTag, nhv, ndata)
  {
    data[0]  = fc;
    data[1]  = ec;
    data[2]  = fu;
    data[3]  = eu;

    trial.Tangent()= 2.0*data[0]/data[1];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasConcrMaterial::FedeasConcrMaterial(int tag, int classTag, int nhv, int ndata, const Vector &d)
  : XC::FedeasMaterial(tag, classTag, nhv, ndata)
  {
    if(d.Size() != numData)
      {
        std::cerr << "XC::FedeasConcrMaterial::FedeasConcrMaterial -- not enough input arguments\n";
        exit(-1);
      }

    for(int i = 0; i < numData; i++)
      data[i] = d(i);

    trial.Tangent()= 2.0*data[0]/data[1];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasConcrMaterial::FedeasConcrMaterial(int tag, int classTag, int nhv, int ndata)
  : XC::FedeasMaterial(tag, classTag, nhv, ndata) {}


double XC::FedeasConcrMaterial::getInitialTangent(void) const
  {
    //return 2.0*fc/ec;
    return 2.0*data[0]/data[1];
  }
