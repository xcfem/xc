# -*- coding: utf-8 -*-

'''Eurocode 3 checks for a beam or column.'''
__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"


import math
import xc_base
import geom
import xc
from model.geometry import geom_utils as gu
from materials.ec3 import EC3_limit_state_checking as EC3lsc
from model.sets import sets_mng as smg
from miscUtils import LogMessages as lmsg

class EC3Beam(object):
    '''Steel beam defined by an arbitrary name, a cross-section shape, 
    its section class, the coefficients of supports and the type  
    
    :ivar ec3Shape: cross-section shape (e.g. IPNShape, IPEShape, ...)
    :ivar sectionClass: section class (1 to 3, 4 not yet implemented)
          (defaults to 1).
    :ivar supportCoefs:instance of EC3_limit_state_checking.SupportCoefficients
          that wraps the support coefficients: ky, kw, k1 and k2. where ky is 
          the lateral bending coefficient, kw the warping coefficient,  k1 and            the warping AND lateral bending coefficients at first and last ends 
          respectively (1.0 => free,  0.5 => prevented). 
          (Defaults to ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0)
    :ivar typo: 'rolled' or 'welded' (defaults to rolled)
    :ivar lstLines: ordered list of lines that make up the beam 
          (defaults to None).
    :ivar lstPoints: ordered list of points that make up the beam. Ignored if 
          lstLines is given (defaults to None) 
          
    '''
    def __init__(self,name,ec3Shape,sectionClass=1,supportCoefs=EC3lsc.SupportCoefficients(ky=1.0,kw=1.0,k1=1.0,k2=1.0),typo= 'rolled',lstLines=None,lstPoints=None):
        self.name=name
        self.ec3Shape= ec3Shape
        self.sectionClass=sectionClass
        self.supportCoefs=supportCoefs
        self.typo=typo
        self.lstLines=lstLines
        self.lstPoints=lstPoints

    def setControlPoints(self):
        '''Set the five equally spaced points in the beam where the moment Mz 
        will be evaluated in order to obtain the moment gradient factor 
        involved in the calculation of the lateral-torsional buckling reduction 
        factor. That moment gradient factor will be calculated following   
        the general expression proposed by A. López, D. J. Yong, 
        M. A. Serna.
        An attribute of EC3Beam is created, named 'contrPnt' that contains 
        a list of five tuples (elem,relativDist), each of which contains the
        element of the beam nearest to one control-point and the relative 
        distance from this control point to the first node of the element.
        The method also creates the attributes 'length' and 'elemSet' that 
        represent the lenght of the beam and the set of elements included in it.
        '''
        if self.lstLines:
            lstLn=self.lstLines
            pointsHandler=lstLn[0].getPreprocessor.getMultiBlockTopology.getPoints
            lstP3d=[pointsHandler.get(l.getKPoints()[0]).getPos for l in lstLn]
            lstP3d.append(pointsHandler.get(lstLn[-1].getKPoints()[1]).getPos)
        elif self.lstPoints:
            lstP3d=[p.getPos for p in self.lstPoints]
            lstLn=[smg.get_lin_2Pts(self.lstPoints[i],self.lstPoints[i+1]) for i in range (len(self.lstPoints)-1)]
            lstLn.append(smg.get_lin_2Pts(self.lstPoints[-2],self.lstPoints[-1]))
        else:
            lmsg.warning('Beam insufficiently defined: list of lines or points  required' )
        #set of elements included in the EC3beam
        s=lstLn[0].getPreprocessor.getSets.defSet(self.name+'Set')
        self.elemSet=s.getElements
        for l in lstLn:
            for e in l.getElements():
                self.elemSet.append(e)
        nmbSpac=4   # number of divisions (five points equally spaced)
        pol=geom.Polyline3d()
        for p in lstP3d:
            pol.append(p)
        self.length=pol.getLength()
        nTotSegm=pol.getNumSegments()
        eqDistCP=.25*self.length          #equidistance between control points
        nmbCP=nmbSpac-1         #number of intermediate control points
        cumLength=0             #cumulate length in the polyline
        nmbSegm=1               #starting number of segment
        self.contrPnt=list()
        firstPnt=lstP3d[0]
        self.contrPnt.append((lstLn[0].getNearestElement(firstPnt),0)) 
        for i in range(1,nmbCP+1):    #intermediate points
            lengthCP=i*eqDistCP-cumLength
            for j in range (nmbSegm,nTotSegm+1):
                sg=pol.getSegment(j)
                LSegm=sg.getLength()
                if lengthCP<LSegm:
                    pnt=sg.getPoint(lengthCP/LSegm)
                    elem=lstLn[nmbSegm-1].getNearestElement(pnt)
                    elSegm=elem.getLineSegment(0)
                    relDistPointInElem=(pnt-elSegm.getOrigen()).getModulo()/elSegm.getLength()
                    self.contrPnt.append((elem,relDistPointInElem))
                    break
                else:
                    nmbSegm+=1
                    cumLength+=LSegm
                    lengthCP-=LSegm
        lastPnt=lstP3d[-1]
        self.contrPnt.append((lstLn[-1].getNearestElement(lastPnt),1)) 
        return
        
        
    def getLateralBucklingReductionFactor(self):
        ''' Returns lateral torsional buckling reduction factor value
        for the elements of the beam.'''
        if not self.contrPnt:
            self.setControlPoints()
        Mi=list()
        for i in range(5):
            e=self.contrPnt[i][0]
            e.getResistingForce()
            Mz1=e.getMz1  #Z bending moment at the back end of the element
            Mz2=e.getMz2  #Z bending moment at the front end of the element
            MzCP=Mz1+(Mz2-Mz1)*self.contrPnt[i][1] # Z bending moment at the control point
            Mi.append(MzCP)
        return self.ec3Shape.getLateralBucklingReductionFactor(self.sectionClass,self.length,Mi,self.supportCoefs)

    def updateLateralBucklingReductionFactor(self):
        chiLT= self.getLateralBucklingReductionFactor()
        for e in self.elemSet:
             e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.


    def installULSControlRecorder(self,recorderType,sectionClass= 1, chiLT=1.0):
        '''Install recorder for verification of ULS criterion.'''
        preprocessor= self.line.getPreprocessor
        nodes= preprocessor.getNodeHandler
        domain= preprocessor.getDomain
        recorder= domain.newRecorder(recorderType,None)
        elems= self.getElements()
        eleTags= list()
        for e in elems:
            eleTags.append(e.tag)
            e.setProp('ULSControlRecorder',recorder)
        idEleTags= xc.ID(eleTags)
        recorder.setElements(idEleTags)
        self.ec3Shape.setupULSControlVars(elems,sectionClass,chiLT)
        if(nodes.numDOFs==3):
            recorder.callbackRecord= EC3lsc.controlULSCriterion2D()
        else:
            recorder.callbackRecord= EC3lsc.controlULSCriterion()

        recorder.callbackRestart= "print \"Restart method called.\""
        return recorder

