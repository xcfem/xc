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
//  bajo los términos de l Licencia Pública General GNU publicada 
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
//MaterialVector.h

#ifndef MaterialVector_h
#define MaterialVector_h

#include <vector>
#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/matrix/Vector.h"


namespace XC {

//! @ingroup MatUnx
//
//! @brief Vector de punteros a materiales.
//! se emplea en los elementos para guardar los materiales
//! en los puntos de integración.
template <class MAT>
class MaterialVector: public std::vector<MAT *>, public EntCmd, public MovableObject
  {
  protected:
    void borra_materiales(void);
    void clearAll(void);
    void alloc(const std::vector<MAT *> &mats);

    void ejecuta_bloque_for_each(CmdStatus &status,const std::string &bloque);
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);  
    int recvData(const CommParameters &);
  public:
    typedef typename std::vector<MAT *> mat_vector;
    typedef typename mat_vector::iterator iterator;
    typedef typename mat_vector::reference reference;
    typedef typename mat_vector::const_reference const_reference;

    MaterialVector(const size_t &nMat,const MAT *matModel= nullptr);
    MaterialVector(const MaterialVector<MAT> &);
    MaterialVector<MAT> &operator=(const MaterialVector<MAT> &);
    ~MaterialVector(void)
      { clearAll(); }

    void setMaterial(const MAT *nuevo_mat);
    void setMaterial(size_t i,MAT *nuevo_mat);
    void setMaterial(const MAT *nuevo_mat,const std::string &tipo);
    bool empty(void) const;
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void setInitialGeneralizedStrains(const std::vector<Vector> &);
    void addInitialGeneralizedStrains(const std::vector<Vector> &);
    void zeroInitialGeneralizedStrains(void);

    static any_const_ptr get_prop_vector(const Vector *ptrVector);
    static any_const_ptr get_prop_vector(const Vector &ptrVector);

    size_t getGeneralizedStressSize(void) const;
    size_t getGeneralizedStrainSize(void) const;
    m_double getGeneralizedStresses(void) const;
    m_double getGeneralizedStrains(void) const;
    const Vector &getMeanGeneralizedStress(void) const;
    const Vector &getMeanGeneralizedStrain(void) const;
    double getMeanGeneralizedStrain(const int &defID) const;
    double getMeanGeneralizedStress(const int &defID) const;
    double getMeanGeneralizedStrainByName(const std::string &) const;
    double getMeanGeneralizedStressByName(const std::string &) const;
    m_double getGeneralizedStrain(const int &defID) const;
    m_double getGeneralizedStress(const int &defID) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

//! @brief Constructor por defecto.
template <class MAT>
MaterialVector<MAT>::MaterialVector(const size_t &nMat,const MAT *matModel)
  : std::vector<MAT *>(nMat,nullptr), MovableObject(MAT_VECTOR_TAG)
  {
    if(matModel)
      {
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)= matModel->getCopy();
            if(!(*i))
              std::cerr<<" MaterialVector -- failed allocate material model pointer\n";
          }
      }
  }

//! @brief Copia los materiales.
template <class MAT>
void MaterialVector<MAT>::alloc(const std::vector<MAT *> &mats)
  {
    clearAll();
    const size_t nMat= mats.size();
    this->resize(nMat);
    for(size_t i= 0;i<nMat;i++)
      {
        if(mats[i])
          {
            (*this)[i]= mats[i]->getCopy();
            if(!(*this)[i])
              std::cerr<<" MaterialVector -- failed allocate material model pointer\n";
          }
      }
  }

//! @brief Constructor de copia.
template <class MAT>
MaterialVector<MAT>::MaterialVector(const MaterialVector<MAT> &otro)
  : std::vector<MAT *>(otro.size(),nullptr), MovableObject(MAT_VECTOR_TAG)
  { alloc(otro); }

