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
import vtk
import random as rd 
from miscUtils import LogMessages as lmsg
from solution import predefined_solutions
from postprocess.xcVtk import vtk_graphic_base
from postprocess.xcVtk.FE_model import vtk_FE_graphic
from postprocess.xcVtk.FE_model import Fields
from postprocess.xcVtk import control_var_diagram as cvd
from postprocess.xcVtk import linear_load_diagram as lld
from postprocess.xcVtk import load_vector_field as lvf
from postprocess.xcVtk import node_property_diagram as npd
from postprocess.xcVtk import local_axes_vector_field as lavf
from postprocess.xcVtk import vector_field as vf

class QuickGraphics(object):
    '''This class is aimed at providing the user with a quick and easy way to 
    display results (internal forces, displacements).

    :ivar loadCaseName: OPTIONAL. name of the load case to be created
    :ivar loadCaseExpr: OPTIONAL. expression that defines de load case as a
                        combination of previously defined actions
                        e.g. '1.0*GselfWeight+1.0*GearthPress'
    '''
    def __init__(self,loadCaseName= '',loadCaseExpr= ''):
        self.loadCaseName=loadCaseName
        self.loadCaseExpr=loadCaseExpr

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

    def getDisplay(self,wmDef= vtk_graphic_base.CameraParameters('XYZPos',1.0)):
        '''Returns a suitable display to show the graphics.

        :param vwDef:  camera parameters.
        '''
        defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
        defDisplay.cameraParameters= wmDef
        return defDisplay

    def checkSetToDisp(self,setToDisplay):
        if(setToDisplay):
            self.xcSet= setToDisplay
            self.xcSet.fillDownwards()
            if self.xcSet.color.Norm()==0:
                self.xcSet.color=xc.Vector([rd.random(),rd.random(),rd.random()])
        else:
            lmsg.error('Set to display not defined; using total set.')
            self.xcSet= None

    def displayDispRot(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,unitDescription= '',viewDef= vtk_graphic_base.CameraParameters('XYZPos',1.0),fileName=None,defFScale=0.0,rgMinMax=None):
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
        self.checkSetToDisp(setToDisplay)
        vCompDisp= self.getDispComponentFromName(itemToDisp)
        nodSet= self.xcSet.nodes
        for n in nodSet:
            n.setProp('propToDisp',n.getDisp[vCompDisp])
        field= Fields.ScalarField(name='propToDisp',functionName="getProp",component=None,fUnitConv=fConvUnits,rgMinMax=rgMinMax)
        defDisplay= self.getDisplay(viewDef)
        defDisplay.displayMesh(xcSets=self.xcSet,field=field,diagrams= None, fName=fileName,caption=self.loadCaseName+' '+itemToDisp+' '+unitDescription+' '+self.xcSet.description,defFScale=defFScale)

    def displayIntForc(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,unitDescription= '',viewDef= vtk_graphic_base.CameraParameters('XYZPos'),fileName=None,defFScale=0.0):
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
        :param viewDef:  camera parameters.
        :param fileName: name of the file to plot the graphic. Defaults to None,
             in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        '''
        self.checkSetToDisp(setToDisplay)
        vCompDisp= self.getIntForceComponentFromName(itemToDisp)
        elSet= self.xcSet.elements.pickElemsOfDimension(2)
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
            defDisplay= self.getDisplay(viewDef)
            field.display(defDisplay=defDisplay,fName=fileName,caption=self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.description,defFScale=defFScale)


    def displayIntForcDiag(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,scaleFactor=1.0,unitDescription= '',viewDef= vtk_graphic_base.CameraParameters('XYZPos'),fileName=None,defFScale=0.0):
        '''displays the component of internal forces in the set of entities as a 
         diagram over lines (i.e. appropiated for beam elements).

        :param itemToDisp: component of the internal forces 
          ('N', 'Qy' (or 'Vy'), 'Qz' (or 'Vz'), 'My', 'Mz', 'T') to be depicted 
        :param setToDisplay: set of entities (elements of type beam) to be 
          represented
        :param fConvUnits:   factor of conversion to be applied to the results 
            (defaults to 1)
        :param scaleFactor:  factor of scale to apply to the diagram display
        :param unitDescription: string like '[kN/m] or [kN m/m]'
        :param vwDef:  camera parameters.
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        '''
        self.checkSetToDisp(setToDisplay)
        #auto-scale parameters
        LrefModSize=self.xcSet.getBnd(1.0).diagonal.getModulus() #representative length of set size (to autoscale)
        diagAux=cvd.ControlVarDiagram(scaleFactor= scaleFactor,fUnitConv= fConvUnits,sets=[self.xcSet],attributeName= "intForce",component= itemToDisp)
        maxAbs=diagAux.getMaxAbsComp()
        if maxAbs > 0:
            scaleFactor*=0.15*LrefModSize/(maxAbs*fConvUnits)
        #
        caption= self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.description
        display_diagram(scaleFactor= scaleFactor,fConvUnits= fConvUnits,setToDispRes=self.xcSet,setDisp=self.xcSet,attributeName= "intForce",component= itemToDisp,caption=caption,viewDef=viewDef,defFScale=defFScale,fileName=fileName)

    def displayReactions(self,setToDisplay=None,fConvUnits=1.0,scaleFactor=1.0,unitDescription= '',viewDef= vtk_graphic_base.CameraParameters('XYZPos'),fileName=None,defFScale=0.0):
        self.checkSetToDisp(setToDisplay)
        preprocessor= setToDisplay.getPreprocessor
        preprocessor.getNodeHandler.calculateNodalReactions(True,1e-7)
        #auto-scale
        LrefModSize=self.xcSet.getBnd(1.0).diagonal.getModulus() #representative length of set size (to autoscale)
        maxAbs=0.0
        forcePairs= list()
        momentPairs= list()
        threshold= LrefModSize/1000.0
        for n in self.xcSet.nodes:
            f3d= n.getReactionForce3d
            m3d= n.getReactionMoment3d
            modF3d= f3d.getModulus()
            if(modF3d>threshold):
                p=n.getCurrentPos3d(defFScale)
                forcePairs.append(([p.x,p.y,p.z],[f3d.x,f3d.y,f3d.z]))
            modM3d= m3d.getModulus()
            if(modM3d>threshold):
                p=n.getCurrentPos3d(defFScale)
                momentPairs.append(([p.x,p.y,p.z],[m3d.x,m3d.y,m3d.z]))
            modR= max(modF3d,modF3d)
            if(modR>maxAbs):
                maxAbs=modR
        if(maxAbs>0):
            scaleFactor*=0.15*LrefModSize/(maxAbs*fConvUnits)
        #
        caption= self.loadCaseName+' Reactions'+ ' '+unitDescription +' '+self.xcSet.description
        vFieldF= vf.VectorField(name='Freact',fUnitConv=fConvUnits,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource()) # Force
        vFieldM= vf.VectorField(name='Mreact',fUnitConv=fConvUnits,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource()) # Moment
        vFieldF.populateFromPairList(forcePairs)
        vFieldM.populateFromPairList(momentPairs)

        defDisplay= self.getDisplay(viewDef)
        defDisplay.setupGrid(self.xcSet)
        defDisplay.defineMeshScene(None,defFScale,color=self.xcSet.color)
        if(len(forcePairs)>0):
            vFieldF.addToDisplay(defDisplay)
        if(len(momentPairs)>0):
            vFieldM.addToDisplay(defDisplay,'V')
        defDisplay.displayScene(caption,fileName)

    def displayEigenvectors(self,mode= 1, setToDisplay=None,fConvUnits=1.0,scaleFactor=1.0,viewDef= vtk_graphic_base.CameraParameters('XYZPos'),fileName=None,defFScale=0.0):
        self.checkSetToDisp(setToDisplay)
        preprocessor= setToDisplay.getPreprocessor
        #auto-scale
        LrefModSize=self.xcSet.getBnd(1.0).diagonal.getModulus() #representative length of set size (to autoscale)
        maxAbs=0.0
        dispPairs= list()
        rotPairs= list()
        threshold= LrefModSize/1000.0
        for n in self.xcSet.nodes:
            disp3d= n.getEigenvectorDisp3dComponents(mode)
            rot3d= n.getEigenvectorRot3dComponents(mode)
            modDisp3d= disp3d.getModulus()
            if(modDisp3d>threshold):
                p=n.getCurrentPos3d(defFScale)
                dispPairs.append(([p.x,p.y,p.z],[disp3d.x,disp3d.y,disp3d.z]))
            modRot3d= rot3d.getModulus()
            if(modRot3d>threshold):
                p=n.getCurrentPos3d(defFScale)
                rotPairs.append(([p.x,p.y,p.z],[rot3d.x,rot3d.y,rot3d.z]))
            modR= max(modDisp3d,modRot3d)
            if(modR>maxAbs):
                maxAbs=modR
        if maxAbs > 0:
            scaleFactor*=0.15*LrefModSize/(maxAbs*fConvUnits)
        #
        caption= 'Mode '+ str(mode) + ' eigenvectors' + ' '+self.xcSet.description
        vFieldD= vf.VectorField(name='Deigenvectors',fUnitConv=fConvUnits,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource()) #Force
        vFieldR= vf.VectorField(name='Reigenvectors',fUnitConv=fConvUnits,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource())
        vFieldD.populateFromPairList(dispPairs)
        vFieldR.populateFromPairList(rotPairs)
            
        defDisplay= self.getDisplay(viewDef)
        defDisplay.setupGrid(self.xcSet)
        defDisplay.defineMeshScene(None,defFScale,color=self.xcSet.color)
        if(len(dispPairs)>0):
            vFieldD.addToDisplay(defDisplay)
        if(len(rotPairs)>0):
            vFieldR.addToDisplay(defDisplay,'V')
        defDisplay.displayScene(caption,fileName)

    def dispLoadCaseBeamEl(self,loadCaseName='',setToDisplay=None,fUnitConv=1.0,elLoadComp='transComponent',elLoadScaleF=1.0,nodLoadScaleF=1.0,viewDef= vtk_graphic_base.CameraParameters('XYZPos'),caption='',fileName=None,defFScale=0.0):
        '''Display the loads applied on beam elements and nodes for a given load case

        :param loadCaseName: load case name
        :param setToDisplay: set of beam elements to be represented
        :param fUnitConv:  factor of conversion to be applied to the results
                        (defaults to 1)
        :param elLoadComp:  component of the loads on elements to be depicted
                     [possible components: 'axialComponent', 'transComponent', 
                      'transYComponent', 'transZComponent']
        :param elLoadScaleF:    factor of scale to apply to the diagram display 
                      of element loads (defaults to 1)
        :param nodLoadScaleF: factor of scale to apply to the vector display of 
                      nodal loads (defaults to 1)
        :param vwDef:  camera parameters.
        :param caption:   caption for the graphic
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                          in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
        '''
        self.checkSetToDisp(setToDisplay)
        preprocessor= setToDisplay.getPreprocessor
        loadPatterns= preprocessor.getLoadHandler.getLoadPatterns
        loadPatterns.addToDomain(loadCaseName)
        defDisplay= self.getDisplay(viewDef)
        grid= defDisplay.setupGrid(self.xcSet)
        defDisplay.defineMeshScene(None,defFScale,color=self.xcSet.color)
        orNodalLBar='H'  #default orientation of scale bar for nodal loads
        # auto-scaling parameters
        LrefModSize=setToDisplay.getBnd(1.0).diagonal.getModulus() #representative length of set size (to auto-scale)
        diagAux=lld.LinearLoadDiagram(scale=elLoadScaleF,fUnitConv=fUnitConv,loadPatternName=loadCaseName,component=elLoadComp)
        maxAbs=diagAux.getMaxAbsComp(preprocessor)
        if maxAbs > 0:
            elLoadScaleF*=LrefModSize/maxAbs*100
        #
        diagram= lld.LinearLoadDiagram(scale=elLoadScaleF,fUnitConv=fUnitConv,loadPatternName=loadCaseName,component=elLoadComp)
        diagram.addDiagram(preprocessor)
        if diagram.isValid():
            defDisplay.appendDiagram(diagram)
        orNodalLBar='V'
        # nodal loads
        vField=lvf.LoadVectorField(loadPatternName=loadCaseName,fUnitConv=fUnitConv,scaleFactor=nodLoadScaleF,showPushing= True)
    #    loadPatterns= preprocessor.getLoadHandler.getLoadPatterns
        lPattern= loadPatterns[loadCaseName]
        count= 0
        if(lPattern):
            count=vField.dumpNodalLoads(preprocessor,defFScale=defFScale)
        else:
            lmsg.error('load pattern: '+ loadCaseName + ' not found.')
        if count >0:
            vField.addToDisplay(defDisplay,orientation=orNodalLBar)
        defDisplay.displayScene(caption=caption,fName=fileName)

    def displayNodeValueDiagram(self,itemToDisp='',setToDisplay=None,fConvUnits=1.0,scaleFactor=1.0,unitDescription= '',viewDef= vtk_graphic_base.CameraParameters('XYZPos'),fileName=None,defFScale=0.0):
        '''displays the a displacement (uX,uY,...) or a property defined in nodes 
        as a diagram over lines (i.e. appropiated for beam elements).

        :param itemToDisp:   item to display.
        :param setToDisplay: set of entities (elements of type beam) to be 
               represented
        :param fConvUnits:   factor of conversion to be applied to the results 
               (defalts to 1)
        :param scaleFactor:  factor of scale to apply to the diagram display of
        :param unitDescription: string like '[m]' or '[rad]' or '[m/s2]'
        :param viewDef: camera parameters.
        :param fileName: name of the file to plot the graphic. Defaults to None,
                         in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
         '''
        self.checkSetToDisp(setToDisplay)
        diagram= npd.NodePropertyDiagram(scaleFactor= scaleFactor,fUnitConv= fConvUnits,sets=[self.xcSet],attributeName= itemToDisp)
        diagram.addDiagram()
        defDisplay= self.getDisplay(viewDef)
        defDisplay.setupGrid(self.xcSet)
        defDisplay.defineMeshScene(None,defFScale,color=self.xcSet.color)
        defDisplay.appendDiagram(diagram) #Append diagram to the scene.

        caption= self.loadCaseName+' '+itemToDisp+' '+unitDescription +' '+self.xcSet.description
        defDisplay.displayScene(caption=caption,fName=fileName)

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
    def __init__(self,feProblem,loadCaseName= '',loadCaseExpr= ''):
        super(LoadCaseResults,self).__init__(loadCaseName, loadCaseExpr)
        self.feProblem= feProblem
        if(self.feProblem):
          self.prep= feProblem.getPreprocessor

    def solve(self):
        combs=self.prep.getLoadHandler.getLoadCombinations
        lCase=combs.newLoadCombination(self.loadCaseName,self.loadCaseExpr)
        self.prep.resetLoadCase()
        combs.addToDomain(self.loadCaseName)
        #Solution
        lmsg.warning('Here we use a simple linear static solution that is not always a suitable method.')
        analysis= predefined_solutions.simple_static_linear(self.feProblem)
        result= analysis.analyze(1)
        combs.removeFromDomain(self.loadCaseName)
    def checkSetToDisp(self,setToDisplay):
        if(setToDisplay):
            self.xcSet= setToDisplay
        else:
            lmsg.warning('Set to display not defined; using total set.')
            self.xcSet= self.prep.getSets.getSet('total')
        self.xcSet.fillDownwards()
        if self.xcSet.color.Norm()==0:
            self.xcSet.color=xc.Vector([rd.random(),rd.random(),rd.random()])

def checkSetToDisp(preprocessor,setToDisplay):
    if(setToDisplay == None):
        setToDisplay=preprocessor.getSets.getSet('total')
        setToDisplay.fillDownwards()
        lmsg.warning('set to display not defined; using total set.')
    if setToDisplay.color.Norm()==0:
        setToDisplay.color=xc.Vector([rd.random(),rd.random(),rd.random()])
    return setToDisplay
        
def display_axes(vectorField, prep, setToDisplay=None,vectorScale=1.0,viewDef= vtk_graphic_base.CameraParameters("XYZPos",1.0),caption= '',fileName=None,defFScale=0.0):
    '''vector field display of the loads applied to the chosen set of elements in the load case passed as parameter

    :param vectorField: function that generates the vector field.
    :param setToDisplay: set of elements to be displayed (defaults to total set)
    :param vectorScale: factor to apply to the vectors length in the 
           representation.
    :param viewDef: camera parameters.
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
    defDisplay.cameraParameters= viewDef
    defDisplay.defineMeshScene(None,defFScale) 
    vectorField.addToDisplay(defDisplay)
    defDisplay.displayScene(caption,fileName)
    return defDisplay
    
