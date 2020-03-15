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
//----------------------------------------------------------------------------
//IntegrationPointsCoords.cc

#include "IntegrationPointsCoords.h"
#include "BeamIntegration.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf.h"
#include "xc_utils/src/functions/algebra/MapValores.h"
#include "xc_utils/src/functions/algebra/ExprAlgebra.h"

XC::IntegrationPointsCoords::IntegrationPointsCoords(const BeamIntegration &bInteg,int nIP,const CrdTransf &trf)
  : rst(nIP,3), rnsntn(nIP,3), xyz(nIP,3), XYZ(bInteg.getIntegrPointGlobalCoords(nIP,trf))
  {
    const Matrix &naturales= bInteg.getIntegrPointNaturalCoords(nIP,trf);
    rst.Zero();
    for(int i= 0;i<nIP;i++)
      rst(i,1)= naturales(i,1);
    
    const Matrix &normalized= bInteg.getIntegrPointNormalizedCoords(nIP,trf);
    rnsntn.Zero();
    for(int i= 0;i<nIP;i++)
      rnsntn(i,1)= normalized(i,1);

    const Matrix &locales= bInteg.getIntegrPointLocalCoords(nIP,trf);
    xyz.Zero();
    for(int i= 0;i<nIP;i++)
      xyz(i,1)= locales(i,1);

    //Global ones are already prepared.
  }

//! @brief Returns the mapa de valores necesario para evaluar la expresión.
MapValores XC::IntegrationPointsCoords::getMapValores(const size_t &i,const std::vector<std::string> &namesOfVariables) const
  {
    MapValores retval;
    for(std::vector<std::string>::const_iterator j= namesOfVariables.begin();j!=namesOfVariables.end();j++)
      {
        const std::string &nmb= *j;
        if(nmb == "r")
          retval["r"]= ExprAlgebra(r(i));
        else if(nmb == "s")
          retval["s"]= ExprAlgebra(s(i));
        else if(nmb == "t")
          retval["t"]= ExprAlgebra(t(i));
        else if(nmb == "rn")
          retval["rn"]= ExprAlgebra(rn(i));
        else if(nmb == "sn")
          retval["sn"]= ExprAlgebra(sn(i));
        else if(nmb == "tn")
          retval["tn"]= ExprAlgebra(tn(i));
        else if(nmb == "x")
          retval["x"]= ExprAlgebra(x(i));
        else if(nmb == "y")
          retval["y"]= ExprAlgebra(y(i));
        else if(nmb == "z")
          retval["z"]= ExprAlgebra(z(i));
        else if(nmb == "X")
          retval["X"]= ExprAlgebra(X(i));
        else if(nmb == "Y")
          retval["Y"]= ExprAlgebra(Y(i));
        else if(nmb == "Z")
          retval["Z"]= ExprAlgebra(Z(i));
        else
	  std::cerr << "IntegrationPointsCoords" << "::" << __FUNCTION__
	            << " variable: '"
                    << nmb << "' not found.\n";
      }
    return retval;
  }

//! @brief Returns the values of the expression on each integration point.
const XC::Vector &XC::IntegrationPointsCoords::eval(const ExprAlgebra &expr) const
  {
    const size_t nIP= rst.noRows();
    static Vector retval;
    retval.resize(nIP);
    retval.Zero();
    std::vector<std::string> names= expr.getNamesOfVariables();
    for(size_t i= 0;i<nIP;i++)
      retval[i]= expr.ToNum(getMapValores(i,names));
    return retval;
  }