//! @brief Operador asignación.
template <class MAT>
MaterialVector<MAT> &MaterialVector<MAT>::operator=(const MaterialVector<MAT> &otro)
  { 
    alloc(otro);
    return *this;
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(const MAT *nuevo_mat)
  {
    borra_materiales();
    if(nuevo_mat)
      {
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)= nuevo_mat->getCopy();
            if(!(*i))
              std::cerr<<" MaterialVector -- failed allocate material model pointer\n";
          }
      }
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(const MAT *nuevo_mat, const std::string &tipo)
  {
    borra_materiales();
    if(nuevo_mat)
      {
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)= nuevo_mat->getCopy(tipo.c_str());
            if(!(*i))
              std::cerr<<" MaterialVector::setMaterial -- failed allocate material model pointer\n";
          }
      }
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(size_t i,MAT *nuevo_mat)
  {
    if((*this)[i])
      delete (*this)[i];
    (*this)[i]= nuevo_mat;
  }

template <class MAT>
void MaterialVector<MAT>::borra_materiales(void)
  {
    for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
      {
        if(*i) delete (*i);
          (*i)= nullptr;
      }
  }

//! @brief Devuelve verdadero si no se ha asignado material.
template <class MAT>
bool MaterialVector<MAT>::empty(void) const
  {
    if(mat_vector::empty())
      return true;
    else
      return ((*this)[0]==nullptr);
  }

template <class MAT>
void MaterialVector<MAT>::clearAll(void)
  {
    borra_materiales();
    std::vector<MAT *>::clear();
  }


//! @brief Consuma el estado de los materiales.
template <class MAT>
int MaterialVector<MAT>::commitState(void)
  {
    int retVal= 0;

    for(iterator i=mat_vector::begin();i!=mat_vector::end();i++)
      retVal+= (*i)->commitState();
    return retVal;
  }

//! @brief Devuelve el estado de los materiales al del último commit.
template <class MAT>
int MaterialVector<MAT>::revertToLastCommit(void)
  {
    int retVal= 0;

    for(iterator i=mat_vector::begin();i!=mat_vector::end();i++)
      retVal+= (*i)->revertToLastCommit() ;
    return retVal;
  }


//! @brief Devuelve el estado de los materiales al inicial.
template <class MAT>
int MaterialVector<MAT>::revertToStart(void)
  {
    int retVal = 0;

    for(iterator i=mat_vector::begin();i!=mat_vector::end();i++)
      retVal+= (*i)->revertToStart() ;
    return retVal;
  }

//! @brief Hace que cada una de los materiales ejecute el bloque que se pasa como parámetro.
template <class MAT>
void MaterialVector<MAT>::ejecuta_bloque_for_each(CmdStatus &status,const std::string &bloque)
  {
    const std::string nmbBlq= nombre_clase()+":for_each";
    iterator i= mat_vector::begin();
    for(;i!= mat_vector::end();i++)
      if(*i)
        (*i)->EjecutaBloque(status,bloque,nmbBlq);
      else
	std::cerr << "MaterialVector::ejecuta_bloque_for_each; Puntero a material nulo." << std::endl;
  }

//! @brief Devuelve el tamaño del vector de tensiones.
template <class MAT>
size_t MaterialVector<MAT>::getGeneralizedStressSize(void) const
  { return (*this)[0]->getGeneralizedStress().Size(); }

//! @brief Devuelve el tamaño del vector de deformaciones.
template <class MAT>
size_t MaterialVector<MAT>::getGeneralizedStrainSize(void) const
  { return (*this)[0]->getGeneralizedStrain().Size(); }

//! @brief Devuelve el valor de la tensión generalizada en cada punto de integración.
template <class MAT>
m_double MaterialVector<MAT>::getGeneralizedStresses(void) const
  {
    const size_t ncol= getGeneralizedStressSize();
    const size_t nMat= this->size();
    m_double retval(nMat,ncol,0.0);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStress();
        for(size_t j= 0;j<ncol;j++)
          retval(i,j)= s(j);
      }
    return retval;
  }

//! @brief Devuelve el valor de la deformación generalizada en cada punto de integración.
template <class MAT>
m_double MaterialVector<MAT>::getGeneralizedStrains(void) const
  {
    const size_t ncol= getGeneralizedStrainSize();
    const size_t nMat= this->size();
    m_double retval(nMat,ncol,0.0);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStrain();
        for(size_t j= 0;j<ncol;j++)
          retval(i,j)= s(j);
      }
    return retval;
  }

