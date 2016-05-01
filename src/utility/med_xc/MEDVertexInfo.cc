//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//MEDVertexInfo

#include "MEDVertexInfo.h"



#include "domain/mesh/Mesh.h"
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/node/Node.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! Constructor.
XC::MEDVertexInfo::MEDVertexInfo(void)
  : spaceDimension(3), tipoCoordenadas("CARTESIAN") {}

//! Constructor.
XC::MEDVertexInfo::MEDVertexInfo(const Mesh &mesh)
  : spaceDimension(3), tipoCoordenadas("CARTESIAN"), nombresCoordenadas(mesh.getNombresCoordenadas())
  {
    nombresUnidades= std::vector<std::string>(spaceDimension, mesh.getNombreUnidades());
    const Node *theNode= nullptr;
    Mesh &mesh_no_const= const_cast<Mesh &>(mesh);
    NodeIter &theNodes = mesh_no_const.getNodes();
    while((theNode = theNodes()) != nullptr)
      {
        const int tag= theNode->getTag();
        const Pos3d pos= theNode->getPosInicial3d();
        std::vector<double> coo(3);
        coo[0]= pos.x();coo[1]= pos.y();coo[2]= pos.z();
        nuevo_vertice(tag,coo);
      }
  }

//! Borra la malla.
void XC::MEDVertexInfo::clear(void)
  {
    coordenadas.clear();
    MEDBaseInfo::clear();
  }

//! @brief Agrega las coordenadas del vértice de índice i.
void XC::MEDVertexInfo::nuevo_vertice(size_t i,const std::vector<double> &coo)
  {
    const size_t sz= coo.size();
    if(sz>=spaceDimension)
      {
        for(size_t j= 0;j<spaceDimension;j++)
          coordenadas.push_back(coo[j]);
        nuevo_tag(i);
      }
    else
      std::cerr << "MEDVertexInfo::nuevo_vertice; se esperaban: "
                << spaceDimension << " coordenadas, se obtuvieron: "
                << sz << std::endl;
  }

//! @brief Agrega las coordenadas del vértice de índice i.
void XC::MEDVertexInfo::newVertex(const size_t &i,const boost::python::list &l)
  {
    const size_t sz= len(l);
    std::vector<double> coo(sz,0.0);
    for(size_t j=0; j<sz; j++)
      coo[j]= boost::python::extract<double>(l[j]);
    nuevo_vertice(i,coo);
  }

//! @brief Acceso al vector de coordenadas.
const std::vector<double> &XC::MEDVertexInfo::getCoordenadas(void) const
  { return coordenadas; }

//! @brief Assigns coordinate names.
void XC::MEDVertexInfo::setCoordinateNames(const boost::python::list &l)
  {
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      nombresCoordenadas.push_back(boost::python::extract<std::string>(l[i]));
  }

//! @brief Assigns coordinate units.
void XC::MEDVertexInfo::setUnitNames(const boost::python::list &l)
  {
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      nombresUnidades.push_back(boost::python::extract<std::string>(l[i]));
  }

//! @brief Vuelca la definición de las celdas en la
//! malla MED
void XC::MEDVertexInfo::to_med(MEDMEM::MESHING &malla) const
  {
    const int numNodos= getNumVertices();
    const std::vector<double> &coo= getCoordenadas();
    const size_t &sd= getSpaceDimension();
    if(sd>0)
      {
        malla.setCoordinates(getSpaceDimension(),numNodos,&coo[0],getTipoCoordenadas(),::MED_FULL_INTERLACE);
        if(nombresCoordenadas.size()<sd)
          std::cerr << "No se han especificado los nombres de las coordenadas."
                    << std::endl;
        else 
          malla.setCoordinatesNames(&nombresCoordenadas[0]);
        if(nombresCoordenadas.size()<sd)
          std::cerr << "No se han especificado las unidades de las coordenadas."
                    << std::endl;
        malla.setCoordinatesUnits(&nombresUnidades[0]);
      }
    else
      std::cerr << "Espacio de dimensión cero." << std::endl;
  }
