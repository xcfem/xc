# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import node_properties as ncd
import material_properties as mp
import ep_plane_properties as eppp
import node_support_properties as nsp
from scia_loads import LoadGroupProperties as lgp
from scia_loads import LoadCaseProperties as lcp
from scia_loads import LoadCombProperties as lcmbp
from scia_loads import NodeLoadProperties as nlp
from scia_loads import ElementLoadProperties as elp
from scia_loads import PointForceFreeProperties as pffp
from scia_loads import SurfacePressureFreeProperties as sffp
import xml.etree.cElementTree as ET

class ProjectProperties(object):    
    
  def __init__(self,xmlns= 'http://www.scia.cz', fileName= ''):
    self.xmlns= xmlns
    self.fileName= fileName
    self.nodeProperties= ncd.NodeProperties()
    self.materialProperties= mp.MaterialProperties()
    self.epPlaneProperties= eppp.EPPlaneProperties()
    self.nodeSupportProperties= nsp.NodeSupportProperties()
    self.loadGroupProperties= lgp.LoadGroupProperties()
    self.loadCaseProperties= lcp.LoadCaseProperties()
    self.loadCombProperties= lcmbp.LoadCombProperties()
    self.nodeLoadProperties= nlp.NodeLoadProperties()
    self.elementLoadProperties= elp.ElementLoadProperties()
    self.pointLoadFreeProperties= pffp.PointForceFreeProperties()
    self.surfaceLoadFreeProperties= sffp.SurfacePressureFreeProperties()

  def getXMLElement(self,defFileName):
    '''Returns the corresponding XML element for the object.
       Parameters:
       defFileName: XML definition file name.'''
    project= ET.Element("def_project")
    project.set("xmlns",self.xmlns)
    containers= [self.nodeProperties, self.materialProperties, self.epPlaneProperties, self.nodeSupportProperties, self.loadGroupProperties, self.loadCaseProperties, self.loadCombProperties, self.nodeLoadProperties, self.elementLoadProperties, self.pointLoadFreeProperties, self.surfaceLoadFreeProperties]
    for c in containers:
      elem= c.getXMLElement(project)
    return project

  def getXMLTree(self,defFileName):
    '''Returns the corresponding XML tree.'''
    project= self.getXMLElement(defFileName)
    tree = ET.ElementTree(project)
    return tree

  def writeXMLFile(self): 
    '''Writes the corresponding XML element in a file.'''
    tree= self.getXMLTree(self.fileName)
    tree.write(self.fileName,encoding="UTF-8", xml_declaration=None, default_namespace=None, method="xml")
