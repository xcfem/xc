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
from materials import buckling_base
from materials.ec3 import EC3_limit_state_checking as EC3lsc
from misc_utils import log_messages as lmsg

class EC3Beam(buckling_base.MemberBase):
    '''Steel beam defined by an arbitrary name, a cross-section shape, 
    its section class, the coefficients of supports and the type  
    
    :ivar sectionClass: section class (1 to 3, 4 not yet implemented) 
          (defaults to 1).
    :ivar supportCoefs: instance of EC3_limit_state_checking.SupportCoefficients
          that wraps the support coefficients: ky, kw, k1 and k2. where ky is 
          the lateral bending coefficient, kw the warping coefficient,  k1 and            the warping AND lateral bending coefficients at first and last ends 
          respectively (1.0 => free,  0.5 => prevented). 
          (Defaults to ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0)
    :ivar typo: 'rolled' or 'welded' (defaults to rolled)
    '''
    def __init__(self, name, ec3Shape,sectionClass=1,supportCoefs=EC3lsc.SupportCoefficients(ky=1.0,kw=1.0,k1=1.0,k2=1.0), typo= 'rolled', lstLines=None, lstPoints=None):
        '''Constructor.

        :param name: object name.
        :param ec3Shape: cross-section shape (e.g. IPNShape, IPEShape, ...)
        :param sectionClass: section class (1 to 3, 4 not yet implemented) 
                             (defaults to 1).
        :param supportCoefs: instance of EC3_limit_state_checking.SupportCoefficients
                             that wraps the support coefficients: ky, kw, k1 
                             and k2; where ky is the lateral bending 
                             coefficient, kw the warping coefficient,  k1 and 
                             the warping AND lateral bending coefficients at first
                             and last ends respectively (1.0 => free, 0.5 => prevented). 
                             (Defaults to ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0)
        :param typo: 'rolled' or 'welded' (defaults to rolled)
        :param lstLines: ordered list of lines that make up the beam 
                        (defaults to None).
        :param lstPoints: ordered list of points that make up the beam. 
                          Ignored if lstLines is given (defaults to None)
        '''
        super(EC3Beam,self).__init__(name, ec3Shape, lstLines, lstPoints)
        self.sectionClass= sectionClass
        self.supportCoefs= supportCoefs
        self.typo=typo
            
    def getLateralBucklingReductionFactor(self):
        ''' Return lateral torsional buckling reduction factor value
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
        return self.shape.getLateralBucklingReductionFactor(self.sectionClass,self.length,Mi,self.supportCoefs)

    def updateLateralBucklingReductionFactor(self):
        '''Update the value of the lateral buckling reduction factor.'''
        chiLT= self.getLateralBucklingReductionFactor()
        for e in self.elemSet:
             e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.

    def installULSControlRecorder(self,recorderType, chiLT=1.0):
        '''Install recorder for verification of ULS criterion.'''
        prep= self.getPreprocessor()
        nodes= prep.getNodeHandler
        domain= prep.getDomain
        recorder= domain.newRecorder(recorderType,None)
        if(not self.elemSet):
            self.createElementSet()
        eleTags= list()
        for e in self.elemSet:
            eleTags.append(e.tag)
            e.setProp('ULSControlRecorder',recorder)
        idEleTags= xc.ID(eleTags)
        recorder.setElements(idEleTags)
        self.shape.setupULSControlVars(self.elemSet,self.sectionClass,chiLT)
        if(nodes.numDOFs==3):
            recorder.callbackRecord= EC3lsc.controlULSCriterion2D()
        else:
            recorder.callbackRecord= EC3lsc.controlULSCriterion()
#        recorder.callbackRestart= "print \"Restart method called.\"" #20181121
        return recorder

