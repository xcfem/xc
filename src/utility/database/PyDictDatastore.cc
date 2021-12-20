//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
// PyDictDatastore.cpp

#include <utility/database/PyDictDatastore.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>
#include "boost/lexical_cast.hpp"

XC::PyDictDatastore::PyDictDatastore(const std::string &projectName, Preprocessor &preprocessor, FEM_ObjectBroker &theObjectBroker, int run)
  : FE_Datastore(preprocessor, theObjectBroker), pyDict()
  {}

int XC::PyDictDatastore::sendMsg(int dataTag, int commitTag,const XC::Message &,ChannelAddress *theAddress)
  {
    std::cerr << "PyDictDatastore::sendMsg() - not yet implemented\n";
    return -1;
  }

int XC::PyDictDatastore::recvMsg(int dataTag, int commitTag, Message &, ChannelAddress *theAddress)
  {
    std::cerr << "PyDictDatastore::recvMsg() - not yet implemented\n";
    return -1;
  }

//! @brief Inserts double data.
void XC::PyDictDatastore::insertData(const std::string &tbName,const int &dbTag,const int &commitTag,const double *data, const int &sz)
  {
    if(!pyDict.has_key(tbName))
      { pyDict[tbName]= boost::python::dict(); }
    boost::python::dict tableDict= boost::python::extract<boost::python::dict>(pyDict[tbName]);
    if(!tableDict.has_key(dbTag))
      { tableDict[dbTag]= boost::python::dict(); }
    boost::python::dict objDict= boost::python::extract<boost::python::dict>(tableDict[dbTag]);
    if(!objDict.has_key(commitTag))
      { objDict[commitTag]= boost::python::list(); }
    boost::python::list valueList= boost::python::extract<boost::python::list>(objDict[commitTag]);
    valueList= boost::python::list();
    for(int i= 0; i<sz;i++)
      {
	const double &v= data[i];
        valueList.append(v);
      }
  }

//! @brief Inserts integer data.
void XC::PyDictDatastore::insertData(const std::string &tbName,const int &dbTag,const int &commitTag,const int *data, const int &sz)
  {
    if(!pyDict.has_key(tbName))
      { pyDict[tbName]= boost::python::dict(); }
    boost::python::dict tableDict= boost::python::extract<boost::python::dict>(pyDict[tbName]);
    if(!tableDict.has_key(dbTag))
      { tableDict[dbTag]= boost::python::dict(); }
    boost::python::dict objDict= boost::python::extract<boost::python::dict>(tableDict[dbTag]);
    if(!objDict.has_key(commitTag))
      { objDict[commitTag]= boost::python::list(); }
    boost::python::list valueList= boost::python::extract<boost::python::list>(objDict[commitTag]);
    valueList= boost::python::list();
    for(int i= 0; i<sz;i++)
      {
	const int &v= data[i];
        valueList.append(v);
      }
  }

std::vector<double> XC::PyDictDatastore::retrieveDoubleData(const std::string &tbName,const int &dbTag,const int &commitTag)
  {
    std::vector<double> retval;
    if(!pyDict.has_key(tbName))
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ERROR: " << tbName << " not found." << std::endl;
    else
      { 
	boost::python::dict tableDict= boost::python::extract<boost::python::dict>(pyDict[tbName]);
	if(!tableDict.has_key(dbTag))
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; ERROR: key " << dbTag << " not found." << std::endl;
  	else
	  { 
	    boost::python::dict objDict= boost::python::extract<boost::python::dict>(tableDict[dbTag]);
	    if(!objDict.has_key(commitTag))
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; ERROR: commit tag: " << commitTag << " not found." << std::endl;
	    else
	      { 
		boost::python::list valueList= boost::python::extract<boost::python::list>(objDict[commitTag]);
		const int sz= len(valueList);
		for(int i= 0; i<sz;i++)
		  { retval[i]= boost::python::extract<double>(valueList[i]); }
	      }
	  }
      }
    return retval;
  }

std::vector<int> XC::PyDictDatastore::retrieveIntData(const std::string &tbName,const int &dbTag,const int &commitTag)
  {
    std::vector<int> retval;
    if(!pyDict.has_key(tbName))
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ERROR: " << tbName << " not found." << std::endl;
    else
      { 
	boost::python::dict tableDict= boost::python::extract<boost::python::dict>(pyDict[tbName]);
	if(!tableDict.has_key(dbTag))
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; ERROR: key " << dbTag << " not found." << std::endl;
  	else
	  { 
	    boost::python::dict objDict= boost::python::extract<boost::python::dict>(tableDict[dbTag]);
	    if(!objDict.has_key(commitTag))
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; ERROR: commit tag: " << commitTag << " not found." << std::endl;
	    else
	      { 
		boost::python::list valueList= boost::python::extract<boost::python::list>(objDict[commitTag]);
		const int sz= len(valueList);
		for(int i= 0; i<sz;i++)
		  { retval[i]= boost::python::extract<int>(valueList[i]); }
	      }
	  }
      }
    return retval;
  }

int XC::PyDictDatastore::sendMatrix(int dbTag, int commitTag, const Matrix &theMatrix, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en PyDictDatastore::sendMatrix." << std::endl;
    insertData("Matrices",dbTag,commitTag,theMatrix.getDataPtr(),theMatrix.getDataSize());
    return 0;
  }

int XC::PyDictDatastore::recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en PyDictDatastore::recvMatrix." << std::endl;
    const std::vector<double> blobData= retrieveDoubleData("Matrices",dbTag,commitTag);
    // place the results into the vectors double array
    double *matrixData= theMatrix.getDataPtr();
    for(int i=0; i<theMatrix.getDataSize(); i++)
      matrixData[i]= blobData[i];
    return 0;
  }

int XC::PyDictDatastore::sendVector(int dbTag, int commitTag, const Vector &theVector, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en PyDictDatastore::sendVector." << std::endl;
    insertData("Vectors",dbTag,commitTag,theVector.getDataPtr(),theVector.Size());
    return 0;
  }

int XC::PyDictDatastore::recvVector(int dbTag, int commitTag, Vector &theVector,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en PyDictDatastore::recvVector." << std::endl;
    const std::vector<double> blobData= retrieveDoubleData("Vectors",dbTag,commitTag);
    // place the results into the vectors double array
    for(int i=0; i<theVector.Size(); i++)
      theVector[i] = blobData[i];
    return 0;
  }


int XC::PyDictDatastore::sendID(int dbTag, int commitTag, const ID &theID, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en PyDictDatastore::sendID." << std::endl;
    insertData("IDs",dbTag,commitTag,theID.getDataPtr(),theID.Size());
    return 0;
  }

int XC::PyDictDatastore::recvID(int dbTag, int commitTag,ID &theID,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en PyDictDatastore::recvID." << std::endl;
    const std::vector<int> blobData= retrieveIntData("IDs",dbTag,commitTag);
    for(int i=0; i<theID.Size(); i++)
      theID[i] = blobData[i];
    return 0;
  }



