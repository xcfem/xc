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
//SectionFDPhysicalProperties.cpp

#include "SectionFDPhysicalProperties.h"
#include <material/section/SectionForceDeformation.h>
#include <domain/domain/Domain.h>


#include "domain/mesh/element/gauss_models/GaussModel.h"


//! @brief Constructor
XC::SectionFDPhysicalProperties::SectionFDPhysicalProperties(const size_t &nMat,const SectionForceDeformation *ptr_mat)
  : PhysicalProperties<SectionForceDeformation>(nMat,ptr_mat)
  { }


//! @brief print out element data
void XC::SectionFDPhysicalProperties::Print(std::ostream &s, int flag )
  {
    if(flag == -1)
      {
        s << "SectionFDPhysicalProperties\t" << theMaterial.size() << "\t";
        s << std::endl;
      }
    else if(flag < -1)
      {
        int counter= (flag + 1) * -1;
        for(size_t i= 0;i < theMaterial.size();i++)
          {
            const Vector &stress= theMaterial[i]->getStressResultant();
            s << "STRESS\t" << counter << "\t" << i << "\tTOP";
            for(int j=0; j<6; j++)
              s << "\t" << stress(j);
            s << std::endl;
          }
      }
    else
      {
        s << std::endl;
        s << "Material Information : \n ";
        theMaterial[0]->Print( s, flag );
        s << std::endl;
      }
  }

//! Returns mean value of internal force.
double XC::SectionFDPhysicalProperties::getMeanInternalForce(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1") //Esfuerzo axil medio por unidad de longitud, paralelo al eje 1.
      retval= theMaterial.getMeanGeneralizedStress(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2") //Esfuerzo axil medio por unidad de longitud, paralelo al eje 2.
      retval= theMaterial.getMeanGeneralizedStress(MEMBRANE_RESPONSE_n2);
    else if(cod == "n12")
      retval= theMaterial.getMeanGeneralizedStress(MEMBRANE_RESPONSE_n12);
    else if(cod == "m1") //Flector medio por unidad de longitud, en torno al eje 1.
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector medio por unidad de longitud, en torno al eje 2.
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_m2);
    else if(cod == "m12")
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_m12);
    else if(cod == "q13")
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_q23);
    else
      std::cerr << "Unknown internal force: '"
                << cod << "'\n";
    return retval;
  }

//! Returns mean value of internal force.
double XC::SectionFDPhysicalProperties::getMeanInternalDeformation(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1") //Esfuerzo axil medio por unidad de longitud, paralelo al eje 1.
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2") //Esfuerzo axil medio por unidad de longitud, paralelo al eje 2.
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n2);
    else if(cod == "n12")
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n12);
    else if(cod == "m1") //Flector medio por unidad de longitud, en torno al eje 1.
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector medio por unidad de longitud, en torno al eje 2.
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_m2);
    else if(cod == "m12")
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_m12);
    else if(cod == "q13")
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_q23);
    else
      std::cerr << "Unknown internal deformation: '"
                << cod << "'\n";
    return retval;
  }

//! @brief check to see if have mass
bool XC::SectionFDPhysicalProperties::haveRho(void) const
  {
    const size_t numMaterials= theMaterial.size();
    bool retval= false;
    for(size_t i=0; i<numMaterials; i++)
      {
        if(theMaterial[i]->getRho() != 0.0)
	  {
	    retval= true;
            break;
          }
      }
    return retval;
  }

//! @brief Returns densities for each position.
XC::Vector XC::SectionFDPhysicalProperties::getRhoi(void) const
  {
    const size_t numMaterials= theMaterial.size();
    Vector retval(numMaterials);
    for(size_t i=0; i<numMaterials; i++)
      retval[i]= theMaterial[i]->getRho();
    return retval;
  }

