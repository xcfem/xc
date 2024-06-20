# -*- coding: utf-8 -*-

from __future__ import print_function
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2019, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import sys
import vtk
from postprocess.xcVtk import vtk_graphic_base
from postprocess.xcVtk.CAD_model import vtk_CAD_graphic
from postprocess.xcVtk.FE_model import vtk_FE_graphic
from postprocess.xcVtk.fields import fields
from postprocess.xcVtk.fields import vector_field as vf
from postprocess.xcVtk.fields import load_vector_field as lvf
from postprocess.xcVtk.diagrams import control_var_diagram as cvd
from postprocess.xcVtk.diagrams import beam_result_diagram as brd
from postprocess.xcVtk.diagrams import internal_force_diagram as ifd
from postprocess.xcVtk.diagrams import linear_load_diagram as lld
from postprocess.xcVtk.diagrams import node_property_diagram as npd
from postprocess.xcVtk.diagrams import element_property_diagram as epd
from postprocess import output_styles
from misc_utils import log_messages as lmsg # Error messages.

class OutputHandler(object):
    ''' Object that handles the ouput (graphics, etc.)

       :ivar modelSpace: FE model data.
       :ivar outputStyle: style of the output.
    '''
    def __init__(self, modelSpace= None, outputStyle= output_styles.defaultOutputStyle):
        '''Defines the dimension of the space and the number 
         of DOFs for each node.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param outputStyle: style of the output.
        '''
        self.modelSpace= modelSpace
        self.outputStyle= outputStyle

    def getCaptionText(self, itemToDisp, unitDescription, setToDisplay):
        ''' Return the text to use in the image caption.'''
        loadCaseName= self.modelSpace.getCurrentLoadCaseName()
        return loadCaseName+' '+itemToDisp+' '+unitDescription+' '+setToDisplay.description

    def getOutputLengthUnitSym(self):
        return self.outputStyle.outputUnits.dynamicUnits.lengthUnit.symbol

    def getOutputForceUnitSym(self):
        return self.outputStyle.outputUnits.dynamicUnits.forceUnit.symbol

    def getDefaultCameraParameters(self):
        '''Return the default camera parameters.'''
        if(self.modelSpace.getSpaceDimension()==3): # 3D problem
            return vtk_graphic_base.CameraParameters('XYZPos')
        else: # 2D or 1D problem
            retval= vtk_graphic_base.CameraParameters('Custom')
            retval.viewUpVc= [0,1,0]#[0,0,1]
            retval.posCVc= [0,0,100] #[0,-100,0]
            return retval
        
    def getCameraParameters(self):
        ''' Return a suitable set of camera parameters if not
            already defined.'''
        if(not self.outputStyle.cameraParameters):
            self.outputStyle.cameraParameters= self.getDefaultCameraParameters()
        return self.outputStyle.cameraParameters

    def setCameraParameters(self, cameraParameters):
        ''' Set the camera parameters from the arguments.

        :param cameraParameters: CameraParameters object (see vtk_graphic_base)
                                 parameters that define the camera.               
        '''
        self.outputStyle.cameraParameters= cameraParameters
        
    def displayBlocks(self, setToDisplay= None, caption= None, fileName=None):
        '''Display the blocks (points, lines, surfaces and volumes)
           of the set.

           :param setToDisplay: set to display.
           :param caption: title of the graphic.
           :param fileName: name of the file to plot the graphic. Defaults to 
                       None, in that case an screen display is generated
        '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        if(caption is None):
            caption= setToDisplay.name+' set; blocks'
        displaySettings= vtk_CAD_graphic.DisplaySettingsBlockTopo()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.displayBlocks(setToDisplay,caption= caption, fileName= fileName)
        
    def displayFEMesh(self, setsToDisplay= None, caption= None, fileName= None, defFScale= 0.0):
        '''Display the mesh (nodes, elements and constraints)
           of the set.

           :param setsToDisplay: list of sets to display (defaults to TotalSet).
           :param caption: title of the graphic.
           :param fileName: name of the file to plot the graphic. Defaults to 
                       None, in that case an screen display is generated
           :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
        '''
        if(setsToDisplay is None):
            setsToDisplay= [self.modelSpace.getTotalSet()]
        if(caption is None):
            setNames= ''
            if(type(setsToDisplay)==list):
                for s in setsToDisplay:
                    setNames+= s.name+' '
            else:
                setNames= setsToDisplay.name    
            caption= setNames+'; mesh'
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.displayMesh(xcSets=setsToDisplay,caption= caption, scaleConstr= self.outputStyle.constraintsScaleFactor, fileName= fileName, defFScale= defFScale)

    def displayLocalAxes(self, setToDisplay= None, caption= None, fileName=None, defFScale= 0.0):
        '''Display the local axes of the elements contained in the set.

           :param setToDisplay: set to display.
           :param caption: title of the graphic.
           :param fileName: name of the file to plot the graphic. Defaults to 
                       None, in that case an screen display is generated
           :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
        '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        if(caption is None):
            caption= setToDisplay.name+' set; local axes'
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.displayLocalAxes(setToDisplay,caption= caption, vectorScale= self.outputStyle.localAxesVectorsScaleFactor, fileName= fileName, defFScale= defFScale)

    def displayStrongWeakAxis(self, setToDisplay= None, caption= None, fileName=None, defFScale= 0.0):
        '''Display the local axes of the elements contained in the set.

           :param setToDisplay: set to display.
           :param caption: title of the graphic.
           :param fileName: name of the file to plot the graphic. Defaults to 
                       None, in that case an screen display is generated
           :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
         '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        if(caption is None):
            caption= setToDisplay.name+' set; strong [red] and weak [blue] axes'
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
#        displaySettings.displayStrongWeakAxis(setToDisplay,caption= caption, vectorScale= self.outputStyle.localAxesVectorsScaleFactor, fileName= fileName, defFScale= defFScale)
        displaySettings.displayStrongWeakAxis(setToDisplay,caption= caption, vectorScale= self.outputStyle.localAxesVectorsScaleFactor)

    def displayScalarPropertyAtNodes(self, propToDisp, fUnitConv, unitDescription, captionText, setToDisplay, fileName=None, defFScale=0.0, rgMinMax=None):
        '''displays the scalar property defined at the nodes of the set.

        :param propeToDisp: scalar property defined at nodes. 
        :param fUnitConv: conversion factor for units
        :param unitDescription: unit(s) symbol(s)
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
              of the field to be represented (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)

        '''
        field= fields.ScalarField(name=propToDisp,functionName="getProp",component=None, fUnitConv= fUnitConv, rgMinMax=rgMinMax)
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.displayMesh(xcSets=setToDisplay, field= field, diagrams= None, caption= captionText, fileName=fileName, defFScale=defFScale)
        
    def displayDispRot(self,itemToDisp, setToDisplay=None, fileName=None,defFScale=0.0, rgMinMax=None,captionText=None):
        '''displays the component of the displacement or rotations in the 
        set of entities.

        :param itemToDisp: component of the displacement ('uX', 'uY' or 'uZ') 
                    or the rotation ('rotX', rotY', 'rotZ') to be depicted 
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)
       :param captionText: caption text. Defaults to None, in which case the default 
                                 caption text (internal force +  units + set name) is created
        '''
        # Define the property at nodes.
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        propertyName= 'disp'+itemToDisp
        vCompDisp= self.modelSpace.getDispComponentFromName(itemToDisp)
        nodSet= setToDisplay.nodes
        for n in nodSet:
            n.setProp(propertyName,n.getDisp[vCompDisp])
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)
        if not captionText:
            captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
        self.displayScalarPropertyAtNodes(propertyName, fUnitConv= unitConversionFactor, unitDescription= unitDescription, captionText= captionText, setToDisplay= setToDisplay, fileName= fileName, defFScale= defFScale, rgMinMax= rgMinMax)

    def displayStresses(self,itemToDisp, setToDisplay=None, fileName=None,defFScale=0.0, rgMinMax=None,captionText=None):
        '''display the stresses on the elements.

        :param itemToDisp: component of the stress ('sigma_11', 'sigma_22'...)
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red (defaults to None)
        :param captionText: caption text. Defaults to None, in which case the default 
                                 caption text (internal force +  units + set name) is created
        '''
        # Define the property at nodes.
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        propertyName= self.modelSpace.setNodePropertyFromElements(compName= itemToDisp, xcSet= setToDisplay, function= self.modelSpace.getStressComponentFromName, propToDefine= 'stress')
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)
        if not captionText:
            captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
        self.displayScalarPropertyAtNodes(propertyName, unitConversionFactor, unitDescription, captionText, setToDisplay, fileName, defFScale, rgMinMax)

    def displayStrains(self,itemToDisp, setToDisplay=None, fileName=None,defFScale=0.0, rgMinMax=None,captionText=None):
        '''displays the strains on the elements.

        :param itemToDisp: component of the stress ('eps_11', 'eps_22'...)
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)
        :param captionText: caption text. Defaults to None, in which case the default 
                                 caption text (internal force +  units + set name) is created
        '''
        # Define the property at nodes.
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        propertyName= self.modelSpace.setNodePropertyFromElements(compName= itemToDisp, xcSet= setToDisplay, function= self.modelSpace.getStrainComponentFromName, propToDefine= 'strain')
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)
        if not captionText:
            captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
        self.displayScalarPropertyAtNodes(propertyName, unitConversionFactor, unitDescription, captionText, setToDisplay, fileName, defFScale, rgMinMax)
        
    def displayVonMisesStresses(self, vMisesCode= 'von_mises_stress', setToDisplay=None, fileName=None,defFScale=0.0, rgMinMax=None,captionText=None):
        '''display the stresses on the elements.

        :param vMisesCode: string that will be passed to the element
                             getValues method to retrieve the Von Mises
                             stress. This may vary depending on the
                             element type.
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)
        :param captionText: caption text. Defaults to None, in which case the default 
                                 caption text (internal force +  units + set name) is created
        '''
        # Define the property at nodes.
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        propertyName= self.modelSpace.setNodePropertyFromElements(compName= None, xcSet= setToDisplay, function= self.modelSpace.getStressComponentFromName, propToDefine= vMisesCode)
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters('stress')
        if not captionText:
            captionText= self.getCaptionText(vMisesCode, unitDescription, setToDisplay)
        self.displayScalarPropertyAtNodes(propertyName, unitConversionFactor, unitDescription, captionText, setToDisplay, fileName, defFScale, rgMinMax)
        
    def displayState(self, itemToDisp, setToDisplay=None, fileName=None,defFScale=0.0, rgMinMax=None):
        '''displays the strains on the elements.

        :param itemToDisp: component of the state vector as defined in DruckerPrager.cpp (Invariant_1, norm_eta, Invariant_ep, norm_dev_ep, norm_ep)
        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)

        '''
        # Define the property at nodes.
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        propertyName= self.modelSpace.setNodePropertyFromElements(compName= itemToDisp, xcSet= setToDisplay, function= self.modelSpace.getStateComponentFromName, propToDefine= 'state')
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)

        captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
        self.displayScalarPropertyAtNodes(propertyName, unitConversionFactor, unitDescription, captionText, setToDisplay, fileName, defFScale, rgMinMax)
        
    def displayReactions(self, setToDisplay=None, fileName=None, defFScale=0.0, inclInertia= False, reactionCheckTolerance= 1e-7,captionText=None):
        ''' Display reactions.

        :param setToDisplay: set of entities to be represented.
        :param fileName: name of the file to plot the graphic. Defaults to 
                    None, in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param inclInertia: include inertia effects (defaults to false).
        :param reactionCheckTolerance: relative tolerance when checking reaction values.
        :param captionText: caption text. Defaults to None, in which case the default 
                                 caption text (internal force +  units + set name) is created
         '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        self.modelSpace.preprocessor.getNodeHandler.calculateNodalReactions(inclInertia, reactionCheckTolerance)
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
        maxAbs=0.0
        forcePairs= list()
        momentPairs= list()
        threshold= LrefModSize/1000.0
        for n in setToDisplay.nodes:
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
        unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
        unitDescription= self.outputStyle.getForceUnitsDescription()
        
        scaleFactor= self.outputStyle.reactionVectorsScaleFactor
        if(maxAbs>0):
            scaleFactor*=0.15*LrefModSize/(maxAbs*unitConversionFactor)
        if not captionText:
            captionText= self.getCaptionText('Reactions', unitDescription, setToDisplay)
        vFieldF= vf.VectorField(name='Freact', fUnitConv=unitConversionFactor,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource()) # Force
        vFieldM= vf.VectorField(name='Mreact', fUnitConv=unitConversionFactor,scaleFactor=scaleFactor,showPushing= True,symType=vtk.vtkArrowSource()) # Moment
        vFieldF.populateFromPairList(forcePairs)
        vFieldM.populateFromPairList(momentPairs)

        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.setupGrid(setToDisplay)
        meshSceneOk= displaySettings.defineMeshScene(None,defFScale,color= setToDisplay.color)
        if(meshSceneOk):
            scOrient=1 # scalar bar orientation (1 horiz., 2 left-vert, 3 right-vert)
            if(len(forcePairs)>0):
                vFieldF.addToDisplay(displaySettings,orientation=scOrient,
                                     title='Forces ('+ self.getOutputForceUnitSym() +')')
                scOrient+=1
            if(len(momentPairs)>0):
                vFieldM.addToDisplay(displaySettings,orientation=scOrient,
                                     title='Moments (' + self.getOutputForceUnitSym() + self.getOutputLengthUnitSym() +')')
            displaySettings.displayScene(captionText,fileName)
        
    def displayReactionsOnSets(self, setsToDisplayReactions, fileName=None, defFScale=0.0, inclInertia= False, reactionCheckTolerance= 1e-7):
        '''displays the reactions as vector on affected nodes

        :param setsToDisplayReactions: ordered list of sets of nodes to display 
                              reactions on them.
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param inclInertia: include inertia effects (defaults to false).
        :param reactionCheckTolerance: relative tolerance when checking reaction values.
        '''
        for st in setsToDisplayReactions:
            self.displayReactions(setToDisplay= st, fileName= fileName, defFScale= defFScale, inclInertia= inclInertia, reactionCheckTolerance= reactionCheckTolerance)
            
    def displayDiagram(self, attributeName, component, setToDispRes, setToDisplay,caption, scaleFactor= 1.0, fileName= None, defFScale= 0.0,orientScbar=1,titleScbar=None, defaultDirection= 'J'):
        '''Auxiliary function to display results on linear elements.

        :param attributeName: attribute name(e.g. 'ULS_normalStressesResistance')
        :param component:    result item to display (e.g. 'N', 'My', ...)
        :param setToDispRes: set of linear elements to which display results
        :param setToDisplay: set of elements (any type) to be depicted 
                             as context.
        :param scaleFactor:  factor of scale to apply to the auto-scaled display
                             (defaults to 1).
        :param caption:      caption to display
        :param fileName:     file to dump the display
        :param defFScale:    factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        :param orientScbar: orientation of the scalar bar (defaults to 1-horiz)
        :param titleScbar: title for the scalar bar (defaults to None)
        :param defaultDirection: default direction of the diagram (J: element 
                                 local j vector or K: element local K vector).
        '''
        unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
        diagram= cvd.ControlVarDiagram(scaleFactor= scaleFactor,fUnitConv= unitConversionFactor,sets=[setToDispRes],attributeName= attributeName,component= component, defaultDirection= defaultDirection, lRefModSize= LrefModSize)
        diagram.addDiagram()
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.setupGrid(setToDisplay)
        meshSceneOk= displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
        if(meshSceneOk):
            displaySettings.appendDiagram(diagram,orientScbar,titleScbar) #Append diagram to the scene.
            displaySettings.displayScene(caption=caption,fileName=fileName)

    def displayIntForcDiag(self, itemToDisp, setToDisplay=None,fileName=None,defFScale=0.0, overrideScaleFactor= None,orientScbar=1, titleScbar=None, defaultDirection= 'J',captionText=None):
        '''displays the component of internal forces in the set of entities as
           a diagram over lines (i.e. appropriated for beam elements).

        :param itemToDisp: component of the internal forces 
          ('N', 'Qy' (or 'Vy'), 'Qz' (or 'Vz'), 'My', 'Mz', 'T') to be depicted 
        :param setToDisplay: set of entities (elements of type beam) to be 
          represented
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param overrideScaleFactor: if not none, override the scale factor in outputStyle.
        :param orientScbar: orientation of the scalar bar (defaults to 1-horiz)
        :param titleScbar: title for the scalar bar (defaults to None)
        :param defaultDirection: default direction of the diagram (J: element 
                                 local j vector or K: element local K vector).
        :param captionText: caption text. Defaults to None, in which case the default 
                                 caption text (internal force +  units + set name) is created
        '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        #auto-scale parameters
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
        if(overrideScaleFactor):
            scaleFactor= overrideScaleFactor
        else:
            scaleFactor= self.outputStyle.internalForcesDiagramScaleFactor
        unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
        unitDescription= self.outputStyle.getForceUnitsDescription()
        if not captionText:
            captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
        diagram= ifd.InternalForceDiagram(scaleFactor= scaleFactor, lRefModSize= LrefModSize,fUnitConv= unitConversionFactor,sets=[setToDisplay],attributeName= "intForce",component= itemToDisp, defaultDirection= defaultDirection)
        diagram.addDiagram() # add the diagram to the scene.
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.setupGrid(setToDisplay)
        meshSceneOk= displaySettings.defineMeshScene(None,defFScale,color= setToDisplay.color)
        if(meshSceneOk):
            displaySettings.appendDiagram(diagram,orientScbar,titleScbar) #Append diagram to the scene.
            displaySettings.displayScene(caption= captionText,fileName= fileName)
        
    def displayIntForc(self,itemToDisp, setToDisplay=None,fileName=None,defFScale=0.0, rgMinMax=None,captionText=None):
        '''displays the component of internal forces in the 
        set of entities as a scalar field (i.e. appropriated for 2D elements; 
        shells...).

        :param itemToDisp:   component of the internal forces ('N1', 'N2', 'N12', 
             'M1', 'M2', 'M12', 'Q1', 'Q2') to be depicted 
        :param setToDisplay: set of entities to be represented (default to all 
             entities)
        :param fileName: name of the file to plot the graphic. Defaults to None,
             in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values 
              of the field to be represented  (in units of calculation, 
              not units of display). All the values less than vmin are 
              displayed in blue and those greater than vmax in red
              (defaults to None)
        :param captionText: caption text. Defaults to None, in which case the default 
                                 caption text (internal force +  units + set name) is created
           '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        vCompDisp= self.modelSpace.getIntForceComponentFromName(itemToDisp)
        elSet= setToDisplay.elements.pickElemsOfDimension(2)
        if(len(elSet)>0):
            propName= 'propToDisp_'+str(itemToDisp)
            for e in elSet:
                if(e.getDimension==2):
                    e.getResistingForce()
                    physProp= e.physicalProperties
                    e.setProp(propName,physProp.getMeanGeneralizedStressByName(vCompDisp))
                else:
                    lmsg.warning('OutputHandler::displayIntForc; not a 2D element; ignored.')
            unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
            unitDescription= self.outputStyle.getForceUnitsDescription()
            field= fields.ExtrapolatedProperty(propName,"getProp",setToDisplay,fUnitConv= unitConversionFactor,rgMinMax=rgMinMax)
            displaySettings= vtk_FE_graphic.DisplaySettingsFE()
            displaySettings.cameraParameters= self.getCameraParameters()
            if not captionText: captionText= self.getCaptionText(itemToDisp, unitDescription, setToDisplay)
            field.display(displaySettings=displaySettings,caption= captionText,fileName=fileName, defFScale=defFScale)
            
    def displayLoadVectors(self, setToDisplay= None, caption= None, fileName= None, defFScale= 0.0):
        '''Displays load vectors on the set argument.

        :param setToDisplay: set of elements to be displayed (defaults to total set)
        :param caption: text to display in the graphic. Defaults to 
               ` None` in this case the text is the load case description
               and the units of the loads.
        :param fileName: full name of the graphic file to generate. Defaults to 
                     ` None`, in this case it returns a console output graphic.,
        :param defFScale: factor to apply to current displacement of nodes 
                      so that the display position of each node equals to
                      the initial position plus its displacement multiplied
                      by this factor. (Defaults to 0.0, i.e. display of 
                      initial/undeformed shape)
        '''
        lmsg.warning('displayLoadVectors:: deprecated; Use displayLoads')
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        preprocessor= self.modelSpace.preprocessor
        loadCaseName= self.modelSpace.preprocessor.getDomain.currentCombinationName
        unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
        unitDescription= self.outputStyle.getForceUnitsDescription()
        if(not caption):
            caption= 'load case: ' + loadCaseName + ', set: ' + setToDisplay.name + ', '  + unitDescription
        LrefModSize=setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to auto-scale)
        vectorScale= self.outputStyle.loadVectorsScaleFactor*LrefModSize/10.
        vField= lvf.LoadVectorField(loadCaseName,setToDisplay,unitConversionFactor,vectorScale)
        vField.multiplyByElementArea= self.outputStyle.multLoadsByElemArea
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.setupGrid(setToDisplay)
        vField.dumpVectors(preprocessor,defFScale,showElementalLoads= True, showNodalLoads= True)
        meshSceneOk= displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
        if(meshSceneOk):
            vField.addToDisplay(displaySettings)
            displaySettings.displayScene(caption,fileName)
        return displaySettings
        
    def displayLoads(self,  setToDisplay=None, elLoadComp='xyzComponents',fUnitConv=1,caption= None,fileName=None, defFScale=0.0, scaleConstr= 0.2):
        '''Display the loads applied on beam elements and nodes for a given load case

        :param setToDisplay: set of beam elements to be represented (defaults to TotalSet)
        :param elLoadComp:component of the linear loads on elements to be 
               depicted [available components: 'xyzComponents' (default),
               'axialComponent', 'transComponent', 'transYComponent', 
               'transZComponent']
        :param fUnitConv:  factor of conversion to be applied to the results
                        (defaults to 1)
        :param caption:   caption for the graphic
        :param fileName:  name of the file to plot the graphic. Defaults to None
                          in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
        :param scaleConstr: scale of SPConstraints symbols (defaults to 0.2)
        '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        preprocessor= self.modelSpace.preprocessor
        loadCaseName= preprocessor.getDomain.currentCombinationName
        unitConversionFactor= self.outputStyle.getForceUnitsScaleFactor()
        unitDescription= self.outputStyle.getForceUnitsDescription()
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        grid= displaySettings.setupGrid(setToDisplay)
        if __debug__:
            if(not grid):
                AssertionError('Can\'t setup grid.')
        meshSceneOk= displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
        if(meshSceneOk):
            scOrient=1 # scalar bar orientation (1 horiz., 2 left-vert, 3 right-vert)
            # auto-scaling parameters
            LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to auto-scale)
            elLoadScaleF= self.outputStyle.loadDiagramsScaleFactor
            diagram= lld.LinearLoadDiagram(setToDisp=setToDisplay, scale=elLoadScaleF, lRefModSize= LrefModSize, fUnitConv= unitConversionFactor, component=elLoadComp)
            maxAbs= diagram.autoScale(preprocessor)
            if(maxAbs>0.0):
                #Linear loads
                diagram.addDiagram(preprocessor)
                if(diagram.rangeIsValid()):
                    displaySettings.appendDiagram(diagram, orientScbar=scOrient,
                                                  titleScbar='Linear loads ('+self.getOutputForceUnitSym()+'/'+
                                                  self.getOutputLengthUnitSym()+')' )
                    scOrient+=1

            vectorScale= self.outputStyle.loadVectorsScaleFactor*LrefModSize/10.
            # concentrated loads (on elements).

            # surface loads
            forceComponents= self.modelSpace.getForceComponents()
            vFieldEl= lvf.LoadVectorField(loadPatternName= loadCaseName, setToDisp=setToDisplay,
                                          fUnitConv= unitConversionFactor, scaleFactor= vectorScale,
                                          showPushing= self.outputStyle.showLoadsPushing,
                                          multiplyByElementArea= self.outputStyle.multLoadsByElemArea, components= forceComponents)
            count= vFieldEl.dumpElementalLoads(preprocessor, defFScale=defFScale)
            if(count >0):
                vFieldEl.addToDisplay(displaySettings,orientation= scOrient, title='Surface loads ('+self.getOutputForceUnitSym()+'/'+self.getOutputLengthUnitSym()+'2)' )
                scOrient+=1
            # nodal loads
            ## forces on nodes.
            forceComponents= self.modelSpace.getForceComponents()
            vFieldFNod= lvf.LoadVectorField(loadPatternName= loadCaseName, setToDisp=setToDisplay, fUnitConv= unitConversionFactor, scaleFactor= vectorScale, showPushing= self.outputStyle.showLoadsPushing, components= forceComponents)
            count= vFieldFNod.dumpNodalLoads(preprocessor, defFScale=defFScale)
            if(count >0):
                vFieldFNod.addToDisplay(displaySettings,orientation= scOrient, title='Nodal loads ('+self.getOutputForceUnitSym()+')')
                scOrient+=1
            momentComponents= self.modelSpace.getMomentComponents()
            ## moments on nodes.
            if(momentComponents):
                vFieldMNod= lvf.LoadVectorField(loadPatternName= loadCaseName, setToDisp=setToDisplay, fUnitConv= unitConversionFactor, scaleFactor= vectorScale, showPushing= self.outputStyle.showLoadsPushing, components= momentComponents)
                count= vFieldMNod.dumpNodalLoads(preprocessor, defFScale=defFScale)
            else:
                count= 0
            if(count >0):
                vFieldMNod.addToDisplay(displaySettings,orientation= scOrient, title='Nodal moments ('+self.getOutputForceUnitSym()+')')
                scOrient+=1
            if(not caption):
              caption= 'load case: ' + loadCaseName +' '+elLoadComp + ', set: ' + setToDisplay.name + ', '  + unitDescription
            displaySettings.displaySPconstraints(setToDisplay= setToDisplay, scale= scaleConstr)
            displaySettings.displayScene(caption=caption,fileName=fileName)

    def displayNodeValueDiagram(self, itemToDisp, setToDisplay=None,caption= None,fileName=None,defFScale=0.0):
        '''displays the a displacement (uX,uY,...) or a property defined in 
           nodes as a diagram over lines.

        :param itemToDisp: item to display (uX,uY,...).
        :param setToDisplay: set of entities (elements of type beam) to be 
               represented
        :param fileName: name of the file to plot the graphic. Defaults to None,
                         in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
         '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
        diagram= npd.NodePropertyDiagram(scaleFactor= 1.0, lRefModSize= LrefModSize, fUnitConv= unitConversionFactor,sets=[setToDisplay], attributeName= itemToDisp)
        diagram.addDiagram()
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        grid= displaySettings.setupGrid(setToDisplay)
        if __debug__:
            if(not grid):
                AssertionError('Can\'t setup grid.')
        meshSceneOk= displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
        if(meshSceneOk):
            displaySettings.appendDiagram(diagram) #Append diagram to the scene.

            loadCaseName= self.modelSpace.preprocessor.getDomain.currentCombinationName
            if(not caption):
                caption= loadCaseName+' '+itemToDisp+' '+unitDescription +' '+setToDisplay.description
            displaySettings.displayScene(caption=caption,fileName=fileName)

    def displayElementValueDiagram(self, itemToDisp, setToDisplay=None,caption= None,fileName=None,defFScale=0.0):
        '''displays the a displacement (uX,uY,...) or a property defined in 
           nodes as a diagram over lines.

        :param itemToDisp: item to display (uX,uY,...).
        :param setToDisplay: set of entities (elements of type beam) to be 
               represented
        :param fileName: name of the file to plot the graphic. Defaults to None,
                         in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
         '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
        diagram= epd.ElementPropertyDiagram(scaleFactor= 1.0, lRefModSize= LrefModSize, fUnitConv= unitConversionFactor,sets=[setToDisplay], propertyName= itemToDisp)
        diagram.addDiagram()
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        grid= displaySettings.setupGrid(setToDisplay)
        if __debug__:
            if(not grid):
                AssertionError('Can\'t setup grid.')
        meshSceneOk= displaySettings.defineMeshScene(None,defFScale,color=setToDisplay.color)
        if(meshSceneOk):
            displaySettings.appendDiagram(diagram) #Append diagram to the scene.

            loadCaseName= self.modelSpace.preprocessor.getDomain.currentCombinationName
            if(not caption):
                caption= loadCaseName+' '+itemToDisp+' '+unitDescription +' '+setToDisplay.description
            displaySettings.displayScene(caption=caption,fileName=fileName)

    def extractEigenvectorComponents(self, mode= 1, setToDisplay=None, defFScale=0.0, extractDispComponents= True, extractRotComponents= True):
        '''Displays the computed eigenvectors on the set argument.

        :param mode: mode to which the eigenvectors belong.
        :param setToDisplay: set of elements to be displayed (defaults to total set)
        :param defFScale: factor to apply to current displacement of nodes 
                      so that the display position of each node equals to
                      the initial position plus its displacement multiplied
                      by this factor. (Defaults to 0.0, i.e. display of 
                      initial/undeformed shape)
        :param extractDispComponents: if false, don't extract the displacement components of the eigenvectors.
        :param extractRotComponents: if false, don't extract the rotational components of the eigenvectors.
        '''
        preprocessor= self.modelSpace.preprocessor
        domain= preprocessor.getDomain
        numModes= domain.numModes # number of computed modes.
        dispPairs= list()
        rotPairs= list()
        maxAbs= 0
        LrefModSize= 0.0
        if(mode<=numModes):
            norm= preprocessor.getDomain.getMesh.normalizeEigenvectors(mode)
            if __debug__:
                if(not norm):
                    AssertionError('Can\'t normalize eigenvectors.')
            #auto-scale
            LrefModSize=setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
            threshold= LrefModSize/1000.0
            for n in setToDisplay.nodes:
                if(extractDispComponents): # extract displacement components.
                    disp3d= n.getEigenvectorDisp3dComponents(mode)
                    modDisp3d= disp3d.getModulus()
                    if(modDisp3d>threshold):
                        p= n.getCurrentPos3d(defFScale)
                        dispPairs.append(([p.x,p.y,p.z],[disp3d.x,disp3d.y,disp3d.z]))
                else: # don't extract displacement components.
                    modDisp3d= 0.0
                if(extractRotComponents): # extract rotational components.
                    rot3d= n.getEigenvectorRot3dComponents(mode)
                    modRot3d= rot3d.getModulus()
                    if(modRot3d>threshold):
                        p= n.getCurrentPos3d(defFScale)
                        rotPairs.append(([p.x,p.y,p.z],[rot3d.x,rot3d.y,rot3d.z]))
                else: # don't extract rotational components.
                    modRot3d= 0.0
                modR= max(modDisp3d, modRot3d)
                if(modR>maxAbs):
                    maxAbs=modR
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; mode: '+str(mode)+' out of range (1,'+str(numModes)+')')
        return dispPairs, rotPairs, LrefModSize, maxAbs
        
    def displayEigenvectors(self, mode= 1, setToDisplay=None, caption= None, fileName=None, defFScale=0.0, showDispComponents= True, showRotComponents= True):
        '''Displays the computed eigenvectors on the set argument.

        :param mode: mode to which the eigenvectors belong.
        :param setToDisplay: set of elements to be displayed (defaults to total set)
        :param caption: text to display in the graphic. Defaults to 
               ` None` in this case the text is the load case description
               and the units of the loads.
        :param fileName: full name of the graphic file to generate. Defaults to 
                     ` None`, in this case it returns a console output graphic.,
        :param defFScale: factor to apply to current displacement of nodes 
                      so that the display position of each node equals to
                      the initial position plus its displacement multiplied
                      by this factor. (Defaults to 0.0, i.e. display of 
                      initial/undeformed shape)
        :param showDispComponents: if false, don't show the displacement components of the eigenvectors.
        :param showRotComponents: if false, don't show the rotational components of the eigenvectors.
        '''
        if((showDispComponents==False) and (showRotComponents==False)):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; no components to display. Command ignored.')
            return
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        # Extract components of eigenvectors from the model results.
        dispPairs, rotPairs, LrefModSize, maxAbs= self.extractEigenvectorComponents(mode= mode, setToDisplay= setToDisplay, defFScale= defFScale, extractDispComponents= showDispComponents, extractRotComponents= showRotComponents)
        scaleFactor= self.outputStyle.eigenvectorsScaleFactor
        if(maxAbs > 0):
            scaleFactor*=0.15*LrefModSize/(maxAbs)
        # Set caption.
        if(not caption):
            caption= 'Mode '+ str(mode) + ' eigenvectors' + ' '+setToDisplay.description
        if(showDispComponents):
            if(len(dispPairs)>0):
                vFieldD= vf.VectorField(name='Deigenvectors', fUnitConv=1.0, scaleFactor=scaleFactor, showPushing= False, symType=vtk.vtkArrowSource()) #Force
                vFieldD.populateFromPairList(dispPairs)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; mode: '+str(mode)+' no displacement components to display.')
        if(showRotComponents):
            if(len(rotPairs)>0):
                vFieldR= vf.VectorField(name='Reigenvectors', fUnitConv=1.0, scaleFactor=scaleFactor,showPushing= False, symType=vtk.vtkArrowSource())
                vFieldR.populateFromPairList(rotPairs)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; mode: '+str(mode)+' no rotational components to display.')
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        displaySettings.setupGrid(setToDisplay)
        meshSceneOk= displaySettings.defineMeshScene(None, defFScale, color= setToDisplay.color)
        if(meshSceneOk):
            scOrient= 1 # scalar bar orientation (1 horiz., 2 left-vert, 3 right-vert)
            if(len(dispPairs)>0):
                vFieldD.addToDisplay(displaySettings, orientation= scOrient, title= 'Displacement')
                scOrient+=1
            if(len(rotPairs)>0):
                vFieldR.addToDisplay(displaySettings, orientation= scOrient, title= 'Rotation')
            displaySettings.displayScene(caption,fileName)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; something went wrong when defining the mesh scene.')
            
        
    def displayEigenResult(self,eigenMode, setToDisplay=None,  accelMode=None, caption= '',fileName=None, defFScale= 0.0):
        '''Display the deformed shape and/or the equivalent static forces 
        associated with the eigenvibration mode passed as parameter.

        :param eigenMode: eigenvibration mode to be displayed
        :param setToDisplay: set of elements to be displayed (defaults to total set)
        :param accelMode: acceleration associated with the eigen mode depicted, 
                only used if the equivalent static loads are to be displayed.
        :param caption:   text to display in the graphic 
        :param fileName:  full name of the graphic file to generate. 
                 Defaults to ` None`, in this case it returns a console output 
                 graphic.
        :param defFScale: factor to apply to deformed shape so that the 
               displayed position of each node equals to the initial position 
               plus its eigenVector multiplied by this factor. (Defaults to 0.0 
               i.e. display of initial/undeformed shape)
        '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        equLoadVctScale= self.outputStyle.equivalentLoadVectorsScaleFactor
        if((equLoadVctScale!=0.0) and accelMode is None):
            lmsg.warning("Can't display equivalent static loads. Parameter accelMode should not be null ")
            equLoadVctScale=None
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.setupGrid(setToDisplay)
        displaySettings.cameraParameters= self.getCameraParameters()
        meshSceneOk= displaySettings.defineMeshScene(None,defFScale,eigenMode,color=setToDisplay.color)
        if(meshSceneOk):
            unitsScale= 1.0
            if equLoadVctScale not in [None,0]:
                vField=vf.VectorField(name='mode'+str(eigenMode),fUnitConv=unitsScale,scaleFactor=equLoadVctScale,showPushing= False)
                setNodes= setToDisplay.nodes
                for n in setNodes:
                    pos= n.getEigenPos3d(defFScale,eigenMode)
                    vEqLoad= n.getEquivalentStaticLoad(eigenMode,accelMode)
                    vField.data.insertNextPair(pos.x,pos.y,pos.z,vEqLoad[0],vEqLoad[1],vEqLoad[2],unitsScale,pushing= True)
                vField.addToDisplay(displaySettings)
            if(not caption):
                caption= 'Mode '+ str(eigenMode) + ' shape' + ' '+setToDisplay.description            
            displaySettings.displayScene(caption,fileName)
        return displaySettings

    def displayBeamResult(self, attributeName, itemToDisp, beamSetDispRes, setToDisplay=None, caption=None, fileName=None, defFScale=0.0, defaultDirection= 'J', rgMinMax= None):
        '''display results for beam elements from a limit state verification file.

        :param attributeName:attribute name(e.g. 'ULS_normalStressesResistance')
        :param itemToDisp:   result item to display (e.g. 'N', 'My', ...)
        :param beamSetDispRes:set of linear elements to which display results 
        :param setToDisplay: set of elements (any type) to be depicted
               (defaults to None, in that case only elements in beamSetDispRes
               are displayed)
        :param caption:      caption to display
               (defaults to 'attributeName + itemToDisp')
        :param fileName:     file to dump the display (defaults to screen display)
        :param defFScale:    factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)

        :param defaultDirection: default direction of the diagram (J: element 
                                 local j vector or K: element local K vector).
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        #auto-scale parameters
        if(len(beamSetDispRes.elements)):
            # Check that the elements have the required property.
            e0= beamSetDispRes.elements[0]
            propName= attributeName+'Sect1'
            if(e0.hasProp(propName)):
                lRefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus() #representative length of set size (to autoscale)
                unitConversionFactor, unitDescription= self.outputStyle.getUnitParameters(itemToDisp)
                if not caption:
                    if hasattr(beamSetDispRes,'description'):
                        descrSet= beamSetDispRes.description.capitalize()
                    if(len(descrSet)==0): # No description provided.
                        descrSet= beamSetDispRes.name
                    caption= attributeName + ', ' + itemToDisp +' '+unitDescription+ '. '+ descrSet
                diagram= brd.BeamResultDiagram(scaleFactor= 1.0, lRefModSize= lRefModSize, fUnitConv= unitConversionFactor, sets=[beamSetDispRes], attributeName= attributeName, component= itemToDisp, defaultDirection= defaultDirection, rgMinMax= rgMinMax)
                diagram.addDiagram()
                displaySettings= vtk_FE_graphic.DisplaySettingsFE()
                displaySettings.cameraParameters= self.getCameraParameters()
                displaySettings.setupGrid(setToDisplay)
                meshSceneOk= displaySettings.defineMeshScene(None, defFScale,color= setToDisplay.color)
                if(meshSceneOk):
                    displaySettings.appendDiagram(diagram) #Append diagram to the scene.
                    displaySettings.displayScene(caption= caption,fileName= fileName)
            else:
                lmsg.warning('Some elements in set: \''+beamSetDispRes.name+"' don\'t have the required property: '"+str(propName)+"' can't display the results.")
        else:
            lmsg.warning('Element set: \''+beamSetDispRes.name+'\' is empty. There is nothing to display.')
                    
        
    def displayEigenvectorsOnSets(self, eigenMode, setsToDisplay, fileName=None,defFScale=0.0):
        '''displays the reactions as vector on affected nodes

        :param eigenMode: mode to which the eigenvectors belong.
        :param setsToDisplay: sets to display on batch mode.
        :param fileName:  name of the file to plot the graphic. Defaults to None,
                       in that case an screen display is generated
        :param defFScale: factor to apply to current displacement of nodes 
                so that the display position of each node equals to
                the initial position plus its displacement multiplied
                by this factor. (Defaults to 0.0, i.e. display of 
                initial/undeformed shape)
        '''
        for st in setsToDisplay:
            self.displayEigenvectors(mode= eigenMode, setToDisplay= st,fileName= fileName, defFScale= defFScale)

    def displayFieldDirs1and2(self, limitStateLabel, argument, component, setToDisplay, fileName, defFScale=0.0, rgMinMax=None):
        '''Display a field defined over bi-dimensional elements in its two 
           directions.

        :param limitStateLabel: label that identifies the limit state.
        :param argument: name of the control var to represent.
        :param component: component of the control var to represent.
        :param setToDisplay: represent the field over those elements.
        :param fileName: file name to store the image. If none -> window on screen.
        :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                    of the scalar field (if any) to be represented  (in units 
                    of calculation, not units of display). All the values 
                    less than vmin are displayed in blue and those greater than vmax 
                    in red (defaults to None)
        '''
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        self.displayField(limitStateLabel, section= 1, argument= argument, component= component, setToDisplay= setToDisplay, fileName= fileName, defFScale= defFScale, rgMinMax= rgMinMax)
        self.displayField(limitStateLabel, section=2, argument= argument, component= component, setToDisplay= setToDisplay, fileName= fileName, defFScale= defFScale, rgMinMax= rgMinMax)
        
    def displayField(self, limitStateLabel, section,argument, component, setToDisplay, fileName, defFScale=0.0, rgMinMax=None,):
        '''Display a field defined over bi-dimensional elements in its two 
           directions.

        :param limitStateLabel: label that identifies the limit state.
        :param section: section to display (1 or 2 or None if the value is not section dependent).
        :param argument: name of the control var to represent.
        :param component: component of the control var to represent.
        :param setToDisplay: represent the field over those elements.
        :param fileName: file name to store the image. If none -> window on screen.
        :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                    of the scalar field (if any) to be represented  (in units 
                    of calculation, not units of display). All the values 
                    less than vmin are displayed in blue and those greater than vmax 
                    in red (defaults to None)
        '''
        sectRef= ''
        sectDescr= ''
        if(section):
            if section not in [1,2]:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; section '+str(section)+" doesn't exist, section 1 is displayed instead")
                section=1
            sectRef='Sect'+str(section)
            sectDescr= self.outputStyle.directionDescription[section-1]
  
        if(setToDisplay is None):
            setToDisplay= self.modelSpace.getTotalSet()
        
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        displaySettings.cameraParameters= self.getCameraParameters()
        attributeName= limitStateLabel + sectRef
        fUnitConv, unitDescription= self.outputStyle.getUnitParameters(argument)

        field= fields.getScalarFieldFromControlVar(attributeName= attributeName,argument= argument, xcSet= setToDisplay, component= component, fUnitConv= fUnitConv, rgMinMax= rgMinMax)
        captionTexts= self.outputStyle.getCaptionTextsDict()
        limitStateLabelCaption= captionTexts[limitStateLabel]
        if(argument in captionTexts):
            argumentCaption= captionTexts[argument]
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; caption for argument: '+str(argument)+" not found, we leave it empty.")
            argumentCaption= ''
        captionBaseText= limitStateLabelCaption + ', ' + argumentCaption+ ', ' + unitDescription + '. '+ setToDisplay.description.capitalize()
        field.display(displaySettings,caption=  captionBaseText + ', ' + sectDescr, fileName= fileName, defFScale= defFScale)

def insertGrInTex(texFile, grFileNm, grWdt, capText, labl=''):
    '''Include a graphic in a LaTeX file.

    :param texFile:    laTex file where to include the graphics 
                       (e.g.\:'text/report_loads.tex')
    :param grFileNm:   name of the graphic file with path and without extension
    :param grWdt:      width to be applied to graphics
    :param capText:    text for the caption
    :param labl:       label
    '''
    texFile.write('\\begin{figure}[ht]\n')
    texFile.write('\\begin{center}\n')
    texFile.write('\\includegraphics[width='+grWdt+']{'+grFileNm+'}\n')
    texFile.write('\\caption{'+capText+'}\n')
    if(labl!=''):
        texFile.write('\\label{'+labl+'}\n')
    texFile.write('\\end{center}\n')
    texFile.write('\\end{figure}\n')
    return
