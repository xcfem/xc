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
                                                                        
                                                                        
#ifndef CommParameters_h
#define CommParameters_h

#include "utility/matrix/ID.h"
#include <string>
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "BrokedPtrCommMetaData.h"
#include <set>
#include "DbTagData.h"

namespace XC {
class Channel;
//class FEM_ObjectBroker;
class MovableID;
class Matrix;
class Vector;
class BJtensor;
class MovableVector;
class MovableMatrix;
class MovableString;
class MovableObject;
class ArrayCommMetaData;
class MatrixCommMetaData;
class TensorCommMetaData;
class Material;
class TimeSeries;
class ResponseId;
class DomainDecompositionAnalysis;
class CrdTransf;
class CrdTransf2d;
class CrdTransf3d;
class BeamIntegration;

//! @ingroup ACTOR
//
//! @brief Parámetros para comunicación entre procesos.
class CommParameters
  {
  private:
    int commitTag;
    Channel *canal;
    FEM_ObjectBroker *broker;
  public:
    CommParameters(int cTag, Channel &);
    CommParameters(int cTag, Channel &, FEM_ObjectBroker &);

    inline const int &getCommitTag(void) const
      { return commitTag; }
    int getDbTag(void) const;
    bool isDatastore(void) const;
    inline const Channel *getChannel(void) const
      { return canal; }
    inline Channel *getChannel(void)
      { return canal; }

    int sendIdData(const DbTagData &,const int &);
    int receiveIdData(DbTagData &,const int &) const;
    int sendID(const ID &,const int &);
    int receiveID(ID &v,const int &) const;
    int sendVector(const Vector &,const int &);
    int receiveVector(Vector &v,const int &) const;
    int sendMatrix(const Matrix &,const int &);
    int receiveMatrix(Matrix &v,const int &) const;

    int sendID(const ID &,DbTagData &, const CommMetaData &);
    int receiveID(ID &v,DbTagData &, const CommMetaData &) const;
    int sendIDPtr(ID *,DbTagData &, const ArrayCommMetaData &);
    ID *receiveIDPtr(ID* &,DbTagData &, const ArrayCommMetaData &) const;
    int sendMovableIDPtr(MovableID *,DbTagData &, const PtrCommMetaData &);
    MovableID *receiveMovableIDPtr(MovableID* &,DbTagData &, const PtrCommMetaData &);
    int sendMatrix(const Matrix &,DbTagData &, const CommMetaData &);
    int receiveMatrix(Matrix &v,DbTagData &, const CommMetaData &) const;
    int sendMatrixPtr(Matrix *ptr,DbTagData &, const MatrixCommMetaData &);
    Matrix *receiveMatrixPtr(Matrix* &ptr,DbTagData &, const MatrixCommMetaData &) const;
    int sendTensorPtr(BJtensor *ptr,DbTagData &, const TensorCommMetaData &);
    BJtensor *receiveTensorPtr(BJtensor* &ptr,DbTagData &, const TensorCommMetaData &) const;
    int sendMovableMatrixPtr(MovableMatrix *,DbTagData &, const PtrCommMetaData &);
    MovableMatrix *receiveMovableMatrixPtr(MovableMatrix* &,DbTagData &, const PtrCommMetaData &);

    int sendMatrices(const std::vector<Matrix> &,DbTagData &, const CommMetaData &);
    int receiveMatrices(std::vector<Matrix> &,DbTagData &, const CommMetaData &) const;

    int sendString(const std::string &,DbTagData &, const CommMetaData &);
    int receiveString(std::string &v,DbTagData &, const CommMetaData &) const;
    int sendStrings(std::deque<std::string> &,DbTagData &, const CommMetaData &);
    int receiveStrings(std::deque<std::string> &,DbTagData &, const CommMetaData &) const;
    int sendStrings(std::vector<std::string> &,DbTagData &, const CommMetaData &);
    int receiveStrings(std::vector<std::string> &,DbTagData &, const CommMetaData &) const;