def display_local_axes(prep,setToDisplay=None,vectorScale=1.0,viewDef= vtk_graphic_base.CameraParameters("XYZPos",1.0),caption= '',fileName=None,defFScale=0.0):
    '''vector field display of the loads applied to the chosen set of elements in the load case passed as parameter
    
    :param setToDisplay: set of elements to be displayed (defaults to total set)
    :param vectorScale: factor to apply to the vectors length in the 
           representation.
    :param viewDef: camera parameters.
    :param fileName: full name of the graphic file to generate. 
           Defaults to `None`, in this case it returns a console output graphic.
    :param caption: text to display in the graphic 
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
    '''
    setToDisplay=checkSetToDisp(prep,setToDisplay)
    vField=lavf.LocalAxesVectorField(setToDisplay.name+'_localAxes',vectorScale)
    vField.dumpVectors(setToDisplay)
    return display_axes(vField, prep,setToDisplay,vectorScale,viewDef,caption,fileName,defFScale)


def display_strong_weak_axis(preprocessor,setToDisplay=None,vectorScale=1.0,viewDef= vtk_graphic_base.CameraParameters("XYZPos",1.0),caption= '',fileName=None,defFScale=0.0):
    '''vector field display of the loads applied to the chosen set of elements 
       in the load case passed as parameter
    
    :param setToDisplay: set of elements to be displayed (defaults to total set)
    :param vectorScale: factor to apply to the vectors length in the 
           representation.
    :param viewDef: camera parameters.
    :param fileName: full name of the graphic file to generate. Defaults to 
           `None`, in this case it returns a console output graphic.
    :param caption: text to display in the graphic 
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
    '''
    setToDisplay=checkSetToDisp(preprocessor,setToDisplay)
    vField=lavf.StrongWeakAxisVectorField(setToDisplay.name+'_strongWeakAxis',vectorScale)
    vField.dumpVectors(setToDisplay)
    return display_axes(vField,preprocessor,setToDisplay,vectorScale,viewDef,caption,fileName,defFScale)


