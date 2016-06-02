# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from import_export.sciaXML.xml_basics import ObjectItem as oI
import LoadCaseContainer as lcc

class LoadComponentBase(object):
  ''' Each of the load components (X, Z or Z).'''
  def __init__(self, loadCaseId, loadCaseName, direction, value, globalCooSys= True):
    ''' Constructor.
        Parameters:
        loadCaseId: load case identifier.
        loadCaseName: load case name.
        direction: load direction.
        value: load value.
        globalCooSys: true if load is defined in the global coordinate system.
    '''
    self.loadCaseId= loadCaseId
    self.loadCaseName= loadCaseName
    self.direction= direction
    self.value= value
    self.globalCooSys= globalCooSys

  def getLoadCaseName(self):
    '''Returns load case name.'''
    retval= self.loadCaseName
    if(retval==''):
      retval= lcc.loadCasePrefix+lcId
    return retval

  def getLoadCaseReferenceItem(self):
    lcId= str(self.loadCaseId) #Reference to load case.
    retval= oI.ObjectItem('',lcId)
    retval.n= self.getLoadCaseName()
    return retval

  def getDirectionObjectItem(self):
    '''returns an item which represents load component direction.'''
    dirId= '0'
    if(self.direction=='Y'):
      dirId= '1'
    elif(self.direction=='Z'):
      dirId= '2'
    return oI.ObjectItem(dirId,'','',self.direction) #Direction X, Y or Z

  def getValueObjectItem(self):
    '''returns an ObjectItem which represents the value of the load component.'''
    return oI.ObjectItem(str(self.value))

  def getSystemItem(self):
    '''returns an ObjectItem which represents the reference system of the load component.'''
    if(self.globalCooSys):
      return oI.ObjectItem('0','','','GCS')
    else:
      return oI.ObjectItem('1','','','LCS')

