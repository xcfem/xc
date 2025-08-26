# -*- coding: utf-8 -*-
''' Parameters used in the output routines.'''

from __future__ import print_function
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2019, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import locale
#from misc_utils import log_messages as lmsg
from postprocess import output_units
from postprocess.xcVtk import vtk_graphic_base

class OutputStyle(object):
    ''' Pararameters used in the output routines (graphics, etc.)

    :ivar cameraParameters: camera position and orientation.
    :ivar backgroundColor: color for the graphic background.
    :ivar lineWidth: width of the displayed lines in screen units.
    :ivar outputUnits: output units and unit conversion.
    :ivar constraintsScaleFactor: scale factor to display DOF constraints.
    :ivar localAxesVectorScaleFactor: scale factor to display local axes vectors.
    :ivar reactionVectorScaleFactor: scale factor to display reactions.
    :ivar eigenvectorScaleFactor: scale factor to display eigenvectors.
    :ivar equivalentLoadVectorsScaleFactor: factor to apply to the vectors 
             length in the representation of equivalent loads on nodes. If 
             vectorEqLoadScale equals 0, equivalent static loads are not 
             represented.
             (defaults to 0.0 in which case only deformed shape is depicted)
    :ivar loadVectorsScaleFactor: factor to apply to the vectors length in the 
                                 representation of element loads (defaults to 1).
    :ivar loadDiagramsScaleFactor:  scale factor to apply to the diagram display 
                                    of element loads (defaults to 1).
    :ivar showLoadsPushing: true if the vector ends in the loaded node (push
                            the node).
    :iver nodalLoadBarOrientation: orientation of scale bar for nodal loads
                                   'H' -> horizontal, 'RV' -> right-vertical
                                   'LV' left-vertical
    :ivar multByElemSizeLoads: boolean value that must be True if we want to 
                               represent the total load on each element 
                               (=load multiplied by element size -length, 
                               area or volume-) and False if we are going 
                               to depict the value of the uniform load 
                               per unit area (defaults to False)
    :ivar language: english, spanish, french 
    :ivar directionDescr: text list to identify each direction (as ['vertical 
                          reinforcement', 'horizontal reinforcement']).
    '''
    def __init__(self, cameraParameters= None, outputUnits= output_units.OutputUnits(), constraintsScaleFactor= 0.4, localAxesVectorScaleFactor= 0.25, language= None, backgroundColor= vtk_graphic_base.defaultBackgroundColorRGB, lineWidth= None):
        '''Defines the dimension of the space and the number 
         of DOFs for each node.

        :param cameraParameters: camera position and orientation.
        :param outputUnits: output units and unit conversion.
        :param constraintsScaleFactor: scale factor to display DOF constraints.
        :param localAxesVectorScaleFactor: scale factor to display local axes vectors.
        :param language: english, spanish, french 
        :param backgroundColor: (red, green, blue) components of the background
                                color.
        :param lineWidth: width of the displayed lines in screen units.
        '''
        # Graphic stuff.
        self.cameraParameters= cameraParameters
        self.outputUnits= outputUnits
        self.constraintsScaleFactor= constraintsScaleFactor
        self.localAxesVectorsScaleFactor= localAxesVectorScaleFactor
        self.reactionVectorsScaleFactor= 1.0
        self.eigenvectorsScaleFactor= 1.0
        self.equivalentLoadVectorsScaleFactor= 0.0
        self.internalForcesDiagramScaleFactor= 1.0
        self.loadVectorsScaleFactor= 1.0
        self.loadDiagramsScaleFactor= 1.0
        self.nodalLoadBarOrientation= 2 #right-vertical
        self.showLoadsPushing= True
        self.multLoadsByElemSize=False
        if(language):
            self.language= language
        else:
            self.language= locale.getdefaultlocale()[0][:2]
        self.directionDescription= ['dir. 1', 'dir. 2']
        self.backgroundColor= backgroundColor
        self.lineWidth= lineWidth

    def setBackgroundColor(self, rgbComponents):
        ''' Sets the background color for the renderer.

        :param rgbComponents: (red, green, blue) components of the background
                              color.
        '''
        self.backgroundColor= rgbComponents

    def getBackgroundColor(self):
        ''' Return the background color for the renderer.'''
        return self.backgroundColor

    def setLineWidth(self, lineWidth):
        ''' Set the width for the displayed lines.

        :param lineWidth: width of the lines in screen units.
        '''
        self.lineWidth= lineWidth

    def getLineWidth(self):
        ''' Return the value of the width for the displayed lines.'''
        return self.lineWidth
    
    def getStrainUnitsScaleFactor(self):
        ''' Return the scale factor for the strain units.'''
        return self.outputUnits.getStrainUnitsScaleFactor()
    
    def getStrainUnitsDescription(self):
        ''' Return the description for the strain units.'''
        return self.outputUnits.getStrainUnitsDescription()
    
    def getDisplacementUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.outputUnits.getDisplacementUnitsScaleFactor()
    
    def getDisplacementUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.outputUnits.getDisplacementUnitsDescription()
    
    def getRotationUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.outputUnits.getRotationUnitsScaleFactor()
    
    def getRotationUnitsDescription(self):
        ''' Return the descrioption for the displacement units.'''
        return self.outputUnits.getRotationUnitsDescription()

    def getForceUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.outputUnits.getForceUnitsScaleFactor()
    
    def getForceUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.outputUnits.getForceUnitsDescription()
    
    def getLengthUnitSymbol(self):
        ''' Return the symbol for the length units.'''
        return self.outputUnits.getLengthUnitSymbol()
    
    def getLengthUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.outputUnits.getLengthUnitsScaleFactor()
    
    def getLengthUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.outputUnits.getLengthUnitsDescription()
    
    def getForceUnitSymbol(self):
        ''' Return the symbol for the force units.'''
        return self.outputUnits.getForceUnitSymbol()
    
    def getMomentUnitSymbol(self):
        ''' Return the symbol for the moment units.'''
        return self.outputUnits.getMomentUnitSymbol()
    
    def getPressureUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.outputUnits.getPressureUnitsScaleFactor()
    
    def getPressureUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.outputUnits.getPressureUnitsDescription()

    def getUnitParameters(self, itemToDisp):
        ''' Return the pair (scale factor, description) for
            the argument parameter.

        :param itemToDisp: item to display (uX, uY, N, Mz,...).
        '''
        if(itemToDisp=='uX' or itemToDisp=='uY' or itemToDisp=='uZ'):
            fConvUnits= self.getDisplacementUnitsScaleFactor()
            unitDescription= self.getDisplacementUnitsDescription()
        elif(itemToDisp=='rotX' or itemToDisp=='rotY' or itemToDisp=='rotZ'):
            fConvUnits= self.getRotationUnitsScaleFactor()
            unitDescription= self.getRotationUnitsDescription()
        if(itemToDisp=='dispMax'):
            fConvUnits= self.getDisplacementUnitsScaleFactor()
            unitDescription= self.getDisplacementUnitsDescription()
        elif(itemToDisp=='wk' or itemToDisp=='s_rmax'):
            fConvUnits= self.getDisplacementUnitsScaleFactor()
            unitDescription= self.getDisplacementUnitsDescription()
        elif(itemToDisp=='CF' or itemToDisp=='getCF'): # capacity factor
            fConvUnits= 1.0
            unitDescription= ''
        elif(itemToDisp.endswith('_cf')): # capacity factor.
            fConvUnits= 1.0
            unitDescription= ''
        elif(itemToDisp.startswith('sigma_')):
            fConvUnits= self.getPressureUnitsScaleFactor()
            unitDescription= self.getPressureUnitsDescription()
        elif(itemToDisp.endswith('stress')):
            fConvUnits= self.getPressureUnitsScaleFactor()
            unitDescription= self.getPressureUnitsDescription()
        elif(itemToDisp.endswith('getMaxSteelStress')):
            fConvUnits= self.getPressureUnitsScaleFactor()
            unitDescription= self.getPressureUnitsDescription()
        elif(itemToDisp.startswith('epsilon_')):
            fConvUnits= 1.0
            unitDescription= '-'
        elif(itemToDisp.startswith('chi')): # strength reduction factors
            fConvUnits= 1.0
            unitDescription= '-'
        elif(itemToDisp=='Leff' or itemToDisp=='LeffY' or  itemToDisp=='LeffZ'):
            fConvUnits= self.getLengthUnitsScaleFactor()
            unitDescription= self.getLengthUnitsDescription()
        elif(itemToDisp=='mechLambda' or itemToDisp=='mechLambdaY' or  itemToDisp=='mechLambdaZ'):
            fConvUnits= 1.0
            unitDescription= '-'
        elif(itemToDisp=='ef' or itemToDisp=='efY' or  itemToDisp=='efZ'):
            fConvUnits= self.getLengthUnitsScaleFactor()
            unitDescription= self.getLengthUnitsDescription()
        elif(itemToDisp=='mode'):
            fConvUnits= 1.0
            unitDescription= '-'
        elif(itemToDisp=='alpha_cr'): # factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)).
            fConvUnits= 1.0
            unitDescription= ''
        elif(itemToDisp=='strongAxisBucklingPercent'):
            fConvUnits= 1.0
            unitDescription= 'Dot product (projection) of the buckling eigenvector over the strong axis.'
        else:
            fConvUnits= self.getForceUnitsScaleFactor()
            unitDescription= self.getForceUnitsDescription()
        return fConvUnits, unitDescription
    
    def getCaptionTextsDict(self):
        ''' Return a dictionary with the caption texts to use 
            in graphic output.'''
        retval= sp_capTexts
        lang_id= self.language[:2].lower()
        if lang_id=='en':
            retval= en_capTexts
        elif lang_id=='fr':
            retval= fr_capTexts
        return retval

