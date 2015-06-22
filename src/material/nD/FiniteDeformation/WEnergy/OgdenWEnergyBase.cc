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


#include "OgdenWEnergyBase.h"
#include <utility/matrix/Vector.h>

//================================================================================
// Normal constructor
//================================================================================
XC::OgdenWEnergyBase::OgdenWEnergyBase(int N_in, double  *cr_in, double *mur_in )
  : N_Ogden(N_in), cr_Ogden(cr_in), mur_Ogden(mur_in) {}

XC::OgdenWEnergyBase::OgdenWEnergyBase(void)
  : N_Ogden(0), cr_Ogden(nullptr), mur_Ogden(nullptr) {}


//================================================================================
// w
//================================================================================
const double  XC::OgdenWEnergyBase::wE(const double &J_in, const XC::Vector &lambda_wave_in )
  {
    double wEnergy = 0.0;
    for (int i=0; i<N_Ogden; i++)
     {
        wEnergy += *(cr_Ogden+i) / *(mur_Ogden+i) *
                   (pow ( lambda_wave_in(0),  *(mur_Ogden+i) ) +
                    pow ( lambda_wave_in(1),  *(mur_Ogden+i) ) +
                    pow ( lambda_wave_in(2),  *(mur_Ogden+i) ) - 3.0 ) ;
     }
    return wEnergy;
  }

//================================================================================
// d(iso)w / d(lambda)
//================================================================================
const XC::Vector  XC::OgdenWEnergyBase::disowOdlambda( const XC::Vector &lambda_wave_in)
  {
    Vector disowOverdlambda(3);
    for (int i=0; i<N_Ogden; i++)
     {
        disowOverdlambda(0) += *(cr_Ogden+i) * pow( lambda_wave_in(0),  (*(mur_Ogden+i) -1.0) );
        disowOverdlambda(1) += *(cr_Ogden+i) * pow( lambda_wave_in(1),  (*(mur_Ogden+i) -1.0) );
        disowOverdlambda(2) += *(cr_Ogden+i) * pow( lambda_wave_in(2),  (*(mur_Ogden+i) -1.0) );
     }
    return disowOverdlambda;
  }

//================================================================================
// d2(iso)w / d(lambda)2
//================================================================================
const XC::Vector  XC::OgdenWEnergyBase::d2isowOdlambda2(const XC::Vector &lambda_wave_in )
  {
    Vector d2isowOverdlambda2(3);
    for (int i=0; i<N_Ogden; i++)
     {
        d2isowOverdlambda2(0) +=
            *(cr_Ogden+i) * (*(mur_Ogden+i) -1.0) * pow( lambda_wave_in(0),  (*(mur_Ogden+i) -2.0) );
        d2isowOverdlambda2(1) +=
            *(cr_Ogden+i) * (*(mur_Ogden+i) -1.0) * pow( lambda_wave_in(1),  (*(mur_Ogden+i) -2.0) );
        d2isowOverdlambda2(2) +=			 
            *(cr_Ogden+i) * (*(mur_Ogden+i) -1.0) * pow( lambda_wave_in(2),  (*(mur_Ogden+i) -2.0) );
     }
    return d2isowOverdlambda2;
  }

