# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

'''Provide the user with a quick and easy way to 
   display results (internal forces, displacements) of an user-defined
   load case.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es    l.pereztato@ciccp.es"

import math
import xc_base
import geom
import xc
from misc_utils import log_messages as lmsg
from solution import predefined_solutions
from postprocess.xcVtk import vtk_graphic_base
from postprocess.xcVtk.FE_model import vtk_FE_graphic
from postprocess.xcVtk.fields import local_axes_vector_field as lavf
from postprocess import output_handler as oh
from postprocess import output_styles
from model import predefined_spaces

class QuickGraphics(oh.OutputHandler):
    '''This class is aimed at providing the user with a quick and easy way to 
    display results (internal forces, displacements).

    :ivar loadCaseName: OPTIONAL. name of the load case to be created
    :ivar loadCaseExpr: OPTIONAL. expression that defines de load case as a
                        combination of previously defined actions
                        e.g. '1.0*GselfWeight+1.0*GearthPress'
    '''
    def __init__(self, modelSpace= None, loadCaseName= '',loadCaseExpr= '', outputStyle= output_styles.OutputStyle()):
        ''' Constructor.'''
        super(QuickGraphics,self).__init__(modelSpace, outputStyle)
        self.loadCaseName=loadCaseName
        self.loadCaseExpr=loadCaseExpr

class LoadCaseResults(QuickGraphics):
    '''This class is aimed at providing the user with a quick and easy way to 
    display results (internal forces, displacements) of an user-defined
    load case.

    :ivar feProblem:   finite element problem
    :ivar loadCaseName:   name of the load case to be created
    :ivar loadCaseExpr:   expression that defines de load case as a
                     combination of previously defined actions
                     e.g. '1.0*GselfWeight+1.0*GearthPress'
    '''
    def __init__(self,feProblem,loadCaseName= '',loadCaseExpr= '', outputStyle= output_styles.OutputStyle()):
        modelSpace= predefined_spaces.getModelSpaceFromPreprocessor(feProblem.getPreprocessor)
        super(LoadCaseResults,self).__init__(modelSpace, loadCaseName, loadCaseExpr, outputStyle)
        self.feProblem= feProblem
        if(self.feProblem):
          self.prep= feProblem.getPreprocessor

    def solve(self):
        self.modelSpace.removeAllLoadPatternsFromDomain()
        self.modelSpace.addNewLoadCaseToDomain(self.loadCaseName,self.loadCaseExpr)
        #Solution
        lmsg.warning('Here we use a simple linear static solution that is not always a suitable method.')
        analysis= predefined_solutions.simple_static_linear(self.feProblem)
        result= analysis.analyze(1)
        #combs.removeFromDomain(self.loadCaseName)
        
    def checkSetToDisp(self,setToDisplay):
        if(setToDisplay):
            self.xcSet= setToDisplay
        else:
            lmsg.warning('Set to display not defined; using total set.')
            self.xcSet= self.prep.getSets.getSet('total')
        self.xcSet.fillDownwards()
        if self.xcSet.color.Norm()==0:
            self.xcSet.color=xc.Vector([rd.random(),rd.random(),rd.random()])

