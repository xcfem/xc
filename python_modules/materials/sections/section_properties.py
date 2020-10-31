# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "

import sys
import math
from materials import typical_materials
from misc_utils import log_messages as lmsg
import scipy.interpolate
import geom
import xc


class SectionProperties(object):
    '''Abstract section properties (area, moments of inertia,...)

     :ivar sectionName:  name identifying the section
     :ivar xc_material:  pointer to XC material.    
    '''
    def __init__(self,name):
        self.sectionName= name
        self.xc_material= None
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        if(self.xc_material):
            lmsg.error('Cannot export xc materials yet.')
        retval= {'sectionName':self.sectionName, 'xc_material':None}
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.sectionName= dct['sectionName']
        self.xc_material= dct['xc_material']
        
    def A(self):
        '''cross-sectional area (abstract method)'''
        raise "Abstract method, please override"
        return 0.0
    
    def Iy(self):
      '''second moment of area about the local y-axis (abstract method)'''
      raise "Abstract method, please override"
      return 0.0
  
    def iy(self):
      '''Return the radius of gyration of the section around
         the axis parallel to Z that passes through section centroid.
      '''
      return math.sqrt(self.Iy()/self.A())
    def Iz(self):
      '''second moment of area about the local z-axis (abstract method)'''
      raise "Abstract method, please override"
      return 0.0
  
    def iz(self):
      '''Return the radius of gyration of the section around
         the axis parallel to Z that passes through section centroid.
      '''
      return math.sqrt(self.Iz()/self.A())
  
    def J(self):
      '''torsional constant of the section (abstract method)'''
      raise "Abstract method, please override"
      return 0.0
  
    def Wyel(self):
      '''section modulus with respect to local y-axis (abstract method)'''
      raise "Abstract method, please override"
      return 0.0
  
    def Wzel(self):
      '''section modulus with respect to local z-axis (abstract method)'''
      raise "Abstract method, please override"
      return 0.0
  
    def SteinerY(self,z):
      '''Return the moment of inertia obtained by applying
         the parallel axis theorem (or Huygens-Steiner theorem
         or Steiner's theorem.

        :param pos: position of the original section centroid
      '''
      return self.Iy()+self.A()*z**2
  
    def SteinerZ(self,y):
      '''Return the moment of inertia obtained by applying
         the parallel axis theorem (or Huygens-Steiner theorem
         or Steiner's theorem.

        :param pos: position of the original section centroid
      '''
      return self.Iz()+self.A()*y**2
  
    def Steiner(self,pos):
      '''Return the moments of inertia obtained by applying
         the parallel axis theorem (or Huygens-Steiner theorem
         or Steiner's theorem.

        :param pos: position of the original section centroid
      '''
      y= pos.x
      z= pos.y
      A= self.A()
      newIy= self.Iy()+A*z**2
      newIz= self.Iz()+A*y**2
      return newIy,newIz
  
    def SteinerJ(self,pos):
      '''Return the moments of inertia obtained by applying
         the parallel axis theorem (or Huygens-Steiner theorem
         or Steiner's theorem.

        :param pos: position of the original section centroid
      '''
      d2= pos.x**2+pos.y**2
      A= self.A()
      return self.J()+A*d2
  
    def getPlasticSectionModulusY(self):
      '''Returns the plastic section modulus around Y axis.

         Computes the plastic section modulus assuming that plastic neutral 
         axis passes through section centroid (which is true whenever the 
         rectangular section is homogeneous).
      '''
      lmsg.error('getPlasticSectionModulusY not implemented.')
      return 0.0
  
    def getPlasticMomentY(self,fy):
      '''Return section plastic moment around Y axis.

         Computes the plastic moment of the section assuming that plastic
         neutral axis passes through section centroid (which is true
         whenever the rectangular section is homogeneous).
      '''
      return 2*self.getPlasticSectionModulusY()*fy
  
    def getPlasticSectionModulusZ(self):
      '''Returns the plastic section modulus around Z axis.

         Computes the plastic section modulus assuming that plastic neutral 
         axis passes through section centroid (which is true whenever the 
         rectangular section is homogeneous).
      '''
      lmsg.error('getPlasticSectionModulusZ not implemented.')
      return 0.0
  
    def getPlasticMomentZ(self,fy):
      '''Return section plastic moment around Z axis.

         Computes the plastic moment of the section assuming that plastic
         neutral axis passes through section centroid (which is true
         whenever the rectangular section is homogeneous).
      '''
      return 2*self.getPlasticSectionModulusZ()*fy
  
    def respTName(self):
        ''' returns a name to identify the torsional response of the section'''
        return self.sectionName+"RespT"
    
    def respVyName(self):
        ''' returns a name to identify the shear Y response of the section'''
        return self.sectionName+"RespVy"

    def respVzName(self):
        ''' returns a name to identify the shear Z response of the section'''
        return self.sectionName+"RespVz"
  
    def getRespT(self,preprocessor, G):
        '''return an elastic material for modeling torsional response of section'''
        return typical_materials.defElasticMaterial(preprocessor,self.respTName(),self.getTorsionalStiffness(G)) # Torsional response of the section.
    
    def getRespVy(self, preprocessor, G):
        ''' return an elastic material for modeling Y shear response of section'''
        return typical_materials.defElasticMaterial(preprocessor, self.respVyName(), self.getShearStiffnessY(G))

    def getRespVz(self, preprocessor, G):
        '''Material for modeling Z shear response of section'''
        return typical_materials.defElasticMaterial(preprocessor, self.respVzName(), self.getShearStiffnessZ(G))

    def defElasticSection3d(self, preprocessor, material):
        ''' Return an elastic section appropiate for 3D beam analysis

        :param  preprocessor: preprocessor object.
        :param material:      material (for which E is the Young's modulus and G() the shear modulus)  
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.sectionName)):
                lmsg.warning("Section: "+self.sectionName+" already defined.")
                self.xc_material= materialHandler.getMaterial(self.sectionName)
            else:
                self.xc_material= typical_materials.defElasticSection3d(preprocessor,self.sectionName,self.A(),material.E,material.G(),self.Iz(),self.Iy(),self.J(), linearRho= material.rho*self.A())
        else:
            lmsg.warning('Material: '+self.sectionName+ ' already defined as:'+str(self.xc_material))
        return self.xc_material
    
    def defElasticShearSection3d(self, preprocessor, material):
        '''elastic section appropiate for 3D beam analysis, including shear 
           deformations

        :param  preprocessor: preprocessor object.
        :param material:      material (for which E is the Young's modulus and G() the shear modulus)  
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.sectionName)):
                lmsg.warning("Section: "+self.sectionName+" already defined.")
                self.xc_material= materialHandler.getMaterial(self.sectionName)
            else:
                self.xc_material= typical_materials.defElasticShearSection3d(preprocessor,self.sectionName,self.A(),material.E,material.G(),self.Iz(),self.Iy(),self.J(),self.alphaY(), linearRho= material.rho*self.A())
        else:
            lmsg.warning('Material: '+self.sectionName+ ' already defined as:'+str(self.xc_material))
        return self.xc_material

    def defElasticSection1d(self, preprocessor, material):
        ''' Return an elastic section appropiate for truss analysis.

        :param preprocessor: preprocessor object.
        :param material:     material constitutive model 
                             (for which E is the Young's modulus)
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.sectionName)):
                lmsg.warning("Section: "+self.sectionName+" already defined.")
                self.xc_material= materialHandler.getMaterial(self.sectionName)
            else:
                self.xc_material= typical_materials.defElasticSection1d(preprocessor,self.sectionName,self.A(),material.E, linearRho= material.rho*self.A())
        else:
            lmsg.warning('Material: '+self.sectionName+ ' already defined as:'+str(self.xc_material))
        return self.xc_material
    
    def defElasticSection2d(self, preprocessor, material, majorAxis= True):
        ''' Return an elastic section appropiate for 2D beam analysis

        :param preprocessor: preprocessor object.
        :param material:     material constitutive model 
                             (for which E is the Young's modulus)
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.sectionName)):
                lmsg.warning("Section: "+self.sectionName+" already defined.")
                self.xc_material= materialHandler.getMaterial(self.sectionName)
            else:
                I= self.Iz();
                if(not majorAxis):
                    I= self.Iy()
                self.xc_material= typical_materials.defElasticSection2d(preprocessor,self.sectionName,self.A(),material.E,I, linearRho= material.rho*self.A())
        else:
            lmsg.warning('Material: '+self.sectionName+ ' already defined as:'+str(self.xc_material))
        return self.xc_material
    
    def defElasticShearSection2d(self, preprocessor, material, majorAxis= True):
        '''elastic section appropiate for 2D beam analysis, including shear deformations

        :param  preprocessor: preprocessor object.
        :param material: material constitutive model (for which 
                         E is the Young's modulus and G() the shear modulus).
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.sectionName)):
                lmsg.warning("Section: "+self.sectionName+" already defined.")
                self.xc_material= materialHandler.getMaterial(self.sectionName)
            else:
                I= self.Iz();
                if(not majorAxis):
                    I= self.Iy()
                self.xc_material= typical_materials.defElasticShearSection2d(preprocessor,self.sectionName,self.A(),material.E,material.G(),I,self.alphaY(), linearRho= material.rho*self.A())
        else:
            lmsg.warning('Material: '+self.sectionName+' already defined as:'+str(self.xc_material))
        return self.xc_material
    
    def getCrossSectionProperties2D(self, material):
      '''Return a CrossSectionProperties object with the
         2D properties of the section.'''
      retval= xc.CrossSectionProperties2d()
      retval.E= material.E
      retval.A= self.A()
      retval.I= self.Iz()
      retval.G= material.G()
      retval.Alpha= self.alphaY()
      return retval

class RectangularSection(SectionProperties):
    '''Rectangular section geometric parameters

    :ivar b:            cross-section width (parallel to local z-axis)
    :ivar h:            cross-section depth (parallel to local y-axis)
    '''
    # Points that define alpha value as a function of h/b
    #   see book "hormigón" Jiménez Montoya 14th edition page 405
    xAlpha= [1,1.25,1.5,2,3,4,6,10,10000]
    yAlpha= [0.14,0.171,0.196,0.229,0.263,0.281,0.299,0.313,1.0/3.0]
    alphaTable= scipy.interpolate.interp1d(xAlpha,yAlpha)

    # Points that define beta value as a function of h/b
    #   see book "hormigón" Jiménez Montoya 14th edition page 405
    xBeta= [1,1.25,1.5,2,3,4,6,8,10,10000]
    yBeta= [0.208, 0.221, 0.231, 0.246, 0.267, 0.282, 0.299, 0.307, 0.313, 1.0/3]
    betaTable= scipy.interpolate.interp1d(xBeta,yBeta)

    def __init__(self,name,b,h):
      super(RectangularSection,self).__init__(name)
      self.b= b
      self.h= h
      
    def A(self):
      '''Return cross-sectional area of the section'''
      return self.b*self.h
  
    def Iy(self):
      '''Return second moment of area about the local y-axis'''
      return 1/12.0*self.h*self.b**3
    def Iz(self):
      '''Return second moment of area about the local z-axis'''
      return 1/12.0*self.b*self.h**3
  
    def J(self):
      '''Return torsional constant of the section'''
      return self.getJTorsion()
  
    def Wyel(self):
      '''Return section modulus with respect to local y-axis'''
      return self.Iy()/(self.b/2.0)
  
    def Wzel(self):
      '''Return section modulus with respect to local z-axis'''
      return self.Iz()/(self.h/2.0)
  
    def alphaY(self):
      '''Return shear shape factor with respect to local y-axis'''
      return 5.0/6.0 #Shear distortion constant. See E. Oñate book page 122.
  
    def alphaZ(self):
      '''Return shear shape factor with respect to local z-axis'''
      return self.alphaY()
  
    def getYieldMomentY(self,fy):
      '''Return section yield moment.

         :param fy: material yield stress.
      '''
      return 2*fy/self.b*self.Iy()
  
    def getElasticSectionModulusY(self):
      '''Returns the plastic section modulus.

         Computes the plastic section modulus assuming that plastic neutral 
         axis passes through section centroid (which is true whenever the 
         rectangular section is homogeneous).
      '''
      return (self.b*self.h)*self.b/6.0
  
    def getPlasticSectionModulusY(self):
      '''Returns the plastic section modulus.

         Computes the plastic section modulus assuming that plastic neutral 
         axis passes through section centroid (which is true whenever the 
         rectangular section is homogeneous).
      '''
      return (self.b*self.h/2.0)*self.b/4.0
  
    def getYieldMomentZ(self,fy):
      '''Return section yield moment.

         :param fy: material yield stress.
      '''
      return 2*fy/self.h*self.Iz()
  
    def getElasticSectionModulusZ(self):
      '''Returns the plastic section modulus.

         Computes the plastic section modulus assuming that plastic neutral 
         axis passes through section centroid (which is true whenever the 
         rectangular section is homogeneous).
      '''
      return (self.b*self.h)*self.h/6.0
  
    def getPlasticSectionModulusZ(self):
      '''Returns the plastic section modulus.

         Computes the plastic section modulus assuming that plastic neutral 
         axis passes through section centroid (which is true whenever the 
         rectangular section is homogeneous).
      '''
      return (self.b*self.h/2.0)*self.h/4.0
  
    def getAlphaTorsion(self):
      '''Return alpha coefficient of the section.

      Reference: concrete book Jiménez Montoya 14a. edition page 405
      '''
      retval= 0.0
      if self.b<self.h:
        retval= self.alphaTable(self.h/self.b)
      else:
        retval= self.alphaTable(self.b/self.h)
      return retval
  
    def getBetaTorsion(self):
      '''Return beta coefficient of the section.

      Reference: concrete book Jiménez Montoya 14a. edition page 405
      '''
      retval= 0.0
      if self.b<self.h:
        retval= self.betaTable(self.h/self.b)
      else:
        retval= self.betaTable(self.b/self.h)
      return retval
  
    def getJTorsion(self):
      '''Return torsional constant of the section.

      Reference: concrete book Jiménez Montoya 14a. edition page 405
      '''
      alphaJT= self.getAlphaTorsion()
      retval= 0.0
      if self.b<self.h:
        retval= alphaJT*pow(self.b,3)*self.h
      else:
        retval= alphaJT*self.b*pow(self.h,3)
      return retval
  
    def getTorsionalStiffness(self, G):
        '''Return the torsional stiffness of the section.'''
        return G*self.getJTorsion()
    
    def getShearStiffnessY(self, G):
        '''Return the shear stiffness of the section.'''
        return 5.0/6.0*G*self.A()
    
    def getShearStiffnessZ(self, G):
        '''Return the shear stiffness of the section.'''
        return 5.0/6.0*G*self.A()
    
    def getRegion(self,gm,nmbMat):
      '''generation of a quadrilateral region from the section 
      geometry (sizes and number of divisions for the cells)
      made of the specified material

      :param gm: object of type section_geometry
      :param nmbMat: name of the material (string)
      '''
      regions= gm.getRegions
      reg= regions.newQuadRegion(nmbMat)
      reg.nDivIJ= self.nDivIJ
      reg.nDivJK= self.nDivJK
      reg.pMin= geom.Pos2d(-self.h/2.0,-self.b/2.0)
      reg.pMax= geom.Pos2d(self.h/2.0,self.b/2.0)
      return reg

class CircularSection(SectionProperties):
    '''Geometric parameters of a circular or circular hollow section

    :ivar Rext:      external radius
    :ivar Rint:      internal radius (defaults to 0)
     '''
    r= 0.0 # radius.
    def __init__(self,name,Rext,Rint=0):
        super(CircularSection,self).__init__(name)
        self.Rext= Rext
        self.Rint=Rint
      
    def A(self):
        '''Return cross-sectional area of the section'''
        return math.pi*(self.Rext*self.Rext-self.Rint*self.Rint)

    def getThickness(self):
        '''Return the section thickness.'''
        return self.Rext-self.Rint
  
    def Iy(self):
        '''Return second moment of area about the local y-axis'''
        return 1.0/4.0*math.pi*(self.Rext**4-self.Rint**4)
  
    def Iz(self):
        '''Return second moment of area about the local z-axis'''
        return self.Iy()
  
    def J(self):
        '''Return torsional constant of the section'''
        return 2*self.Iy()
  
    def alphaY(self):
        '''Return distorsion coefficient with respect to local Y axis
           (see Oñate, Cálculo de estructuras por el MEF page 122)
         '''
        if self.Rint==0:
          alpha=6.0/7.0
        else:
          c=self.Rint/self.Rext
          K=c/(1+c**2)
          alpha=6/(7+20*K**2)
        return alpha
  
    def alphaZ(self):
        '''Return distorsion coefficient with respect to local Z axis.'''
        return self.alphaY()

    def getTorsionalStiffness(self, G):
        '''Return the torsional stiffness of the section.'''
        return G*self.J()
    
    def getShearStiffnessY(self, G):
        '''Return the shear stiffness of the section.'''
        retval= 0.0
        if(self.Rint==0):
            retval= 32.0/37.0*G*self.A()
        else:
            lmsg.error('getShearStiffness for tubes not implemented yet.')
        return retval
    
    def getShearStiffnessZ(self, G):
        '''Return the shear stiffness of the section.'''
        return self.getShearStiffnessY(G)
        

class GenericSection(SectionProperties):
    '''Mechanical properties of generic section 

    :ivar area:         cross-sectional area
    :ivar Iy:           second moment of area about the local y-axis
    :ivar Iz:           second moment of area about the local z-axis
    :ivar Jtors:        torsional constant of the section
    :ivar Wy:           section modulus with respect to local y-axis
    :ivar Wz:           section modulus with respect to local z-axis
    :ivar alphY:        shear shape factor with respect to local y-axis
    :ivar alphZ:        shear shape factor with respect to local z-axis
    '''
    def __init__(self,name,area,I_y,I_z,Jtors,W_y,W_z,alphY,alphZ):
      super(GenericSection,self).__init__(name)
      self.area=area
      self.I_y=I_y
      self.I_z=I_z
      self.Jtors=Jtors
      self.W_y=W_y
      self.W_z=W_z
      self.alphY=alphY
      self.alphZ=alphZ
      
    def A(self):
      '''Return cross-sectional area'''
      return self.area
  
    def Iy(self):
      '''Return second moment of area about the local y-axis'''
      return self.I_y
  
    def Iz(self):
      '''Return second moment of area about the local z-axis'''
      return self.I_z
  
    def J(self):
      '''Return torsional constant of the section'''
      return self.Jtors
  
    def Wyel(self):
      '''Return section modulus with respect to local y-axis'''
      return self.W_y
  
    def Wzel(self):
      '''Return section modulus with respect to local z-axis'''
      return self.W_z
  
    def alphaY(self):
      '''Return shear shape factor with respect to local y-axis'''
      return self.alphY #Shear distortion constant. See E. Oñate book page 122.
  
    def alphaZ(self):
      '''Return shear shape factor with respect to local z-axis'''
      return self.alphZ



class ISection(SectionProperties):
    '''I section geometric parameters

     :ivar wdTopFlange:  width of the top flange (parallel to local z-axis)
     :ivar thTopFlange:  thickness of the top flange (parallel to local y-axis)
     :ivar thWeb:        thickness of the web (parallel to local z-axis)
     :ivar hgWeb:        height of the web (parallel to local y-axis)
     :ivar wdBotFlange:  width of the bottom flange (parallel to local z-axis)
     :ivar thBotFlange:  thickness of the bottom flange (parallel to local y-axis)
   ''' 
      #      wdTopFlange
      # --------------------- thTopFlange
      #           |
      #           |
      #           |<-thWeb
      #           |
      #           |          hgWeb
      #           |
      #           |
      #           |
      #           |
      #      ----------- thBotFlange 
      #      wdBotFlange

    def __init__(self,name,wdTopFlange,thTopFlange,thWeb,hgWeb,wdBotFlange,thBotFlange):
      super(ISection,self).__init__(name)
      self.wTF= wdTopFlange
      self.tTF= thTopFlange
      self.tW=thWeb
      self.hW=hgWeb
      self.wBF= wdBotFlange
      self.tBF= thBotFlange
      
    def hTotal(self):
      '''Return total height (parallel to local y axis) of the section
      '''
      retval=self.tTF+self.hW+self.tBF
      return retval
  
    def A(self):
      '''Return cross-sectional area of the section'''
      retval=self.wTF*self.tTF+self.tW*self.hW+self.wBF*self.tBF
      return retval
  
    def hCOG(self):
      '''Return distance from the bottom fiber of the inferior flange to the 
      centre of gravity of the section
      '''
      ATF=self.wTF*self.tTF
      AW=self.tW*self.hW
      ABF=self.wBF*self.tBF
      retval=(ATF*(self.hTotal()-self.tTF/2.0)+AW*(self.tBF+self.hW/2.0)+ABF*self.tBF/2.0)/self.A()
      return retval
  
    def Iy(self):
      '''Return second moment of area about the local y-axis
      '''
      retval=1/12.0*self.tTF*self.wTF**3+1/12.0*self.hW*self.tW**3+1/12.0*self.tBF*self.wBF**3
      return retval
  
    def Iz(self):
      '''Return second moment of area about the local z-axis
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
      '''Return torsional constant of the section'''
      hPrf=self.hTotal()-self.tTF/2.0-self.tBF/2.0
      retval=(self.wTF*self.tTF**3+self.wBF*self.tBF**3+hPrf*self.tW**3)/3.0
      return retval
  
    def Wyel(self):
      '''Return section modulus with respect to local y-axis'''
      zmax=max(self.wTF/2.0,self.wBF/2.0)
      return self.Iy()/zmax
  
    def Wzel(self):
      '''Return section modulus with respect to local z-axis'''
      ymax=max(self.hCOG(),self.hTotal()-self.hCOG())
      return self.Iz()/ymax
  
    def Wxel(self):
      ''' Return torsional section modulus of the section.

      reference: article «I Beam» of Wikipedia.
      '''
      return self.J()/max(max(self.tTF,self.tBF),self.tW)
  
    def alphaY(self):
      '''Return shear shape factor with respect to local y-axis'''
      return 0.32 #Shear distortion constant. See E. Oñate book page 122.
  
    def alphaZ(self):
      '''Return shear shape factor with respect to local z-axis'''
      return 0.69
  
    def getWarpingMoment(self):
      '''Return warping moment of a I-section

      reference: article «I Beam» of Wikipedia.
      '''
      hPrf=self.hTotal()-self.tTF/2.0-self.tBF/2.0
      return (self.tTF+self.tBF)/2.0*hPrf**2/12*self.wTF**3*self.wBF**3/(self.wTF**3+self.wBF**3)

