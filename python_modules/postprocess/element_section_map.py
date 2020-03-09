# -*- coding: utf-8 -*-

from miscUtils import log_messages as lmsg

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2016, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

class ElementSectionMap(dict):
  '''dictionary that stores a section name(s)
     for each element number. This way it defines
     a spatial distribution of the sections over
     the structure.'''
  propName= 'sectionName'
  def assign(self,elemSet,setRCSects):
    '''Assigns the sections names: setRCSectsName+'1', setRCSectsName+'2', ...
       to the elements of the set.

       :param elemSet: set of elements that receive the section name property.
       :param setRCSects: RC section definition, name, concrete type,
                          rebar positions,...
    '''
    if len(elemSet)== 0:
        lmsg.warning("element set is empty\n")
      
    for e in elemSet:
      if(not e.hasProp(self.propName)):
        self[e.tag]=[]
        for i in range(len(setRCSects.lstRCSects)):
          sname=setRCSects.name+str(i+1)
          self[e.tag].append(sname)
        e.setProp(self.propName,setRCSects.name)
      else:
        lmsg.error("element: "+ str(e.tag) + " has already a section ("+e.getProp(self.propName)+")\n")

def loadElementSectionMap():
  lmsg.error('loadElementSectionMap DEPRECATED use loadRCMaterialDistribution' )


