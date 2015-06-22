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
//MEDMesh

#include "MEDMesh.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/Lista.h"
#include "utility/matrix/Matrix.h"

//! @brief Libera la memoria ocupada.
void XC::MEDMesh::libera(void) const
  {
    if(malla)
      {
        delete malla;
        malla= nullptr;
      }
  }

//! @brief Reserva memoria para la malla MED.
void XC::MEDMesh::alloc(const std::string &fName) const
  {
    libera();
    malla= new MEDMEM::MESH(MEDMEM::MED_DRIVER,fName,meshName);
    if(!malla)
      std::cerr << "MEDMesh::alloc; problema al leer la malla: "
                << meshName << " en el archivo: " << fName << std::endl;
  }

void XC::MEDMesh::alloc(const MEDMesh &otro) const
  {
    libera();
    if(otro.malla)
      malla= new MEDMEM::MESH(*otro.malla);
  }

//! Constructor.
XC::MEDMesh::MEDMesh(void)
  : malla(nullptr) {}

//! Constructor.
XC::MEDMesh::MEDMesh(const MEDMesh &otro)
  : MEDObject(otro), malla(nullptr)
  {
    meshName= otro.meshName;
    alloc(otro);
  }

//! Operador asignación.
XC::MEDMesh &XC::MEDMesh::operator=(const MEDMesh &otro)
  {
    MEDObject::operator=(otro);
    meshName= otro.meshName;
    alloc(otro);
    return *this;
  }
//! Destructor.
XC::MEDMesh::~MEDMesh(void)
  { libera(); }

void XC::MEDMesh::read(const std::string &fileName)
  {
    alloc(fileName);
    const std::string nmb= malla->getName();
    if(nmb!=meshName)
      {
        std::cerr << "Error al leer el nombre de la malla: se preguntó por #"
                  << meshName <<"# y se obtuvo #"
                  << nmb <<"#" << std::endl ;
      }
  }

//! @brief Lectura del objeto desde archivo.
bool XC::MEDMesh::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(MEDMesh) Procesando comando: " << cmd << std::endl;

    if(cmd == "setName") //Asigna nombre a la malla.
      {
        meshName= interpretaString(status.GetBloque());
        return true;
      }
    else if(cmd == "read")
      {
        const std::string fileName= interpretaString(status.GetString());
        read(fileName);
        return true;
      }
    else    
      return MEDObject::procesa_comando(status);
  }

