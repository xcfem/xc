# -*- coding: utf-8 -*-

'''Providing the user with a quick and easy way to 
   display results (internal forces, displacements) of an user-defined
   load case.'''

__author__= "Ana I. Ortega Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 AOO LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega.Ort@gmail.com l.pereztato@gmail.com"

from miscUtils import LogMessages as lmsg
from solution import predefined_solutions
from xcVtk.malla_ef import vtk_grafico_ef
from xcVtk.malla_ef import Fields

class QuickGraphics(object):
  '''This class is aimed at providing the user with a quick and easy way to 
  display results (internal forces, displacements) of an user-defined
  load case.
  Attributes:
   loadCaseName:   name of the load case to be created
   loadCaseExpr:   expression that defines de load case as a
                   combination of previously defined actions
                   e.g. '1.0*GselfWeight+1.0*GearthPress'
  '''
  def __init__(self,feProblem):
    self.feProblem= feProblem
    self.xcSet= self.feProblem.getPreprocessor.getSets.getSet('total')

  def solve(self,loadCaseName='',loadCaseExpr=''):
    self.loadCaseName=loadCaseName
    self.loadCaseExpr=loadCaseExpr
    preprocessor= self.feProblem.getPreprocessor
    combs=preprocessor.getLoadLoader.getLoadCombinations
    lCase=combs.newLoadCombination(self.loadCaseName,self.loadCaseExpr)
    preprocessor.resetLoadCase()
    combs.addToDomain(self.loadCaseName)
    #Solution
    analysis= predefined_solutions.simple_static_linear(self.feProblem)
    result= analysis.analyze(1)
    combs.removeFromDomain(self.loadCaseName)

  def getDispComponentFromName(self,compName):
    if compName == 'uX':
      return 0
    elif compName == 'uY':
      return 1
    elif compName == 'uZ':
      return 2
    elif compName == 'rotX':
      return 3
    elif compName == 'rotY':
      return 4
    elif compName == 'rotZ':
      return 5
    else:
      lmsg.error('Item '+str(compName) +'is not a valid component. Displayable items are: uX, uY, uZ, rotX, rotY, rotZ')
      return 2

  def getIntForceComponentFromName(self,componentName):
    if componentName[0] in ['N','M']:
      return componentName.lower()
    elif componentName == 'Q1':
      return 'q13'
    elif componentName == 'Q2':
      return 'q23'
    else: #LCPT I don't like this too much, I prefer let the user make the program to crass. Maybe a Warning? 
      lmsg.error('Item '+str(componentName) +'is not a valid component. Displayable items are: N1, N2, N12, M1, M2, M12, Q1, Q2')
      return 'N1'


  def displayDispRot(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,unitDescription= ''):
    '''displays the component of the displacement or rotations in the 
    set of entities.
    Parameters:
      itemToDisp: component of the displacement ('uX', 'uY' or 'uZ') or the 
                  rotation ('rotX', rotY', 'rotZ') to be depicted 
      setToDisplay:   set of entities to be represented (default to all entities)
      fConvUnits:     factor of conversion to be applied to the results (defalts to 1)
      unitDescription: string describing units like '[mm] or [cm]'
    '''
    if(setToDisplay):
      self.xcSet= setToDisplay
    else:
      lmsg.warning('QuickGraphics::displayDispRot; set to display not defined; using previously defined set (total if None).')
    vCompDisp= self.getDispComponentFromName(itemToDisp)
    nodSet= self.xcSet.getNodes
    for n in nodSet:
      n.setProp('propToDisp',n.getDisp[vCompDisp])
    field= Fields.ScalarField('propToDisp',"getProp",None,fConvUnits)
    defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
    defDisplay.displayMesh(self.xcSet,field,caption=self.loadCaseName+' '+itemToDisp+' '+unitDescription+' '+self.xcSet.name)

  def displayIntForc(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,unitDescription= ''):
    '''displays the component of internal forces in the 
    set of entities.
    Parameters:
      itemToDisp: component of the internal forces ('N1', 'N2', 'N12', 'M1', 'M2', 'M12', 'Q1', 'Q2')
                  to be depicted 
      setToDisplay:   set of entities to be represented (default to all entities)
      fConvUnits:     factor of conversion to be applied to the results (defalts to 1)
      unitDescription: string like '[kN/m] or [kN m/m]'
    '''
    if(setToDisplay):
      self.xcSet= setToDisplay
    else:
      lmsg.warning('QuickGraphics::displayIntForc; set to display not defined; using previously defined set (total if None).')
    vCompDisp= self.getIntForceComponentFromName(itemToDisp)
    elSet= self.xcSet.getElements
    propName= 'propToDisp_'+str(itemToDisp)
    for e in elSet:
      e.getResistingForce()
      mat= e.getPhysicalProperties.getVectorMaterials
      e.setProp(propName,mat.getMeanGeneralizedStressByName(vCompDisp))
    field= Fields.ExtrapolatedProperty(propName,"getProp",self.xcSet,fUnitConv= fConvUnits)
    defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
    field.display(defDisplay,caption=self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.name)
