# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import os
from xml_basics import Definition as df
import NodeContainer as nCtr
import EPPlaneContainer as eppc
import NodeSupportContainer as nsc
import LoadGroupContainer as lgc
import LoadCaseContainer as lcc
import LoadCombContainer as lcmb
import NodeLoadContainer as nlc
import ElementLoadContainer as elc
import ProjectProperties as prjDef
import xml.etree.cElementTree as ET

class SXMLBase(object):
  def __init__(self,xmlns, mesh,loadContainer):
    self.xmlns= xmlns
    self.uuid= ''
    self.defn= df.Definition(mesh.name)
    print 'XXX Write material container code.'
    self.materialContainer= None
    loads= loadContainer.loads
    self.loadGroupContainer= lgc.LoadGroupContainer(loads.loadGroups)
    self.loadCaseContainer= lcc.LoadCaseContainer(loads.loadCases)
    self.loadCombContainer= lcmb.LoadCombContainer(loads.loadCombs)
    self.fileName= mesh.name+'.xml'
  def getContainers(self):
    return [self.loadGroupContainer, self.loadCaseContainer, self.loadCombContainer]

  def getXMLElement(self,defFileName):
    project= ET.Element("project")
    project.set("xmlns",self.xmlns)
    df= ET.SubElement(project,"def")
    df.set("uri",defFileName)
    containers= self.getContainers()
    for c in containers:
      elem= c.getXMLElement(project)
    return project

  def getXMLTree(self,defFileName):
    project= self.getXMLElement(defFileName)
    tree = ET.ElementTree(project)
    return tree

  def getFileName(self):
    return self.fileName

  def getDefFileName(self):
    return self.fileName + ".def"

  def writeXMLFile(self): 
    defFileName= self.getDefFileName()
    prj_def= prjDef.ProjectProperties(self.xmlns,defFileName)
    def_tree= prj_def.getXMLTree(defFileName)
    def_tree.write(defFileName,encoding="UTF-8", xml_declaration=None, default_namespace=None, method="xml")
    tree= self.getXMLTree(defFileName)
    tree.write(self.fileName,encoding="UTF-8", xml_declaration=None, default_namespace=None, method="xml")

  def indent(self):
    os.system("xmlindent -w "+self.fileName)
    os.system("xmlindent -w "+self.getDefFileName())
