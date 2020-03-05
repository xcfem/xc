# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from xml_basics import scxml_properties_container as ctr
import node_container as nc
from xml_basics import scxml_property as prop
from xml_basics import scxml_properties_table as propTable
import xml.etree.cElementTree as ET

idName= '{4364BC01-AAB7-11D4-B3D9-00104BC3B531}'
idXCoord= '{C1DD759A-4291-481B-B819-92E3AA9E04B7}'
idYCoord= '{A84A15AC-FEC2-4C27-84B4-CFA676615934}'
idZCoord= '{1017EA1E-F76B-4D4B-8A4C-7E58FDED1B02}'
idNodeTable= 'F67B790F-88D4-41ED-A3B9-3D9F9744B5AF' #sin llavecitas

class NodeProperties(ctr.SCXMLPropertiesContainer):
  tableDef= None
  def __init__(self):
    super(NodeProperties,self).__init__(nc.idNodeContainer,nc.idNodeContainer,nc.progIdNodes)
    self.tableDef= propTable.SCXMLPropertiesTable(idNodeTable,"XML\default","vertical",nc.idNodeContainer,nc.progIdNodes)
    self.tableDef.properties.append(prop.SCXMLProperty("0","Name","string",idName))
    self.tableDef.properties.append(prop.SCXMLProperty("1","XCoord","param",idXCoord,"33619968"))
    self.tableDef.properties.append(prop.SCXMLProperty("2","YCoord","param",idYCoord,"33619968"))
    self.tableDef.properties.append(prop.SCXMLProperty("3","ZCoord","param",idZCoord,"33619968"))

  def getXMLElement(self,parent):
    '''Returns the corresponding XML element for the node properties.
       Parameters:
       parent: owner of this object in the XML structure.'''
    container= ET.SubElement(parent,"def_container")
    self.populateXMLElement(container)
    self.tableDef.getXMLElement(container)
    return container

