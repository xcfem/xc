# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Base as b
import xml.etree.cElementTree as ET


class Ref(b.Base):
  progid= ''
  def __init__(self,id,progid):
    super(Ref,self).__init__(id,'')
    self.progid= progid
  
  def getXMLElement(self,parent):
    ref= ET.SubElement(parent,"def_ref")
    super(Ref,self).populateXMLElement(ref)
    if(self.progid!=''):
      ref.set("progid",self.progid)
      

  
