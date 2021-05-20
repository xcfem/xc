//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_utils.cc

#include "python_utils.h"
#include "utility/kernel/CommandEntity.h"
#include "utility/kernel/NamedEntity.h"
#include "utility/matrices/ProtoMatrix.h"
#include "CImg.h"

typedef std::vector<int> std_vector_int;

std::vector<int> vector_int_from_py_list(const boost::python::list &l)
  {
    std::vector<int> retval;
    const size_t sz= len(l);
    for(size_t i= 0;i<sz;++i)
      retval.push_back(boost::python::extract<int>(l[i]));
    return retval;
  }

std::vector<double> vector_double_from_py_list(const boost::python::list &l)
  {
    std::vector<double> retval;
    const size_t sz= len(l);
    for(size_t i= 0;i<sz;++i)
      retval.push_back(boost::python::extract<double>(l[i]));
    return retval;
  }

boost::python::list vector_int_to_py_list(const std::vector<int> &l)
  {
    boost::python::list retval;
    BOOST_FOREACH(const int& p, l) retval.append(p);
    return retval;
  }

boost::python::list vector_double_to_py_list(const std::vector<double> &l)
  {
    boost::python::list retval;
    BOOST_FOREACH(const double& p, l) retval.append(p);
    return retval;
  }

std::vector<std::string> vector_string_from_py_list(const boost::python::list &l)
  {
    std::vector<std::string> retval;
    const size_t sz= len(l);
    for(size_t i= 0;i<sz;++i)
      retval.push_back(boost::python::extract<std::string>(l[i]));
    return retval;
  }

std::set<int> set_int_from_py_list(const boost::python::list &l)
  {
    std::set<int> retval;
    const size_t sz= len(l);
    for(size_t i= 0;i<sz;++i)
      retval.insert(boost::python::extract<int>(l[i]));
    return retval;
  }

inline bool file_exists(const std::string &FileName)
  {
    struct stat my_stat;
    return (stat(FileName.c_str(), &my_stat) == 0);
  }


typedef cimg_library::CImg<unsigned char> CImagen;

float mse_imagenes(const CImagen &a,const CImagen &b)
  {
    float retval= -1.0;
    if(&a == &b)
      return retval;
    else if(!a.is_sameXY(b))
      retval= 1e38;
    else
      retval= a.MSE(b);
    return retval;
  }

float psnr_imagenes(const CImagen &a,const CImagen &b)
  {
    float retval= -1.0;
    if(&a == &b)
      return retval;
    else if(!a.is_sameXY(b))
      retval= 1e38;
    else
      retval= a.PSNR(b);
    return retval;
  }

float compara_imagenes(const CImagen &a,const CImagen &b)
  {
    float retval= -1.0;
    if(&a == &b)
      return retval;
    else if(!a.is_sameXY(b))
      retval= 1e38;
    else
      {
        const double mse= a.MSE(b);
        const double invPsnr= 1.0/std::max(a.PSNR(b),1.0);
        retval= mse*mse+invPsnr*invPsnr;
      }
    return retval;
  }

float mseImages(const std::string &fNameA, const std::string &fNameB)
  {
    float retval= -1.0;
    if(file_exists(fNameA) & file_exists(fNameB))
      {
        const CImagen imgA(fNameA.c_str());
        const CImagen imgB(fNameB.c_str());
        retval= mse_imagenes(imgA,imgB);
      }
    else
      {
        if(!file_exists(fNameA))
          std::cerr << "mseImagenes; no se encontró el archivo: '"
                    << fNameA << "'\n";
        if(!file_exists(fNameB))
	  std::cerr << "mseImagenes; no se encontró el archivo: '"
                    << fNameB << "'\n";
      }
    return retval;
  }

float psnrImagenes(const std::string &fNameA, const std::string &fNameB)
  {
    float retval= -1.0;
    if(file_exists(fNameA) & file_exists(fNameB))
      {
        const CImagen imgA(fNameA.c_str());
        const CImagen imgB(fNameB.c_str());
        retval= psnr_imagenes(imgA,imgB);
      }
    else
      {
        if(!file_exists(fNameA))
          std::cerr << "psnrImagenes; no se encontró el archivo: '"
                    << fNameA << "'\n";
        if(!file_exists(fNameB))
	  std::cerr << "psnrImagenes; no se encontró el archivo: '"
                    << fNameB << "'\n";
      }
    return retval;
  }

float compare_images(const std::string &fNameA, const std::string &fNameB)
  {
    float retval= -1.0;
    if(file_exists(fNameA) & file_exists(fNameB))
      {
        const CImagen imgA(fNameA.c_str());
        const CImagen imgB(fNameB.c_str());
        retval= compara_imagenes(imgA,imgB);
      }
    else
      {
        if(!file_exists(fNameA))
          std::cerr << "psnrImagenes; no se encontró el archivo: '"
                    << fNameA << "'\n";
        if(!file_exists(fNameB))
	  std::cerr << "psnrImagenes; no se encontró el archivo: '"
                    << fNameB << "'\n";
      }
    return retval;
  }

// boost::python::set vector_int_to_py_set(const std::set<int> &l)
//   {
//     boost::python::set retval;
//     BOOST_FOREACH(const int& p, l) retval.append(p);
//     return retval;
//   }

boost::python::object CommandEntity_eval(boost::python::object self, const std::string &str)
  {
    boost::python::object retval;
    if(!str.empty())
      {
        // Get a handle to the Python object's __dict__.
        boost::python::object self_dict= self.attr("__dict__");

        // Inject the __builtin__ module into the Python object's __dict__.
        #ifdef PYTHON2
          self_dict["__builtins__"]= boost::python::import("__builtin__");
	#else
          self_dict["__builtins__"]= boost::python::import("builtins");
        #endif  
        self_dict["self"]= self;

        // Extract the C++ object from the Python object.
        CommandEntity &ent= boost::python::extract<CommandEntity &>(self);

        // Pass the Python object's `__dict__` to the C++ object.
        retval= ent.evalPy(self_dict,str);
      }
    return retval;
  }

boost::python::object CommandEntity_exec(boost::python::object self, const std::string &block)
  {
    boost::python::object retval;
    if(!block.empty())
      {
        // Get a handle to the Python object's __dict__.
        boost::python::object self_dict= self.attr("__dict__");

        // Inject the __builtin__ module into the Python object's __dict__.
        #ifdef PYTHON2
          self_dict["__builtins__"]= boost::python::import("__builtin__");
	#else
          self_dict["__builtins__"]= boost::python::import("builtins");
        #endif  

        self_dict["self"]= self;

        // Extract the C++ object from the Python object.
        CommandEntity &ent= boost::python::extract<CommandEntity &>(self);

        // Pass the Python object's `__dict__` to the C++ object.
        retval= ent.execPy(self_dict,block);
      }
    return retval;
  }

boost::python::object CommandEntity_exec_file(boost::python::object self, const std::string &fileName)
  {
    boost::python::object retval;
    if(!fileName.empty())
      {
        // Get a handle to the Python object's __dict__.
        boost::python::object self_dict= self.attr("__dict__");

        // Inject the __builtin__ module into the Python object's __dict__.
        #ifdef PYTHON2
          self_dict["__builtins__"]= boost::python::import("__builtin__");
	#else
          self_dict["__builtins__"]= boost::python::import("builtins");
        #endif  

        self_dict["self"]= self;

        // Extract the C++ object from the Python object.
        CommandEntity &ent= boost::python::extract<CommandEntity &>(self);

        // Pass the Python object's `__dict__` to the C++ object.
        retval= ent.execFilePy(self_dict,fileName);
      }
    return retval;
  }

