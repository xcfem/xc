# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import NodeProperties as ncd
import EPPlaneProperties as eppp
import xml.etree.cElementTree as ET

class ProjectProperties(object):    
    
  xmlns= 'http://www.scia.cz'
  fileName= ''
  nodeProperties= ncd.NodeProperties()
  epPlaneProperties= eppp.EPPlaneProperties()

  def __init__(self,xmlns= 'http://www.scia.cz', fileName= ''):
    self.xmlns= xmlns
    self.fileName= fileName

  def getXMLElement(self,defFileName):
    project= ET.Element("def_project")
    project.set("xmlns",self.xmlns)
    containers= [self.nodeProperties,self.epPlaneProperties]
    for c in containers:
      elem= c.getXMLElement(project)
    return project

  def getXMLTree(self,defFileName):
    project= self.getXMLElement(defFileName)
    tree = ET.ElementTree(project)
    return tree

  def writeXMLFile(self): 
    tree= self.getXMLTree(self.fileName)
    tree.write(self.fileName,encoding="UTF-8", xml_declaration=None, default_namespace=None, method="xml")
