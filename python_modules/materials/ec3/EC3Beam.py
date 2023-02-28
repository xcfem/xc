# -*- coding: utf-8 -*-
'''Eurocode 3 checks for a beam or column.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"


from materials.ec3 import EC3_limit_state_checking as EC3lsc
# from misc_utils import log_messages as lmsg

class EC3Beam(EC3lsc.Member):
    '''Steel beam defined by an arbitrary name, a cross-section shape, 
    its section class, the coefficients of supports and the type  
    
    '''
    def __init__(self, name, ec3Shape, beamSupportCoefs= EC3lsc.BeamSupportCoefficients(ky=1.0,kw=1.0,k1=1.0,k2=1.0), typo= 'rolled', lstLines=None, lstPoints=None):
        '''Constructor.

        :param name: object name.
        :param ec3Shape: cross-section shape (e.g. IPNShape, IPEShape, ...)
        :param beamSupportCoefs: instance of EC3_limit_state_checking.BeamSupportCoefficients
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
        super(EC3Beam,self).__init__(name= name, ec3Shape= ec3Shape, lstLines= lstLines, lstPoints= lstPoints, beamSupportCoefs= beamSupportCoefs, typo= typo)
            

