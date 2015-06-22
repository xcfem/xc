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
//ResponseId.h
//Etiquetas de los esfuerzos o GDLs para los que el material aporta rigidez.

#ifndef ResponseId_h
#define ResponseId_h

#include "utility/matrix/ID.h"

namespace XC {
 class Channel;
 class FEM_ObjectBroker;


const int SECTION_RESPONSE_MZ= 1; // Curvature about the section z-axis
const int SECTION_RESPONSE_P= 2; // Axial strain
const int SECTION_RESPONSE_VY= 3;
const int SECTION_RESPONSE_MY= 4; // Curvature about the section y-axis
const int SECTION_RESPONSE_VZ= 5;
const int SECTION_RESPONSE_T= 6; // Torsión.


const int MEMBRANE_RESPONSE_n1= 11; // Axil por unidad de longitud, paralelo al eje 1
const int MEMBRANE_RESPONSE_n2= 12; // Axil por unidad de longitud, paralelo al eje 2
const int MEMBRANE_RESPONSE_n12= 13; // Esfuerzo rasante por unidad de longitud.
const int PLATE_RESPONSE_m1= 14; // Momento por unidad de longitud, paralelo al eje 1.
const int PLATE_RESPONSE_m2= 15; // Momento por unidad de longitud, paralelo al eje 2.
const int PLATE_RESPONSE_m12= 16; // Esfuerzo torsor por unidad de longitud.
const int PLATE_RESPONSE_q13= 17; // Cortante por unidad de longitud, perpendicular al eje 1 y paralelo al eje 3.
const int PLATE_RESPONSE_q23= 18; // Cortante por unidad de longitud, perpendicular al eje 2 y paralelo al eje 3.

//! \ingroup MATSCC
//
//! @brief Etiquetas de los esfuerzos o de los GDLs para los que el material aporta rigidez.
class ResponseId: public ID
  {
  public:
    ResponseId(int sz);
    ResponseId(const std::deque<std::string> &indices);
    ResponseId(const std::vector<std::string> &indices);
    ResponseId(const ResponseId &otro);
    ResponseId(int *data, int size, bool cleanIt = false);
    explicit ResponseId(const ID &);
    bool hasResponse(const int &ri) const;
    static int StringToRespId(const std::string &str);
    static std::string RespIdToString(const int &ri);
    std::string getString(void) const;
  };

class RespPMz: public ResponseId
  {
  public:
    RespPMz(void);
  };

class RespPMzV: public ResponseId
  {
  public:
    RespPMzV(void);
  };

class RespVyP: public ResponseId
  {
  public:
    RespVyP(void);
  };

class RespPMzMy: public ResponseId
  {
  public:
    RespPMzMy(void);
  };

class RespPMzMyT: public ResponseId
  {
  public:
    RespPMzMyT(void);
  };

class RespPMzVyMyVzT: public ResponseId
  {
  public:
    RespPMzVyMyVzT(void);
  };

class RespFiberSectionShear3d: public ResponseId
  {
  public:
    RespFiberSectionShear3d(void);
  };

class RespPVyMz: public ResponseId
  {
  public:
    RespPVyMz(void);
  };

class RespMembraneMaterial: public ResponseId
  {
  public:
    RespMembraneMaterial(void);
  };

class RespPlateMaterial: public ResponseId
  {
  public:
    RespPlateMaterial(void);
  };

class RespShellMaterial: public ResponseId
  {
  public:
    RespShellMaterial(void);
  };

const RespPMz RespElasticSection2d;
const RespPMzV RespElasticShSection2d;
const RespVyP RespBidirectional;
const RespPMzMy RespFiberSection3d;
const RespPMzMyT RespElasticSection3d;
const RespPMzVyMyVzT RespElasticShSection3d;
const RespFiberSectionShear3d RespFiberSectionSh3d;
const RespPVyMz RespIsolator2spring;
const RespMembraneMaterial RespMembraneMat;
const RespPlateMaterial RespPlateMat;
const RespShellMaterial RespShellMat;

} // fin namespace XC

#endif
