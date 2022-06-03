# -*- coding: utf-8 -*-
'''Definition of typical reinforcement schemes.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2018, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from rough_calculations import ng_simple_bending_reinforcement
from postprocess.reports import common_formats as fmt
import math

class Rebar(object):
    ''' Reinforcement bar.

      :ivar diam: diameter of the bars.
      :ivar steel: steel material.
    '''
    def __init__(self, diam:float, steel= None):
        ''' Constructor.

        :param diam: diameter of the bar.
        :param steel: steel material.
        '''
        self.diam= diam
        self.steel= steel
      
    def getDiam(self):
        ''' Return the diameter of the bars.'''
        return self.diam
    
    def getArea(self):
        ''' Return the area of each bar.'''
        return math.pi*(self.diam/2.0)**2

    def getDesignStrength(self):
        ''' Return the design value of the bar strength.'''
        retval= 0.0
        if(self.steel is not None):
            retval= self.getArea()*self.steel.fyd()
        else:
            lmsg.warning('Reinforcing steel undefined.')
        return retval
    
class RebarRow(Rebar):
    ''' Row of reinforcement bars.

      :ivar diam: diameter of the bars.
      :ivar spacing: spacing of the bars.
      :ivar concreteCover: concrete cover of the bars.
    '''
    def __init__(self,diam:float ,spacing:float ,concreteCover:float, steel= None):
        ''' Constructor.

        :param diam: diameter of the bars.
        :param spacing: spacing of the bars.
        :param concreteCover: concrete cover of the bars.
        '''
        super(RebarRow, self).__init__(diam= diam, steel= steel)
        self.spacing= spacing
        self.concreteCover= concreteCover
      
    def getBarArea(self):
        ''' Return the area of each bar.'''
        return super(RebarRow, self).getArea()
    
    def getNumBarsPerMeter(self, roundUp= False):
        ''' Return the number of bars per unit length.

        :param roundUp: if true return an integer number of bars greater or
                        equal to (width/spacing).
        '''
        retval= 1.0/self.spacing
        if(roundUp):
            retval= math.ceil(retval)
        return retval
        
    def getNumBars(self, width, roundUp= False):
        ''' Return the number of bars in the length argument.

        :param width: width of the reinforcement.
        :param roundUp: if true return an integer number of bars greater or
                        equal to (width/spacing).
        '''
        retval= width/self.spacing
        if(roundUp):
            retval= math.ceil(retval)
        return retval
    
    def getAs(self, width= 1.0, roundUp= False):
        ''' Return the total area of the bars.

        :param width: width of the reinforcement.
        '''
        return self.getNumBars(width, roundUp)*self.getBarArea()
    
    def getEffectiveCover(self):
        ''' returns the effective cover of the rebar family.

        Returns the distance between the surface of the concrete and the 
        centroid of the rebars family.
        '''
        return self.concreteCover+self.diam/2.0
    
    def getT(self, width= 1.0, roundUp= False):
        ''' Return the design value of the ultimate tension force in 
            the reinforcement.

        :param width: width of the reinforcement.
        :param roundUp: if true return an integer number of bars greater or
                        equal to (width/spacing).
        '''
        return self.getAs(width, roundUp)*self.steel.fyd()
    
class RebarFamily(RebarRow):
    ''' Family or reinforcement bars.'''
    minDiams= 50
    
    def __init__(self, steel, diam, spacing, concreteCover):
        ''' Constructor.

        :param steel: reinforcing steel material.
        :param diam: diameter of the bars.
        :param spacing: spacing of the bars.
        :param concreteCover: concrete cover of the bars.
        '''
        super(RebarFamily,self).__init__(diam= diam, spacing= spacing, concreteCover= concreteCover, steel= steel)
      
    def __repr__(self):
        return self.steel.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.spacing*1e3))
    
    def getCopy(self):
        return RebarFamily(steel= self.steel, diam= self.diam, spacing= self.spacing, concreteCover= self.concreteCover)
            
    def getMR(self,concrete,b,thickness, z= None):
        '''Return the bending resistance of the (b x thickness) rectangular section.

        :param concrete: concrete material.
        :param b: width of the rectangular section.
        :param thickness: height of the rectangular section.
        :param z: inner lever arm (if None z= 0.9*d).
        '''
        return ng_simple_bending_reinforcement.Mu(As= self.getAs(), fcd= concrete.fcd(), fsd= self.steel.fyd(), b= b, d= self.d(thickness), z= z, c_depth= concrete.getCDepth())
      
    def d(self, thickness):
        ''' Return the effective depth of the reinforcement.

        :param thickness: height of the rectangular section.
        '''
        return thickness-self.getEffectiveCover()
      
    def getDefStr(self):
        ''' Return definition strings for drawSchema.'''
        return ("  $\\phi$ "+ fmt.Diam.format(self.getDiam()*1000) + " mm, e= "+ fmt.Diam.format(self.spacing*1e2)+ " cm")
    
    def getDefStrings(self):
        ''' Return definition strings for drawSchema.'''
        retval= []
        retval.append(self.getDefStr())
        retval.append(" - ")
        return retval
    
    def writeDef(self,outputFile,concrete):
        outputFile.write("  diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, spacing: "+ fmt.Diam.format(self.spacing*1e3)+ " mm")
        reinfDevelopment= self.getBasicAnchorageLength(concrete)
        outputFile.write("  reinf. development L="+ fmt.Length.format(reinfDevelopment) + " m ("+ fmt.Diam.format(reinfDevelopment/self.getDiam())+ " diameters).\\\\\n")

class FamNBars(RebarFamily):
    ''' Family of "n" rebars.

    :ivar n: number of rebars.
    '''
    n= 2 #Number of bars.
    def __init__(self,steel,n,diam,spacing,concreteCover):
        super(RebarFamily, self).__init__(steel= steel, diam= diam, spacing= spacing, concreteCover= concreteCover)
        self.n= int(n) # number of bars
        
    def __repr__(self):
      return str(n) + " x " + self.steel.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.spacing*1e3))
  
    def getNumBars(self):
        ''' Return the number of bars in the length argument.

        :param width: width of the reinforcement.
        :param roundUp: if true return an integer number of bars greater or
                        equal to (width/spacing).
        '''
        return self.n

    def getWidth(self):
        ''' Return the width occupied by the rebar family.'''
        return (self.n-1)*self.spacing()
    
    def getAs(self):
        ''' Return the total area of the bars.
        '''
        return self.getNumBars()*self.getBarArea()
    
    def getT(self):
        ''' Return the design value of the ultimate tension force in 
            the reinforcement.
        '''
        return self.getAs()*self.steel.fyd()
    
    def writeDef(self,outputFile,concrete):
      outputFile.write("  n= "+str(self.n)+" diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, spacing: "+ fmt.Diam.format(self.spacing*1e3)+ " mm")
      reinfDevelopment= self.getBasicAnchorageLength(concrete)
      outputFile.write("  reinf. development L="+ fmt.Length.format(reinfDevelopment) + " m ("+ fmt.Diam.format(reinfDevelopment/self.getDiam())+ " diameters).\\\\\n")


class DoubleRebarFamily(object):
    ''' Two reinforcement bars families.

    :ivar f1: first rebar family.
    :ivar f2: second rebar family.
    '''
    def __init__(self,f1,f2):
        ''' Constructor.

        :param f1: first rebar family.
        :param f2: second rebar family.
        '''
        self.f1= f1
        self.f2= f2
    def __repr__(self):
        return self.f1.__repr__() + " + " + self.f2.__repr__()
    def getCopy(self):
        return DoubleRebarFamily(self.f1.getCopy(),self.f2.getCopy())
    def getAs(self):
      ''' Return the total area of the bars.'''
      return self.f1.getAs()+self.f2.getAs()
    def getSpacing(self):
        ''' Return the average spacing of the bars.'''
        n1= self.f1.getAs()/self.f1.getBarArea()
        n2= self.f2.getAs()/self.f2.getBarArea()
        return 1/(n1+n2)
    def getEffectiveCover(self):
        ''' returns the effective cover of the rebar family.

        Returns the distance between the surface of the concrete and the 
        centroid of the rebars family.
        '''
        T1= self.f1.getT()
        T2= self.f2.getT()
        T= T1+T2
        return (self.f1.getEffectiveCover()*T1+self.f2.getEffectiveCover()*T2)/T
    def getBasicAnchorageLength(self,concrete):
        ''' Return the basic anchorage length of the bars.

        :param concrete: concrete material.
        '''
        l1= self.f1.getBasicAnchorageLength(concrete)
        l2= self.f2.getBasicAnchorageLength(concrete)
        return max(l1,l2)
    def getMinReinfAreaUnderFlexion(self,concrete,thickness):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under flexion.

        :param concrete: concrete material.
        :param thickness: thickness of the bended member.
        '''
        retval= self.f1.getMinReinfAreaUnderFlexion(concrete= concrete,thickness= thickness)
        return retval
    def getMinReinfAreaUnderTension(self,concrete,thickness):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under tension.

        :param concrete: concrete material.
        :param thickness: thickness of the tensioned member.
        '''
        retval= self.f1.getMinReinfAreaUnderTension(concrete= concrete,thickness= thickness)
        return retval
    def getMR(self,concrete,b,thickness):
        '''Return the bending resistance of the (b x thickness) rectangular section.

        :param concrete: concrete material.
        :param b: width of the rectangular section.
        :param thickness: height of the rectangular section.
        '''
        MR1= self.f1.getMR(concrete,b,thickness)
        MR2= self.f2.getMR(concrete,b,thickness)
        return MR1+MR2
    
    def d(self,thickness):
        return thickness-self.getEffectiveCover()

    def getDefStrings(self):
        ''' Return definition strings for drawSchema.'''
        retval= []
        retval.append(self.f1.getDefStr())
        retval.append(self.f2.getDefStr())
        return retval

    def writeDef(self,outputFile,concrete):
        self.f1.writeDef(outputFile,concrete)
        self.f2.writeDef(outputFile,concrete)

def writeF(outputFile,text,F):
    fmt= "{:4.2f}"
    if(F>1):
        outputFile.write(text+ "= "+ fmt.format(F)+ " OK!\\\\\n")
    elif(F>=0.95):
        outputFile.write(text+ "= "+ fmt.format(F)+ " $\\sim$ OK!\\\\\n")
    else:
        outputFile.write(text+ "= "+ fmt.format(F)+ " Error!\\\\\n")