    int sendVector(const Vector &,DbTagData &, const CommMetaData &);
    int receiveVector(Vector &v,DbTagData &, const CommMetaData &) const;
    int sendVectors(std::vector<Vector> &,DbTagData &, const CommMetaData &);
    int receiveVectors(std::vector<Vector> &v,DbTagData &, const CommMetaData &) const;
    int sendVector(const std::vector<double> &,DbTagData &, const CommMetaData &);
    int receiveVector(std::vector<double> &v,DbTagData &, const CommMetaData &) const;
    int sendVectorPtr(Vector *ptr,DbTagData &, const ArrayCommMetaData &);
    Vector *receiveVectorPtr(Vector* &ptr,DbTagData &, const ArrayCommMetaData &) const;
    int sendMovableVectorPtr(MovableVector *,DbTagData &, const PtrCommMetaData &);
    MovableVector *receiveMovableVectorPtr(MovableVector* &,DbTagData &, const PtrCommMetaData &);
    int sendTensor(const BJtensor &,DbTagData &, const CommMetaData &);
    int receiveTensor(BJtensor &v,DbTagData &, const CommMetaData &) const;

    int sendDoublePtr(double *,DbTagData &, const ArrayCommMetaData &);
    double *receiveDoublePtr(double* &,DbTagData &, const ArrayCommMetaData &) const;
    int sendDouble(const double &,DbTagData &, const CommMetaData &);
    int sendDoubles(const double &,const double &,DbTagData &, const CommMetaData &);
    int sendDoubles(const double &,const double &,const double &,DbTagData &, const CommMetaData &);
    int sendDoubles(const double &,const double &,const double &,const double &,DbTagData &, const CommMetaData &);
    int sendDoubles(const double &,const double &,const double &,const double &,const double &,DbTagData &, const CommMetaData &);
    int sendDoubles(const double &,const double &,const double &,const double &,const double &,const double &,DbTagData &, const CommMetaData &);
    int receiveDouble(double &,DbTagData &, const CommMetaData &) const;
    int receiveDoubles(double &,double &,DbTagData &, const CommMetaData &) const;
    int receiveDoubles(double &,double &,double &,DbTagData &, const CommMetaData &) const;
    int receiveDoubles(double &,double &,double &,double &,DbTagData &, const CommMetaData &) const;
    int receiveDoubles(double &,double &,double &,double &,double &,DbTagData &, const CommMetaData &) const;
    int receiveDoubles(double &,double &,double &,double &,double &,double &,DbTagData &, const CommMetaData &) const;

    int sendBool(const bool &,DbTagData &, const CommMetaData &);
    int receiveBool(bool &,DbTagData &, const CommMetaData &) const;

    int sendInt(const int &,DbTagData &, const CommMetaData &);
    int sendInts(const int &,const int &,DbTagData &, const CommMetaData &);
    int sendInts(const int &,const int &,const int &,DbTagData &, const CommMetaData &);
    int sendInts(const int &,const int &,const int &,const int &,DbTagData &, const CommMetaData &);
    int sendInts(const int &,const int &,const int &,const int &,const int &,DbTagData &, const CommMetaData &);
    int sendInts(const int &,const int &,const int &,const int &,const int &,const int &,DbTagData &, const CommMetaData &);
    int receiveInt(int &,DbTagData &, const CommMetaData &) const;
    int receiveInts(int &,int &,DbTagData &, const CommMetaData &) const;
    int receiveInts(int &,int &,int &,DbTagData &, const CommMetaData &) const;
    int receiveInts(int &,int &,int &,int &,DbTagData &, const CommMetaData &) const;
    int receiveInts(int &,int &,int &,int &,int &,DbTagData &, const CommMetaData &) const;
    int receiveInts(int &,int &,int &,int &,int &,int &,DbTagData &, const CommMetaData &) const;

