# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

class SCXMLBase(object):
  '''Base class for al sciaXML module objects.'''
  id= ''
  t= ''

  def __init__(self,id,t):
    ''' Constructor.
        Parameters:
        id: identifier.
        t: t field.
    '''
    self.id = id
    self.t = t

  def getId(self):
    '''Returns object identifier.'''
    return self.id
  def setId(self,id):
    '''Assigns object idenfier.'''
    self.id = id
    
  def getT(self):
    '''Returns T field.'''
    return self.t
  def setT(self,t):
    '''Assigns T field.'''
    self.t = t

  def populateXMLElement(self,xmlElement):
    '''Inserts items on xmlElement object.'''
    if(self.id!=''):
      xmlElement.set("id",self.id)
    if(self.t!=''):
      xmlElement.set("t",self.t)
        

