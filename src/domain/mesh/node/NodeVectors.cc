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
//NodeVectors.cpp

#include <domain/mesh/node/NodeVectors.h>
#include <utility/tagged/TaggedObject.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

void XC::NodeVectors::libera(void)
  {
    // delete anything that we created with new
    if(commitData) delete commitData;
    commitData= nullptr;
    if(trialData) delete trialData;
    trialData= nullptr;
    if(data) delete [] data;
    data= nullptr;
  }

void XC::NodeVectors::copia(const NodeVectors &otro)
  {
    libera();
    numVectors= otro.numVectors;
    if(otro.commitData)
      {
        const size_t nDOF= otro.getVectorsSize();
        const size_t sz= numVectors*nDOF;
        if(this->createData(nDOF) < 0)
          {
            std::cerr << " FATAL NodeVectors::Node(node *) - ran out of memory for data\n";
            exit(-1);
          }
        for(register size_t i=0;i<sz;i++)
          data[i] = otro.data[i];
      }
  }

//! @brief Constructor.
XC::NodeVectors::NodeVectors(const size_t &nv)
  :EntCmd(),MovableObject(NOD_TAG_NodeVectors), numVectors(nv), commitData(nullptr),trialData(nullptr),data(nullptr) {}


//! @brief Constructor de copia.
XC::NodeVectors::NodeVectors(const NodeVectors &otro)
  : EntCmd(otro),MovableObject(NOD_TAG_NodeVectors), numVectors(otro.numVectors), commitData(nullptr), trialData(nullptr), data(nullptr)
  { copia(otro); }

XC::NodeVectors &XC::NodeVectors::operator=(const NodeVectors &otro)
  {
    EntCmd::operator=(otro);
    MovableObject::operator=(otro);
    copia(otro);
    return *this;
  }

