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
//MapSuperficies.h

#ifndef MAPSuperficies_H
#define MAPSuperficies_H

#include "MapEnt.h"
#include "Face.h"

namespace XC {
  class SupCuadrilatera;

//! @ingroup Cad
//
//! @brief Contenedor de puntos del modelo.
class MapSuperficies: public MapEnt<Face>
  {
  private:
    void UpdateSets(Face *) const;
  protected:
    virtual bool procesa_comando(CmdStatus &status);

  public:
    MapSuperficies(Cad *cad= NULL);

    bool conciliaNDivs(void);
    bool checkNDivs(void) const;

    template <class F>
    Face *Nueva(void);

    template <class F>
    Face *Nueva(CmdStatus &);

    SupCuadrilatera *newQuadSurfacePts(const size_t &, const size_t &,const size_t &,const size_t &);
    SupCuadrilatera *newQuadSurfaceLines(const size_t &, const size_t &,const size_t &,const size_t &);
    SupCuadrilatera *newQuadSurfaceGridPoints(const boost::python::list &);

    any_const_ptr GetProp(const std::string &cod) const;
  };


//! @brief Crea una nueva superficie.
template <class F>
Face *MapSuperficies::Nueva(void)
  {
    Face *retval= busca(getTag());
    if(!retval) //La superficie es nueva.
      {
        Preprocessor *preprocessor= getPreprocessor();
        retval= new F(preprocessor);
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

//! @brief Lee una superficie.
template <class F>
Face *MapSuperficies::Nueva(CmdStatus &status)
  {
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    bool nueva= true;
    size_t old_tag= getTag();
    if(fnc_indices.size()>0)
      {
        setTag(convert_to_size_t(fnc_indices[0])); //Identificador del punto.
        if(existe(getTag()))
          nueva= false;
      }
    Face *retval= Nueva<F>();
    if(!nueva)
      setTag(old_tag);
    retval->LeeCmd(status);
    this->conciliaNDivs();
    if(!this->checkNDivs())
	  std::cerr << "MapSuperficies::Nueva; error al agregar la superficie: "
                    << retval->GetNombre() << std::endl;
    return retval;
  }

} //fin namespace XC
#endif
