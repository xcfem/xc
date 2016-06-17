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
//BeamColumnWithSectionFDTrf2d.cpp

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf2d.h>
#include <domain/mesh/element/coordTransformation/CrdTransf2d.h>


//! @brief Asigna la transformación de coordenadas.
void XC::BeamColumnWithSectionFDTrf2d::set_transf(const CrdTransf *trf)
  {
    if(theCoordTransf)
      {
        delete theCoordTransf;
        theCoordTransf= nullptr;
      }
    if(trf)
      {
        const CrdTransf2d *tmp= dynamic_cast<const CrdTransf2d *>(trf);
        if(tmp)
          theCoordTransf = tmp->getCopy();
        else
          {
            std::cerr << "XC::BeamColumnWithSectionFDTrf2d::set_transf -- failed to get copy of coordinate transformation\n";
            exit(1);
          }
      }
    else
      std::cerr << "BeamColumnWithSectionFDTrf3d::set_transf; se pasó un puntero nulo." << std::endl;
  }

XC::BeamColumnWithSectionFDTrf2d::BeamColumnWithSectionFDTrf2d(int tag, int classTag,int numSec)
  :BeamColumnWithSectionFD(tag, classTag,numSec), theCoordTransf(nullptr) {}

XC::BeamColumnWithSectionFDTrf2d::BeamColumnWithSectionFDTrf2d(int tag, int classTag,int numSec,const Material *mat,const CrdTransf *coordTransf)
  :BeamColumnWithSectionFD(tag, classTag,numSec,mat), theCoordTransf(nullptr)
  { set_transf(coordTransf); }

XC::BeamColumnWithSectionFDTrf2d::BeamColumnWithSectionFDTrf2d(int tag, int classTag,int numSec, int nodeI, int nodeJ,CrdTransf2d &coordTransf)
  :BeamColumnWithSectionFD(tag, classTag,numSec,nullptr,nodeI,nodeJ), theCoordTransf(nullptr)
  { set_transf(&coordTransf); }

//! @brief Constructor de copia.
XC::BeamColumnWithSectionFDTrf2d::BeamColumnWithSectionFDTrf2d(const BeamColumnWithSectionFDTrf2d &otro)
  :BeamColumnWithSectionFD(otro), theCoordTransf(nullptr)
  { set_transf(otro.theCoordTransf); }

//! @brief Operador asignación.
XC::BeamColumnWithSectionFDTrf2d &XC::BeamColumnWithSectionFDTrf2d::operator=(const BeamColumnWithSectionFDTrf2d &otro)
  {
    //BeamColumnWithSectionFD::operator=(otro);
    //set_transf(otro.theCoordTransf);
    std::cerr << "BeamColumnWithSectionFDTrf2d; operador asignación no implementado." << std::endl;
    return *this;
  }

//! @brief Destructor.
XC::BeamColumnWithSectionFDTrf2d::~BeamColumnWithSectionFDTrf2d(void)
  {
    if(theCoordTransf) delete theCoordTransf;
    theCoordTransf= nullptr;
  }

void XC::BeamColumnWithSectionFDTrf2d::initialize_trf(void)
  {
    if(theCoordTransf->initialize(theNodes[0], theNodes[1]))
      {
        std::cerr << "XC::BeamColumnWithSectionFDTrf2d::initialize_trf() -- failed to initialize coordinate transformation\n";
        exit(-1);
      }
  }

//! @brief Devuelve (si puede) un apuntador a la transformación de coordenadas.
XC::CrdTransf *XC::BeamColumnWithSectionFDTrf2d::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Devuelve (si puede) un apuntador a la transformación de coordenadas.
const XC::CrdTransf *XC::BeamColumnWithSectionFDTrf2d::getCoordTransf(void) const
  { return theCoordTransf; }

//! @brief Send members through the channel being passed as parameter.
int XC::BeamColumnWithSectionFDTrf2d::sendData(CommParameters &cp)
  {
    int res= BeamColumnWithSectionFD::sendData(cp);
    res+= sendCoordTransf(9,10,11,cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::BeamColumnWithSectionFDTrf2d::recvData(const CommParameters &cp)
  {
    int res= BeamColumnWithSectionFD::recvData(cp);
    theCoordTransf= recvCoordTransf2d(9,10,11,cp);
    return res;
  }
