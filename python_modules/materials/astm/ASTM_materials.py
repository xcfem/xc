# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

''' Structural steel as specified in ASTM standard.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
from materials import steel_base
from miscUtils import LogMessages as lmsg

class ASTMSteel(steel_base.BasicSteel):
    '''ASTM structural steel.'''
    rho= 7850 # kg/m3
    def __init__(self, fy= 250e6, fu= 400e6, gammaM= 1.0):
        super(ASTMSteel,self).__init__(200e9,0.3,fy,fu,gammaM)

A36= ASTMSteel(250e6,400e6,1.0)
A529= ASTMSteel(290e6,414e6,1.0)
A53= ASTMSteel(240e6,414e6,1.0)
A992= ASTMSteel(345e6,450e6,1.0)
A500= ASTMSteel(315e6,400e6,1.0)
A307= ASTMSteel(245e6,390e6,1.0)

class ASTMShape(object):
    """Steel shape with ASTM verification routines."""
    def __init__(self, name):
       '''
         Constructor.

       '''
       self.name=name
       
from materials.sections.structural_shapes import aisc_metric_shapes

class WShape(ASTMShape,aisc_metric_shapes.WShape):
    """W shape with ASTM verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. W40X431)
        '''
        ASTMShape.__init__(self,name)
        aisc_metric_shapes.WShape.__init__(self,steel,name)

class CShape(ASTMShape,aisc_metric_shapes.CShape):
    """C shape with ASTM 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name  (i.e. C380X74).
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.CShape.__init__(self,steel,name)

class HSSShape(ASTMShape,aisc_metric_shapes.HSSShape):
    """HSS shape with ASTM verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. HSS2X2X_250).
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.HSSShape.__init__(self,steel,name)
        
    def getLimitingWithToThicknessRatio(self):
        ''' Return the Limiting Width-to- Width-to-Thickness
            Ratio according to table B4.1A of AISC-360-16.
        '''
        return 1.4*math.sqrt(self.steelType.E/self.steelType.fy)
    def getBClassification(self):
        ''' Return the classification for local buckling of the
            "b" wall of the section.
        '''
        retval= 'nonslender'
        bSlendernessRatio= self.get('bSlendernessRatio')
        lambda_r= self.getLimitingWithToThicknessRatio()
        if(bSlendernessRatio>lambda_r):
            retval= 'slender'
        return retval
    def getHClassification(self):
        ''' Return the classification for local buckling of the
            "h" wall of the section.
        '''
        retval= 'nonslender'
        hSlendernessRatio= self.get('hSlendernessRatio')
        lambda_r= self.getLimitingWithToThicknessRatio()
        if(hSlendernessRatio>lambda_r):
            retval= 'slender'
        return retval
    def getClassification(self):
        ''' Return the classification for local buckling of the
            section.
        '''
        retval= 'nonslender'
        if((self.getHClassification()=='slender') or (self.getBClassification()=='slender')):
            retval= 'slender'
        return retval
       
    def getReducedEffectiveH(self):
        '''Return the reduced effective width corresponding
           to the "h" walls of the shape.'''
        rt= math.sqrt(self.steelType.E/self.steelType.fy) #Simplification
        h_t= self.get('hSlendernessRatio')
        t= self.get('t')
        retval= 1.92*t*rt*(1.0-0.38/h_t*rt)
        retval= min(retval,self.get('h'))
        return retval
    def getReducedEffectiveB(self):
        '''Return the reduced effective width corresponding
           to the "b" walls of the shape.'''
        rt= math.sqrt(self.steelType.E/self.steelType.fy) #Simplification
        b_t= self.get('bSlendernessRatio')
        t= self.get('t')
        retval= 1.92*t*rt*(1.0-0.38/b_t*rt)
        retval= min(retval,self.get('b'))
        return retval
    def getEffectiveArea(self):
        '''Return the effective area.'''
        retval= self.get('A')
        clasif= self.getClassification()
        if(clasif == 'slender'):
            t= self.get('t')
            h_ineff= self.get('h_flat')-self.getReducedEffectiveH()
            retval-= 2.0*h_ineff*t
            b_ineff= self.get('b_flat')-self.getReducedEffectiveB()
            retval-= 2.0*b_ineff*t
        return retval

class MemberConnection(object):
    '''Member length and connections

       :ivar L: member length.
       :ivar rotI: fixity of the rotation at member start.
       :ivar transI: fixity of the translation at member start.
       :ivar rotJ: fixity of the rotation at member end.
       :ivar transJ: fixity of the translation at member end.
    '''
    def __init__(self,L,rotI='free',transI='fixed',rotJ= 'free',transJ= 'fixed'):
        '''Constructor.'''
        self.L= L
        self.rotI= rotI
        self.transI= transI
        self.rotJ= rotJ
        self.transJ= transJ
    def getEffectiveBucklingLengthCoefficientRecommended(self):
        '''Return the effective length factor
           according to table C-A-7.1 or AISC specification'''
        if(self.rotI=='fixed'):
            if(self.rotJ=='fixed'):
                if(self.transJ=='fixed'):
                    retval= .65 # Theoretical .5
                else: # self.transJ=='free'
                    retval= 1.2 #Theoretical 1.0
            else: # self.rotJ=='free'
                if(self.transJ== 'fixed'):
                    retval= .8 # Theoretical .7
                else: # self.transJ=='free'
                    retval= 2.1 # Theoretical 2.0
        else: # self.rotI=='free'
             if(self.rotJ=='fixed'):
                 if(self.transJ=='free'):
                     retval= 2.0 # Theoretical 2.0
                 else:
                     retval= 0.8 # Theoretical .7
             else: # self.rotJ=='free'
                 if(self.transI=='fixed' and self.transJ=='fixed'):
                     retval= 1.0 # Theoretical 1.0
                 else:
                     retval= 1e6 # Stiffness matrix singular
        return retval

class Member(object):
    """C shape with ASTM 3 verification routines."""
    def __init__(self,shape,connection):
        ''' Constructor.

        :param shape: structural shape.
        :param connection: member length and connection information.
        '''
        self.shape= shape
        self.connection= connection
    def getEffectiveLength(self):
        '''Return the member effective length according to
           section E2 of AISC 360-16.'''
        K= self.connection.getEffectiveBucklingLengthCoefficientRecommended()
        return  K*self.connection.L #Effective length of member.
    def getSlendernessRatio(self):
        '''Return the slenderness ratio of the member.'''
        Lc= self.getEffectiveLength()
        r= min(self.shape.get('iy'),self.shape.get('iz'))
        return Lc/r
    def getElasticBucklingStress(self):
        '''Return the elastic buckling stress of the member.'''
        return math.pi**2*self.shape.steelType.E/(self.getSlendernessRatio())**2
    def getCriticalStress(self):
        '''Return the critical stress as definded in
           section E7 of AISC 360-16.
        '''
        treshold= 4.71*math.sqrt(self.shape.steelType.E/self.shape.steelType.fy)
        r= self.getSlendernessRatio()
        Fe= self.getElasticBucklingStress()
        fy_fe= self.shape.steelType.fy/Fe
        if((r<=treshold) or (fy_fe<=2.25)):
            return math.pow(0.658,fy_fe)*self.shape.steelType.fy
        else:
            return 0.877*Fe
    def getNominalCompressiveStrength(self):
        ''' Return the nominal compressive strength according to
            section E7 of AISC 360-16.
        '''
        return self.shape.getEffectiveArea()*self.getCriticalStress()
