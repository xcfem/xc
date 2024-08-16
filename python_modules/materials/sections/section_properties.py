# -*- coding: utf-8 -*-
''' Calculation of cross-section mechanical properties (area, inertia,...).'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "

import sys
import math
import uuid
from materials import typical_materials
from misc_utils import log_messages as lmsg
import numpy as np
import matplotlib.pyplot as plt
import scipy.interpolate
import geom
import xc


class SectionProperties(object):
    '''Abstract section properties (area, moments of inertia,...)

     :ivar name:  name identifying the section
     :ivar xc_material:  pointer to XC material.
     :ivar torsionalStiffnessFactor: factor to apply to the sectional
                                     stiffness (defaults to 1.0).
    '''
    def __init__(self,name):
        if(not name):
            name= str(uuid.uuid1())        
        self.name= name
        self.xc_material= None
        self.torsionalStiffnessFactor= 1.0
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (self.name == other.name)
                if(retval):
                    retval= (self.xc_material == other.xc_material)
                if(retval):
                    retval= (self.torsionalStiffnessFactor== other.torsionalStiffnessFactor)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        xcMat= None
        if(self.xc_material):
            xcMat= self.xc_material
        retval= {'name':self.name, 'xc_material':xcMat, 'torsional_stiffness_factor': self.torsionalStiffnessFactor}
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        self.name= dct['name']
        self.xc_material= dct['xc_material']
        self.torsionalStiffnessFactor= dct['torsional_stiffness_factor']
        
    def A(self):
        '''cross-sectional area (abstract method)'''
        raise Exception('Abstract method, please override')
    
    def yCenterOfMass(self):
        '''y coordinate of the center of mass.'''
        return 0.0
  
    def zCenterOfMass(self):
        '''z coordinate of the center of mass.'''
        return 0.0

    def yMax(self):
        ''' Return the maximum distance from the section contour
            to the local Z axis.'''
        raise Exception('Abstract method, please override')
        
    def zMax(self):
        ''' Return the maximum distance from the section contour
            to the local Y axis.'''
        raise Exception('Abstract method, please override')
    
    def hCOG(self):
        '''Return distance from the bottom fiber to the 
        center of gravity of the section.
        '''
        raise Exception('Abstract method, please override')
  
    def bCOG(self):
        '''Return distance from the leftmost fiber to the 
        center of gravity of the section.
        '''
        raise Exception('Abstract method, please override')
    
    def Iy(self):
        '''second moment of area about the local y-axis (abstract method)'''
        raise Exception('Abstract method, please override')
  
    def iy(self):
        '''Return the radius of gyration of the section around
           the axis parallel to Z that passes through section centroid.
        '''
        return math.sqrt(self.Iy()/self.A())
  
    def Iz(self):
        '''second moment of area about the local z-axis (abstract method)'''
        raise Exception('Abstract method, please override')
  
    def iz(self):
        '''Return the radius of gyration of the section around
           the axis parallel to Z that passes through section centroid.
        '''
        return math.sqrt(self.Iz()/self.A())
      
    def I(self, majorAxis):
        ''' Return the second moment of area about the local major or minor
            axis.

        :param majorAxis: true if the required inertia corresponds to the
                          bending around major axis. 
        '''
        Imax= self.Iz()
        Imin= self.Iy()
        if(Imin>Imax): # swap values
            Imax, Imin= Imin, Imax            
        if(majorAxis):
            return Imax
        else:
            return Imin
  
    def J(self):
        '''torsional constant of the section (abstract method)'''
        raise Exception('Abstract method, please override')
  
    def getWarpingConstant(self):
        '''torsional constant of the section (abstract method)'''
        raise Exception('Abstract method, please override')
  
    def Wyel(self):
        '''Return section modulus with respect to local y-axis'''
        zmax= self.zMax()
        return self.Iy()/zmax
  
    def Wzel(self):
        '''Return section modulus with respect to local z-axis'''
        ymax= self.yMax()
        return self.Iz()/ymax
  
    def SteinerY(self,z):
        '''Return the moment of inertia obtained by applying
           the parallel axis theorem (or Huygens-Steiner theorem
           or Steiner's theorem).

          :param pos: position of the original section centroid
        '''
        return self.Iy()+self.A()*z**2
  
    def SteinerZ(self,y):
        '''Return the moment of inertia obtained by applying
           the parallel axis theorem (or Huygens-Steiner theorem
           or Steiner's theorem).

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
  
    def SteinerJ(self, dist):
        '''Return the moments of inertia obtained by applying
           the parallel axis theorem (or Huygens-Steiner theorem
           or Steiner's theorem.

          :param dist: distance to the original section centroid
        '''
        d2= dist**2
        A= self.A()
        return self.J()+A*d2
  
    def getPlasticSectionModulusY(self):
        '''Returns the plastic section modulus around Y axis.

           Computes the plastic section modulus assuming that plastic neutral 
           axis passes through section centroid (which is true whenever the 
           rectangular section is homogeneous).
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
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
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
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
        return self.name+"RespT"
    
    def respVyName(self):
        ''' returns a name to identify the shear Y response of the section'''
        return self.name+"RespVy"

    def respVzName(self):
        ''' returns a name to identify the shear Z response of the section'''
        return self.name+"RespVz"
  
    def getRespT(self, preprocessor, G):
        ''' Return an elastic material for modeling torsional response of 
           section.

        :param preprocessor: preprocessor of the finite element problem.
        :param G: shear modulus.
        '''
        return typical_materials.defElasticMaterial(preprocessor, self.respTName(), self.getTorsionalStiffness(G)) # Torsional response of the section.
    
    def getRespVy(self, preprocessor, G):
        ''' Return an elastic material for modeling the resoponse of the
            section along the Y axis.

        :param preprocessor: preprocessor of the finite element problem.
        :param G: shear modulus.
        '''
        return typical_materials.defElasticMaterial(preprocessor, self.respVyName(), self.getShearStiffnessY(G))

    def getRespVz(self, preprocessor, G):
        ''' Return an elastic material for modeling the resoponse of the
            section along the Z axis.

        :param preprocessor: preprocessor of the finite element problem.
        :param G: shear modulus.
        '''
        return typical_materials.defElasticMaterial(preprocessor, self.respVzName(), self.getShearStiffnessZ(G))

    def defElasticSection3d(self, preprocessor, material, overrideRho= None, reductionFactor= 1.0):
        ''' Return an elastic section appropriate for 3D beam analysis

        :param preprocessor: preprocessor of the finite element problem.
        :param material: material (for which E is the Young's modulus 
                         and G() the shear modulus).
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the elastic
                                modulus to simulate the effect of cracking,
                                creep, etc.
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.name)):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; section: '+self.name+' already defined.')
                self.xc_material= materialHandler.getMaterial(self.name)
            else:
                csp= self.getCrossSectionProperties3D(material)
                rho= material.rho*csp.A
                if(overrideRho!=None):
                    rho= overrideRho
                E= csp.E
                if(reductionFactor!= 1.0):
                    E/= reductionFactor
                self.xc_material= typical_materials.defElasticSection3d(preprocessor,self.name, A= csp.A,E= E, G= csp.G, Iz= csp.Iz, Iy= csp.Iy, J= csp.J, Iw= csp.Iw, linearRho= rho)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; material: '+self.name+ ' already defined as:'+str(self.xc_material))
        return self.xc_material
    
    def defElasticShearSection3d(self, preprocessor, material, overrideRho= None, reductionFactor= 1.0):
        '''elastic section appropriate for 3D beam analysis, including shear 
           deformations

        :param preprocessor: preprocessor object.
        :param material: material (for which E is the Young's modulus 
                         and G() the shear modulus)  
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the elastic
                                modulus to simulate the effect of cracking,
                                creep, etc.
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.name)):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; section: '+self.name+' already defined.')
                self.xc_material= materialHandler.getMaterial(self.name)
            else:
                csp= self.getCrossSectionProperties3D(material)
                rho= material.rho*csp.A
                if(overrideRho!=None):
                    rho= overrideRho
                E= csp.E
                if(reductionFactor!= 1.0):
                    E/= reductionFactor
                self.xc_material= typical_materials.defElasticShearSection3d(preprocessor, name= self.name, A= csp.A, E= E, G= csp.G, Iz= csp.Iz, Iy= csp.Iy,J= csp.J, Iw= csp.Iw, alpha_y= csp.AlphaY, alpha_z= csp.AlphaZ, linearRho= rho)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; material: '+self.name+ ' already defined as:'+str(self.xc_material))
        return self.xc_material

    def defElasticSection1d(self, preprocessor, material, overrideRho= None, reductionFactor= 1.0):
        ''' Return an elastic section appropriate for truss analysis.

        :param preprocessor: preprocessor object.
        :param material:     material constitutive model 
                             (for which E is the Young's modulus)
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the elastic
                                modulus to simulate the effect of cracking,
                                creep, etc.
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.name)):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; section: '+self.name+' already defined.')
                self.xc_material= materialHandler.getMaterial(self.name)
            else:
                csp= self.getCrossSectionProperties2D(material)
                rho= material.rho*csp.A
                if(overrideRho!=None):
                    rho= overrideRho
                E= csp.E
                if(reductionFactor!= 1.0):
                    E/= reductionFactor
                self.xc_material= typical_materials.defElasticSection1d(preprocessor,self.name,A= csp.A, Iw= csp.Iw, E= E, linearRho= rho)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; material: '+self.name+ ' already defined as:'+str(self.xc_material))
        return self.xc_material
    
    def defElasticSection2d(self, preprocessor, material, majorAxis= True, overrideRho= None, reductionFactor= 1.0):
        ''' Return an elastic section appropriate for 2D beam analysis

        :param preprocessor: preprocessor object.
        :param material:     material constitutive model 
                             (for which E is the Young's modulus)
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the elastic
                                modulus to simulate the effect of cracking,
                                creep, etc.
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.name)):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; section: '+self.name+' already defined.')
                self.xc_material= materialHandler.getMaterial(self.name)
            else:
                csp= self.getCrossSectionProperties3D(material)
                I= csp.Iz;
                if(not majorAxis):
                    I= csp.Iy
                rho= material.rho*csp.A
                if(overrideRho!=None):
                    rho= overrideRho
                E= csp.E
                if(reductionFactor!= 1.0):
                    E/= reductionFactor
                self.xc_material= typical_materials.defElasticSection2d(preprocessor,self.name,csp.A, Iw= csp.Iw,E= E, I= I, linearRho= rho)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; material: '+self.name+ ' already defined as:'+str(self.xc_material))
        return self.xc_material
    
    def defElasticShearSection2d(self, preprocessor, material, majorAxis= True, overrideRho= None, reductionFactor= 1.0):
        '''elastic section appropriate for 2D beam analysis, including shear deformations

        :param  preprocessor: preprocessor object.
        :param material: material constitutive model (for which 
                         E is the Young's modulus and G() the shear modulus).
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the elastic
                                modulus to simulate the effect of cracking,
                                creep, etc.
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.name)):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; section: '+self.name+' already defined.')
                self.xc_material= materialHandler.getMaterial(self.name)
            else:
                csp= self.getCrossSectionProperties3D(material)
                I= csp.Iz;
                if(not majorAxis):
                    I= csp.Iy
                rho= material.rho*csp.A
                if(overrideRho!=None):
                    rho= overrideRho
                E= csp.E
                if(reductionFactor!= 1.0):
                    E/= reductionFactor
                self.xc_material= typical_materials.defElasticShearSection2d(preprocessor,self.name,csp.A, Iw= csp.Iw, E= E, G= csp.G, I= I, alpha= csp.Alpha, linearRho= rho)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; material: '+self.name+ ' already defined as:'+str(self.xc_material))
        return self.xc_material
        
    def getCrossSectionProperties2D(self, material):
        '''Return a CrossSectionProperties object with the 2D properties of 
           the section.'''
        retval= xc.CrossSectionProperties2d()
        retval.E= material.E
        retval.A= self.A()
        retval.Iw= self.getWarpingConstant()
        retval.I= self.Iz()
        retval.G= material.G()
        retval.Alpha= self.alphaY()
        return retval
    
    def getCrossSectionProperties3D(self, material):
        '''Return a CrossSectionProperties object with the 2D properties of 
           the section.'''
        retval= xc.CrossSectionProperties3d()
        retval.E= material.E
        retval.A= self.A()
        retval.Iw= self.getWarpingConstant()
        retval.Iz= self.Iz()
        retval.Iy= self.Iy()
        retval.G= material.G()
        retval.J= self.J()
        retval.AlphaY= self.alphaY()
        retval.AlphaZ= self.alphaZ()
        return retval
    
    def getXYVertices(self, offset:geom.Vector2d= None):
        ''' Return the contour X,Y coordinates in two separate
            lists to be used with pyplot.

        :param offset: displacement vector to sum to the positions.
        ''' 
        x= list()
        y= list()
        vertices= self.getContourPoints()
        for p in vertices:
            if(offset!=None):
                p+= offset
            x.append(p.x)
            y.append(p.y)
        if(offset!=None):
            x.append(vertices[0].x+offset.x)
            y.append(vertices[0].y+offset.y)
        else:
            x.append(vertices[0].x)
            y.append(vertices[0].y)
        return x,y
    
    def draw(self, notes= None):
        ''' Draw the section contour using pyplot.

        :param notes: notes to insert in the plot.
        '''
        fig = plt.figure()
        ax = fig.add_subplot(111, aspect= 'equal') # subplot axes
        x,y= self.getXYVertices()
        ax.fill(x,y,'tab:gray')
        ax.plot(x,y,'k')
        if(notes):
            # build a rectangle in axes coords
            left, width = .25, .5
            bottom, height = .25, .5
            top = bottom + height
            for text in notes:
                ax.text(left+width/2, top, text,
                horizontalalignment='left',
                verticalalignment='top',
                transform= ax.transAxes)
                top-= 0.05
        plt.show()
        
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        os.write(indentation+'name: '+str(self.name)+'\n')
        os.write(indentation+'xc material:: '+str(self.xc_material)+'\n')

