# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import scxml_base as b
import scxml_table_xmlnodes as tn
import xml.etree.cElementTree as ET

class SCXMLTableContainer(b.SCXMLBase):
  ''' Table container. '''
  def __init__(self,id,t,table= None):
    ''' Constructor.
        Parameters:
        tables: tables to populate the container.
    '''
    super(SCXMLTableContainer,self).__init__(id,t)
    self.tables= list()
    if(table):
      self.tables.append(table)

  def getTables(self):
    '''Returns container tables.'''
    return self.tables
  def appendTable(self,table):
    '''Appends a table into the container.'''
    self.tables.append(table)
        
  def getXMLElement(self,parent):
    '''Returns an XML element for the container.
       Parameters:
       parent: owner of this object in the XML structure.'''
    container= ET.SubElement(parent,"container")
    super(SCXMLTableContainer,self).populateXMLElement(container)
    print 'container id= ', self.id, ' table number: ', len(self.tables)
    for tb in self.tables:
      xmlTB= tb.getXMLElement(container)
    return container

  
