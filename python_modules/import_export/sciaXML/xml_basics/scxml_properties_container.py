# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import scxml_base as b
import xml.etree.cElementTree as ET

class SCXMLPropertiesContainer(b.SCXMLBase):
  '''SCIA XML properties container.'''
  clsid= ''
  progid= ''
  def __init__(self,cId,clsid,progid):
    ''' Constructor.
        Parameters:
        cId: container identifier.
        clsid: class identifier
        progid: program identifier.
    '''
    super(SCXMLPropertiesContainer,self).__init__(cId,'')
    self.clsid= clsid
    self.progid= progid
  
  def populateXMLElement(self, element):
    '''Returns an XML element for the object.'''
    super(SCXMLPropertiesContainer,self).populateXMLElement(element)
    element.set("clsid",self.clsid)
    if(self.progid!=''):
      element.set("progid",self.progid)
      

  
