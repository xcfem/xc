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
//SeccionBarraPrismatica.cpp

#include "SeccionBarraPrismatica.h"
#include "material/section/interaction_diagram/DeformationPlane.h"
#include <domain/mesh/element/utils/Information.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <deque>


#include "material/section/ResponseId.h"
#include "preprocessor/loaders/MaterialLoader.h"
#include "material/section/interaction_diagram/InteractionDiagram.h"
#include "xc_utils/src/geom/d1/Recta3d.h"
#include "xc_utils/src/geom/d1/Recta2d.h"
#include "xc_utils/src/geom/d2/Semiplano2d.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "xc_utils/src/geom/sis_ref/EjesPrincInercia2d.h"
#include "xc_basic/src/util/inercia.h"

//! @brief Constructor.
XC::SeccionBarraPrismatica::SeccionBarraPrismatica(int tag, int classTag,MaterialLoader *mat_ldr)
  : SectionForceDeformation(tag,classTag,mat_ldr) {}

//! @brief Copy constructor.
XC::SeccionBarraPrismatica::SeccionBarraPrismatica(const SeccionBarraPrismatica &otro)
  : SectionForceDeformation(otro) {}

//! @brief Assignment operator.
XC::SeccionBarraPrismatica &XC::SeccionBarraPrismatica::operator=(const SeccionBarraPrismatica &otro)
  {
    SectionForceDeformation::operator=(otro);
    return *this;
  }

//! @brief Establece el plano de deformaciones de la sección.
int XC::SeccionBarraPrismatica::setTrialDeformationPlane(const DeformationPlane &plano)
  { return setTrialSectionDeformation(getVectorDeformacion(plano)); }

//! @brief Establece el plano de initial strains de la sección.
int XC::SeccionBarraPrismatica::setInitialDeformationPlane(const DeformationPlane &plano)
  { return setInitialSectionDeformation(getVectorDeformacion(plano)); }

//! @brief Returns initial deformation plane.
XC::DeformationPlane XC::SeccionBarraPrismatica::getInitialDeformationPlane(void) const
  { return DeformationPlane(getInitialSectionDeformation()); }


//! @brief Returns the generalized strains vector que corresponde
//! al plano de deformaciones being passed as parameter.
const XC::Vector &XC::SeccionBarraPrismatica::getVectorDeformacion(const DeformationPlane &plano) const
  {
    const int order= getOrder();
    const ResponseId &code= getType();
    return plano.getDeformation(order,code);
  }

//! @brief Returns the vector de deformaciones de la sección.
XC::DeformationPlane XC::SeccionBarraPrismatica::getDeformationPlane(void) const
  {
    DeformationPlane retval= DeformationPlane(getSectionDeformation());
    return retval;
  }

//! @brief Returns strain at postion being passed as parameter.
double XC::SeccionBarraPrismatica::getStrain(const double &y,const double &z) const
  {
    std::cerr << "getStrain not implemented for class: "
              << nombre_clase() << std::endl;
    return 0.0;
  }

//! @brief Returns the coordenada «y» del centro de gravedad de la sección.
double XC::SeccionBarraPrismatica::getCdgY(void) const
  { return 0.0; }

//! @brief Returns the coordenada «z» del centro de gravedad de la sección.
double XC::SeccionBarraPrismatica::getCdgZ(void) const
  { return 0.0; }

//! @brief Returns the posición del centro de gravedad de la sección.
Pos2d XC::SeccionBarraPrismatica::getCdg(void) const
  { return Pos2d(getCdgY(),getCdgZ()); }

//! @brief Returns true ifla sección está sometida
//! a axil.
bool XC::SeccionBarraPrismatica::hayAxil(const double &tol) const
  {
    bool retval= false;
    const ResponseId &code= getType();
    if(code.hasResponse(SECTION_RESPONSE_P))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_P))>=tol;
    return retval;
  }

//! @brief Returns internal axial force.
double XC::SeccionBarraPrismatica::getN(void) const
  { return getStressResultant(SECTION_RESPONSE_P); }

//! @brief Returns internal bending moment around y axis.
double XC::SeccionBarraPrismatica::getMy(void) const
  { return getStressResultant(SECTION_RESPONSE_MY); }

//! @brief Returns internal bending moment around z axis.
double XC::SeccionBarraPrismatica::getMz(void) const
  { return getStressResultant(SECTION_RESPONSE_MZ); }

//! @brief Returns the section axial stiffness.
const double &XC::SeccionBarraPrismatica::EA(void) const
  { return getSectionTangent()(0,0); }
//! @brief Returns the bending stiffness de la sección en torno al eje z.
const double &XC::SeccionBarraPrismatica::EIz(void) const
  { return getSectionTangent()(1,1); }
//! @brief Returns the bending stiffness de la sección en torno al eje y.
const double &XC::SeccionBarraPrismatica::EIy(void) const
  { return getSectionTangent()(2,2); }
//! @brief Returns the producto de inercia multiplicado por el
//! módulo de deformación.
const double &XC::SeccionBarraPrismatica::EIyz(void) const
  { return getSectionTangent()(1,2); }

//! @brief Returns the ángulo que define un eje principal de inercia.
double XC::SeccionBarraPrismatica::getTheta(void) const
  { return theta_inercia(EIy(),EIz(),EIyz()); }

//! @brief Returns the bending stiffness en
//! torno al eje de inercia principal mayor.
double XC::SeccionBarraPrismatica::getEI1(void) const
  { return I1_inercia(EIy(),EIz(),EIyz()); }

//! @brief Returns the bending stiffness en
//! torno al eje de inercia principal menor.
double XC::SeccionBarraPrismatica::getEI2(void) const
  { return I2_inercia(EIy(),EIz(),EIyz()); }