//! @brief Lee un objeto XC::Node desde archivo
//!
//! Soporta los comandos:
//! -coo: Lee las coordenadas del nodo.
bool XC::NodeVectors::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(NodeVectors) Procesando comando: " << cmd << std::endl;
    if(cmd == "setTrialData")
      {
	std::clog << "NodeVectors; el comando: " << cmd
                  << " está pensado para pruebas." << std::endl;
        const std::string argumento= status.GetString();
	const std::vector<double> data_nodo= crea_vector_double(argumento);
        size_t nDOF= getVectorsSize();
        if(nDOF==0)
          {
            nDOF= data_nodo.size();
            createData(nDOF);
          }
        const size_t sz= std::min(data_nodo.size(),nDOF);
        if(sz==0)
	  std::cerr << "(NodeVectors) Procesando comando: " << cmd
                    << " el argumento: '" << argumento
                    << "' produce un vector de dimensión 0." << std::endl;
        for(size_t i= 0;i<sz;i++)
          setTrialData(nDOF,data_nodo[i],i);
        return true;
      }
    else if(cmd == "commit")
      {
	std::clog << "NodeVectors; el comando: " << cmd
                  << " está pensado para pruebas." << std::endl;
        status.GetString(); //Ignoramos entrada.
        const size_t sz= getVectorsSize();
        if(sz>0)
          commitState(sz);
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

//! @brief destructor
XC::NodeVectors::~NodeVectors(void)
  { libera(); }

//! @brief Devuelve el número de grados de libertad del nodo.
size_t XC::NodeVectors::getVectorsSize(void) const
  {
    if(commitData)
      return commitData->Size();
    else
      return 0;
  }

//! @brief Devuelve el vector de datos.
const XC::Vector &XC::NodeVectors::getData(const size_t &nDOF) const
  {
    if(!commitData)
      {
        NodeVectors *this_no_const= const_cast<NodeVectors *>(this);
        if(this_no_const->createData(nDOF) < 0)
          {
            std::cerr << "FATAL XC::NodeVectors::getData() -- ran out of memory\n";
            exit(-1);
          }
      }
    // return the committed data
    return *commitData;
  }
  
//! @brief Devuelve los valores "consumados"
const XC::Vector &XC::NodeVectors::getCommitData(void) const
  {
    assert(commitData);
    return *commitData;
  }

//! @brief Devuelve el vector de datos de prueba.
const XC::Vector &XC::NodeVectors::getTrialData(const size_t &nDOF) const
  {
    if(!trialData)
      {
        NodeVectors *this_no_const= const_cast<NodeVectors *>(this);
        if(this_no_const->createData(nDOF) < 0)
          {
            std::cerr << "FATAL XC::NodeVectors::getTrialData() -- ran out of memory\n";
            exit(-1);
          }
      }
    return *trialData;
  }

//! @brief Devuelve los valores de prueba
const XC::Vector &XC::NodeVectors::getTrialData(void) const
  {
    assert(trialData);
    return *trialData;
  }

int XC::NodeVectors::setTrialData(const size_t &nDOF,const double &value,const size_t &dof)
  {
    // check vector arg is of correct size
    if(dof < 0 || dof >= nDOF)
      {
        std::cerr << "WARNING NodeVectors::setTrialData() - incompatible sizes\n";
        return -2;
      }

    // perform the assignment .. we dont't go through Vector interface
    // as we are sure of size and this way is quicker
    if(data)
      data[dof]= value;
    return 0;
  }

//! @brief Asigna el valor que se pasa como parámetro
int XC::NodeVectors::setTrialData(const size_t &nDOF,const Vector &newTrialData)
  {
    // check vector arg is of correct size
    if(size_t(newTrialData.Size()) != nDOF)
      {
        std::cerr << "WARNING NodeVectors::setTrialData() - incompatible sizes\n";
        return -2;
      }

    // construct memory and Vectors for trial and committed
    // accel on first call to this method, getTrialData(),
    // getData(), or incrTrialData()
    if(!data)
      {
        if(this->createData(nDOF) < 0)
          {
            std::cerr << "FATAL NodeVectors::setTrialData() - ran out of memory\n";
            exit(-1);
          }
      }

    // perform the assignment .. we dont't go through XC::Vector interface
    // as we are sure of size and this way is quicker
    for(size_t i=0;i<nDOF;i++)
      data[i]= newTrialData(i);
    return 0;
  }


int XC::NodeVectors::incrTrialData(const size_t &nDOF,const Vector &incrData)
  {
    // check vector arg is of correct size
    if(size_t(incrData.Size()) != nDOF)
      {
        std::cerr << "WARNING XC::NodeVectors::incrTrialData() - incompatable sizes\n";
        return -2;
      }

    // create a copy if no trial exists andd add committed
    if(!data)
      {
        if(this->createData(nDOF) < 0)
          {
            std::cerr << "FATAL XC::NodeVectors::incrTrialData() - ran out of memory\n";
            exit(-1);
          }
      }
    // set trial = incr + trial
    for(size_t i= 0;i<nDOF;i++)
      data[i]+= incrData(i);
    return 0;
  }

int XC::NodeVectors::commitState(const size_t &nDOF)
  {
    // check data exists, if does set commit = trial, incr = 0.0
    if(trialData)
      {
        for(register size_t i=0; i<nDOF; i++)
          data[i+nDOF] = data[i];
      }
    return 0;
  }


//! @brief Devuelve los vectores al estado correspondiente al último commit.
int XC::NodeVectors::revertToLastCommit(const size_t &nDOF)
  {
    // check data exists, if does set trial = last commit, incr = 0
    if(data)
      {
        for(size_t i=0;i<nDOF;i++)
          data[i] = data[nDOF+i];
      }
    return 0;
  }


//! @brief Devuelve el nodo a su estado inicial.
int XC::NodeVectors::revertToStart(const size_t &nDOF)
  {
    // check data exists, if does set all to zero
    const size_t sz= numVectors*nDOF;
    if(data)
      {
        for(size_t i=0;i<sz;i++)
          data[i]= 0.0;
      }
    return 0;
  }


//! @brief private method to create the arrays to hold the data
//! values and the Vector objects for the committed and trial quantities.
int XC::NodeVectors::createData(const size_t &nDOF)
  {
    libera();
    // trial , committed, incr = (committed-trial)
    const size_t sz= numVectors*nDOF;
    data= new double[sz];

    if(data)
      {
        for(size_t i=0;i<sz;i++)
          data[i]= 0.0;

        trialData= new Vector(&data[0], nDOF);
        commitData= new Vector(&data[nDOF], nDOF);

        if(!commitData || !trialData)
          {
            std::cerr << "WARNING - XC::NodeVectors::createData() "
                  << "ran out of memory creating Vectors(double *,int)";
            return -2;
          }
        return 0;
      }
    else
      {
        std::cerr << "WARNING - XC::NodeVectors::createData() ran out of memory for array of size " << sz << std::endl;
        return -1;
      }
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::NodeVectors::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }
//! @brief Envia los miembros del objeto a través del canal que se pasa como parámetro.
int XC::NodeVectors::sendData(CommParameters &cp)
  {
    int res= 0;
    ID datos(3);
    if(!commitData)
      datos(0) = 1;
    else
      {
        datos(0)= 0;
        datos(1)= cp.getDbTag();
        datos(2)= commitData->Size();
        res+= cp.sendVector(*commitData,datos(1));
        if(res < 0)
          {
            std::cerr << "NodeVectors::sendSelf() - failed to send Disp data\n";
            return res;
          }
      }
    res+= cp.sendID(datos,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::NodeVectors::recvData(const CommParameters &cp)
  {
    ID datos(3);
    int res= cp.receiveID(datos,getDbTagData(),CommMetaData(1));


    const int dbTag1= datos(1);
    const int nDOF= datos(2);

    if(datos(0) == 0)
      {
        // create the disp vectors if node is a total blank
        createData(nDOF);
        // recv the data
        if(cp.receiveVector(*commitData,dbTag1) < 0)
          {
            std::cerr << "NodeVectors::recvSelf - failed to receive data\n";
            return res;
          }

        // set the trial quantities equal to committed
        for(int i=0; i<nDOF; i++)
          data[i] = data[i+nDOF];  // set trial equal commited
      }
    else if(commitData)
      {
        // if going back to initial we will just zero the vectors
        commitData->Zero();
        trialData->Zero();
      }
    return res;
  }

//! @brief Envia los vectores por el canal que se pasa como parámetro.E
int XC::NodeVectors::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::NodeVectors::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//! nnod: Devuelve el número de nodos del dominio.
any_const_ptr XC::NodeVectors::GetProp(const std::string &cod) const
  {
    if(cod=="getTrialData" && trialData)
      return TaggedObject::get_prop_vector(trialData);
    else if(cod=="getCommitData" && commitData)
      return TaggedObject::get_prop_vector(commitData);
    else
      return EntCmd::GetProp(cod);
  }
