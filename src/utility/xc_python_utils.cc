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
//xc_python_utils.cpp

#include "xc_python_utils.h"
#include <boost/python/extract.hpp>
#include "utility/matrix/ID.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/nucleo/python_utils.h"


boost::python::list XC::xc_id_to_py_list(const XC::ID &id)
  {
    boost::python::list retval;
    const size_t sz= id.Size();
    for(size_t i= 0;i<sz;++i)
      retval.append(id(i));
    return retval;
  }

std::vector<double> XC::vector_double_from_py_object(const boost::python::object &o)
  {
    std::vector<double> retval;
    boost::python::extract<double> x(o);
    if (x.check())
      { retval= std::vector<double>(1,x()); }
    else
      {
        boost::python::extract<std::vector<double> > v(o);
        if(v.check())
          retval= v();
        else
	  {
            boost::python::extract<XC::Vector> vt(o);
            if(vt.check())
              retval= vector_to_std_vector(vt());
            else
              {
                boost::python::extract<boost::python::list> l(o);
                if(l.check())
                  retval= vector_double_from_py_list(l());
              }
          }
      }
    return retval;
  }

std::vector<int> XC::vector_int_from_py_object(const boost::python::object &o)
  {
    std::vector<int> retval;
    boost::python::extract<int> x(o);
    if (x.check())
      { retval= std::vector<int>(1,x()); }
    else
      {
        boost::python::extract<std::vector<int> > v(o);
        if(v.check())
          retval= v();
        else
	  {
            boost::python::extract<XC::ID > vt(o);
            if(vt.check())
              retval= id_to_std_vector(vt());
            else
              {
                boost::python::extract<boost::python::list> l(o);
                if(l.check())
                  retval= vector_int_from_py_list(l());
              }
          }
      }
    return retval;
  }

m_double XC::m_double_from_py_object(const boost::python::object &o)
  {
    m_double retval;
    boost::python::extract<double> x(o);
    if (x.check())
      { retval= m_double(1,1,x()); }
    else
      {
        boost::python::extract<m_double> v(o);
        if(v.check())
          retval= v();
        else
	  {
            boost::python::extract<XC::Matrix > mt(o);
            if(mt.check())
              retval= matrix_to_m_double(mt());
          }
      }
    return retval;
  }
