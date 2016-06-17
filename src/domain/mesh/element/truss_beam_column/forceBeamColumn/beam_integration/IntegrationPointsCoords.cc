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
//IntegrationPointsCoords.cc

#include "IntegrationPointsCoords.h"
#include "BeamIntegration.h"
#include "domain/mesh/element/coordTransformation/CrdTransf.h"
#include "xc_basic/src/funciones/algebra/MapValores.h"
#include "xc_basic/src/funciones/algebra/ExprAlgebra.h"

XC::IntegrationPointsCoords::IntegrationPointsCoords(const BeamIntegration &bInteg,int nIP,const CrdTransf &trf)
  : rst(nIP,3), rnsntn(nIP,3), xyz(nIP,3), XYZ(bInteg.getIntegrPointGlobalCoords(nIP,trf))
  {
    const Matrix &naturales= bInteg.getIntegrPointNaturalCoords(nIP,trf);
    rst.Zero();
    for(int i= 0;i<nIP;i++)
      rst(i,1)= naturales(i,1);
    
    const Matrix &normalizadas= bInteg.getIntegrPointNormalizedCoords(nIP,trf);
    rnsntn.Zero();
    for(int i= 0;i<nIP;i++)
      rnsntn(i,1)= normalizadas(i,1);

    const Matrix &locales= bInteg.getIntegrPointLocalCoords(nIP,trf);
    xyz.Zero();
    for(int i= 0;i<nIP;i++)
      xyz(i,1)= locales(i,1);

    //Las globales ya están listas.
  }

//! @brief Devuelve el mapa de valores necesario para evaluar la expresión.
MapValores XC::IntegrationPointsCoords::getMapValores(const size_t &i,const std::vector<std::string> &nombresVariables) const
  {
    MapValores retval;
    for(std::vector<std::string>::const_iterator j= nombresVariables.begin();j!=nombresVariables.end();j++)
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
	  std::cerr << "IntegrationPointsCoords::getMapValores; no se encontró la variable: '"
                    << nmb << "'\n";
      }
    return retval;
  }

//! @brief Evalúa la expresión being passed as parameter en los puntos de integración.
const XC::Vector &XC::IntegrationPointsCoords::eval(const ExprAlgebra &expr) const
  {
    const size_t nIP= rst.noRows();
    static Vector retval;
    retval.resize(nIP);
    retval.Zero();
    std::vector<std::string> nombres= expr.getNombresVariables();
    for(size_t i= 0;i<nIP;i++)
      retval[i]= expr.ToNum(getMapValores(i,nombres));
    return retval;
  }
