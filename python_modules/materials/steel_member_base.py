# -*- coding: utf-8 -*-
''' Base classes and functions for structural steel members.'''

from __future__ import division


__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg
from model.geometry import geom_utils as gu
from materials import member_base
import xc_base
import geom

class Member(member_base.Member):
    '''Base class for steel members.
    
    :ivar shape: cross-section steel shape (e.g. IPNShape, IPEShape, ...)
    '''
    def __init__(self,name,shape, lstLines=None):
        '''Constructor.

        :param name: object name.
        :param shape: cross-section steel shape.
        :param lstLines: ordered list of lines that make up the member 
                        (defaults to None).
        '''
        super(Member,self).__init__(name, lstLines)
        self.shape= shape
        

class BucklingMember(Member):
    '''Base class for steel members that could buckle.
    
    :ivar lstPoints: ordered list of points that make up the member. Ignored if 
          lstLines is given (defaults to None) 
    :ivar contrPnt: control points along the member.
    '''
    def __init__(self,name,shape, lstLines=None, lstPoints=None):
        '''Constructor.

        :param name: object name.
        :param shape: cross-section shape
        :param lstLines: ordered list of lines that make up the member 
                        (defaults to None).
        :param lstPoints: ordered list of points that make up the member. 
                          Ignored if lstLines is given (defaults to None)
        '''
        super(BucklingMember,self).__init__(name, shape, lstLines)
        self.lstPoints= lstPoints
        self.contrPnt= None # control points along the member.
        self.pline= None # member axis.
        
    def getPreprocessor(self):
        ''' Return the XC preprocessor.'''
        retval= super(BucklingMember,self).getPreprocessor()
        if((not retval) and self.lstPoints):
            retval= self.lstPoints[0].getPreprocessor
        if(not retval):
            lmsg.error('No lines nor points set.')
        return retval

    def getMemberGeometry(self):
        ''' Return the lines and points along the member.'''
        lstLn= None
        lstP3d= None
        if self.lstLines:
            lstLn= self.lstLines
            lstP3d= gu.lstP3d_from_lstLns(lstLn)
        elif self.lstPoints:
            lstP3d= [p.getPos for p in self.lstPoints]
            lstLn= gu.lstLns_from_lstPnts(self.lstPoints)
        else:
            lmsg.warning('Incomplete member definition: list of lines or points  required')
        # member representation
        pol= geom.Polyline3d()
        for p in lstP3d:
            pol.append(p)
        self.pline= pol
        return lstLn, lstP3d
                     
    def setControlPoints(self):
        '''Set the five equally spaced points in the member where the moment Mz 
        will be evaluated in order to obtain the moment gradient factor 
        involved in the calculation of the lateral-torsional buckling reduction 
        factor. 

        An attribute of member is created, named 'contrPnt' that contains 
        a list of five tuples (elem,relativDist), each of which contains the
        element of the member nearest to one control-point and the relative 
        distance from this control point to the first node of the element.
        The method also creates the attributes 'length' and 'elemSet' that 
        represent the length of the member and the set of elements included 
        in it.
        '''
        if(not self.elemSet):
            self.createElementSet()
        
        lstEqPos3d= gu.lstEquPnts_from_polyline(self.pline, nDiv=4) #(five points equally spaced)
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
            Mz1= 0.0
            if(hasattr(e,'getMz1')):
               Mz1= e.getMz1  #Z bending moment at the back end of the element
            Mz2= 0.0
            if(hasattr(e,'getMz2')):
               Mz2= e.getMz2  #Z bending moment at the front end of the element
            MzCP=Mz1+(Mz2-Mz1)*self.contrPnt[i][1] # Z bending moment at the control point
            Mi.append(MzCP)
        return Mi;
            