//! @brief Devuelve el valor medio de la resultante de tensiones en el elemento.
template <class MAT>
const Vector &MaterialVector<MAT>::getMeanGeneralizedStress(void) const
  {
    static Vector retval;
    retval= (*this)[0]->getGeneralizedStress();
    const size_t nMat= this->size();
    for(size_t i= 1;i<nMat;i++)
      retval+= (*this)[i]->getGeneralizedStress();
    retval/=nMat;
    return retval;
  }

//! @brief Devuelve el valor medio de las deformaciones en el elemento.
template <class MAT>
const Vector &MaterialVector<MAT>::getMeanGeneralizedStrain(void) const
  {
    static Vector retval;
    retval= (*this)[0]->getGeneralizedStrain();
    const size_t nMat= this->size();
    for(size_t i= 0;i<nMat;i++)
      retval+= (*this)[i]->getGeneralizedStrain();
    retval/=nMat;
    return retval;
  }

//! @brief Devuelve la componente del vector de deformaciones medias que
//! corresponde al índice que se pasa como parámetro.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStrain(const int &defID) const
  {
    double retval= 0.0;
    const Vector &e= getMeanGeneralizedStrain(); //Vector de deformaciones.
    const ResponseId &code= (*this)[0]->getType();
    const int order= code.Size();
    for(register int i= 0;i<order;i++)
      if(code(i) == defID)
        retval+= e(i);
    return retval;
  }

//! @brief Devuelve la componente del vector de deformaciones medias que
//! corresponde al codigo que se pasa como parámetro.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStrainByName(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1")
      retval= this->getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2")
      retval= this->getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n2);
    else if(cod == "m1") //Flector en torno al eje 1.
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector en torno al eje 2.
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_m2);
    else if(cod == "q13")
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_q23);
    else if(cod == "m12")
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_m12);
    else if(cod == "n12")
      retval= this->getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n12);
    else
      std::cerr << "stress code: '" << cod << " unknown." << std::endl;
    return retval;
  }

//! @brief Devuelve la componente del vector resultante de tensiones que
//! corresponde al índice que se pasa como parámetro.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStress(const int &defID) const
  {
    double retval= 0.0;
    const Vector &f= getMeanGeneralizedStress(); //Vector de esfuerzos.
    const ResponseId &code= (*this)[0]->getType();
    const int order= code.Size();
    for(register int i= 0;i<order;i++)
      if(code(i) == defID)
        retval+= f(i);
    return retval;
  }

//! @brief Devuelve la componente del vector resultante de tensiones que
//! corresponde al código que se pasa como parámetro.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStressByName(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1") //Esfuerzo axil medio por unidad de longitud, paralelo al eje 1.
      retval= this->getMeanGeneralizedStress(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2") //Esfuerzo axil medio por unidad de longitud, paralelo al eje 2.
      retval= this->getMeanGeneralizedStress(MEMBRANE_RESPONSE_n2);
    else if(cod == "n12")
      retval= this->getMeanGeneralizedStress(MEMBRANE_RESPONSE_n12);
    else if(cod == "m1") //Flector medio por unidad de longitud, en torno al eje 1.
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector medio por unidad de longitud, en torno al eje 2.
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_m2);
    else if(cod == "m12")
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_m12);
    else if(cod == "q13")
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_q23);
    else
      std::cerr << "stress code: '" << cod << " unknown." << std::endl;
    return retval;
  }

//! @brief Devuelve el valor de la tensión generalizada en cada punto de integración.
template <class MAT>
m_double MaterialVector<MAT>::getGeneralizedStress(const int &defID) const
  {
    const size_t nMat= this->size();
    m_double retval(nMat,1,0.0);
    const ResponseId &code= (*this)[0]->getType();
    const int order= code.Size();
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStress();
        for(register int j= 0;j<order;j++)
          if(code(j) == defID)
            retval(i,1)+= s(i);
      }
    return retval;
  }

