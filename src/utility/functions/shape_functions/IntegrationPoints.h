// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//IntegrationPoints.h
// ORIGINAL: Copyright (c) 1999, A.H. van den Boogaard
//FeaTure is put on the internet as free software. I would very 
//much like to hear your feedback and if possible improvements. See 
//also the 'Future plans' part in the manual.
//Modified by LCPT to integrate it in XC.

#ifndef INTEGRATIONPOINTS_H
#define INTEGRATIONPOINTS_H

#include "utility/matrices/m_double.h"


void Gauss( int ip, int nip, double &xi, double &weight);

void Hammer( int ip, int nip, double& L1, double& L2, double& L3,
             double& weight );

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Integration points container.
class IntegrationPoints
  {
    //! @brief Definition of an integration point.
    class pto_int
      { 
      public:
        pto_int(){};
        ~pto_int(){};
        m_double nat_coord;//!< Natural coordinates of the integration point.
        double weight; //!< Integration point weight.
      };

    private:
      int nr_of_intpt; //!< Number of integration points.
      pto_int *intpts; //!< Integration points.

    public:
      IntegrationPoints();
      IntegrationPoints(const IntegrationPoints &);
      IntegrationPoints(int nr);
      ~IntegrationPoints();

      void set_nr_intpt(int nr);
      int  get_nr_intpt() const;
      void set_values(int ip,const m_double &coor,const double &weight);
      m_double get_coor(int ip) const;
      double get_weight(int ip) const;
};

#endif /* INTEGRATIONPOINTS_H */
