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
// Copyright (c) 1999, A.H. van den Boogaard

#include "IntegrationPoints.h"
#include <sstream>

//! Abscissa and weight of an integration point.
//!
//! Given an integration point defined by its index
//! and the total number of integration points to use
//! , this routine returns its abscissae and its weight.
//! 
//! @param ip: index of the integration point.
//! @param nip: number of integration points.
//! @param xi: abscissa of the intetration point.
//! @param weight: weight of the integration point.
void Gauss( int ip, int nip, double& xi, double& weight )
  {
    if ( ip<1 || ip>nip )
     {
       std::cerr << "wrong integration point number:" << ip <<
                  " of " << nip << " available";
     }

   switch(nip)
     {
     case  1:
       xi = 0.0;
       weight = 2.0;
       break;
     case  2:
       switch ( ip )
         {
         case 1:
           xi = -0.577350269189626;
           break;
         case 2:
           xi =  0.577350269189626;
           break;
         }
       weight = 1.0;
       break;
     case  3:
       switch ( ip )
         {
         case 1:
           xi     = -0.774596669241483;
           weight =  0.555556;
           break;
         case 2:
           xi     =  0.0;
           weight =  0.888888888888889;
           break;
         case 3:
           xi     =  0.774596669241483;
           weight =  0.555555555555556;
           break;
         }
       break;
     case  4:
       switch ( ip )
         {
         case 1:
           xi     = -0.861136311594053;
           weight =  0.347854845137454;
           break;
         case 2:
           xi     = -0.339981043584856;
           weight =  0.652145154862546;
           break;
         case 3:
           xi     =  0.339981043584856;
           weight =  0.652145154862546;
           break;
         case 4:
           xi     =  0.861136311594053;
           weight =  0.347854845137454;
           break;
         }
       break;
     case  5:
       switch ( ip )
         {
         case 1:
           xi     = -0.906179845938664;
           weight =  0.236926885056189;
           break;
         case 2:
           xi     = -0.538469310105683;
           weight =  0.478628670499366;
           break;
         case 3:
           xi     =  0.0;
           weight =  0.568888888888889;
           break;
         case 4:
           xi     =  0.906179845938664;
           weight =  0.236926885056189;
           break;
         case 5:
           xi     =  0.538469310105683;
           weight =  0.478628670499366;
           break;
         }
       break;
     case  6:
       switch ( ip )
         {
         case 1:
           xi     = -0.932469514203152;
           weight =  0.171324492379170;
           break;
         case 2:
           xi     = -0.661209386466265;
           weight =  0.360761573048139;
           break;
         case 3:
           xi     = -0.238619186083197;
           weight =  0.467913934572691;
           break;
         case 4:
           xi     =  0.932469514203152;
           weight =  0.171324492379170;
           break;
         case 5:
           xi     =  0.661209386466265;
           weight =  0.360761573048139;
           break;
         case 6:
           xi     =  0.238619186083197;
           weight =  0.467913934572691;
           break;
         }
       break;
     default:
       { 
         std::cerr << "wrong nr of integration points: " << nip;
       }
     }
  }