def display_vector_field(preprocessor,vField,setToDisplay,viewDef,caption,fileName,defFScale=0.0):
    defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
    defDisplay.setupGrid(setToDisplay)
    vField.dumpVectors(preprocessor,defFScale)
    defDisplay.cameraParameters= viewDef
    defDisplay.defineMeshScene(None,defFScale,color=setToDisplay.color) 
    vField.addToDisplay(defDisplay)
    defDisplay.displayScene(caption,fileName)
    return defDisplay
    

def display_load(preprocessor,setToDisplay=None,loadCaseNm='',unitsScale=1.0,vectorScale=1.0,multByElemArea=False,viewDef= vtk_graphic_base.CameraParameters("XYZPos",1.0),caption= '',fileName=None,defFScale=0.0):
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
    :param viewDef: camera parameters.
    :param fileName: full name of the graphic file to generate. Defaults to 
           ` None`, in this case it returns a console output graphic.
    :param caption: text to display in the graphic 
    :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
    '''
    setToDisplay=checkSetToDisp(preprocessor,setToDisplay)
    LrefModSize=setToDisplay.getBnd(1.0).diagonal.getModulus() #representative length of set size (to auto-scale)
    vectorScale*=LrefModSize/10.
    vField=lvf.LoadVectorField(loadCaseNm,unitsScale,vectorScale)
    vField.multiplyByElementArea=multByElemArea
    display_vector_field(preprocessor,vField,setToDisplay,viewDef,caption,fileName,defFScale)

def display_eigen_result(preprocessor,eigenMode, setToDisplay=None,defShapeScale=0.0,equLoadVctScale=None,accelMode=None, unitsScale=1.0,viewDef= vtk_graphic_base.CameraParameters("XYZPos",1.0),caption= '',fileName=None):
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
    :param viewDef: camera parameters.
    :param caption:   text to display in the graphic 
    :param fileName:  full name of the graphic file to generate. 
             Defaults to ` None`, in this case it returns a console output 
             graphic.
    '''
    setToDisplay=checkSetToDisp(preprocessor,setToDisplay)
    if equLoadVctScale not in [None,0] and accelMode==None:
        lmsg.warning("Can't display equivalent static loads. Parameter accelMode should not be null ")
        equLoadVctScale=None
    defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
    defDisplay.setupGrid(setToDisplay)
    defDisplay.cameraParameters= viewDef
    defDisplay.defineMeshScene(None,defShapeScale,eigenMode,color=setToDisplay.color) 
    if equLoadVctScale not in [None,0]:
        vField=vf.VectorField(name='modo'+str(eigenMode),fUnitConv=unitsScale,scaleFactor=equLoadVctScale,showPushing= True)
        setNodes= setToDisplay.nodes
        for n in setNodes:
            pos= n.getEigenPos3d(defShapeScale,eigenMode)
            vEqLoad=n.getEquivalentStaticLoad(eigenMode,accelMode)
            vField.data.insertNextPair(pos.x,pos.y,pos.z,vEqLoad[0],vEqLoad[1],vEqLoad[2],unitsScale,pushing= True)
        vField.addToDisplay(defDisplay)
    defDisplay.displayScene(caption,fileName)
    return defDisplay


