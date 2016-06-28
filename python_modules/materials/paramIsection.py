# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "

from materials import sectionProperties 

##   Parámetros que definen la sección doble T.
##
##                        b1
##             ---------------------- ef
##                       |
##                       |
##                       |
##                       |ew
##                       |
##                       |
##                 ------------- ef
##                      b2
##   siendo:
##     -b1: Ancho del ala superior.
##     -b2: Ancho del ala inferior.
##     -ef: Espesor de las alas.
##     -ew: Espesor del alma.
##     -ht: Canto del perfil.
   

def getInerciaTorsionSeccDobleT(b1,b2,ef,ew,ht):
    '''
    Devuelve el valor de la inercia a torsión de una sección doble T
    monocelular según el artículo «Perfil doble T» de la Wikipedia.

    :param b1: width of the top flange (parallel to local z-axis)
    :param b2: width of the bottom flange (parallel to local z-axis)
    :param ef: thickness of the flanges (parallel to local y-axis)
    :param ew: thickness of the web  (parallel to local z-axis)
    :param ht: height of the profile (parallel to local y-axis)
    '''
    hPrf=ht-ef
    return ((b1+b2)*ef**3+hPrf*ew**3)/3
  

def getMomentoAlabeoSeccDobleT(b1,b2,ef,ew,ht):
    '''
    :returns: warping moment of a I-section
    reference: article «Perfil doble T» of Wikipedia.

    :param b1: width of the top flange (parallel to local z-axis)
    :param b2: width of the bottom flange (parallel to local z-axis)
    :param ef: thickness of the top flange (parallel to local y-axis)
    :param ew: thickness of the web (parallel to local z-axis)
    :param ht: height of the profile (parallel to local y-axis)
    '''
    hPrf=ht-ef
    return ef*hPrf**2/12*b1**3*b2**3/(b1**3+b2**3)
  

def getMomentoModuloTorsionSeccDobleT(b1,b2,ef,ew,ht):
    ''' :returns: torsional section modulus of an I-section
    reference: article «Perfil doble T» of Wikipedia.

    :param b1: width of the top flange (parallel to local z-axis)
    :param b2: width of the bottom flange (parallel to local z-axis)
    :param ef: thickness of the top flange (parallel to local y-axis)
    :param ew: thickness of the web (parallel to local z-axis)
    :param ht: height of the profile (parallel to local y-axis)
    '''
    J=getInerciaTorsionSeccDobleT(b1,b2,ef,ew,ht)
    return J/max(ef,ew)
  

class ISection(sectionProperties.sectionProperties):
  '''I section geometric parameters

  :ivar name:         name identifying the section
  :ivar wdTopFlange:  width of the top flange (parallel to local z-axis)
  :ivar thTopFlange:  thickness of the top flange (parallel to local y-axis)
  :ivar thWeb:        thickness of the web (parallel to local z-axis)
  :ivar hgWeb:        height of the web (parallel to local y-axis)
  :ivar wdBotFlange:  width of the bottom flange (parallel to local z-axis)
  :ivar thBotFlange:  thickness of the bottom flange (parallel to local y-axis)
 
         wdTopFlange
    --------------------- thTopFlange
              |
              |
              |<-thWeb
              |
              |          hgWeb
              |
              |
              |
              |
         ----------- thBotFlange 
         wdBotFlange

  '''
  def __init__(self,name,wdTopFlange,thTopFlange,thWeb,hgWeb,wdBotFlange,thBotFlange):
    super(ISection,self).__init__(name)
    self.wTF= wdTopFlange
    self.tTF= thTopFlange
    self.tW=thWeb
    self.hW=hgWeb
    self.wBF= wdBotFlange
    self.tBF= thBotFlange
  def hTotal(self):
    ''':returns: total height (parallel to local y axis) of the section
    '''
    retval=self.tTF+self.hW+self.tBF
    return retval
  def A(self):
    ''':returns: cross-sectional area of the section'''
    retval=self.wTF*self.tTF+self.tW*self.hW+self.wBF*self.tBF
    return retval
  def hCOG(self):
    ''':returns: distance from the bottom fiber of the inferior flange to the 
    centre of gravity of the section
    '''
    ATF=self.wTF*self.tTF
    AW=self.tW*self.hW
    ABF=self.wBF*self.tBF
    retval=(ATF*(self.hTotal()-self.tTF/2.0)+AW*(self.tBF+self.hW/2.0)+ABF*self.tBF/2.0)/self.A()
    return retval
  def Iy(self):
    ''':returns: second moment of area about the local y-axis
    '''
    retval=1/12.0*self.tTF*self.wTF**3+1/12.0*self.hW*self.tW**3+1/12.0*self.tBF*self.wBF**3
    return retval
  def Iz(self):
    ''':returns: second moment of area about the local z-axis
    '''
    ATF=self.wTF*self.tTF
    AW=self.tW*self.hW
    ABF=self.wBF*self.tBF
    ITF=1/12.0*self.wTF*self.tTF**3
    IW=1/12.0*self.tW*self.hW**3
    IBF=1/12.0*self.wBF*self.tBF**3
    retval1=ITF+ATF*(self.hTotal()-self.tTF/2.0-self.hCOG())**2
    retval=retval1+IW+AW*(self.tBF+self.hW/2-self.hCOG())**2+IBF+ABF*(self.tBF/2.0-self.hCOG())**2
    return retval
  def J(self):
    ''':returns: torsional moment of inertia of the section'''
    hPrf=self.hTotal()-self.tTF/2.0-self.tBF/2.0
    retval=(self.wTF*self.tTF**3+self.wBF*self.tBF**3+hPrf*self.tW**3)/3.0
    return retval
  def Wyel(self):
    ''':returns: section modulus with respect to local y-axis'''
    zmax=max(self.wTF/2.0,self.wBF/2.0)
    return self.Iy()/zmax
  def Wzel(self):
    ''':returns: section modulus with respect to local z-axis'''
    ymax=max(self.hCOG(),self.hTotal()-self.hCOG())
    return self.Iz()/ymax
  def alphaY(self):
    ''':returns: shear shape factor with respect to local y-axis'''
    return 0.32 #Coeficiente de distorsión, ver libro E. Oñate pág. 122.
  def alphaZ(self):
    ''':returns: shear shape factor with respect to local z-axis'''
    return 0.69