class PolygonalSection(SectionProperties):
    '''Polygonal section geometric parameters

    :ivar name:  name of the section.
    :ivar plg:  contour of the section.

    '''
    def __init__(self,name,plg):
        '''Constructor.'''
        super(PolygonalSection,self).__init__(name)
        self.plg= plg
        self.reCenter()
        
    def reCenter(self):
        '''Put the centroid of the section in the
           origin.'''
        v= geom.Pos2d(0.0,0.0)-self.plg.getCenterOfMass()
        self.plg.move(v)
        
    def hTotal(self):
        '''Return total height (parallel to local y axis) of the section
        '''
        return self.plg.getBnd().height
    
    def A(self):
        '''Return cross-sectional area of the section'''
        return self.plg.getArea()
    
    def Iy(self):
        '''Return second moment of area about the local y-axis
        '''
        return self.plg.getIy()
    
    def Iz(self):
        '''Return second moment of area about the local z-axis
        '''
        return self.plg.getIx()
    
    def J(self):
        '''Return an approximation of the torsional constant of the section

           Return the torsional constant of a circle with the same area.
        '''
        R2= self.A()/math.pi
        return 0.5*math.pi*R2**2
    
    def alphaY(self):
        '''Return shear shape factor with respect to local y-axis'''
        msg= 'alphaY: shear shape factor not implemented for section: '
        msg+= self.sectionName
        msg+= '. 5/6 returned'
        lmsg.warning(msg)
        return 5.0/6.0
    
    def alphaZ(self):
        '''Return shear shape factor with respect to local z-axis'''
        msg= 'alphaZ: shear shape factor not implemented for section: '
        msg+= self.sectionName
        msg+= '. 5/6 returned'
        lmsg.warning(msg)
        return 5.0/6.0

