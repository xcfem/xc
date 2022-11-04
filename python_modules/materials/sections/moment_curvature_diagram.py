# -*- coding: utf-8 -*-
''' Moment curvature diagram for a section. '''

import sys
import xc
from solution import predefined_solutions
from model import predefined_spaces
from misc_utils import log_messages as lmsg

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

def getMomentCurvatureDiagram3D(preprocessor, nmbSecc, esfAxil, maxK, numIncr):
    ''' Return the points of the moment curvature diagram of the section.

    :param nmbSecc: Section name.
    :param esfAxil: Axial force over the section.
    :param maxK: Maximum curvature to reach in the analysis.
    :param numIncr: Number of increments.
    '''
    nodes= preprocessor.getNodeHandler

    modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
    nod1= nodes.newNodeIDXYZ(1001,1,0,0)

    nod2= nodes.newNodeIDXYZ(1002,1,0,0)
    elementos= preprocessor.getElementHandler
    elementos.defaultMaterial= nmbSecc
    elementos.defaultTag= 2001 #Tag for the next element.
    zls= elementos.newElement("ZeroLengthSection",xc.ID([nod1.tag,nod2.tag]))
    if(__debug__):
        if(zls is None):
            AssertionError('Can\'t create the element.')

    modelSpace.fixNode000_000(nod1.tag)
    modelSpace.constraints.newSPConstraint(nod2.tag,1,0.0)
    modelSpace.constraints.newSPConstraint(nod2.tag,2,0.0)

    cargas= preprocessor.getLoadHandler
    casos= cargas.getLoadPatterns
    #Load modulation.
    ts= casos.newTimeSeries("constant_ts","ts")
    casos.currentTimeSeries= ts.name
    lp0= casos.newLoadPattern("default","0")
    lp0.newNodalLoad(nod2.tag,xc.Vector([esfAxil,0,0,0,0,0]))
    #We add the load case to domain.
    casos.addToDomain("0")

    prb= modelSpace.getProblem()
    analysis= predefined_solutions.plain_newton_raphson(prb)
    analOk= analysis.analyze(1)
    if(analOk!=0):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; can\'t solve for load case: '+str(lp0.name)+'.')
        exit(-1)

    lp1= casos.newLoadPattern("default","1")
    lp1.newNodalLoad(nod2.tag,xc.Vector([0,0,0,0,0,0,1]))

    # Compute curvature increment.
    # dK= maxK/numIncr
    soluMethods= prb.getSolProc.getSolutionStrategyContainer
    solutionStrategy= soluMethods.getSolutionStrategy("solutionStrategy")
    integ= solutionStrategy.newIntegrator("displacement_control_integrator")
    if(__debug__):
        if(integ is None):
            AssertionError('Can\'t create the integrator.')
    
