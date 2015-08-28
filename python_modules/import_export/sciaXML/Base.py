# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

class Base(object):
  id= ''
  t= ''

  def __init__(self,id,t):
    self.id = id
    self.t = t

  def getId(self):
    return self.id
  def setId(self,id):
    self.id = id
    
  def getT(self):
    return self.t
  def setT(self,t):
    self.t = t

  def populateXMLElement(self,xmlElement):
    if(self.id!=''):
      xmlElement.set("id",self.id)
    if(self.t!=''):
      xmlElement.set("t",self.t)
        

