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
//SeccionBarraPrismatica.cpp

#include "SeccionBarraPrismatica.h"
#include "material/section/interaction_diagram/PlanoDeformacion.h"
#include <domain/mesh/element/Information.h>
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

//! @brief Constructor de copia.
XC::SeccionBarraPrismatica::SeccionBarraPrismatica(const SeccionBarraPrismatica &otro)
  : SectionForceDeformation(otro) {}

//! @brief Operador asignación.
XC::SeccionBarraPrismatica &XC::SeccionBarraPrismatica::operator=(const SeccionBarraPrismatica &otro)
  {
    SectionForceDeformation::operator=(otro);
    return *this;
  }

//! @brief Establece el plano de deformaciones de la sección.
int XC::SeccionBarraPrismatica::setTrialDeformationPlane(const PlanoDeformacion &plano)
  { return setTrialSectionDeformation(getVectorDeformacion(plano)); }

//! @brief Establece el plano de deformaciones iniciales de la sección.
int XC::SeccionBarraPrismatica::setInitialDeformationPlane(const PlanoDeformacion &plano)
  { return setInitialSectionDeformation(getVectorDeformacion(plano)); }

//! @brief Devuelve el vector de deformaciones generalizadas que corresponde
//! al plano de deformaciones being passed as parameter.
const XC::Vector &XC::SeccionBarraPrismatica::getVectorDeformacion(const PlanoDeformacion &plano) const
  {
    const int order= getOrder();
    const ResponseId &code= getType();
    return plano.getDeformation(order,code);
  }

//! @brief Devuelve el vector de deformaciones de la sección.
XC::PlanoDeformacion XC::SeccionBarraPrismatica::getPlanoDeformacion(void) const
  {
    PlanoDeformacion retval= PlanoDeformacion(getSectionDeformation());
    return retval;
  }

//! @brief Devuelve la coordenada «y» del centro de gravedad de la sección.
double XC::SeccionBarraPrismatica::getCdgY(void) const
  { return 0.0; }

//! @brief Devuelve la coordenada «z» del centro de gravedad de la sección.
double XC::SeccionBarraPrismatica::getCdgZ(void) const
  { return 0.0; }

//! @brief Devuelve la posición del centro de gravedad de la sección.
Pos2d XC::SeccionBarraPrismatica::getCdg(void) const
  { return Pos2d(getCdgY(),getCdgZ()); }

//! @brief Devuelve verdadero si la sección está sometida
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

//! @brief Devuelve la rigidez a tracción de la sección.
const double &XC::SeccionBarraPrismatica::EA(void) const
  { return getSectionTangent()(0,0); }
//! @brief Devuelve la rigidez a flexión de la sección en torno al eje z.
const double &XC::SeccionBarraPrismatica::EIz(void) const
  { return getSectionTangent()(1,1); }
//! @brief Devuelve la rigidez a flexión de la sección en torno al eje y.
const double &XC::SeccionBarraPrismatica::EIy(void) const
  { return getSectionTangent()(2,2); }
//! @brief Devuelve el producto de inercia multiplicado por el
//! módulo de deformación.
const double &XC::SeccionBarraPrismatica::EIyz(void) const
  { return getSectionTangent()(1,2); }

//! @brief Devuelve el ángulo que define un eje principal de inercia.
double XC::SeccionBarraPrismatica::getTheta(void) const
  { return theta_inercia(EIy(),EIz(),EIyz()); }

//! @brief Devuelve la rigidez a flexión en
//! torno al eje de inercia principal mayor.
double XC::SeccionBarraPrismatica::getEI1(void) const
  { return I1_inercia(EIy(),EIz(),EIyz()); }

//! @brief Devuelve la rigidez a flexión en
//! torno al eje de inercia principal menor.
double XC::SeccionBarraPrismatica::getEI2(void) const
  { return I2_inercia(EIy(),EIz(),EIyz()); }

//! @brief Devuelve los ejes principales de inercia de la sección.
EjesPrincInercia2d XC::SeccionBarraPrismatica::getEjesInercia(void) const
  { return EjesPrincInercia2d(getCdg(),EIy(),EIz(),EIyz());  }
//! @brief Devuelve el vector del eje principal I.
Vector2d XC::SeccionBarraPrismatica::getVDirEje1(void) const
  { return getEjesInercia().getVDirEje1(); }
//! @brief Devuelve el vector del eje principal I.
Vector2d XC::SeccionBarraPrismatica::getVDirEjeFuerte(void) const
  { return getVDirEje1(); }
//! @brief Devuelve el vector del eje principal II.
Vector2d XC::SeccionBarraPrismatica::getVDirEje2(void) const
  { return getEjesInercia().getVDirEje2(); }
//! @brief Devuelve el vector del eje principal II.
Vector2d XC::SeccionBarraPrismatica::getVDirEjeDebil(void) const
  { return getVDirEje2(); }

//! @brief Devuelve verdadero si la sección está sometida
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

//! @brief Devuelve verdadero si la sección está sometida
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

//! @brief Devuelve verdadero si la sección está sometida
//! a axil.
bool XC::SeccionBarraPrismatica::hayTorsor(const double &tol) const
  {
    bool retval= false;
    const ResponseId &code= getType();
    if(code.hasResponse(SECTION_RESPONSE_T))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_T))>=tol;
    return retval;
  }

//! @brief Devuelve la fibra neutra.
Recta2d XC::SeccionBarraPrismatica::getFibraNeutra(void) const
  { return getPlanoDeformacion().getFibraNeutra(); }

//! @brief Devuelve el eje alienado con los esfuerzos que
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

//! @brief Devuelve (si puede) un punto en el que las tensiones son de tracción.
Pos2d XC::SeccionBarraPrismatica::getPuntoSemiplanoTracciones(void) const
  { return getPlanoDeformacion().getPuntoSemiplanoTracciones(); }

//! @brief Devuelve (si puede) un punto en el que las tensiones son de compresion.
Pos2d XC::SeccionBarraPrismatica::getPuntoSemiplanoCompresiones(void) const
  { return getPlanoDeformacion().getPuntoSemiplanoCompresiones(); }

//! @brief Devuelve el semiplano cuyo borde es la recta que se pasa
//! como parámetro y en el que las tensiones son de tracción.
Semiplano2d XC::SeccionBarraPrismatica::getSemiplanoTracciones(const Recta2d &r) const
  { return getPlanoDeformacion().getSemiplanoTracciones(r); }

//! @brief Devuelve el semiplano cuyo borde es la fibra neutra
//! y en el que las tensiones son de tracción.
Semiplano2d XC::SeccionBarraPrismatica::getSemiplanoTracciones(void) const
  { return getPlanoDeformacion().getSemiplanoTracciones(); }

//! @brief Devuelve el semiplano cuyo borde es la recta que se pasa
//! como parámetro y en el que las tensiones son de compresión.
Semiplano2d XC::SeccionBarraPrismatica::getSemiplanoCompresiones(const Recta2d &r) const
  { return getPlanoDeformacion().getSemiplanoCompresiones(r); }

//! @brief Devuelve el semiplano cuyo borde es la fibra neutra
//! y en el que las tensiones son de compresión.
Semiplano2d XC::SeccionBarraPrismatica::getSemiplanoCompresiones(void) const
  { return getPlanoDeformacion().getSemiplanoCompresiones(); }