defaultOutputStyle= OutputStyle()
    
#Spanish caption texts
sp_capTexts={
    'uX':'desplazamiento en dirección X',
    'uY':'desplazamiento en dirección Y',
    'uZ':'desplazamiento en dirección Z',
    'rotX':"rotación en torno al eje X",
    'rotY':"rotación en torno al eje Y",
    'rotZ':"rotación en torno al eje Z",
    'CF':'factor de capacidad',
    'getCF':'factor de capacidad',
    'N':'esfuerzo axil asociado al factor de capacidad',
    'N1':'esfuerzo axil dirección 1',
    'N2':'esfuerzo axil dirección 2',
    'M1':'momento flector dirección 1',
    'M2':'momento flector dirección 2',
    'Q1':'esfuerzo cortante dirección 1',
    'Q2':'esfuerzo cortante dirección 2',
    'Qy':'esfuerzo cortante dirección y',
    'Qz':'esfuerzo cortante dirección z',
    #'Vy':'esfuerzo cortante dirección y',
    #'Vz':'esfuerzo cortante dirección z',
    'My':'momento flector My asociado al factor de capacidad',
    'Mz':'momento flector Mz asociado al factor de capacidad',
    'Mu':'valor último del momento flector',
    'theta':'',
    'Vy':'esfuerzo cortante Vy asociado al factor de capacidad',
    'Vz':'esfuerzo cortante Vz asociado al factor de capacidad',
    'Vcu':'Vcu',
    'Vsu':'Vsu',
    'Vu':'valor último del esfuerzo cortante',
    'LocalAxes': 'ejes locales',
    'wk': 'apertura de fisura',
    'FEmesh': 'malla de elementos',
    'ULS_normalStressesResistance': 'Comprobación ELU tensiones normales',
    'normalStressCheck': 'Comprobación ELU tensiones normales',
    'ULS_shearResistance': 'Comprobación ELU esfuerzo cortante',
    'ULS_VonMisesStressResistance': 'Comprobación ELU tensiones de Von Mises',
    'ULS_Ec2InPlaneStressResistance': 'Comprobación ELU de armaduras en elementos en tensión plana.',
    'getMaxSteelStress': "tensión máxima en la armadura",
    'SLS_rareLoadsCrackControl': 'Comprobación ELS fisuración, casos de carga raros',
    'SLS_frequentLoadsCrackControl': 'Comprobación ELS fisuración, casos de carga frecuentes',
    'SLS_quasiPermanentLoadsCrackControl': 'Comprobación ELS fisuración, casos de carga quasi-permanentes',
    'vm_stress': 'tensión de Von Mises',
    'wk': 'abertura de fisura',
    's_rmax': 'separación entre fisuras',
    'sigma_s': 'máxima tensión en la armadura',
    'sigma_c': 'máxima tensión de compresión en el hormigón',
    'Leff': 'longitud de pandeo.',
    'mechLambda': 'esbeltez mecánica.',
    'ef': 'excentricidad adicional por pandeo.',
    'LeffZ': 'longitud de pandeo (eje Z).',
    'LeffY': 'longitud de pandeo (eje Y).',
    'mechLambdaZ': 'esbeltez mecánica (eje Z).',
    'mechLambdaY': 'esbeltez mecánica (eje Y).',
    'efZ': 'excentricidad adicional por pandeo (eje Z).',
    'efY': 'excentricidad adicional por pandeo (eje Y).',
    'mode': 'modo de pandeo.',
    'alpha_cr': 'Ncr/N'
  }