class RectangularSection(SectionProperties):
    '''Rectangular section geometric parameters

    :ivar b: cross-section width (parallel to local z-axis)
    :ivar h: cross-section depth (parallel to local y-axis)
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

    def __init__(self, name:str, b:float, h:float):
        ''' Constructor.

        :param name: section name.
        :param b: cross-section width (parallel to local z-axis)
        :param h: cross-section depth (parallel to local y-axis)
        ''' 
        super(RectangularSection,self).__init__(name)
        self.b= b
        self.h= h
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super().getDict()
        retval['b']= self.b
        retval['h']= self.h
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        super().setFromDict(dct)
        self.b= dct['b']
        self.h= dct['h']
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(RectangularSection,self).__eq__(other)
                if(retval):
                    retval= (self.b == other.b)
                if(retval):
                    retval= (self.h== other.h)
            else:
                retval= True
        else:
            retval= False
        return retval
      
    def A(self):
        '''Return cross-sectional area of the section'''
        return self.b*self.h

    def Ay(self):
        '''Return the corrected area for shear along y axis.'''
        return 5.0/6.0*self.A()

    def Az(self):
        '''Return the corrected area for shear along z axis.'''
        return 5.0/6.0*self.A()

    def yMax(self):
        ''' Return the maximum distance from the section contour
            to the local Z axis.'''
        return self.h/2.0
        
    def zMax(self):
        ''' Return the maximum distance from the section contour
            to the local Y axis.'''
        return self.b/2.0
  
    def hCOG(self):
        '''Return distance from the bottom fiber to the 
        center of gravity of the section.
        '''
        return self.h/2.0
  
    def bCOG(self):
        '''Return distance from the leftmost fiber to the 
        center of gravity of the section.
        '''
        return self.b/2.0
  
    def Iy(self):
        '''Return second moment of area about the local y-axis'''
        return 1/12.0*self.h*self.b**3
    
    def Iz(self):
        '''Return second moment of area about the local z-axis'''
        return 1/12.0*self.b*self.h**3

    def Iyz(self):
        '''Return the product of inertia about the local axes.'''
        return 0.0
  
    def J(self):
        '''Return torsional constant of the section'''
        return self.getJTorsion()
  
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
        '''Returns the elastic section modulus with respect to the y axis.
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
        '''Returns the elasticc section modulus with respect to the z axis.'''
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
        if self.b<self.h:
            retval= self.alphaTable(self.h/self.b)
        else:
            retval= self.alphaTable(self.b/self.h)
        return retval
  
    def getBetaTorsion(self):
        '''Return beta coefficient of the section.

        Reference: concrete book Jiménez Montoya 14a. edition page 405
        '''
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
        if self.b<self.h:
            retval= alphaJT*pow(self.b,3)*self.h
        else:
            retval= alphaJT*self.b*pow(self.h,3)
        retval*= self.torsionalStiffnessFactor
        return retval
  
    def getTorsionalStiffness(self, G):
        '''Return the torsional stiffness of the section.'''
        return G*self.J()

    def getWarpingConstant(self):
        ''' Return the value of the section warping constant according to
        expression (22) in the article: Lateral torsional stability of timber 
        beams. Ivan Baláž, Yvona Koleková Proceedings of the 6th International
        Conference on Mechanics and Materials in Design, 2015.
        '''
        if(self.b<self.h):
            b= self.b
            h= self.h
        else:
            h= self.b
            b= self.h
        b_h= b/h
        b_h2= b_h**2
        b_h3= b_h2*b_h
        b_h5= b_h3*b_h2
        return (1-4.884*b_h2+4.97*b_h3-1.067*b_h5)*b**3*h**3/144.0
    
    def getShearStiffnessY(self, G):
        '''Return the shear stiffness of the section.'''
        return 5.0/6.0*G*self.A()
    
    def getShearStiffnessZ(self, G):
        '''Return the shear stiffness of the section.'''
        return 5.0/6.0*G*self.A()

    def getRegion(self, gm, nmbMat, twoDimensionalMember= False):
        '''generation of a quadrilateral region from the section 
        geometry (sizes and number of divisions for the cells)
        made of the specified material

        :param gm: object of type section_geometry
        :param nmbMat: name of the material (string)
        :param twoDimensionalMember: true if the region corresponds to a 
                                     two-dimensional member.
        '''
        regions= gm.getRegions
        reg= regions.newQuadRegion(nmbMat) # create region.
        B= self.b
        H= self.h
        if(twoDimensionalMember):
            B, H= H, B # swap dimensions.
        reg.nDivIJ= self.nDivIJ # number of divisions I->J direction.
        reg.nDivJK= self.nDivJK # number of divisions J->K direction.
        reg.pMin= geom.Pos2d(-H/2.0,-B/2.0) # lower left corner.
        reg.pMax= geom.Pos2d(H/2.0,B/2.0) # upper right corner.
        return reg
    
    def getContourPoints(self):
        ''' Return the vertices of the rectangle.'''
        retval= list()
        retval.append(geom.Pos2d(-self.b/2.0, -self.h/2.0))
        retval.append(geom.Pos2d(self.b/2.0, -self.h/2.0))
        retval.append(geom.Pos2d(self.b/2.0, self.h/2.0))
        retval.append(geom.Pos2d(-self.b/2.0, self.h/2.0))
        return retval
    
    def defElasticMembranePlateSection(self, preprocessor, material, overrideRho= None, reductionFactor= 1.0):
        '''Elastic membrane plate section appropriate for shell analysis.

        :param preprocessor: preprocessor object.
        :param material: material constitutive model (for which 
                         E is the Young's modulus nu the Poisson's ratio).
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the elastic
                                modulus to simulate the effect of cracking,
                                creep, etc.
        :param reductionFactor: factor that divides the elastic
                                modulus to simulate the effect of cracking,
                                creep, etc.
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.name)):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; section: '+self.name+' already defined.')
                self.xc_material= materialHandler.getMaterial(self.name)
            else:
                rho= material.rho*self.h
                if(overrideRho!=None):
                    rho= overrideRho
                E= material.E
                if(reductionFactor!= 1.0):
                    E/= reductionFactor
                self.xc_material= typical_materials.defElasticMembranePlateSection(preprocessor,name= self.name, E= E, nu= material.nu, rho= rho, h= self.h)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; material: '+self.name+ ' already defined as:'+str(self.xc_material))
        return self.xc_material
        
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        super(RectangularSection,self).report(os, indentation)
        os.write(indentation+'width: '+str(self.b)+'\n')
        os.write(indentation+'depth: '+str(self.h)+'\n')
        
    
class CircularSection(SectionProperties):
    '''Geometric parameters of a circular or circular hollow section

    :ivar Rext:      external radius
    :ivar Rint:      internal radius (defaults to 0)
     '''
    r= 0.0 # radius.
    def __init__(self,name,Rext,Rint=0):
        super(CircularSection,self).__init__(name)
        self.Rext= Rext
        self.Rint= Rint
      
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super().getDict()
        retval['r_ext']= self.Rext
        retval['r_int']= self.Rint
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        super().setFromDict(dct)
        self.Rext= dct['r_ext']
        self.Rint= dct['r_int']
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(CircularSection,self).__eq__(other)
                if(retval):
                    retval= (self.Rext == other.Rext)
                if(retval):
                    retval= (self.Rint== other.Rint)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def A(self):
        '''Return cross-sectional area of the section'''
        return math.pi*(self.Rext*self.Rext-self.Rint*self.Rint)

    def getThickness(self):
        '''Return the section thickness.'''
        return self.Rext-self.Rint

    def getExtRadius(self):
        ''' Return the external radius.'''
        return self.Rext
    
    def getIntRadius(self):
        ''' Return the internal radius.'''
        return self.Rint
    
    def getAverageRadius(self):
        ''' Return the average radius.'''
        return (self.Rext+self.Rint)/2.0

    def getAverageDiameter(self):
        ''' Return the average radius.'''
        return self.getAverageRadius*2.0
    
    def getDiameter(self):
        ''' Return the external diameter.'''
        return 2.0*self.Rext
    
    def getExternalDiameter(self):
        ''' Return the external diameter.'''
        return 2.0*self.Rext
    
    def getInternalDiameter(self):
        ''' Return the internal diameter.'''
        return 2.0*self.Rint
    
    def yMax(self):
        ''' Return the maximum distance from the section contour
            to the local Z axis.'''
        return self.Rext
        
    def zMax(self):
        ''' Return the maximum distance from the section contour
            to the local Y axis.'''
        return self.Rext
    
    def hCOG(self):
        '''Return distance from the bottom fiber to the 
        center of gravity of the section.
        '''
        return self.Rext
  
    def bCOG(self):
        '''Return distance from the leftmost fiber to the 
        center of gravity of the section.
        '''
        return self.Rext
    
    def Iy(self):
        '''Return second moment of area about the local y-axis'''
        return 1.0/4.0*math.pi*(self.Rext**4-self.Rint**4)
  
    def Iz(self):
        '''Return second moment of area about the local z-axis'''
        return self.Iy()
  
    def J(self):
        '''Return torsional constant of the section'''
        return self.getJTorsion()
  
    def getJTorsion(self):
        '''Return torsional constant of the section.

        Reference: https://en.wikipedia.org/wiki/Torsion_constant#Circle
        '''
        retval= 2.0*self.Iy()*self.torsionalStiffnessFactor
        return retval
    
    def getWarpingConstant(self):
        ''' Return the value of the section warping constant.'''
        return 0.0
    
    def alphaY(self):
        '''Return distortion coefficient with respect to local Y axis
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
        '''Return distortion coefficient with respect to local Z axis.'''
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
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for tubes yet.')
        return retval
    
    def getShearStiffnessZ(self, G):
        '''Return the shear stiffness of the section.'''
        return self.getShearStiffnessY(G)
        
    def getContourPoints(self, nDiv= 100):
        ''' Return the vertices approximating the contour of the circle.'''
        theta = np.linspace(0, 2*np.pi, nDiv)
        retval= list()
        r = np.sqrt(self.Rext)
        if(self.Rint!=0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for tubes yet.')
        x1= r*np.cos(theta)
        x2= r*np.sin(theta)
        for x,y in zip(x1,x2):
            retval.append(geom.Pos2d(x, y))
        return retval

class GenericSection(SectionProperties):
    '''Mechanical properties of generic section 

    :ivar area: cross-sectional area
    :ivar Iy: second moment of area about the local y-axis
    :ivar Iz: second moment of area about the local z-axis
    :ivar Jtors: torsional constant of the section
    :ivar Wy: section modulus with respect to local y-axis
    :ivar Wz: section modulus with respect to local z-axis
    :ivar alphY: shear shape factor with respect to local y-axis
    :ivar alphZ: shear shape factor with respect to local z-axis
    '''
    def __init__(self,name,area,I_y,I_z,Jtors,W_y,W_z,alphY,alphZ, Iw= 0.0):
        ''' Constructor.
        :param area: cross-sectional area
        :param Iy: second moment of area about the local y-axis
        :param Iz: second moment of area about the local z-axis
        :param Jtors: torsional constant of the section
        :param Wy: section modulus with respect to local y-axis
        :param Wz: section modulus with respect to local z-axis
        :param alphY: shear shape factor with respect to local y-axis
        :param alphZ: shear shape factor with respect to local z-axis
        :param Iw: warping constant.
        '''

        super(GenericSection,self).__init__(name)
        self.area=area
        self.I_y=I_y
        self.I_z=I_z
        self.Jtors=Jtors
        self.W_y=W_y
        self.W_z=W_z
        self.alphY=alphY
        self.alphZ=alphZ
        self.Iw= Iw
      
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(GenericSection,self).__eq__(other)
                if(retval):
                    retval= (self.area == other.area)
                if(retval):
                    retval= (self.I_y == other.I_y)
                if(retval):
                    retval= (self.I_z == other.I_z)
                if(retval):
                    retval= (self.Jtors == other.Jtors)
                if(retval):
                    retval= (self.W_y == other.W_y)
                if(retval):
                    retval= (self.W_z == other.W_z)
                if(retval):
                    retval= (self.alphY == other.alphY)
                if(retval):
                    retval= (self.alphZ == other.alphZ)
                if(retval):
                    retval= (self.Iw == other.Iw)
            else:
                retval= True
        else:
            retval= False
        return retval
    
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
        return self.Jtors*self.torsionalStiffnessFactor

    def getWarpingConstant(self):
        ''' Return the value of the section warping constant.'''
        return self.Iw
    
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

     :ivar wdTF: width of the top flange (parallel to local z-axis)
     :ivar tTF: thickness of the top flange (parallel to local y-axis)
     :ivar tW: thickness of the web (parallel to local z-axis)
     :ivar hW: height of the web (parallel to local y-axis)
     :ivar wBF: width of the bottom flange (parallel to local z-axis)
     :ivar tBF: thickness of the bottom flange (parallel to local y-axis)
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
        ''' Constructor.

         :param name:  section name.
         :param wdTopFlange:  width of the top flange (parallel to local z-axis)
         :param thTopFlange:  thickness of the top flange (parallel to local y-axis)
         :param thWeb:        thickness of the web (parallel to local z-axis)
         :param hgWeb:        height of the web (parallel to local y-axis)
         :param wdBotFlange:  width of the bottom flange (parallel to local z-axis)
         :param thBotFlange:  thickness of the bottom flange (parallel to local y-axis)
       ''' 

        super(ISection,self).__init__(name)
        self.wTF= wdTopFlange
        self.tTF= thTopFlange
        self.tW=thWeb
        self.hW=hgWeb
        self.wBF= wdBotFlange
        self.tBF= thBotFlange
      
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(ISection,self).__eq__(other)
                if(retval):
                    retval= (self.wTF == other.wTF)
                if(retval):
                    retval= (self.tTF == other.tTF)
                if(retval):
                    retval= (self.tW == other.tW)
                if(retval):
                    retval= (self.hW == other.hW)
                if(retval):
                    retval= (self.wBF == other.wBF)
                if(retval):
                    retval= (self.tBF == other.tBF)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def hTotal(self):
        '''Return total height (parallel to local y axis) of the section
        '''
        retval=self.tTF+self.hW+self.tBF
        return retval
  
    def A(self):
        '''Return cross-sectional area of the section'''
        retval=self.wTF*self.tTF+self.tW*self.hW+self.wBF*self.tBF
        return retval
  
    def yMax(self):
        ''' Return the maximum distance from the section contour
            to the local Z axis.'''
        ymax= max(self.hCOG(),self.hTotal()-self.hCOG())
        return ymax
        
    def zMax(self):
        ''' Return the maximum distance from the section contour
            to the local Y axis.'''
        zmax=max(self.wTF/2.0,self.wBF/2.0)
        return zmax
    
    def hCOG(self):
        '''Return distance from the bottom fiber of the inferior flange to the 
        center of gravity of the section.
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
        '''Return torsional constant of the section (pág. 800 Puentes II. Apuntes para su diseño, cálculo y construcción. Javier Manterola'''
        hPrf=self.hTotal()-self.tTF/2.0-self.tBF/2.0
        retval=(self.wTF*self.tTF**3+self.wBF*self.tBF**3+hPrf*self.tW**3)/3.0
        retval*= self.torsionalStiffnessFactor
        return retval
    
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
  
    def getWarpingConstant(self):
        '''Return warping constant of an I-section

        reference: article «I Beam» of Wikipedia.
        '''
        hPrf=self.hTotal()-self.tTF/2.0-self.tBF/2.0
        return (self.tTF+self.tBF)/2.0*hPrf**2/12*self.wTF**3*self.wBF**3/(self.wTF**3+self.wBF**3)

class PolygonalSection(SectionProperties):
    '''Polygonal section geometric parameters

    :ivar plg:  contour of the section.
    '''
    def __init__(self, name, plg):
        '''Constructor.

        :param name:  name of the section.
        :param plg:  contour of the section.
        '''
        super(PolygonalSection,self).__init__(name)
        self.plg= plg
        self.reCenter()
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(PolygonalSection,self).__eq__(other)
                if(retval):
                    retval= (self.plg == other.plg)
            else:
                retval= True
        else:
            retval= False
        return retval
        
    def reCenter(self):
        '''Put the centroid of the section in the
           origin.'''
        v= geom.Pos2d(0.0,0.0)-self.plg.getCenterOfMass()
        self.plg.move(v)
        
    def hTotal(self):
        '''Return total height (parallel to local y axis) of the section
        '''
        return self.plg.getBnd().height

    def yMin(self):
        ''' Return the minimum local y coordinate of the section.'''
        return self.plg.getYMin
    
    def yMax(self):
        ''' Return the minimum local y coordinate of the section.'''
        return self.plg.getYMax

    def zMin(self):
        ''' Return the minimum local z coordinate of the section.'''
        return self.plg.getXMin
    
    def zMax(self):
        ''' Return the minimum local z coordinate of the section.'''
        return self.plg.getXMax
    
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
        return 0.5*math.pi*R2**2*self.torsionalStiffnessFactor
    
    def getWarpingConstant(self):
        ''' Return the value of the section warping constant.'''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(className+'.'+methodName+'; not implemented yet.')
        return 0.0
    
    def alphaY(self):
        '''Return shear shape factor with respect to local y-axis'''
        msg= 'alphaY: shear shape factor not implemented for section: '
        msg+= self.name
        msg+= '. 5/6 returned'
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(className+'.'+methodName+'; '+msg)
        return 5.0/6.0
    
    def alphaZ(self):
        '''Return shear shape factor with respect to local z-axis'''
        msg= 'alphaZ: shear shape factor not implemented for section: '
        msg+= self.name
        msg+= '. 5/6 returned'
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(className+'.'+methodName+'; '+msg)
        return 5.0/6.0
    
    def getContourPoints(self):
        ''' Return the vertices of the rectangle.'''
        retval= list()
        for p in self.plg.getVertices():
            retval.append(p)
        return retval


# T-section:
#                 flange width
#        +---------------------------------+
#        |                                 | flange thickness
#        +------------+      +-------------+
#                     |      |
#                     |      |
#                     |      |  web height
#                     |      |
#                     |      |
#                     |      |
#                     +------+
#                      web width
#
class TSection(PolygonalSection):
    ''' T-section.

    :ivar webWidth: web width.
    :ivar webHeight: web height.
    :ivar flangeWidth: flange width.
    :ivar flangeThickness: flange thickness.
    :ivar chamferSide: side of the chamfer between the web and the flange.
    '''
    
    def __init__(self, name, webWidth, webHeight, flangeWidth, flangeThickness, chamferSide= 0.0):
        ''' Constructor.

        :param name:  name of the section.
        :param webWidth: web width.
        :param webHeight: web height.
        :param flangeWidth: flange width.
        :param flangeThickness: flange thickness.
        :param chamferSide: side of the chamfer between the web and the flange.
        '''
        self.webWidth= webWidth # web width.
        self.webHeight= webHeight # web height.
        self.flangeWidth= flangeWidth # flange width.
        self.flangeThickness= flangeThickness # flange thickness.
        self.chamferSide= chamferSide
        super(TSection, self).__init__(name= name, plg= self.buildContour())

    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(TSection,self).__eq__(other)
                if(retval):
                    retval= (self.webWidth == other.webWidth)
                if(retval):
                    retval= (self.webHeight == other.webHeight)
                if(retval):
                    retval= (self.flangeWidth == other.flangeWidth)
                if(retval):
                    retval= (self.flangeThickness == other.flangeThickness)
                if(retval):
                    retval= (self.chamferSide == other.chamferSide)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def buildContour(self):
        ''' Create the section contour.'''
        contour= geom.Polygon2d()
        contour.appendVertex(geom.Pos2d(0,0))
        contour.appendVertex(geom.Pos2d(self.webWidth/2.0, 0))
        contour.appendVertex(geom.Pos2d(self.webWidth/2.0, self.webHeight-self.chamferSide))
        contour.appendVertex(geom.Pos2d(self.webWidth/2.0+self.chamferSide, self.webHeight))
        contour.appendVertex(geom.Pos2d(self.flangeWidth/2.0, self.webHeight))
        contour.appendVertex(geom.Pos2d(self.flangeWidth/2.0, self.webHeight+self.flangeThickness))
        contour.appendVertex(geom.Pos2d(-self.flangeWidth/2.0, self.webHeight+self.flangeThickness))
        contour.appendVertex(geom.Pos2d(-self.flangeWidth/2.0, self.webHeight))
        contour.appendVertex(geom.Pos2d(-self.webWidth/2.0-self.chamferSide, self.webHeight))
        contour.appendVertex(geom.Pos2d(-self.webWidth/2.0, self.webHeight-self.chamferSide))
        contour.appendVertex(geom.Pos2d(-self.webWidth/2.0, 0.0))
        return contour

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

class Hole(SectionProperties):
    '''Section that returns the negative counterparts of the section properties
       to simulate a hole in another section object,...)

    :ivar section: cross-section of the hole. 
    '''
    def __init__(self, name, section):
        ''' Constructor.

        :param name: name identifying the hole.
        :param section: cross-section of the hole.
        '''
        super(Hole,self).__init__(name)
        self.section= section
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(Hole,self).__eq__(other)
                if(retval):
                    retval= (self.section == other.section)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super().getDict()
        retval.update({'section':self.section})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        super().setFromDict(dct)
        self.section= dct['section']
        
    def A(self):
        '''Return cross-sectional area.'''
        return -self.section.A()
    
    def yMax(self):
        ''' Return the maximum distance from the section contour
            to the local Z axis.'''
        return self.section.yMax()
        
    def zMax(self):
        ''' Return the maximum distance from the section contour
            to the local Y axis.'''
        return self.section.zMax()
    
    def hCOG(self):
        '''Return distance from the bottom fiber to the 
        center of gravity of the hole.
        '''
        return self.section.hCOG()

    def bCOG(self):
        '''Return distance from the leftmost fiber to the 
        center of gravity of the section.
        '''
        return self.section.bCOG()
  
    def Iy(self):
        '''Return second moment of area about the local y-axis'''
        return -self.section.Iy()
    
    def Iz(self):
        '''Return second moment of area about the local z-axis'''
        return -self.section.Iz()

    def J(self):
        '''Return torsional constant of the section'''
        return -self.section.J()
  
    def Wyel(self):
        '''Return section modulus with respect to local y-axis'''
        return -self.section.Wyel()
  
    def Wzel(self):
        '''Return section modulus with respect to local z-axis'''
        return -self.section.Wzel()
  
    def alphaY(self):
        '''Return shear shape factor with respect to local y-axis'''
        return self.section.alphaY()
  
    def alphaZ(self):
        '''Return shear shape factor with respect to local z-axis'''
        return self.section.alphaZ()
  
    def getYieldMomentY(self,fy):
        '''Return section yield moment.

           :param fy: material yield stress.
        '''
        return -self.section.getYieldMomentY(fy= fy)
  
    def getElasticSectionModulusY(self):
        '''Returns the elastic section modulus with respect to the y axis.
        '''
        return -self.section.getElasticSectionModulusY()
  
    def getPlasticSectionModulusY(self):
        '''Returns the plastic section modulus.

           Computes the plastic section modulus assuming that plastic neutral 
           axis passes through section centroid (which is true whenever the 
           rectangular section is homogeneous).
        '''
        return -self.section.getPlasticSectionModulusY()
  
    def getYieldMomentZ(self,fy):
        '''Return section yield moment.

           :param fy: material yield stress.
        '''
        return -self.section.getYieldMomentZ(fy= fy)
  
    def getElasticSectionModulusZ(self):
        '''Returns the elastic section modulus with respect to the z axis.'''
        return -self.section.getElasticSectionModulusZ()
  
    def getPlasticSectionModulusZ(self):
        '''Returns the plastic section modulus of the hole.
        '''
        return -self.section.getPlasticSectionModulusZ()
  
    def getJTorsion(self):
        '''Return torsional constant of the hole.
        '''
        return -self.section.getJTorsion()
  
    def getTorsionalStiffness(self, G):
        '''Return the torsional stiffness of the hole.'''
        return -self.section.getTorsinalStiffnes(G= G)

    def getWarpingConstant(self):
        ''' Return the value of the hole warping constant.
        '''
        return -self.section.getWarpingConstant()
    
    def getShearStiffnessY(self, G):
        '''Return the shear stiffness of the hole.'''
        return self.section.getShearStiffnessY(G= G)
    
    def getShearStiffnessZ(self, G):
        '''Return the shear stiffness of the hole.'''
        return self.section.getShearStiffnessZ(G= G)

    def getRegion(self, gm, nmbMat, twoDimensionalMember= False):
        '''Generation of a quadrilateral region from the hole 
        geometry.

        :param gm: object of type section_geometry
        :param nmbMat: name of the material (string)
        :param twoDimensionalMember: true if the region corresponds to a 
                                     two-dimensional member.
        '''
        return self.section.getRegion(gm= gm, nmbMat= nmbMat, twoDimensionalMember= woDimensionalMember)
    
    def getContourPoints(self):
        ''' Return the vertices of the rectangle.'''
        return self.section.getContourPoints()
    
    def defElasticMembranePlateSection(self, preprocessor, material, overrideRho= None, reductionFactor= 1.0):
        '''Elastic membrane plate section appropriate for shell analysis.

        :param preprocessor: preprocessor object.
        :param material: material constitutive model (for which 
                         E is the Young's modulus nu the Poisson's ratio).
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the elastic
                                modulus to simulate the effect of cracking,
                                creep, etc.
        :param reductionFactor: factor that divides the elastic
                                modulus to simulate the effect of cracking,
                                creep, etc.
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(className+'.'+methodName+'; material: '+self.name+ ' cannot return an elastic membrane plate section.')
        return None
        
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        super(Hole,self).report(os= os, indentation= indentation)
        self.section.report(os= os, indentation= indentation+'  ')

        
class CompoundSection(SectionProperties):
    '''Compound section properties (area, moments of inertia,...)

    :ivar section_list: list of sections.
    :ivar Iw: warping constant of the compound section.
    '''
    def __init__(self, name, section_list, Iw):
        ''' Constructor.

        :param name: name identifying the section.
        :param section_list: list of sections.
        :param Iw: warping constant of the compound section.
        '''
        super(CompoundSection,self).__init__(name)
        self.sectionList= section_list
        self.Iw= Iw
      
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(CompoundSection,self).__eq__(other)
                if(retval):
                    retval= (self.Iw == other.Iw)
                if(retval):
                    retval= (len(self.sectionList) == len(other.sectionList))
                for sectionA, sectionB in zip(self.sectionList, other.sectionList):
                    retval= (sectionA == sectionB)
                    if(not retval):
                        break
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def A(self):
        '''cross-sectional area'''
        retval= 0.0
        for s in self.sectionList:
            retval+= s[1].A()
        return retval

    def getWarpingConstant(self):
        ''' Return the value of the section warping constant.'''
        return self.Iw
  
    def yCenterOfMass(self):
        '''y coordinate of the center of mass.'''
        retval= 0.0
        totalArea= 0.0
        for s in self.sectionList:
            area= s[1].A()
            totalArea+=area 
            retval+= s[0].y*area
        retval/= totalArea
        return retval
  
    def zCenterOfMass(self):
        '''z coordinate of the center of mass.'''
        retval= 0.0
        totalArea= 0.0
        for s in self.sectionList:
            area= s[1].A()
            totalArea+=area 
            retval+= s[0].x*area
        retval/= totalArea
        return retval

    def yMax(self):
        ''' Return the maximum distance from the section contour
            to the local Z axis.'''
        yCOG= self.yCenterOfMass()
        retval= -1
        for s in self.sectionList:
            yCOG_s= s[0].y+s[1].yCenterOfMass()-yCOG
            yMax_s= abs(yCOG_s)+s[1].yMax()
            retval= max(retval, yMax_s)
        return retval
        
    def zMax(self):
        ''' Return the maximum distance from the section contour
            to the local Y axis.'''
        zCOG= self.zCenterOfMass()
        retval= -1
        for s in self.sectionList:
            zCOG_s= s[0].x+s[1].zCenterOfMass()-zCOG
            zMax_s= abs(zCOG_s)+s[1].zMax()
            retval= max(retval, zMax_s)
        return retval
    
    def hCOG(self):
        '''Return distance from the bottom fiber to the 
        center of gravity of the section.
        '''
        yCOG= self.yCenterOfMass()
        retval= 1
        for s in self.sectionList:
            yCOG_s= s[0].y+s[1].yCenterOfMass()-yCOG
            hCOG_s= yCOG_s-abs(s[1].hCOG())
            retval= min(retval, hCOG_s)
        return -retval

    def bCOG(self):
        '''Return distance from the leftmost fiber to the 
        center of gravity of the section.
        '''
        raise Exception('Not implemented yet.')
        
    def Iy(self):
      '''second moment of area about the local y-axis.'''
      zCenter= self.zCenterOfMass()
      retval= 0.0
      for s in self.sectionList:
        z= s[0].x
        retval+= s[1].SteinerY(z-zCenter)
      return retval
  
    def Iz(self):
        '''second moment of area about the local z-axis (abstract method)'''
        yCenter= self.yCenterOfMass()
        retval= 0.0
        for s in self.sectionList:
          y= s[0].y
          retval+= s[1].SteinerZ(y-yCenter)
        return retval
  
    def J(self):
        '''torsional constant of the section.'''
        center= geom.Pos2d(self.zCenterOfMass(), self.yCenterOfMass())
        retval= 0.0
        for s in self.sectionList:
            retval+= s[1].SteinerJ(dist= s[0].dist(center))
        retval*= self.torsionalStiffnessFactor
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
    
    def draw(self, notes= None):
        ''' Draw the section contour using pyplot.

        :param notes: notes to insert in the plot.
        '''
        fig = plt.figure()
        ax = fig.add_subplot(111, aspect= 'equal') # subplot axes
        for section in self.sectionList:
            org= section[0]
            x,y= section[1].getXYVertices(offset=geom.Vector2d(org.x,org.y))
            ax.fill(x,y,'tab:gray')
            ax.plot(x,y,'k')
        if(notes):
            # build a rectangle in axes coords
            left, width = .25, .5
            bottom, height = .25, .5
            top = bottom + height
            for text in notes:
                ax.text(left+width/2, top, text,
                horizontalalignment='left',
                verticalalignment='top',
                transform= ax.transAxes)
                top-= 0.05
        plt.show()