    size_t sendSzt(const size_t &,DbTagData &, const CommMetaData &);
    size_t sendSzts(const size_t &,const size_t &,DbTagData &, const CommMetaData &);
    size_t sendSzts(const size_t &,const size_t &,const size_t &,DbTagData &, const CommMetaData &);
    size_t sendSzts(const size_t &,const size_t &,const size_t &,const size_t &,DbTagData &, const CommMetaData &);
    size_t sendSzts(const size_t &,const size_t &,const size_t &,const size_t &,const size_t &,DbTagData &, const CommMetaData &);
    size_t sendSzts(const size_t &,const size_t &,const size_t &,const size_t &,const size_t &,const size_t &,DbTagData &, const CommMetaData &);
    size_t receiveSzt(size_t &,DbTagData &, const CommMetaData &) const;
    size_t receiveSzts(size_t &,size_t &,DbTagData &, const CommMetaData &) const;
    size_t receiveSzts(size_t &,size_t &,size_t &,DbTagData &, const CommMetaData &) const;
    size_t receiveSzts(size_t &,size_t &,size_t &,size_t &,DbTagData &, const CommMetaData &) const;
    size_t receiveSzts(size_t &,size_t &,size_t &,size_t &,size_t &,DbTagData &, const CommMetaData &) const;
    size_t receiveSzts(size_t &,size_t &,size_t &,size_t &,size_t &,size_t &,DbTagData &, const CommMetaData &) const;

    ResponseId *receiveResponseIdPtr(ResponseId* &,DbTagData &, const ArrayCommMetaData &) const;

    int sendMovable(MovableObject &,DbTagData &, const CommMetaData &);
    int receiveMovable(MovableObject &,DbTagData &, const CommMetaData &) const;
    int sendMovablePtr(MovableObject *ptr,DbTagData &, const PtrCommMetaData &);
    template <class MOV>
    MOV *receiveMovablePtr(MOV* &,DbTagData &, const PtrCommMetaData &) const;

    

    int sendBrokedPtr(MovableObject *ptr,DbTagData &, const BrokedPtrCommMetaData &);
    template <class T>
    T *getBrokedPtr(const int &,T *(FEM_ObjectBroker::*ptrFunc)(int)) const;
    template <class T>
    T *getBrokedTagged(T* &ptr,const int &,const int &,const int &,T *(FEM_ObjectBroker::*ptrFunc)(int)) const;

    template <class MAT>
    MAT *getBrokedMaterial(MAT* &,DbTagData &, const BrokedPtrCommMetaData &) const;
    template <class HDL>
    HDL *getBrokedDataOutputHandler(HDL* &,DbTagData &, const BrokedPtrCommMetaData &) const;
    template <class GM>
    GM *getBrokedGroundMotion(GM* &,DbTagData &, const BrokedPtrCommMetaData &) const;