def display_diagram(scaleFactor,fConvUnits,setToDispRes,setDisp,attributeName,component,caption,viewDef,defFScale,fileName):
    '''Auxiliary function to display results on linear elements.

    :param scaleFactor:  factor of scale to apply to the diagram display
    :param fConvUnits:   factor of conversion to be applied to the results 
    :param setToDispRes: set of linear elements to which display results
    :param setDisp:      set of elements (any type) to be depicted
    :param attributeName:attribute name(e.g. 'ULS_normalStressesResistance')
    :param component:    result item to display (e.g. 'N', 'My', ...)
    :param caption:      caption to display
    :param viewDef:      instance of class CameraParameters
    :param defFScale:    factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
    :param fileName:     file to dump the display
    '''
    diagram= cvd.ControlVarDiagram(scaleFactor= scaleFactor,fUnitConv= fConvUnits,sets=[setToDispRes],attributeName= attributeName,component= component)
    diagram.addDiagram()
    defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
    defDisplay.cameraParameters=viewDef
    defDisplay.setupGrid(setDisp)
    if hasattr(setDisp,'color'):
        color=setDisp.color
    else:
        color=xc.Vector([rd.random(),rd.random(),rd.random()])
    defDisplay.defineMeshScene(None,defFScale,color=color)
    defDisplay.appendDiagram(diagram) #Append diagram to the scene.
    defDisplay.displayScene(caption=caption,fName=fileName)
    