#English caption texts
en_capTexts={
    'CF': 'efficiency',
    'getCF':'efficiency',
    'uX':'displacement in global X direction',
    'uY':'displacement in global Y direction',
    'uZ':'displacement in global Z direction',
    'rotX':"rotation about global X axis",
    'rotY':"rotation about global Y axis",
    'rotZ':"rotation about global Z axis",
    'N1':'internal axial force in local direction 1',
    'N2':'internal axial force in local direction 2',
    'M1':'bending moment about local axis 1',
    'M2':'bending moment about local axis 2',
    'Q1':'internal shear force in local direction 1',
    'Q2':'internal shear force in local direction 2',
    'N':'internal axial force',
    'Qy':'internal shear force in local direction y',
    'Qz':'internal shear force in local direction z',
    'Vy':'internal shear force in local direction y',
    'Vz':'internal shear force in local direction z',
    'Vcu':'Vcu',
    'Vsu':'Vsu',
    'Vu':'shear strength',
    'My':'bending moment about local axis y',
    'Mz':'bending moment about local axis z',
    'T':'internal torsional moment',
    'FEmesh': 'FE mesh',
    'ULS_normalStressesResistance': 'ULS normal stresses check',
    'ULS_shearResistance': 'ULS shear check',
    'ULS_VonMisesStressResistance': 'ULS Von Mises stresses check',
    'ULS_Ec2InPlaneStressResistance': 'ULS tension reinforcement check for in-plane stress conditions.',
    'getMaxSteelStress': 'steel maximum stress',
    'SLS_rareLoadsCrackControl': 'SLS cracking, rare actions',
    'SLS_frequentLoadsCrackControl': 'SLS cracking, frequent actions',
    'SLS_quasiPermanentLoadsCrackControl': 'SLS cracking, quasi-permanent actions',
    'LocalAxes': 'local axes',
    'wk': 'crack width',
    's_rmax': 'maximum crack distance',
    'eps_sm': 'mean steel deformation over maximum crack distance',
    'Ncrd': 'design resistance to axial compression',
    'McRdy': 'design moment resistance about Y (weak) axis',
    'McRdz': 'design moment resistance about Z (strong) axis',
    'MvRdz': 'reduced design moment resistance about Z (strong) axis for shear interaction',
    'MbRdz':'reduced design moment resistance about Z (strong) axis for lateral-torsional bucking',
    'chiLT': 'reduction factor for lateral-torsional buckling ',
    'vm_stress': 'Von Mises stress',
    'sigma_s': 'max. steel stress',
    'sigma_c': 'max. concrete compressive-stress',
    'Leff': 'effective length for buckling.',
    'mechLambda': 'mechanical slenderness for buckling.',
    'ef': 'additional eccentricity for buckling.',
    'LeffZ': 'effective length for buckling around Z axis.',
    'LeffY': 'effective length for buckling around Y axis.',
    'mechLambdaZ': 'mechanical slenderness for buckling around Z axis.',
    'mechLambdaY': 'mechanical slenderness for buckling around Y axis.',
    'efZ': 'additional eccentricity for buckling around Z axis.',
    'efY': 'additional eccentricity for buckling around Y axis.',
    'mode': 'buckling mode corresponding to the previous parameters.',
    'alpha_cr': 'Ncr/N'
  }

