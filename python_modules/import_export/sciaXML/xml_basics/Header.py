# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import HeaderItem as hi
import xml.etree.cElementTree as ET


class Header(object):
  ''' Header for SCIA XML. '''
  def __init__(self,h0= hi.HeaderItem(), h1= hi.HeaderItem(), h2= hi.HeaderItem(), h3= hi.HeaderItem(), h4= hi.HeaderItem(), h5= hi.HeaderItem(), h6= hi.HeaderItem(), h7= hi.HeaderItem(), h8= hi.HeaderItem(), h9= hi.HeaderItem(), h10= hi.HeaderItem(), h11= hi.HeaderItem(), h12= hi.HeaderItem(), h13= hi.HeaderItem(), h14= hi.HeaderItem(), h15= hi.HeaderItem(), h16= hi.HeaderItem(), h17= hi.HeaderItem(), h18= hi.HeaderItem(), h19= hi.HeaderItem(), h20= hi.HeaderItem()):
      self.h0= h0
      self.h1= h1
      self.h2= h2
      self.h3= h3
      self.h4= h4
      self.h5= h5
      self.h6= h6
      self.h7= h7
      self.h8= h8
      self.h9= h9
      self.h10= h10
      self.h11= h11
      self.h12= h12
      self.h13= h13
      self.h14= h14
      self.h15= h15
      self.h16= h16
      self.h17= h17
      self.h18= h18
      self.h19= h19
      self.h20= h20

  def getH0(self):
    return self.h0
  def setH0(self, h0):
    self.h0 = h0

  def getH1(self):
    return self.h1

  def setH1(self, h1):
    self.h1 = h1

  def getH2(self):
    return self.h2

  def setH2(self, h2):
    self.h2 = h2

  def getH3(self):
    return self.h3

  def setH3(self, h3):
    self.h3 = h3

  def getH4(self):
    return self.h4

  def setH4(self, h4):
    self.h4 = h4

  def getH5(self):
    return self.h5

  def setH5(self, h5):
    self.h5 = h5

  def getH6(self):
    return self.h6

  def setH6(self, h6):
    self.h6 = h6

  def getH7(self):
    return self.h7

  def setH7(self, h7):
    self.h7 = h7

  def getH8(self):
    return self.h8

  def setH8(self, h8):
    self.h8 = h8

  def getH9(self):
    return self.h9

  def setH9(self, h9):
    self.h9 = h9

  def getH10(self):
    return self.h10

  def setH10(self, h10):
    self.h10 = h10

  def getH11(self):
    return self.h11

  def setH11(self, h11):
    self.h11 = h11

  def getH12(self):
    return self.h12

  def setH12(self, h12):
    self.h12 = h12

  def getH13(self):
    return self.h13

  def setH13(self, h13):
    self.h13 = h13

  def getH14(self):
    return self.h14

  def setH14(self, h14):
    self.h14 = h14

  def getH15(self):
    return self.h15

  def setH15(self, h15):
    self.h15 = h15

  def getH16(self):
    return self.h16

  def setH16(self, h16):
    self.h16 = h16

  def getH17(self):
    return self.h17

  def setH17(self, h17):
    self.h17 = h17

  def getH18(self):
    return self.h18

  def setH18(self, h18):
    self.h18 = h18

  def getH19(self):
    return self.h19

  def setH19(self, h19):
    self.h19 = h19

  def getH20(self):
    return self.h20

  def setH20(self, h20):
    self.h20 = h20

  def getXMLElement(self,parent):
    '''Returns the corresponding XML element for the header.
       Parameters:
       parent: owner of this object in the XML structure.'''
    hh= ET.SubElement(parent,"h")
    cont= 0
    items=[self.h0, self.h1, self.h2, self.h3, self.h4, self.h5, self.h6, self.h7, self.h8, self.h9, self.h10, self.h11, self.h12, self.h13, self.h14, self.h15, self.h16, self.h17, self.h18, self.h19, self.h20]
    for i in items:
      if i is not None:
        if(i.t != ''):
          ii= i.getXMLElement(hh,cont)
      cont+= 1
    return hh