    ConstraintHandler *brokeConstraintHandler(const int &) const;
    DOF_Numberer *brokeNumberer(const int &) const;
    AnalysisModel *brokeAnalysisModel(const int &) const;
    LinearSOE *brokeDDLinearSOE(const int &,const int &) const;
    LinearSOE *brokeLinearSOE(const int &,const int &) const;
    IncrementalIntegrator *brokeIncrementalIntegrator(const int &) const;
    StaticIntegrator *brokeStaticIntegrator(const int &) const;
    TransientIntegrator *brokeTransientIntegrator(const int &) const;
    DomainDecompAlgo *brokeDomainDecompAlgo(const int &) const;
    EquiSolnAlgo *brokeEquiSolnAlgo(const int &) const;
    GroundMotion *brokeGroundMotion(const int &) const;
    DomainSolver *getNewDomainSolver(void) const;
    DomainDecompositionAnalysis *getNewDomainDecompAnalysis(int, Subdomain &) const;
    ConvergenceTest *getNewConvergenceTest(EntCmd *owr,int classTag) const;
    TimeSeries *getNewTimeSeries(int classTag) const;
    CrdTransf2d *getNewCrdTransf2d(int classTag) const;
    CrdTransf3d *getNewCrdTransf3d(int classTag) const;
    CrdTransf *getNewCrdTransf(int classTag) const;
    ShellCrdTransf3dBase *getNewShellCrdTransf3d(int classTag) const;
    BeamIntegration *getNewBeamIntegration(int classTag) const;
  };

template <class T>
T *CommParameters::getBrokedPtr(const int &classTag,T *(FEM_ObjectBroker::*ptrFunc)(int)) const
  {
    assert(broker);
    T *retval=((*broker).*ptrFunc)(classTag);  
    return retval;
  }

template <class MAT>
MAT *CommParameters::getBrokedMaterial(MAT* &ptr,DbTagData &dbTagData, const BrokedPtrCommMetaData &md) const
  {
    if(!ptr)
      {
        assert(broker);
        const int classTag= dbTagData.getDbTagDataPos(md.getPosClassTag());
        Material *tmp= broker->getNewMaterial(classTag);
        if(tmp)
          {
            ptr= dynamic_cast<MAT *>(tmp);
            if(ptr)
              {
                const int res= receiveMovable(*ptr,dbTagData,md);
                if(res<0)
                std::cerr <<"WARNING - receiveMaterialPtr "
                         << "failed to receive material.\n";
              }
            else
              {
                delete tmp;
                std::cerr << "CommParameters::getBrokedMaterial "
                          << " el material es de clase inadecuada."
                          << std::endl;
              }
          }
        else
          std::cerr << "getBrokedMaterial: "
                    << " broker could not create material of class type: "
                    << classTag << std::endl;;
      }
    return ptr;
  }

template <class HDL>
HDL *CommParameters::getBrokedDataOutputHandler(HDL* &ptr,DbTagData &dbTagData, const BrokedPtrCommMetaData &md) const
  {
    if(!ptr)
      {
        assert(broker);
        const int classTag= dbTagData.getDbTagDataPos(md.getPosClassTag());
        DataOutputHandler *tmp= broker->getPtrNewDataOutputHandler(classTag);
        if(tmp)
          {
            ptr= dynamic_cast<HDL *>(tmp);
            if(!ptr) delete tmp;
          }
        else
          std::cerr << "getBrokedMaterial: "
                    << " broker could not create handler of class type: "
                    << classTag << std::endl;;
      }
    return ptr;
  }

template <class GM>
GM *CommParameters::getBrokedGroundMotion(GM* &ptr,DbTagData &dbTagData, const BrokedPtrCommMetaData &md) const
  {
    if(!ptr)
      {
        assert(broker);
        const int classTag= dbTagData.getDbTagDataPos(md.getPosClassTag());
        GroundMotion *tmp= broker->getNewGroundMotion(classTag);
        if(tmp)
          {
            ptr= dynamic_cast<GM *>(tmp);
            if(!ptr) delete tmp;
          }
        else
          std::cerr << "getBrokedGroundMotion: "
                    << " broker could not create ground motion of class type: "
                    << classTag << std::endl;;
      }
    return ptr;
  }

template <class T>
T *CommParameters::getBrokedTagged(T* &ptr,const int &dbTag,const int &objTag,const int &classTag,T *(FEM_ObjectBroker::*ptrFunc)(int)) const
  {
    if(!ptr)
      {
        assert(broker);
        T *retval=((*broker).*ptrFunc)(classTag);
  
        if(retval)
          {
            retval->setDbTag(dbTag);
            retval->setTag(objTag);
            ptr= retval;
          }
        else
          std::cerr << "getBrokedTagged; no se pudo crear el objeto con classTag: " 
                    << classTag << std::endl;
      }
    return ptr;
  }

template <class MOV>
MOV *CommParameters::receiveMovablePtr(MOV* &ptr,DbTagData &dbTagData, const PtrCommMetaData &meta) const
  {
    if(dbTagData.getDbTagDataPos(meta.getPosFlag()) == 0)
      {
        // make some room and read in the ID
        if(!ptr)
          {
            ptr= new MOV();
            if(!ptr)
              std::cerr << "receiveMovableIDPtr -- ran out of memory\n";
          }
        if(ptr)
          {
            ptr->setDbTag(dbTagData.getDbTagDataPos(meta.getPosDbTag()));
            int res= ptr->recvSelf(*this);
            if(res < 0)
              std::cerr << "receiveMovableIDPtr - failed to receive ID data\n";
          }
      }
    return ptr;
  }

} // fin namespace XC

#endif
