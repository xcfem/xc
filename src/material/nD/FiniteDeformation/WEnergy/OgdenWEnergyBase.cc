//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.


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