//! Abscissa and weight of an integration point on a triangular domain
//!
//! Given an integration point defined by its index
//! and the total number of integration points to use
//! , this routine returns its abscissae and its weight.
//! 
//! @param ip: index of the integration point.
//! @param nip: number of integration points.
//! @param L1: First natural coordinate of the integration point.
//! @param L2: Second natural coordinate of the integration point.
//! @param L3: Third natural coordinate of the integration point.
//! @param weight: weight of the integration point.
void Hammer( int ip, int nip, double& L1, double& L2, double& L3,
             double& weight )
  {
    if ( ip<1 || ip>nip )
      {
	std::cerr << "wrong integration point number:" << ip <<
                     " of " << nip << " available";
      }

   switch ( nip )
     {
       case  1:
         L1 = L2 = L3 = 0.333333333333333;
         weight = 0.5;
         break;
    /*  case  3: switch ( ip )   // alternative scheme degree of precision = 2
                                 // unsuitable for axisymmetric, division by r=0
               {
               case 1: L1 = 0.5; L2 = 0.5; L3 = 0.0;
                       break;
               case 2: L1 = 0.5; L2 = 0.0; L3 = 0.5;
                       break;
               case 3: L1 = 0.0; L2 = 0.5; L3 = 0.5;
                       break;
               }
               weight = 0.166666666666667;   // 1.0/6.0;
               break; */
       case  3:
         switch ( ip )    // alternative scheme  degree of precision = 2
           {
           case 1: 
             L1 = 0.166666666666667; L2 = 0.166666666666667; L3 = 0.666666666666667;
             break;
           case 2:
             L1 = 0.666666666666667; L2 = 0.166666666666667; L3 = 0.166666666666667;
             break;
           case 3:
             L1 = 0.166666666666667; L2 = 0.666666666666667; L3 = 0.166666666666667;
             break;
           }
           weight = 0.166666666666667;   // 1.0/6.0;
           break;
       case  4:
         switch ( ip )
           {
           case 1:
             L1 = L2 = L3 = 0.333333333333333;  // 1.0/3.0;
             weight =  -0.28125;   // -9.0/32.0;
             break;
           case 2:
             L1 = 0.6; L2 = L3 = 0.2;
             weight =  0.260416666666667;   // 25.0/96.0;
             break;
           case 3:
             L2 = 0.6; L1 = L3 = 0.2;
             weight =  0.260416666666667;   // 25.0/96.0;
             break;
           case 4:
             L1 = L2 = 0.2; L3 = 0.6;
             weight =  0.260416666666667;   //25.0/96.0;
             break;
           }
               break;
       case  7:
         switch ( ip )   // degree of precision = 5
           {
           case 1:
             L1 = L2 = 0.1012865073235; L3 = 0.7974269853531;
             weight = 0.0629695902724;
             break;
           case 2:
             L1 = 0.7974269853531; L2 = L3 = 0.1012865073235;
             weight = 0.0629695902724;
             break;
           case 3: L1 = L3 = 0.1012865073235; L2 = 0.7974269853531;
             weight = 0.0629695902724;
             break;
           case 4: L1 = L3 = 0.4701420641051; L2 = 0.0597158717898;
             weight =  0.0661970763943;
             break;
           case 5:
             L1 = L2 = 0.4701420641051; L3 = 0.0597158717898;
             weight =  0.0661970763943;
             break;
           case 6:
             L2 = L3 = 0.4701420641051; L1 = 0.0597158717898;
             weight =  0.0661970763943;
             break;
           case 7:
             L1 = L2 = L3 = 0.3333333333333;
             weight = 0.1125;
             break;
           }
         break;
       default:
         {
	   std::cerr << "wrong nr of integration points: " << nip;
         }
     }
  }
//! @brief Constructor por defecto.
IntegrationPoints::IntegrationPoints()
  {
    nr_of_intpt = 0;
    intpts = NULL;
  }

//! @brief Constructor.
//!
//! @param nr: Number of integration points.
IntegrationPoints::IntegrationPoints( int nr )
  {
    nr_of_intpt = nr;
    intpts = new pto_int[nr];
  }

//! @brief Copy constructor.
IntegrationPoints::IntegrationPoints( const IntegrationPoints& intlocs )
  {
    nr_of_intpt = intlocs.nr_of_intpt;
    if( nr_of_intpt == 0 )
      {
        intpts = NULL;
        return;
      }
    intpts = new pto_int[nr_of_intpt];
    for ( int ip=0; ip<nr_of_intpt; ip++ )
      {
        intpts[ip] = intlocs.intpts[ip];
      }
  }

//! @brief Destructor.
IntegrationPoints::~IntegrationPoints()
  { if ( intpts != NULL ) delete [] intpts; }

//! @brief Set the number of integration points.
void IntegrationPoints::set_nr_intpt( int nr )
  {
    if ( intpts != NULL ) delete [] intpts;
    nr_of_intpt = nr;
    intpts = new pto_int[nr];
  }

//! @brief Return the number of integration points.
int IntegrationPoints::get_nr_intpt() const
  { return nr_of_intpt; }

//! @brief Sets the natural coordinates and the weight to the
//! integration point.
//!
//! @param ip: index of the integration point.
//! @param coor: natural coordinates.
//! @param wght: weight of the integration point.
void IntegrationPoints::set_values( int ip,const m_double &coor,const double &wght)
  {
    if( ip < 1 || ip > nr_of_intpt )
      {
        std::cerr << "wrong integration point number: " << ip << std::endl
		  << "should be between 1 and " << nr_of_intpt;
      }
    intpts[ip-1].nat_coord = coor;
    intpts[ip-1].weight = wght;
  }
//! @brief Return the natural coordinates of the integration point.
//!
//! @param ip: index of the integration point.
m_double IntegrationPoints::get_coor(int ip) const
  { return ( intpts[ip-1].nat_coord ); }

//! @brief Return the weigth of the integration point with the
//! index beint passed as parmeter.
double IntegrationPoints::get_weight(int ip) const
  { return ( intpts[ip-1].weight ); }
