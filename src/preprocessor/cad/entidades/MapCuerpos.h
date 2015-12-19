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
//MapCuerpos.h

#ifndef MapCuerpos_H
#define MapCuerpos_H

#include "MapEnt.h"
#include "Body.h"

namespace XC {

//! @ingroup Cad
//
//! @brief Contenedor de puntos del modelo.
class MapCuerpos: public MapEnt<Body>
  {
  private:
    void UpdateSets(Body *) const;
  protected:
    virtual bool procesa_comando(CmdStatus &status);

  public:
    MapCuerpos(Cad *cad= NULL);

    bool conciliaNDivs(void);
    bool checkNDivs(void) const;

    template <class B>
    Body *Nuevo(void);

    template <class B>
    Body *Nuevo(CmdStatus &);

    any_const_ptr GetProp(const std::string &cod) const;
  };


//! @brief Crea un nuevo cuerpo.
template <class B>
Body *MapCuerpos::Nuevo(void)
  {
    Body *retval= busca(getTag());
    if(!retval) //El cuerpo es nuevo.
      {
        Preprocessor *preprocessor= getPreprocessor();
        retval= new B(preprocessor);
        if(retval)
          {
            retval->Nombre()= "f"+boost::lexical_cast<std::string>(getTag());
            (*this)[getTag()]= retval;
            UpdateSets(retval);
            tag++;
	  }
      }
    return retval;
  }

//! @brief Lee un cuerpo.
template <class B>
Body *MapCuerpos::Nuevo(CmdStatus &status)
  {
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    bool nuevo= true;
    size_t old_tag= getTag();
    if(fnc_indices.size()>0)
      {
        setTag(convert_to_size_t(fnc_indices[0])); //Identificador del punto.
        if(existe(getTag()))
          nuevo= false;
      }
    Body *retval= Nuevo<B>();
    if(!nuevo)
      setTag(old_tag);
    retval->LeeCmd(status);
    this->conciliaNDivs();
    if(!this->checkNDivs())
	  std::cerr << "MapCuerpos::Nuevo; error al agregar el cuerpo: "
                    << retval->GetNombre() << std::endl;
    return retval;
  }

} //end of XC namespace
#endif