#Caption texts in French
fr_capTexts={
    'uX':'déplacement en direction X',
    'uY':'déplacement en direction Y',
    'uZ':'déplacement en direction Z',
    'rotX':"rotation autour de l'axe X",
    'rotY':"rotation autour de l'axe Y",
    'rotZ':"rotation autour de l'axe Z",
    'CF':'facteur de capacité',
    'getCF':'facteur de capacité',
    'N':'effort normal associé au facteur de capacité',
    'N1':'effort normal direction 1',
    'N2':'effort normal direction 2',
    'M1':'moment de flexion direction 1',
    'M2':'moment de flexion adirection 2',
    'Q1':'effort tranchant direction 1',
    'Q2':'effort tranchant direction 2',
    'Qy':'effort tranchant direction y',
    'Qz':'effort tranchant direction z',
    'My':'moment de flexion associé au facteur de capacité',
    'Mz':'moment de flexion associé au facteur de capacité',
    'Mu':'valeur ultime du moment de flexion',
    'theta':'',
    'Vy':'effort tranchant associé au facteur de capacité',
    'Vz':'effort tranchant associé au facteur de capacité',
    'Vcu':'Vcu',
    'Vsu':'Vsu',
    'Vu':"valeur ultime de l'effort tranchant",
    'LocalAxes': 'axes locaux',
    'wk': 'ouverture de fissure',
    'FEmesh': 'maillage',
    'ULS_normalStressesResistance': 'Vérification ELU contraintes normales',
    'normalStressCheck': 'Vérification ELU contraintes normales',
    'ULS_shearResistance': 'Vérification ELU effort tranchant',
    'ULS_VonMisesStressResistance': 'Vérification ELU contraintes de Von Mises.',
    'ULS_Ec2InPlaneStressResistance': 'Verification ELU des armatures tendues dans les situations de contraintes planes.',
    'getMaxSteelStress': "contrainte maximale dans l'armature",
    'SLS_rareLoadsCrackControl': 'Vérification ELS fissuration, cas de charge rares',
    'SLS_frequentLoadsCrackControl': 'Vérification ELS fissuration, cas de charge fréquents',
    'SLS_quasiPermanentLoadsCrackControl': 'Vérification ELS fissuration, cas de charge quasi-permanents',
    'ULS_fatigueResistance': 'Vérification ELU fatigue',
    'getAbsSteelStressIncrement': "vérification de l'armature. Différence de contrainte $\\delta_{sd}(Q_{fat})$ sous les actions de fatigue",
    'concreteBendingCF':'vérification du béton. Facteur de capacité contraintes de compression',
    'concreteLimitStress':'vérification du béton. Limites contraintes de compression',
    'concreteShearCF':'vérification du béton. Facteur de capacité effort tranchant',
    'shearLimit': 'vérification du béton. Limites effort tranchant',
    'vm_stress': 'contrainte de von Mises',
    'wk': 'overture de fissure',
    'sigma_s':"contrainte maximale dans l'armature",
    'sigma_c': 'contrainte de compression maximale du béton',
    'Leff': 'longueur de flambement.',
    'mechLambda': 'élancement.',
    'ef': 'eccentricité additionnelle.',
    'LeffZ': 'longueur de flambement (axe Z).',
    'LeffY': 'longueur de flambement (axe Y).',
    'mechLambdaZ': 'élancement (axe Z).',
    'mechLambdaY': 'élancement (axe Y).',
    'efZ': 'eccentricité additionnelle (axe Z).',
    'efY': 'eccentricité additionnelle (axe Y).',
    'mode': 'mode de flambage.',
    'alpha_cr': 'Ncr/N'
  }
