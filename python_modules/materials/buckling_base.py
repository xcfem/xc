# -*- coding: utf-8 -*-
from __future__ import division
''' Base classes and functions for structural steel.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg
from model.geometry import geom_utils as gu
import xc_base
import geom

class MemberConnection(object):
    '''Member connection

       :ivar rotI: fixity of the rotation at member start.
       :ivar transI: fixity of the translation at member start.
       :ivar rotJ: fixity of the rotation at member end.
       :ivar transJ: fixity of the translation at member end.
    '''
    def __init__(self,rotI='free',transI='fixed',rotJ= 'free',transJ= 'fixed'):
        '''Constructor.'''
        self.rotI= rotI
        self.transI= transI
        self.rotJ= rotJ
        self.transJ= transJ
    def getEffectiveBucklingLengthCoefficientRecommended(self):
        '''Return the effective length factor
           according to table C-A-7.1 or AISC specification
           and NDS 2018 appendix G'''
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


class MemberBase(object):
    '''Base class for steel members.
    
    :ivar name: object name.
    :ivar shape: cross-section shape (e.g. IPNShape, IPEShape, ...)
    :ivar lstLines: ordered list of lines that make up the beam 
          (defaults to None).
    :ivar lstPoints: ordered list of points that make up the beam. Ignored if 
          lstLines is given (defaults to None) 
    :ivar contrPnt: control points along the beam.
    :ivar elemSet: elements along the beam.
    '''
    def __init__(self,name,shape, lstLines=None, lstPoints=None):
        '''Constructor.

        :param name: object name.
        :param shape: cross-section shape
        :param lstLines: ordered list of lines that make up the beam 
                        (defaults to None).
        :param lstPoints: ordered list of points that make up the beam. 
                          Ignored if lstLines is given (defaults to None)
        '''
        self.name= name
        self.shape= shape
        self.lstLines= lstLines
        self.lstPoints= lstPoints
        self.contrPnt= None # control points along the beam.
        self.elemSet= None # elements of the beam.
        
    def getPreprocessor(self):
        ''' Return the XC preprocessor.'''
        retval= None
        if self.lstLines:
            retval=self.lstLines[0].getPreprocessor
        elif self.lstPoints:
            retval=self.lstPoints[0].getPreprocessor
        else:
            lmsg.error('No lines nor points set.')
        return retval

    def createElementSet(self):
        '''Create the attributes 'length' and 'elemSet' that 
        represent the length of the beam and the set of elements included in it.'''
        prep=self.getPreprocessor()
        if self.lstLines:
            lstLn= self.lstLines
            lstP3d= gu.lstP3d_from_lstLns(lstLn)
        elif self.lstPoints:
            lstP3d= [p.getPos for p in self.lstPoints]
            lstLn= gu.lstLns_from_lstPnts(self.lstPoints)
        else:
            lmsg.warning('Incomplete member definition: list of lines or points  required' )
        #set of elements included in the member
        s= prep.getSets.defSet(self.name+'Set')
        self.elemSet= s.elements
        for l in lstLn:
            for e in l.elements:
                self.elemSet.append(e)
        pol=geom.Polyline3d()
        for p in lstP3d:
            pol.append(p)
        self.length= pol.getLength()
        return pol
            
    def setControlPoints(self):
        '''Set the five equally spaced points in the beam where the moment Mz 
        will be evaluated in order to obtain the moment gradient factor 
        involved in the calculation of the lateral-torsional buckling reduction 
        factor. 

        An attribute of member is created, named 'contrPnt' that contains 
        a list of five tuples (elem,relativDist), each of which contains the
        element of the beam nearest to one control-point and the relative 
        distance from this control point to the first node of the element.
        The method also creates the attributes 'length' and 'elemSet' that 
        represent the length of the beam and the set of elements included in it.
        '''
        pol= self.createElementSet()
        
        lstEqPos3d= gu.lstEquPnts_from_polyline(pol,nDiv=4) #(five points equally spaced)
        lstEqElem= [self.elemSet.getNearestElement(p) for p in lstEqPos3d]
        self.contrPnt= list()
        for i in range(5):
            elem= lstEqElem[i]
            elSegm= elem.getLineSegment(0)
            relDistPointInElem= (lstEqPos3d[i]-elSegm.getFromPoint()).getModulus()/elSegm.getLength()
            self.contrPnt.append((elem,relDistPointInElem))

    def getBendingMomentsAtControlPoints(self):
        ''' Return the bending moments at control points.'''
        if not self.contrPnt:
            self.setControlPoints()
        sz= len(self.contrPnt)
        Mi= list()
        for i in range(sz):
            e=self.contrPnt[i][0]
            e.getResistingForce()
            Mz1=e.getMz1  #Z bending moment at the back end of the element
            Mz2=e.getMz2  #Z bending moment at the front end of the element
            MzCP=Mz1+(Mz2-Mz1)*self.contrPnt[i][1] # Z bending moment at the control point
            Mi.append(MzCP)
        return Mi;
            

