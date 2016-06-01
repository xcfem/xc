# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import TableBase as tBase
import Header as hdr
import xml.etree.cElementTree as ET

class TableXMLNodes(tBase.TableBase):
  ''' Table of XML nodes (NOT FE nodes). '''
  def __init__(self,id= '',t= '',name= '',h= hdr.Header(),objects= None):
    ''' Constructor.
        Parameters:
        name: table name.
        h: table header
        objects: objects to populate the table.
    '''
    super(TableXMLNodes,self).__init__(id,t,name)
    self.h = h
    if objects:
      self.objects= objects
    else:
      self.objects= list()

  def __len__(self):
    return len(self.objects)
  def getHeader(self):
    '''Returns table header.'''
    return self.h
  def setHeader(self,h):
    '''Sets table header.'''
    self.h = h

  def getObjects(self):
    '''Returs table objects.'''
    return self.objects
  def extend(self, objects):
    '''Extends table objects.'''
    self.objects.extend(objects)
    
  def getXMLElement(self,parent):
    '''Returns the corresponding XML element for the table.
       Parameters:
       parent: owner of this object in the XML structure.'''
    tb= ET.SubElement(parent,"table")
    super(TableXMLNodes,self).populateXMLElement(tb)
    if(self.h is not None):
      header= self.h.getXMLElement(tb)
    for o in self.objects:
      ob= o.getXMLElement(tb)
    return tb

    
    
    

