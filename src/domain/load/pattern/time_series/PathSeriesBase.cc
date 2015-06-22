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
//PathSeriesBase.cpp


#include "PathSeriesBase.h"
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include <fstream>
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::PathSeriesBase::PathSeriesBase(int classTag, const double &theFactor)	
  :CFactorSeries(classTag,theFactor), lastSendCommitTag(-1) {}

		   
//! @brief Constructor.
XC::PathSeriesBase::PathSeriesBase(int classTag,const Vector &theLoadPath,const double &theFactor)
  :CFactorSeries(classTag,theFactor), thePath(theLoadPath), lastSendCommitTag(-1)
  {}

//! @brief Lee un objeto XC::PathSeriesBase desde archivo
bool XC::PathSeriesBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(PathSeriesBase) Procesando comando: " << cmd << std::endl;
    if(cmd == "set_path")
      {
	thePath= Vector(crea_vector_double(status.GetString()));
        return true;
      }
    else
      return CFactorSeries::procesa_comando(status);
  }

//! @brief Devuelve el número de puntos que definen el path.
size_t XC::PathSeriesBase::getNumDataPoints(void) const
  { return thePath.Size(); }

//! @brief Devuelve el número de datos en el archivo cuyo nombre se pasa como parámetro.
size_t XC::PathSeriesBase::getNumDataPointsOnFile(const std::string &fName) const
  {
    double dataPoint;
    std::ifstream theFile;
    size_t retval= 0;
    // first open and go through file containg path
    theFile.open(fName.c_str(), std::ios::in);
    if(theFile.bad() || !theFile.is_open())
      {
        std::cerr << "WARNING - XC::PathSeriesBase::getNumDataPointsOnFile(fName)";
        std::cerr << " - could not open file " << fName << std::endl;
      }
    else
      {
        while(theFile >> dataPoint)
          retval++;
      }
    theFile.close();
    return retval;
  }

//! @brief Lee desde archivo
size_t XC::PathSeriesBase::load_vector_from_file(Vector &v,std::istream &is)
  {
    size_t count = 0;
    double dataPoint;
    while(is >> dataPoint)
      {
	v(count) = dataPoint;
	count++;
      }
    return count;
  }

//! @brief Devuelve el valor de pico de la función.
double XC::PathSeriesBase::getPeakFactor(void) const
  {
    double peak = fabs(thePath(0));
    const int num = thePath.Size();
    double temp;

    for(int i=1;i<num;i++)
      {
        temp = fabs(thePath(i));
        if(temp > peak)
          peak = temp;
      }
    return (peak*cFactor);
  }

//! @brief Imprime el objeto.
void XC::PathSeriesBase::Print(std::ostream &s, int flag) const
  {
    if(flag == 1)
      s << " specified path: " << thePath;
  }


//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::PathSeriesBase::sendData(CommParameters &cp)
  {
    int res= sendData(cp);
    res+= cp.sendVector(thePath,getDbTagData(),CommMetaData(1));
    res+= cp.sendInt(lastSendCommitTag,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::PathSeriesBase::recvData(const CommParameters &cp)
  {
    int res= recvData(cp);
    res+= cp.receiveVector(thePath,getDbTagData(),CommMetaData(1));
    res+= cp.receiveInt(lastSendCommitTag,getDbTagData(),CommMetaData(2));
    return res;
  }

any_const_ptr XC::PathSeriesBase::GetProp(const std::string &cod) const
  {
    if(cod == "getPath")
      return get_prop_vector(thePath);
    else if(cod == "getNumDataPoints")
      {
        tmp_gp_szt= getNumDataPoints();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod == "getLastSendCommitTag")
      return any_const_ptr(lastSendCommitTag);
    else
      return CFactorSeries::GetProp(cod);
  }
