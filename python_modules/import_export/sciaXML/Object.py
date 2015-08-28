# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import ObjectItem as oi
import ObjBase as ob
import xml.etree.cElementTree as ET

class Object(ob.ObjBase):
	
  def __init__(self,p0= oi.ObjectItem(), p1= oi.ObjectItem(), p2= oi.ObjectItem(), p3= oi.ObjectItem(), p4= oi.ObjectItem(), p5= oi.ObjectItem(), p6= oi.ObjectItem(), p7= oi.ObjectItem(), p8= oi.ObjectItem(), p9= oi.ObjectItem(), p10= oi.ObjectItem(), p11= oi.ObjectItem(), p12= oi.ObjectItem(), p13= oi.ObjectItem(), p14= oi.ObjectItem(), p15= oi.ObjectItem(), p16= oi.ObjectItem(), p17= oi.ObjectItem(), p18= oi.ObjectItem(), p19= oi.ObjectItem(), p20= oi.ObjectItem() ):
    self.nm= ''
    self.p0 = p0
    super(Object,self).__init__('',p1,p2)
    self.p3 = p3
    self.p4 = p4
    self.p5 = p5
    self.p6 = p6
    self.p7 = p7
    self.p8 = p8
    self.p9 = p9
    self.p10 = p10
    self.p11 = p11
    self.p12 = p12
    self.p13 = p13
    self.p14 = p14
    self.p15 = p15
    self.p16 = p16
    self.p17 = p17
    self.p18 = p18
    self.p19 = p19
    self.p20 = p20
	
  def getP0(self):
    return self.p0
  def setP0(self, p0):
    self.p0 = p0

  def getP3(self):
    return self.p3
  def setP3(self, p3):
    self.p3 = p3

  def getP4(self):
    return self.p4
  def setP4(self, p4):
    self.p4 = p4

  def getP5(self):
    return self.p5
  def setP5(self, p5):
    self.p5 = p5

  def getP6(self):
    return self.p6
  def setP6(self, p6):
    self.p6 = p6

  def getP7(self):
    return self.p7
  def setP7(self, p7):
    self.p7 = p7

  def getP8(self):
    return self.p8
  def setP8(self, p8):
    self.p8 = p8

  def getP9(self):
    return self.p9
  def setP9(self, p9):
    self.p9 = p9

  def getP10(self):
    return self.p10
  def setP10(self, p10):
    self.p10 = p10

  def getP11(self):
    return self.p11
  def setP11(self, p11):
    self.p11 = p11

  def getP12(self):
    return self.p12
  def setP12(self, p12):
    self.p12 = p12

  def getP13(self):
    return self.p13
  def setP13(self, p13):
    self.p13 = p13

  def getP14(self):
    return self.p14
  def setP14(self, p14):
    self.p14 = p14

  def getP15(self):
    return self.p15
  def setP15(self, p15):
    self.p15 = p15

  def getP16(self):
    return self.p16
  def setP16(self, p16):
    self.p16 = p16

  def getP17(self):
    return self.p17
  def setP17(self, p17):
    self.p17 = p17

  def getP18(self):
    return self.p18
  def setP18(self, p18):
    self.p18 = p18

  def getP19(self):
    return self.p19
  def setP19(self, p19):
    self.p19 = p19

  def getP20(self):
    return self.p20
  def setP20(self, p20):
    self.p20 = p20

  def getNm(self):
    return self.nm
  def setNm(self, nm):
    self.nm = nm

  def getXMLElement(self,parent):
    oo= ET.SubElement(parent,"obj")
    oo.set("nm",self.nm)
    oo.set("id",self.id)
    cont= 0
    items=[self.p0, self.p1, self.p2, self.p3, self.p4, self.p5, self.p6, self.p7, self.p8, self.p9, self.p10, self.p11, self.p12, self.p13, self.p14, self.p15, self.p16, self.p17, self.p18, self.p19, self.p20]
    for i in items:
      if i is not None:
        if((i.t != '') or (i.v != '') or (i.hasRows()) ):
          ii= i.getXMLElement(oo,cont)
      cont+= 1
    return oo
