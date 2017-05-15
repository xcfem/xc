# -*- coding: utf-8 -*-

'''Providing the user with a quick and easy way to 
   display results (internal forces, displacements) of an user-defined
   load case.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es    l.pereztato@ciccp.es"

from miscUtils import LogMessages as lmsg
from solution import predefined_solutions
from xcVtk.malla_ef import vtk_grafico_ef
from xcVtk.malla_ef import Fields
from xcVtk import ControlVarDiagram as cvd
from xcVtk import LinearLoadDiagram as lld
from xcVtk import LoadVectorField as lvf
from xcVtk import NodePropertyDiagram as npd

class QuickGraphics(object):
  '''This class is aimed at providing the user with a quick and easy way to 
  display results (internal forces, displacements) of an user-defined
  load case.
  
  :ivar loadCaseName:   name of the load case to be created
  :ivar loadCaseExpr:   expression that defines de load case as a
                   combination of previously defined actions
                   e.g. '1.0*GselfWeight+1.0*GearthPress'
  '''
  def __init__(self,feProblem):
    self.feProblem= feProblem
    self.xcSet= self.feProblem.getPreprocessor.getSets.getSet('total')
    self.loadCaseName=''
    
  def solve(self,loadCaseName,loadCaseExpr=''):
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

  def getDisplay(self,vwName= 'XYZPos',hCamF=1.0):
    '''Returns a suitable display to show the graphics.
    
    :param viewName:     name of the view that contains the renderer (possible
                         options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                         "ZPos", "ZNeg") (defaults to "XYZPos")
    :param hCamFct:     factor that applies to the height of the camera position
                        in order to change perspective of isometric views 
                        (defaults to 1, usual values 0.1 to 10)
    '''
    defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
    defDisplay.viewName=vwName
    defDisplay.hCamFct=hCamF
    return defDisplay


  def displayDispRot(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,unitDescription= '',viewName='XYZPos',hCamFct=1.0,fileName=None,defFScale=0.0):
    '''displays the component of the displacement or rotations in the 
    set of entities.
    
    :param itemToDisp: component of the displacement ('uX', 'uY' or 'uZ') 
                  or the rotation ('rotX', rotY', 'rotZ') to be depicted 
    :param setToDisplay:   set of entities to be represented (defaults to all 
                  entities)
    :param fConvUnits: factor of conversion to be applied to the results 
                  (defaults to 1)
    :param unitDescription: string describing units like '[mm] or [cm]'
    :param fileName: name of the file to plot the graphic. Defaults to 
                  None, in that case an screen display is generated
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)

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
    defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
    defDisplay.displayMesh(xcSet=self.xcSet,field=field,diagrams= None, fName=fileName,caption=self.loadCaseName+' '+itemToDisp+' '+unitDescription+' '+self.xcSet.name,defFScale=defFScale)

  def displayIntForc(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,unitDescription= '',viewName='XYZPos',hCamFct=1.0,fileName=None,defFScale=0.0):
    '''displays the component of internal forces in the 
    set of entities as a scalar field (i.e. appropiated for shell elements).
    
    :param itemToDisp:   component of the internal forces ('N1', 'N2', 'N12', 
           'M1', 'M2', 'M12', 'Q1', 'Q2') to be depicted 
    :param setToDisplay: set of entities to be represented (default to all 
           entities)
    :param fConvUnits:   factor of conversion to be applied to the results 
           (defalts to 1)
    :param unitDescription: string like '[kN/m] or [kN m/m]'
    :param viewName:     name of the view  that contains the renderer (possible
           options: "XYZPos", "XPos", "XNeg","YPos", "YNeg", "ZPos", "ZNeg") 
           (defaults to "XYZPos")
    :param hCamFct:     factor that applies to the height of the camera position
           in order to change perspective of isometric views (defaults to 1, 
           usual values 0.1 to 10)
    :param fileName:    name of the file to plot the graphic. Defaults to None,
           in that case an screen display is generated
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
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
    defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
    field.display(defDisplay=defDisplay,fName=fileName,caption=self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.name,defFScale=defFScale)


  def displayIntForcDiag(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,scaleFactor=1.0,unitDescription= '',viewName='XYZPos',hCamFct=1.0,fileName=None,defFScale=0.0):
    '''displays the component of internal forces in the 
    set of entities as a diagram over lines (i.e. appropiated for beam elements).
    
    :param itemToDisp:   component of the internal forces ('N', 'Qy' (or 'Vy'), 'Qz' (or 'Vz'), 
                         'My', 'Mz', 'T') to be depicted 
    :param setToDisplay: set of entities (elements of type beam) to be represented
    :param fConvUnits:   factor of conversion to be applied to the results (defalts to 1)
    :param scaleFactor:  factor of scale to apply to the diagram display of
    :param unitDescription: string like '[kN/m] or [kN m/m]'
    :param viewName:     name of the view  that contains the renderer (possible
                         options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                         "ZPos", "ZNeg") (defaults to "XYZPos")
    :param hCamFct:     factor that applies to the height of the camera position
                        in order to change perspective of isometric views 
                        (defaults to 1, usual values 0.1 to 10)
    :param fileName:     name of the file to plot the graphic. Defaults to None,
                         in that case an screen display is generated
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
    '''
    if(setToDisplay):
      self.xcSet= setToDisplay
    else:
      lmsg.warning('QuickGraphics::displayIntForc; set to display not defined; using previously defined set (total if None).')
    diagram= cvd.ControlVarDiagram(scaleFactor= scaleFactor,fUnitConv= fConvUnits,sets=[self.xcSet],attributeName= "intForce",component= itemToDisp)
    diagram.agregaDiagrama()
    defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
    defDisplay.setupGrid(self.xcSet)
    defDisplay.defineEscenaMalla(None,defFScale)
    defDisplay.appendDiagram(diagram) #Append diagram to the scene.

    caption= self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.name
    defDisplay.displayScene(caption=caption,fName=fileName)

  def dispLoadCaseBeamEl(self,loadCaseName='',setToDisplay=None,fUnitConv=1.0,elLoadComp='transComponent',elLoadScaleF=1.0,nodLoadScaleF=1.0,viewName='XYZPos',hCamFct=1.0,caption='',fileName=None,defFScale=0.0):
    '''displays the loads applied on beam elements and nodes for a given load case
    :param setToDisplay:    set of beam elements to be represented
    :param fUnitConv:       factor of conversion to be applied to the results (defaults to 1)
    :param elLoadComp:      component of the loads on elements to be depicted
                            [possible components: 'axialComponent', 'transComponent', 'transYComponent',
                             'transZComponent']
    :param elLoadScaleF:    factor of scale to apply to the diagram display of element loads (defaults to 1)
    :param nodLoadScaleF:   factor of scale to apply to the vector display of nodal loads (defaults to 1)
    :param viewName:        name of the view  that contains the renderer (possible
                            options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                            "ZPos", "ZNeg") (defaults to "XYZPos")
    :param hCamFct:     factor that applies to the height of the camera position in order to
                        change perspective of isometric views (defaults to 1). Usual values 0.1 to 10
    :param caption:         caption for the graphic
    :param fileName:        name of the file to plot the graphic. Defaults to None,
                            in that case an screen display is generated
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
    '''
    preprocessor= self.feProblem.getPreprocessor
    loadPatterns= preprocessor.getLoadLoader.getLoadPatterns
    loadPatterns.addToDomain(loadCaseName)
    defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
    grid= defDisplay.setupGrid(self.xcSet)
    print 'aqui 0'
    defDisplay.defineEscenaMalla(None,defFScale)
    print 'aqui 1'
    orNodalLBar='H'  #default orientation of scale bar for nodal loads
    # element loads
    print 'scale=',elLoadScaleF,'fUnitConv=',fUnitConv,'loadPatternName=',loadCaseName,'component=',elLoadComp
    diagram= lld.LinearLoadDiagram(scale=elLoadScaleF,fUnitConv=fUnitConv,loadPatternName=loadCaseName,component=elLoadComp)
    print 'aqui 2'
    diagram.agregaDiagrama(preprocessor)
    print 'aqui 3'
    if (diagram.valMax > -1e+99) or (diagram.valMin<1e+99):
      defDisplay.appendDiagram(diagram)
      orNodalLBar='V'
    # nodal loads
    print 'aqui 4'
    vField=lvf.LoadVectorField(loadPatternName=loadCaseName,fUnitConv=fUnitConv,scaleFactor=nodLoadScaleF,showPushing= True)
    print 'aqui 5'
#    loadPatterns= preprocessor.getLoadLoader.getLoadPatterns
    count=vField.dumpNodalLoads(preprocessor,lp=loadPatterns[loadCaseName],defFScale=defFScale)
    print 'aqui 1'
    if count >0:
      vField.setupActor()
      defDisplay.renderer.AddActor(vField.actor)
      vField.creaColorScaleBar(orientation=orNodalLBar)
      defDisplay.renderer.AddActor2D(vField.scalarBar)
    print 'aqui 2'
    defDisplay.displayScene(caption=caption,fName=fileName)

  def displayNodeValueDiagram(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,scaleFactor=1.0,unitDescription= '',viewName='XYZPos',hCamFct=1.0,fileName=None,defFScale=0.0):
    '''displays the a displacement (uX,uY,...) or a property defined in nodes 
    as a diagram over lines (i.e. appropiated for beam elements).
    
    :param itemToDisp:   item to display.
    :param setToDisplay: set of entities (elements of type beam) to be represented
    :param fConvUnits:   factor of conversion to be applied to the results (defalts to 1)
    :param scaleFactor:  factor of scale to apply to the diagram display of
    :param unitDescription: string like '[m]' or '[rad]' or '[m/s2]'
    :param viewName:     name of the view  that contains the renderer (possible
                         options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                         "ZPos", "ZNeg") (defaults to "XYZPos")
    :param hCamFct:     factor that applies to the height of the camera position
                        in order to change perspective of isometric views 
                        (defaults to 1, usual values 0.1 to 10)
    :param fileName:     name of the file to plot the graphic. Defaults to None,
                         in that case an screen display is generated
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
     '''
    if(setToDisplay):
      self.xcSet= setToDisplay
    else:
      lmsg.warning('QuickGraphics::displayNodeValueDiagram; set to display not defined; using previously defined set (total if None).')
    diagram= npd.NodePropertyDiagram(scaleFactor= scaleFactor,fUnitConv= fConvUnits,sets=[self.xcSet],attributeName= itemToDisp)
    diagram.agregaDiagrama()
    defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
    defDisplay.setupGrid(self.xcSet)
    defDisplay.defineEscenaMalla(None,defFScale)
    defDisplay.appendDiagram(diagram) #Append diagram to the scene.

    caption= self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.name
    defDisplay.displayScene(caption=caption,fName=fileName)
 