//! @brief Devuelve el valor de la deformación generalizada en cada punto de integración.
template <class MAT>
m_double MaterialVector<MAT>::getGeneralizedStrain(const int &defID) const
  {
    const size_t nMat= this->size();
    m_double retval(nMat,1,0.0);
    const ResponseId &code= (*this)[0]->getType();
    const int order= code.Size();
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStrain();
        for(register int j= 0;j<order;j++)
          if(code(j) == defID)
            retval(i,1)+= s(i);
      }
    return retval;
  }

//! \brief Asigna valores a las deformaciones iniciales de los
//! materiales de este contenedor.
template <class MAT>
void MaterialVector<MAT>::setInitialGeneralizedStrains(const std::vector<Vector> &iS)
  {
    const size_t nMat= this->size();
    const size_t sz= std::min(nMat,iS.size());
    if(iS.size()<nMat)
      std::cerr << "MaterialVector::setInitialGeneralizedStrains; se recibieron: "
                << iS.size() << " vectores deformación, se esperaban: "
                << nMat << ".\n";
    for(size_t i= 0;i<sz;i++)
      (*this)[i]->setInitialGeneralizedStrain(iS[i]);
  }

//! \brief Añade los valores que se pasan como parámetro a las deformaciones iniciales de los
//! materiales de este contenedor.
template <class MAT>
void MaterialVector<MAT>::addInitialGeneralizedStrains(const std::vector<Vector> &iS)
  {
    const size_t nMat= this->size();
    const size_t sz= std::min(nMat,iS.size());
    if(iS.size()<nMat)
      std::cerr << "MaterialVector::setInitialGeneralizedStrains; se recibieron: "
                << iS.size() << " vectores deformación, se esperaban: "
                << nMat << ".\n";
    for(size_t i= 0;i<sz;i++)
      (*this)[i]->addInitialGeneralizedStrain(iS[i]);
  }

//! \brief Initialize initial strains.
template <class MAT>
void MaterialVector<MAT>::zeroInitialGeneralizedStrains(void)
  {
    const size_t nMat= this->size();
    for(size_t i= 0;i<nMat;i++)
      (*this)[i]->zeroInitialGeneralizedStrain();
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
template <class MAT>
XC::DbTagData &MaterialVector<MAT>::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
template <class MAT>
int MaterialVector<MAT>::sendData(CommParameters &cp)
  {
    int res= 0;
    if(this->empty())
      setDbTagDataPos(0,0);
    else
      {
        setDbTagDataPos(0,1);
        const size_t nMat= this->size();
        DbTagData cpMat(nMat*3);

        for(size_t i= 0;i<nMat;i++)
          res+= cp.sendBrokedPtr((*this)[i],cpMat,BrokedPtrCommMetaData(i,i+nMat,i+2*nMat));
        res+= cpMat.send(getDbTagData(),cp,CommMetaData(1));
      }
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
template <class MAT>
int MaterialVector<MAT>::recvData(const CommParameters &cp)
  {
    const int flag = getDbTagDataPos(0);
    int res= 0;
    if(flag!=0)
      {
        const size_t nMat= this->size();
        DbTagData cpMat(nMat*3);
        res+= cpMat.receive(getDbTagData(),cp,CommMetaData(1));

        for(size_t i= 0;i<nMat;i++)
          {
            const BrokedPtrCommMetaData meta(i,i+nMat,i+2*nMat);
            // Receive the material
            (*this)[i]= cp.getBrokedMaterial((*this)[i],cpMat,meta);
          }
      }
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
template <class MAT>
int MaterialVector<MAT>::sendSelf(CommParameters &cp)
  {
    inicComm(2);
    int res= sendData(cp);
    const int dataTag=getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING MaterialVector::sendSelf() - " 
                << dataTag << " failed to send ID";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
template <class MAT>
int MaterialVector<MAT>::recvSelf(const CommParameters &cp)
  {
    const int dataTag= this->getDbTag();
    inicComm(2);
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "WARNING MaterialVector::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }

} // end of XC namespace

#endif