##   Return the torsion constant of a box 
##   according to the book "Puentes (apuntes para su diseño
##   y construcción)" by Javier Manterola Armisén (section 5.2.3 page 251)
##
##                        bs
##             |----------------------|
##
##                        ts
##    -    ---------------------------------
##    |         \                    /
##    |h         \td                /
##    |           \       ti       /
##    -            ----------------
##
##                        bi
##                 |--------------|
##

def getInerciaTorsionCajonMonocelular(bs,bi,h,ts,ti,td):
    '''
    Return torsional section modulus of the section.

    :param bs: Upper deck width (without the overhangs)
    :param bi: Lower deck width.
    :param ts: Upper deck thickness.
    :param ti: Lower deck thickness.
    :param td: Thickness of the webs.
    :param h: Box depth (between mid-planes).
    '''
    longAlma=math.sqrt(h**2+((bs-bi)/2)**2)
    return (bs+bi)**2*h**2/(bs/ts+2*longAlma/td+bi/ti)

def solicitationType(epsCMin, epsSMax):
  ''' Solicitation type from maximum and minimum strain.

  Return:
    1: Pure or combined tension  (all fibers are tensioned).
    2: Pure or combined bending (tensioned and compressed fibers).
    3: Pure or combined compression (all fibers are compressed).

    :param epsCMin: Minimal strain.
    :param epsCMax: Maximal strain.
  '''
  if(epsCMin>0.0): # All material in tension.
    return 1 
  else: # Some material in compression
    if(epsSMax>0): # Some material in tension.
      return 2  # Pure or combined bending
    else: # All material in compression.
      return 3  # Pure or combined compression.