size_t XC::MEDMesh::getSpaceDimension(void)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getSpaceDimension();
    else
      std::cerr << "MEDMesh::getSpaceDimension(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getMeshDimension(void)
  {
    size_t retval= 0;
    if(malla)
      tmp_gp_szt= malla->getMeshDimension();
    else
      std::cerr << "MEDMesh::getMeshDimension(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfNodes(void)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfNodes();
    else
      std::cerr << "MEDMesh::getNumberOfNodes(); no se ha leído la malla." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCoordinatesNames(void)
  {
    boost::python::list retval;
    if(malla)
      {
        const std::string *coordinatesNames= malla->getCoordinatesNames();
        const size_t spaceDim= malla->getSpaceDimension();
        for(size_t i=0;i<spaceDim;i++)
          retval.append(std::string(coordinatesNames[i]));
      }
    else
      std::cerr << "MEDMesh::getCoordinatesNames(); no se ha leído la malla." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCoordinatesUnits(void)
  {
    boost::python::list retval;
    if(malla)
      {
        const std::string *unitsNames= malla->getCoordinatesUnits();
        const size_t spaceDim= malla->getSpaceDimension();
        for(size_t i=0;i<spaceDim;i++)
          retval.append(std::string(unitsNames[i]));
      }
    else
      std::cerr << "MEDMesh::getCoordinatesUnits(); no se ha leído la malla." << std::endl;
    return retval;
  }

XC::Matrix XC::MEDMesh::getCoordinates(void)
  {
    Matrix retval;
    if(malla)
      {
        const double *coordenadas= malla->getCoordinates(::MED_FULL_INTERLACE);
        const size_t numNodos= malla->getNumberOfNodes();
        const size_t spaceDim= malla->getSpaceDimension();
        retval= Matrix(numNodos,spaceDim);
        for(size_t i=0;i<numNodos;i++)
          for(size_t j=0;j<spaceDim;j++)
            retval(i,j)= coordenadas[i*spaceDim+j];
      }
    else
      std::cerr << "MEDMesh::getCoordinates(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfCellTypes()
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfTypes(MED_EN::MED_CELL);
    else
      std::cerr << "MEDMesh::getCoordinates(); no se ha leído la malla." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCellTypes(void)
  {
    boost::python::list retval;
    if(malla)
      {
        const MED_EN::medGeometryElement *tipos= malla->getTypes(MED_EN::MED_CELL);
        const size_t numTipos= malla->getNumberOfTypes(MED_EN::MED_CELL);
        for(size_t i=0;i<numTipos;i++)
          retval.append(tipos[i]);
      }
    else
      std::cerr << "MEDMesh::getCoordinates(); no se ha leído la malla." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCellTypeNames(void)
  {
    boost::python::list retval;
    if(malla)
      {
        const std::string *tipos= malla->getCellTypeNames(MED_EN::MED_CELL);
        const size_t numTipos= malla->getNumberOfTypes(MED_EN::MED_CELL);
        for(size_t i=0;i<numTipos;i++)
          retval.append(std::string(tipos[i]));
      }
    else
      std::cerr << "MEDMesh::getCellTypeNames(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumCellsOfType(int tipo)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfElements(MED_EN::MED_CELL,tipo);
    else
      std::cerr << "MEDMesh::getNumCellsOfType(); no se ha leído la malla." << std::endl;
    return retval;
  }

XC::Matrix XC::MEDMesh::getConnectivityCellsOfType(int tipo)
  {
    Matrix retval;
    size_t numNodosCeldaTipo= tipo%100;
    size_t numCeldasTipo= 0;
    if(malla)
      {
        numCeldasTipo= malla->getNumberOfElements(MED_EN::MED_CELL,tipo);
        retval= Matrix(numCeldasTipo,numNodosCeldaTipo);
        const int *conectividad= malla->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,tipo);
        for(size_t i= 0;i<numCeldasTipo;i++)
          for(size_t j= 0;j<numNodosCeldaTipo;j++)
            retval(i,j)= conectividad[i*numNodosCeldaTipo+j];
      }
    else
      std::cerr << "MEDMesh::getConnectivityCellsOfType(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfGroups(int tipo_entidad)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfGroups(tipo_entidad);
    else
      std::cerr << "MEDMesh::getNumberOfGroups(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfFamilies(int tipo_entidad)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfFamilies(tipo_entidad);
    else
      std::cerr << "MEDMesh::getNumberOfFamilies(); no se ha leído la malla." << std::endl;
    return retval;
  }


//! Devuelve la propiedad del objeto cuyo código se pasa 
//! como parámetro. 
any_const_ptr XC::MEDMesh::GetProp(const std::string &cod) const 
  {
    if(cod=="getSpaceDimension")
      {
        if(malla)
          tmp_gp_szt= malla->getSpaceDimension();
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getMeshDimension")
      {
        if(malla)
          tmp_gp_szt= malla->getMeshDimension();
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumberOfNodes")
      {
        if(malla)
          tmp_gp_szt= malla->getNumberOfNodes();
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getCoordinatesNames")
      {
        if(malla)
          {
            const std::string *coordinatesNames= malla->getCoordinatesNames();
            const size_t spaceDim= malla->getSpaceDimension();
            tmp_gp_lista.clear();
            for(size_t i=0;i<spaceDim;i++)
              tmp_gp_lista.Inserta(std::string(coordinatesNames[i]));
          }
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_lista);
      }
    else if(cod=="getCoordinatesUnits")
      {
        if(malla)
          {
            const std::string *unitsNames= malla->getCoordinatesUnits();
            const size_t spaceDim= malla->getSpaceDimension();
            tmp_gp_lista.clear();
            for(size_t i=0;i<spaceDim;i++)
              tmp_gp_lista.Inserta(std::string(unitsNames[i]));
          }
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_lista);
      }
    else if(cod=="getCoordinates")
      {
        if(malla)
          {
            const double *coordenadas= malla->getCoordinates(::MED_FULL_INTERLACE);
            const size_t numNodos= malla->getNumberOfNodes();
            const size_t spaceDim= malla->getSpaceDimension();
            tmp_gp_mdbl= m_double(numNodos,spaceDim,0.0);
            for(size_t i=0;i<numNodos;i++)
              for(size_t j=0;j<spaceDim;j++)
                tmp_gp_mdbl(i+1,j+1)= coordenadas[i*spaceDim+j];
          }
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getNumberOfCellTypes")
      {
        if(malla)
          tmp_gp_szt= malla->getNumberOfTypes(MED_EN::MED_CELL);
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getCellTypes")
      {
        if(malla)
          {
            const MED_EN::medGeometryElement *tipos= malla->getTypes(MED_EN::MED_CELL);
            const size_t numTipos= malla->getNumberOfTypes(MED_EN::MED_CELL);
            tmp_gp_lista.clear();
            for(size_t i=0;i<numTipos;i++)
              tmp_gp_lista.Inserta(tipos[i]);
          }
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_lista);
      }
    else if(cod=="getCellTypeNames")
      {
        if(malla)
          {
            const std::string *tipos= malla->getCellTypeNames(MED_EN::MED_CELL);
            const size_t numTipos= malla->getNumberOfTypes(MED_EN::MED_CELL);
            tmp_gp_lista.clear();
            for(size_t i=0;i<numTipos;i++)
              tmp_gp_lista.Inserta(std::string(tipos[i]));
          }
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_lista);
      }
    else if(cod=="getNumCellsType")
      {
        MED_EN::medGeometryElement tipo= 0;
        if(InterpreteRPN::Pila().size()>0)
          tipo= convert_to_long_int(InterpreteRPN::Pila().Pop());
        if(malla)
          tmp_gp_szt= malla->getNumberOfElements(MED_EN::MED_CELL,tipo);
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getConnectivityCellsType")
      {
        MED_EN::medGeometryElement tipo= 0;
        size_t numNodosCeldaTipo= 0;
        size_t numCeldasTipo= 0;
        if(InterpreteRPN::Pila().size()>0)
          {
            tipo= convert_to_long_int(InterpreteRPN::Pila().Pop());
            numNodosCeldaTipo= tipo%100;
          }
        if(malla)
          {
            numCeldasTipo= malla->getNumberOfElements(MED_EN::MED_CELL,tipo);
            tmp_gp_mdbl= m_double(numCeldasTipo,numNodosCeldaTipo,0);
            const int *conectividad= malla->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,tipo);
            for(size_t i= 0;i<numCeldasTipo;i++)
              for(size_t j= 0;j<numNodosCeldaTipo;j++)
                tmp_gp_mdbl(i+1,j+1)= conectividad[i*numNodosCeldaTipo+j];
          }
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getNumberOfGroups")
      {
        MED_EN::medEntityMesh tipo_entidad= MED_EN::MED_ALL_ENTITIES;
        if(InterpreteRPN::Pila().size()>0)
          tipo_entidad= convert_to_long_int(InterpreteRPN::Pila().Pop());
        else
	  std::cerr << "@getNumberOfGroups; falta el tipo de entidad."
                    << std::endl;
        if(malla)
          tmp_gp_szt= malla->getNumberOfGroups(tipo_entidad);
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumberOfFamilies")
      {
        MED_EN::medEntityMesh tipo_entidad= MED_EN::MED_ALL_ENTITIES;
        if(InterpreteRPN::Pila().size()>0)
          tipo_entidad= convert_to_long_int(InterpreteRPN::Pila().Pop());
        else
	  std::cerr << "@getNumberOfFamilies; falta el tipo de entidad."
                    << std::endl;
        if(malla)
          tmp_gp_szt= malla->getNumberOfFamilies(tipo_entidad);
        else
          std::cerr << "MEDMesh::GetProp("
                    << cod << "); no se ha leído la malla." << std::endl;
       return any_const_ptr(tmp_gp_szt);
      }
    else
      return MEDObject::GetProp(cod);
  }