//! @brief Returns the ejes principales de inercia de la sección.
EjesPrincInercia2d XC::SeccionBarraPrismatica::getEjesInercia(void) const
  { return EjesPrincInercia2d(getCdg(),EIy(),EIz(),EIyz());  }
//! @brief Returns the vector del eje principal I.
Vector2d XC::SeccionBarraPrismatica::getVDirEje1(void) const
  { return getEjesInercia().getVDirEje1(); }
//! @brief Returns the vector del eje principal I.
Vector2d XC::SeccionBarraPrismatica::getVDirStrongAxis(void) const
  { return getVDirEje1(); }
//! @brief Returns the vector del eje principal II.
Vector2d XC::SeccionBarraPrismatica::getVDirEje2(void) const
  { return getEjesInercia().getVDirEje2(); }
//! @brief Returns the vector del eje principal II.
Vector2d XC::SeccionBarraPrismatica::getVDirWeakAxis(void) const
  { return getVDirEje2(); }

//! @brief Returns true ifla sección está sometida
//! a momento flector.
bool XC::SeccionBarraPrismatica::hayMomento(const double &tol) const
  {
    bool retval= false;
    const ResponseId &code= getType();
    if(code.hasResponse(SECTION_RESPONSE_MY))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_MY))>=tol;
    else if(code.hasResponse(SECTION_RESPONSE_MZ))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_MZ))>=tol;
    return retval;
  }

//! @brief Returns true ifla sección está sometida
//! a cortante.
bool XC::SeccionBarraPrismatica::hayCortante(const double &tol) const
  {
    bool retval= false;
    const ResponseId &code= getType();
    if(code.hasResponse(SECTION_RESPONSE_VY))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_VY))>=tol;
    else if(code.hasResponse(SECTION_RESPONSE_VZ))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_VZ))>=tol;
    return retval;
  }

//! @brief Returns true if the section está sometida
//! a axil.
bool XC::SeccionBarraPrismatica::hayTorsor(const double &tol) const
  {
    bool retval= false;
    const ResponseId &code= getType();
    if(code.hasResponse(SECTION_RESPONSE_T))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_T))>=tol;
    return retval;
  }

//! @brief Returns the fibra neutra.
Recta2d XC::SeccionBarraPrismatica::getFibraNeutra(void) const
  { return getDeformationPlane().getFibraNeutra(); }

//! @brief Returns the eje alienado con los esfuerzos que
//! actúan sobre la sección.
Recta2d XC::SeccionBarraPrismatica::getEjeEsfuerzos(void) const
  {
    Recta2d retval(getCdg(),Vector2d(1,0));
    const ResponseId &code= getType();
    if(hayMomento()) //Tomamos la dirección paralela al momento.
      {
        if(code.hasResponse(SECTION_RESPONSE_MY) && code.hasResponse(SECTION_RESPONSE_MZ))
          retval= Recta2d(getCdg(),Vector2d(getStressResultant(SECTION_RESPONSE_MY),getStressResultant(SECTION_RESPONSE_MZ)));
        else if(code.hasResponse(SECTION_RESPONSE_MY))
          retval= Recta2d(getCdg(),Vector2d(1,0));
        else if(code.hasResponse(SECTION_RESPONSE_MZ))
          retval= Recta2d(getCdg(),Vector2d(0,1));
      }
    else if(hayCortante()) //Tomamos la dirección perpendicular al cortante.
      {
        if(code.hasResponse(SECTION_RESPONSE_VY) && code.hasResponse(SECTION_RESPONSE_VZ))
          retval= Recta2d(getCdg(),Vector2d(-getStressResultant(SECTION_RESPONSE_VZ),getStressResultant(SECTION_RESPONSE_VY)));
        else if(code.hasResponse(SECTION_RESPONSE_VY))
          retval= Recta2d(getCdg(),Vector2d(0,1));
        else if(code.hasResponse(SECTION_RESPONSE_VZ))
          retval= Recta2d(getCdg(),Vector2d(1,0));
      }
    return retval;
  }

//! @brief Returns (if possible) un punto en el que las tensiones son de tracción.
Pos2d XC::SeccionBarraPrismatica::getPuntoSemiplanoTracciones(void) const
  { return getDeformationPlane().getPuntoSemiplanoTracciones(); }

//! @brief Returns (if possible) un punto en el que las tensiones son de compresion.
Pos2d XC::SeccionBarraPrismatica::getPuntoSemiplanoCompresiones(void) const
  { return getDeformationPlane().getPuntoSemiplanoCompresiones(); }

//! @brief Returns the semiplano cuyo borde es la recta que se pasa
//! como parámetro y en el que las tensiones son de tracción.
Semiplano2d XC::SeccionBarraPrismatica::getSemiplanoTracciones(const Recta2d &r) const
  { return getDeformationPlane().getSemiplanoTracciones(r); }

//! @brief Returns the semiplano cuyo borde es la fibra neutra
//! y en el que las tensiones son de tracción.
Semiplano2d XC::SeccionBarraPrismatica::getSemiplanoTracciones(void) const
  { return getDeformationPlane().getSemiplanoTracciones(); }

//! @brief Returns the semiplano cuyo borde es la recta que se pasa
//! como parámetro y en el que las tensiones son de compresión.
Semiplano2d XC::SeccionBarraPrismatica::getSemiplanoCompresiones(const Recta2d &r) const
  { return getDeformationPlane().getSemiplanoCompresiones(r); }

//! @brief Returns the semiplano cuyo borde es la fibra neutra
//! y en el que las tensiones son de compresión.
Semiplano2d XC::SeccionBarraPrismatica::getSemiplanoCompresiones(void) const
  { return getDeformationPlane().getSemiplanoCompresiones(); }