def solicitationTypeString(tipoSol):
  ''' Returns a string describing the solicitation type.

    :param solType: number identifiying the solicitation type:
      1: Pure or combined tension  (all fibers are tensioned).
      2: Pure or combined bending (tensioned and compressed fibers).
      3: Pure or combined compression (all fibers are compressed).

  '''
  if(tipoSol==1):
    return "simple of combined tension"
  elif(tipoSol==2): 
    return "simple or combined bending" 
  elif(tipoSol==3):
    return "simple or combined compression"
  else: 
    return "error"

class CompoundSection(SectionProperties):
    '''Compound section properties (area, moments of inertia,...)

    :ivar name:         name identifying the section
    '''
    def __init__(self,name, section_list):
      super(CompoundSection,self).__init__(name)
      self.sectionList= section_list
      
    def A(self):
      '''cross-sectional area'''
      retval= 0.0
      for s in self.sectionList:
        retval+= s[1].A()
      return retval
  
    def yCenterOfMass(self):
      '''y coordinate of the center of mass.'''
      retval= 0.0
      totalArea= 0.0
      for s in self.sectionList:
        area= s[1].A()
        totalArea+=area 
        retval+= s[0].x*area
      retval/= totalArea
      return retval
  
    def zCenterOfMass(self):
      '''y coordinate of the center of mass.'''
      retval= 0.0
      totalArea= 0.0
      for s in self.sectionList:
        area= s[1].A()
        totalArea+=area 
        retval+= s[0].y*area
      retval/= totalArea
      return retval
  
    def Iy(self):
      '''second moment of area about the local y-axis.'''
      zCenter= self.zCenterOfMass()
      retval= 0.0
      for s in self.sectionList:
        z= s[0].y
        retval+= s[1].SteinerY(z-zCenter)
      return retval
  
    def Iz(self):
      '''second moment of area about the local z-axis (abstract method)'''
      yCenter= self.yCenterOfMass()
      retval= 0.0
      for s in self.sectionList:
        y= s[0].x
        retval+= s[1].SteinerZ(y-yCenter)
      return retval
  
    def J(self):
      '''torsional constant of the section.'''
      center= geom.Pos2d(self.yCenterOfMass(), self.zCenterOfMass())
      retval= 0.0
      for s in self.sectionList:
        retval+= s[1].SteinerJ(s[0].dist(center))
      return retval
  
    def alphaY(self):
      '''return shear shape factor with respect to local y-axis'''
      retval= 0.0
      totalArea= 0.0
      for s in self.sectionList:
        area= s[1].A()
        totalArea+=area 
        retval+= s[1].alphaY()*area
      retval/= totalArea
      return retval
  
    def alphaZ(self):
      '''return shear shape factor with respect to local z-axis'''
      retval= 0.0
      totalArea= 0.0
      for s in self.sectionList:
        area= s[1].A()
        totalArea+=area 
        retval+= s[1].alphaZ()*area
      retval/= totalArea
      return retval

