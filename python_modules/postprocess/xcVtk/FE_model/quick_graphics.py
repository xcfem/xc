# -*- coding: utf-8 -*-
from __future__ import division

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
from postprocess.xcVtk.FE_model import vtk_FE_graphic
from postprocess.xcVtk.FE_model import Fields
from postprocess.xcVtk import control_var_diagram as cvd
from postprocess.xcVtk import linear_load_diagram as lld
from postprocess.xcVtk import load_vector_field as lvf
from postprocess.xcVtk import node_property_diagram as npd
from postprocess.xcVtk import local_axes_vector_field as lavf
from postprocess.xcVtk import vector_field as vf
import xc
import random as rd 

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
        combs=preprocessor.getLoadHandler.getLoadCombinations
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

      :param viewName: name of the view that contains the renderer (possible
                           options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                           "ZPos", "ZNeg") (defaults to "XYZPos")
      :param hCamFct: factor that applies to the height of the camera position
                          in order to change perspective of isometric views 
                          (defaults to 1, usual values 0.1 to 10)
      '''
      defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
      defDisplay.viewName=vwName
      defDisplay.hCamFct=hCamF
      return defDisplay


    def displayDispRot(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,unitDescription= '',viewName='XYZPos',hCamFct=1.0,fileName=None,defFScale=0.0,rgMinMax=None):
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
      :param rgMinMax: range (vmin,vmax) with the maximum and minimum values of 
              the field to be represented. All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)

      '''
      if(setToDisplay):
          self.xcSet= setToDisplay
      else:
          lmsg.warning('QuickGraphics::displayDispRot; set to display not defined; using previously defined set (total if None).')
      vCompDisp= self.getDispComponentFromName(itemToDisp)
      nodSet= self.xcSet.getNodes
      for n in nodSet:
          n.setProp('propToDisp',n.getDisp[vCompDisp])
      field= Fields.ScalarField(name='propToDisp',functionName="getProp",component=None,fUnitConv=fConvUnits,rgMinMax=rgMinMax)
      defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
      defDisplay.displayMesh(xcSets=self.xcSet,field=field,diagrams= None, fName=fileName,caption=self.loadCaseName+' '+itemToDisp+' '+unitDescription+' '+self.xcSet.description,defFScale=defFScale)

    def displayIntForc(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,unitDescription= '',viewName='XYZPos',hCamFct=1.0,fileName=None,defFScale=0.0):
      '''displays the component of internal forces in the 
      set of entities as a scalar field (i.e. appropiated for 2D elements; 
      shells...).

      :param itemToDisp:   component of the internal forces ('N1', 'N2', 'N12', 
             'M1', 'M2', 'M12', 'Q1', 'Q2') to be depicted 
      :param setToDisplay: set of entities to be represented (default to all 
             entities)
      :param fConvUnits:   factor of conversion to be applied to the results 
             (defalts to 1)
      :param unitDescription: string like '[kN/m] or [kN m/m]'
      :param viewName:  name of the view  that contains the renderer (possible
             options: "XYZPos", "XPos", "XNeg","YPos", "YNeg", "ZPos", "ZNeg") 
             (defaults to "XYZPos")
      :param hCamFct:  factor that applies to the height of the camera position
             in order to change perspective of isometric views (defaults to 1, 
             usual values 0.1 to 10)
      :param fileName: name of the file to plot the graphic. Defaults to None,
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
      elSet= self.xcSet.getElements.pickElemsOfDimension(2)
      if(len(elSet)>0):
          propName= 'propToDisp_'+str(itemToDisp)
          for e in elSet:
              if(e.getDimension==2):
                  e.getResistingForce()
                  mat= e.getPhysicalProperties.getVectorMaterials
                  e.setProp(propName,mat.getMeanGeneralizedStressByName(vCompDisp))
              else:
                lmsg.warning('QuickGraphics::displayIntForc; not a 2D element; ignored.')
          field= Fields.ExtrapolatedProperty(propName,"getProp",self.xcSet,fUnitConv= fConvUnits)
          defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
          field.display(defDisplay=defDisplay,fName=fileName,caption=self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.description,defFScale=defFScale)


    def displayIntForcDiag(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,scaleFactor=1.0,unitDescription= '',viewName='XYZPos',hCamFct=1.0,fileName=None,defFScale=0.0):
      '''displays the component of internal forces in the set of entities as a 
       diagram over lines (i.e. appropiated for beam elements).

      :param itemToDisp: component of the internal forces 
          ('N', 'Qy' (or 'Vy'), 'Qz' (or 'Vz'), 'My', 'Mz', 'T') to be depicted 
      :param setToDisplay: set of entities (elements of type beam) to be 
          represented
      :param fConvUnits:   factor of conversion to be applied to the results 
           (defaults to 1)
      :param scaleFactor:  factor of scale to apply to the diagram display of
      :param unitDescription: string like '[kN/m] or [kN m/m]'
      :param viewName:  name of the view  that contains the renderer (possible
                        options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                        "ZPos", "ZNeg") (defaults to "XYZPos")
      :param hCamFct:  factor that applies to the height of the camera position
                          in order to change perspective of isometric views 
                          (defaults to 1, usual values 0.1 to 10)
      :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
      :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
      '''
      if(setToDisplay):
          self.xcSet= setToDisplay
          if self.xcSet.color.Norm()==0:
              self.xcSet.color=xc.Vector([rd.random(),rd.random(),rd.random()])
      else:
          lmsg.warning('QuickGraphics::displayIntForc; set to display not defined; using previously defined set (total if None).')
      diagram= cvd.ControlVarDiagram(scaleFactor= scaleFactor,fUnitConv= fConvUnits,sets=[self.xcSet],attributeName= "intForce",component= itemToDisp)
      diagram.addDiagram()
      defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
      defDisplay.setupGrid(self.xcSet)
      defDisplay.defineMeshScene(None,defFScale,color=self.xcSet.color)
      defDisplay.appendDiagram(diagram) #Append diagram to the scene.

      caption= self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.description
      defDisplay.displayScene(caption=caption,fName=fileName)

    def dispLoadCaseBeamEl(self,loadCaseName='',setToDisplay=None,fUnitConv=1.0,elLoadComp='transComponent',elLoadScaleF=1.0,nodLoadScaleF=1.0,viewName='XYZPos',hCamFct=1.0,caption='',fileName=None,defFScale=0.0):
      '''Display the loads applied on beam elements and nodes for a given load case

      :param setToDisplay:    set of beam elements to be represented
      :param fUnitConv:  factor of conversion to be applied to the results
                      (defaults to 1)
      :param elLoadComp:  component of the loads on elements to be depicted
                   [possible components: 'axialComponent', 'transComponent', 
                    'transYComponent', 'transZComponent']
      :param elLoadScaleF:    factor of scale to apply to the diagram display 
                    of element loads (defaults to 1)
      :param nodLoadScaleF: factor of scale to apply to the vector display of 
                    nodal loads (defaults to 1)
      :param viewName: name of the view  that contains the renderer (possible
                              options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                              "ZPos", "ZNeg") (defaults to "XYZPos")
      :param hCamFct: factor that applies to the height of the camera position 
            in order to change perspective of isometric views (defaults to 1).
                           Usual values 0.1 to 10
      :param caption:   caption for the graphic
      :param fileName:  name of the file to plot the graphic. Defaults to None,
                        in that case an screen display is generated
      :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
      '''
      if(setToDisplay):
          self.xcSet= setToDisplay
          if self.xcSet.color.Norm()==0:
              self.xcSet.color=xc.Vector([rd.random(),rd.random(),rd.random()])
      else:
          lmsg.warning('QuickGraphics::displayLoadCaseBeamEl; set to display not defined; using previously defined set (total if None).')
      print 'xcSet', self.xcSet.name
      preprocessor= self.feProblem.getPreprocessor
      loadPatterns= preprocessor.getLoadHandler.getLoadPatterns
      loadPatterns.addToDomain(loadCaseName)
      defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
      grid= defDisplay.setupGrid(self.xcSet)
      defDisplay.defineMeshScene(None,defFScale,color=self.xcSet.color)
      orNodalLBar='H'  #default orientation of scale bar for nodal loads
      # element loads
#      print 'scale=',elLoadScaleF,'fUnitConv=',fUnitConv,'loadPatternName=',loadCaseName,'component=',elLoadComp
      diagram= lld.LinearLoadDiagram(scale=elLoadScaleF,fUnitConv=fUnitConv,loadPatternName=loadCaseName,component=elLoadComp)
      diagram.addDiagram(preprocessor)
      if (diagram.valMax > -1e+99) or (diagram.valMin<1e+99):
          defDisplay.appendDiagram(diagram)
          orNodalLBar='V'
      # nodal loads
      vField=lvf.LoadVectorField(loadPatternName=loadCaseName,fUnitConv=fUnitConv,scaleFactor=nodLoadScaleF,showPushing= True)
  #    loadPatterns= preprocessor.getLoadHandler.getLoadPatterns
      lPattern= loadPatterns[loadCaseName]
      count= 0
      if(lPattern):
          count=vField.dumpNodalLoads(preprocessor,lp=loadPatterns[loadCaseName],defFScale=defFScale)
      else:
          lmsg.error('load pattern: '+ loadCaseName + ' not found.')
      if count >0:
          vField.setupActor()
          defDisplay.renderer.AddActor(vField.actor)
          vField.creaColorScaleBar(orientation=orNodalLBar)
          defDisplay.renderer.AddActor2D(vField.scalarBar)
      defDisplay.displayScene(caption=caption,fName=fileName)

    def displayNodeValueDiagram(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,scaleFactor=1.0,unitDescription= '',viewName='XYZPos',hCamFct=1.0,fileName=None,defFScale=0.0):
      '''displays the a displacement (uX,uY,...) or a property defined in nodes 
      as a diagram over lines (i.e. appropiated for beam elements).

      :param itemToDisp:   item to display.
      :param setToDisplay: set of entities (elements of type beam) to be 
             represented
      :param fConvUnits:   factor of conversion to be applied to the results 
             (defalts to 1)
      :param scaleFactor:  factor of scale to apply to the diagram display of
      :param unitDescription: string like '[m]' or '[rad]' or '[m/s2]'
      :param viewName: name of the view  that contains the renderer (possible
                       options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                       "ZPos", "ZNeg") (defaults to "XYZPos")
      :param hCamFct:  factor that applies to the height of the camera position
                          in order to change perspective of isometric views 
                          (defaults to 1, usual values 0.1 to 10)
      :param fileName: name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
      :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
       '''
      if(setToDisplay):
          self.xcSet= setToDisplay
          if self.xcSet.color.Norm()==0:
              self.xcSet.color=xc.Vector([rd.random(),rd.random(),rd.random()])
      else:
          lmsg.warning('QuickGraphics::displayNodeValueDiagram; set to display not defined; using previously defined set (total if None).')
      diagram= npd.NodePropertyDiagram(scaleFactor= scaleFactor,fUnitConv= fConvUnits,sets=[self.xcSet],attributeName= itemToDisp)
      diagram.addDiagram()
      defDisplay= self.getDisplay(vwName=viewName,hCamF= hCamFct)
      defDisplay.setupGrid(self.xcSet)
      defDisplay.defineMeshScene(None,defFScale,color=self.xcSet.color)
      defDisplay.appendDiagram(diagram) #Append diagram to the scene.

      caption= self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.description
      defDisplay.displayScene(caption=caption,fName=fileName)

def displayAxes(vectorField, preprocessor, setToDisplay=None,vectorScale=1.0,viewNm="XYZPos",hCamFct=1.0,caption= '',fileName=None,defFScale=0.0):
    '''vector field display of the loads applied to the chosen set of elements in the load case passed as parameter
    
    :param vectorField: function that generates the vector field.
    :param setToDisplay: set of elements to be displayed (defaults to total set)
    :param vectorScale: factor to apply to the vectors length in the 
           representation.
    :param viewNm: name of the view  that contains the renderer 
           (possible options: 
           "XYZPos", "XPos", "XNeg","YPos", "YNeg", "ZPos", "ZNeg")
    :param hCamFct: factor that applies to the height of the camera position 
           in order to change perspective of isometric views (defaults to 1). 
           Usual values 0.1 to 10
    :param fileName: full name of the graphic file to generate. Defaults to 
           `None`, in this case it returns a console output graphic.
    :param caption: text to display in the graphic 
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
    '''
    defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
    defDisplay.setupGrid(setToDisplay)
    defDisplay.viewName= viewNm
    defDisplay.hCamFct=hCamFct
    defDisplay.defineMeshScene(None,defFScale) 
    vectorField.addToDisplay(defDisplay)
    defDisplay.displayScene(caption,fileName)
    return defDisplay
    
def displayLocalAxes(prep,setToDisplay=None,vectorScale=1.0,viewNm="XYZPos",hCamFct=1.0,caption= '',fileName=None,defFScale=0.0):
    '''vector field display of the loads applied to the chosen set of elements in the load case passed as parameter
    
    :param setToDisplay: set of elements to be displayed (defaults to total set)
    :param vectorScale: factor to apply to the vectors length in the 
           representation.
    :param viewNm: name of the view  that contains the renderer 
           (possible options: 
            "XYZPos", "XPos", "XNeg","YPos", "YNeg", "ZPos", "ZNeg")
    :param hCamFct: factor that applies to the height of the camera position 
           in order to change perspective of isometric views (defaults to 1). 
           Usual values 0.1 to 10
    :param fileName: full name of the graphic file to generate. 
           Defaults to `None`, in this case it returns a console output graphic.
    :param caption: text to display in the graphic 
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
    '''
    if(setToDisplay == None):
        setToDisplay=preprocessor.getSets.getSet('total')
        setToDisplay.fillDownwards()
        lmsg.warning('set to display not defined; using total set.')
    vField=lavf.LocalAxesVectorField(setToDisplay.name+'_localAxes',vectorScale)
    vField.dumpVectors(setToDisplay)
    return displayAxes(vField, prep,setToDisplay,vectorScale,viewNm,hCamFct,caption,fileName,defFScale)


def displayStrongWeakAxis(preprocessor,setToDisplay=None,vectorScale=1.0,viewNm="XYZPos",hCamFct=1.0,caption= '',fileName=None,defFScale=0.0):
    '''vector field display of the loads applied to the chosen set of elements 
       in the load case passed as parameter
    
    :param setToDisplay: set of elements to be displayed (defaults to total set)
    :param vectorScale: factor to apply to the vectors length in the 
           representation.
    :param viewNm: name of the view  that contains the renderer 
            (possible options: 
             "XYZPos", "XPos", "XNeg","YPos", "YNeg", "ZPos", "ZNeg")
    :param hCamFct: factor that applies to the height of the camera position 
           in order to change perspective of isometric views (defaults to 1). 
           Usual values 0.1 to 10
    :param fileName: full name of the graphic file to generate. Defaults to 
           `None`, in this case it returns a console output graphic.
    :param caption: text to display in the graphic 
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
    '''
    if(setToDisplay == None):
        setToDisplay=preprocessor.getSets.getSet('total')
        setToDisplay.fillDownwards()
        lmsg.warning('set to display not defined; using total set.')
    vField=lavf.StrongWeakAxisVectorField(setToDisplay.name+'_strongWeakAxis',vectorScale)
    vField.dumpVectors(setToDisplay)
    return displayAxes(vField,preprocessor,setToDisplay,vectorScale,viewNm,hCamFct,caption,fileName,defFScale)


def displayLoad(preprocessor,setToDisplay=None,loadCaseNm='',unitsScale=1.0,vectorScale=1.0,multByElemArea=False,viewNm="XYZPos",hCamFct=1.0,caption= '',fileName=None,defFScale=0.0):
    '''vector field display of the loads applied to the chosen set of elements in the load case passed as parameter
    
    :param setToDisplay: set of elements to be displayed (defaults to total set)
    :param loadCaseNm: name of the load case to be depicted
    :param unitsScale: factor to apply to the results if we want to change the 
           unit system.
    :param vectorScale: factor to apply to the vectors length in the 
           representation
    :param multByElemArea: boolean value that must be `True` if we want to 
           represent the total load on each element (=load multiplied by 
           element area) and `False` if we are going to depict the value of
            the uniform load per unit area
    :param viewNm:         name of the view  that contains the renderer 
    :param hCamFct: factor that applies to the height of the camera position 
    :param fileName: full name of the graphic file to generate. Defaults to 
           ` None`, in this case it returns a console output graphic.
    :param caption: text to display in the graphic 
    :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
    '''
    if(setToDisplay == None):
        setToDisplay=preprocessor.getSets.getSet('total')
        setToDisplay.fillDownwards()
        lmsg.warning('set to display not defined; using total set.')

    LrefModSize=setToDisplay.getBnd(1.0).diagonal.getModulo() #representative length of set size (to autoscale)
    vectorScale*=LrefModSize/10.
    if setToDisplay.color.Norm()==0:
        setToDisplay.color=xc.Vector([rd.random(),rd.random(),rd.random()])
    defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
    defDisplay.setupGrid(setToDisplay)
    vField=lvf.LoadVectorField(loadCaseNm,unitsScale,vectorScale)
    vField.multiplyByElementArea=multByElemArea
    vField.dumpLoads(preprocessor,defFScale)
    defDisplay.viewName= viewNm
    defDisplay.hCamFct=hCamFct
    defDisplay.defineMeshScene(None,defFScale,color=setToDisplay.color) 
    vField.addToDisplay(defDisplay)
    defDisplay.displayScene(caption,fileName)
    return defDisplay

def displayEigenResults(preprocessor,eigenMode, setToDisplay=None,defShapeScale=0.0,equLoadVctScale=None,accelMode=None, unitsScale=1.0,viewNm="XYZPos",hCamFct=1.0,caption= '',fileName=None):
    '''Display the deformed shape and/or the equivalent static forces 
    associated with the eigenvibration mode passed as parameter.
    
    :param eigenMode: eigenvibration mode to be displayed
    :param setToDisplay: set of elements to be displayed (defaults to total set)
    :param defShapeScale: factor to apply to deformed shape so that the 
           displayed position of each node equals to the initial position 
           plus its eigenVector multiplied by this factor. (Defaults to 0.0, 
           i.e. display of initial/undeformed shape)
    :param equLoadVctScale: factor to apply to the vectors length in the 
            representation of equivalent loads on nodes. If vectorEqLoadScale
            equals 0 or None, equivalent static loads are not represented.
            (defaults to  None, in which case only deformed shape is depicted)
    :param accelMode: acceleration associated with the eigen mode depicted, 
            only used if the equivalent static loads are to be displayed.
    :param unitsScale: factor to apply to the results if we want to change the 
           unit system.
    :param viewNm: name of the view  that contains the renderer 
               (possible options: 
               `XYZPos`, `XYZNeg`,`XPos`, `XNeg`,`YPos`, `YNeg`, `ZPos`, `ZNeg`)
    :param hCamFct: factor that applies to the height of the camera position 
             in order to change perspective of isometric views (defaults to 1).
             Usual values 0.1 to 10
    :param caption:   text to display in the graphic 
    :param fileName:  full name of the graphic file to generate. 
             Defaults to ` None`, in this case it returns a console output 
             graphic.
    '''
    if(setToDisplay == None):
        setToDisplay=preprocessor.getSets.getSet('total')
        setToDisplay.fillDownwards()
        lmsg.warning('set to display not defined; using total set.')
    if equLoadVctScale not in [None,0] and accelMode==None:
        lmsg.warning("Can't display equivalent static loads. Parameter accelMode should not be null ")
        equLoadVctScale=None
    if setToDisplay.color.Norm()==0:
        setToDisplay.color=xc.Vector([rd.random(),rd.random(),rd.random()])
    defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
    defDisplay.setupGrid(setToDisplay)
    defDisplay.viewName= viewNm
    defDisplay.hCamFct=hCamFct
    defDisplay.defineMeshScene(None,defShapeScale,eigenMode,color=setToDisplay.color) 
    if equLoadVctScale not in [None,0]:
        vField=vf.VectorField(name='modo'+str(eigenMode),fUnitConv=unitsScale,scaleFactor=equLoadVctScale,showPushing= True)
        setNodes= setToDisplay.getNodes
        for n in setNodes:
            pos= n.getEigenPos3d(defShapeScale,eigenMode)
            vEqLoad=n.getEquivalentStaticLoad(eigenMode,accelMode)
            vField.data.insertNextPair(pos.x,pos.y,pos.z,vEqLoad[0],vEqLoad[1],vEqLoad[2],unitsScale,pushing= True)
        vField.addToDisplay(defDisplay)
    defDisplay.displayScene(caption,fileName)
    return defDisplay
