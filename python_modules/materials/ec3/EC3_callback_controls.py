# -*- coding: utf-8 -*-

'''EC3_callback_controls.py: routines to install in recorder  to execute in every commit to check
eurocode 3 criterions.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def controlBiaxialBendingEfficiency():
  '''Code to execute in every commit to check stress criterion (bars in 3D problems).'''
  return '''nmbComb= recorder.getNombreCombActual
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkBiaxialBendingForElement(self,nmbComb)'''

def controlYShearEfficiency():
  return '''nmbComb= recorder.getNombreCombActual
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkYShearForElement(self,nmbComb)'''

def controlULSCriterion():
  return '''nmbComb= recorder.getNombreCombActual
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkBiaxialBendingForElement(self,nmbComb)
crossSection.checkYShearForElement(self,nmbComb)'''

