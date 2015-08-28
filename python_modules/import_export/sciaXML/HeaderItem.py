# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub
import xml.etree.cElementTree as ET

class HeaderItem(object):
  t= ''

  def __init__(self,t= ''):
    self.t = t

  def getT(self):
    return self.t
  def setT(self,t):
    self.t= t

  def getXMLElement(self,parent, cont):
    itemId= 'h'+str(cont) 
    hi= ET.SubElement(parent,itemId)
    hi.set("t",self.t)
    return hi