def display_beam_result(attributeName,itemToDisp,beamSetDispRes,setToDisplay=None,fConvUnits=1.0,scaleFactor=1.0,caption=None,viewDef= vtk_graphic_base.CameraParameters('XYZPos'),fileName=None,defFScale=0.0):
    '''display results for beam elements from a limit state verification file.

    :param attributeName:attribute name(e.g. 'ULS_normalStressesResistance')
    :param itemToDisp:   result item to display (e.g. 'N', 'My', ...)
    :param beamSetDispRes:set of linear elements to which display results 
    :param setToDisplay: set of elements (any type) to be depicted
           (defaults to None, in that case only elements in beamSetDispRes
           are displayed)
    :param fConvUnits:   factor of conversion to be applied to the results 
           (defaults to 1)
    :param scaleFactor:  factor of scale to apply to the auto-scaled display
           (defaults to 1)
    :param caption:      caption to display
           (defaults to 'attributeName + itemToDisp')
    :param viewDef:      instance of class CameraParameters
           (defaults to 'XYZPos')
    :param fileName:     file to dump the display (defaults to screen display)
    :param defFScale:    factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)

   '''
    #auto-scale parameters
    LrefModSize=setToDisplay.getBnd(1.0).diagonal.getModulus() #representative length of set size (to autoscale)
    lstArgVal=[e.getProp(attributeName+'Sect1')(itemToDisp) for e in beamSetDispRes.elements]
    maxAbs=max(abs(max(lstArgVal)),abs(min(lstArgVal)))
    if maxAbs > 0:
        scaleFactor*=0.15*LrefModSize/(maxAbs*fConvUnits)
    if not setToDisplay:
        setToDisplay=beamSetDispRes
    if not caption:
        if hasattr(beamSetDispRes,'description'):
            descrSet=beamSetDispRes.description.capitalize()
        else:
            descrSet=''
        caption= attributeName + ', ' + itemToDisp + '. '+ descrSet
    display_diagram(scaleFactor,fConvUnits,beamSetDispRes,setToDisplay,attributeName,itemToDisp,caption,viewDef,defFScale,fileName)
   
