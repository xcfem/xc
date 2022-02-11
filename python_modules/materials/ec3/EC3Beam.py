# -*- coding: utf-8 -*-
from __future__ import print_function


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
from materials import steel_member_base
from materials.ec3 import EC3_limit_state_checking as EC3lsc
from misc_utils import log_messages as lmsg

class EC3Beam(steel_member_base.BucklingMember):
    '''Steel beam defined by an arbitrary name, a cross-section shape, 
    its section class, the coefficients of supports and the type  
    
    :ivar sectionClass: section class (1 to 3, 4 not yet implemented) 
          (defaults to 1).
    :ivar supportCoefs: instance of EC3_limit_state_checking.SupportCoefficients
          that wraps the support coefficients: ky, kw, k1 and k2. where ky is 
          the lateral bending coefficient, kw the warping coefficient,  k1 and
          the warping AND lateral bending coefficients at first and last ends 
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
        Mi= self.getBendingMomentsAtControlPoints()
        return self.shape.getLateralBucklingReductionFactor(self.sectionClass,self.length,Mi,self.supportCoefs)

    def updateLateralBucklingReductionFactor(self):
        '''Update the value of the lateral buckling reduction factor.'''
        chiLT= self.getLateralBucklingReductionFactor()
        for e in self.elemSet:
             e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.
    def updateReductionFactors(self):
        '''Update the value of the appropriate reduction factors.'''
        self.updateLateralBucklingReductionFactor()

    def installULSControlRecorder(self,recorderType, chiLT=1.0, calcSet= None):
        '''Install recorder for verification of ULS criterion.

        :param recorderType: type of the recorder to install.
        :param calcSet: set of elements to be checked (defaults to 'None' which 
                        means that this set will be created elsewhere). In not
                        'None' the member elements will be appended to this set.
        '''
        recorder= self.createRecorder(recorderType, calcSet)
        self.shape.setupULSControlVars(self.elemSet,self.sectionClass,chiLT)
        nodHndlr= self.getPreprocessor().getNodeHandler        
        if(nodHndlr.numDOFs==3):
            recorder.callbackRecord= EC3lsc.controlULSCriterion2D()
        else:
            recorder.callbackRecord= EC3lsc.controlULSCriterion()
#        recorder.callbackRestart= "print(\"Restart method called.\")" #20181121
        return recorder

