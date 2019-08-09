# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
from postprocess import limit_state_data as lsd


class envConfig(object):
    '''Generic configuration of environment variables.
    '''
    def __init__(self,language,intForcPath,verifPath,annexPath,grWidth='120mm'):
        '''
        :ivar language: english, spanish, french 
        :ivar intForcPath: full path of the directory where results of 
                      internal forces are placed.
        :ivar verifPath: full path of the directory where results of 
                      limit state  verifications are placed
        :ivar annexPath: full path of the directory where to place graphic and 
                      text files for the generation of the annex
        :ivar grWidth: size of the graphics to be included in the annex
                            
        '''
        #default names of files with data for FE model generation, results of
        #limit state verifications, ..

        lsd.LimitStateData.internal_forces_results_directory= intForcPath
        lsd.LimitStateData.check_results_directory= verifPath
        
        self.verifNormStrFile=verifPath+'verifRsl_normStrsULS.py'
        self.reportNormStrFile=annexPath+'text/report_normStrsULS.tex'
        self.reportNormStrGrPath=annexPath+'text/graphics/normStrsULS/'
        
        self.verifShearFile=verifPath+'verifRsl_shearULS.py'
        self.reportShearFile=annexPath+'text/graphics/report_shearULS.tex'
        self.reportShearGrPath=annexPath+'text/graphics/shearULS/'
        
        self.verifCrackFreqFile=verifPath+'verifRsl_crackingSLS_freq.py'
        self.reportCrackFreqFile=annexPath+'text/report_crackingSLS_freq.tex'
        self.reportCrackFreqGrPath=annexPath+'text/graphics/crackingSLS_freq/' 
        
        self.verifCrackQpermFile=verifPath+'verifRsl_crackingSLS_qperm.py'
        self.reportCrackQpermFile=annexPath+'text/report_crackingSLS_qperm.tex'
        self.reportCrackQpermGrPath=annexPath+'text/graphics/crackingSLS_qperm/' 
        
        self.verifFatigueFile=verifPath+'verifRsl_fatigueULS.py'
        self.reportFatigueFile=annexPath+'text/report_fatigueStrsULS.tex' 
        self.reportFatigueGrPath=annexPath+'text/graphics/fatigueStrsULS/'

        self.reportSimplLCFile=annexPath+'text/report_resSimplLC.tex'
        self.reportSimplLCGrPath=annexPath+'text/graphics/resSimplLC/'

        self.capTexts=sp_capTexts
        if language[:2].lower()=='en':
            self.capTexts=en_capTexts
        elif language[:2].lower()=='fr':
            self.capTexts=fr_capTexts
        self.colors=setBasicColors
        self.grWidth=grWidth


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
    'N':'esfuerzo normal asociado al factor de capacidad',
    'N1':'esfuerzo normal dirección 1',
    'N2':'esfuerzo normal dirección 2',
    'M1':'momento flector dirección 1',
    'M2':'momento flector dirección 2',
    'Q1':'esfuerzo cortante dirección 1',
    'Q2':'esfuerzo cortante dirección 2',
    'Qy':'esfuerzo cortante dirección y',
    'Qz':'esfuerzo cortante dirección z',
    'Vy':'esfuerzo cortante dirección y',
    'Vz':'esfuerzo cortante dirección z',
    'My':'momento flector asociado al factor de capacidad',
    'Mz':'momento flector asociado al factor de capacidad',
    'Mu':'valor último del momento flector',
    'theta':'',
    'Vy':'esfuerzo cortante asociado al factor de capacidad',
    'Vz':'esfuerzo cortante asociado al factor de capacidad',
    'Vcu':'',
    'Vsu':'',
    'Vu':"valor último del esfuerzo cortante",
    'LocalAxes': 'ejes locales',
    'FEmesh': 'malla de elementos',
    'ULS_normalStressesResistance': 'Comprobación ELU tensiones normales',
    'normalStressCheck': 'Comprobación ELU tensiones normales',
    'ULS_shearResistance': 'Comprobación ELU esfuerzo cortante',
    'getMaxSteelStress': "tensión máxima en la armadura",
    'SLS_frequentLoadsCrackControl': 'Comprobación ELS fisuración, casos de carga frecuentes',
    'SLS_quasiPermanentLoadsLoadsCrackControl': 'Comprobación ELS fisuración, casos de carga quasi-permanentes',
    
  }

#English caption texts
en_capTexts={
    'CF': 'efficiency',
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
    'My':'bending moment about local axis y',
    'Mz':'bending moment about local axis z',
    'T':'internal torsional moment',
    'FEmesh': 'FE mesh',
    'ULS_normalStressesResistance': 'ULS normal stresses check',
    'ULS_shearResistance': 'ULS shear check',
    'getMaxSteelStress': 'steel maximum stress',
    'SLS_frequentLoadsCrackControl': 'SLS cracking, frequent actions',
    'SLS_quasiPermanentLoadsLoadsCrackControl': 'SLS cracking, quasi-permanent actions',
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
    'Vcu':'',
    'Vsu':'',
    'Vu':"valeur ultime de l'effort tranchant",
    'LocalAxes': 'axes locaux',
    'FEmesh': 'maillage',
    'ULS_normalStressesResistance': 'Vérification ELU contraintes normales',
    'normalStressCheck': 'Vérification ELU contraintes normales',
    'ULS_shearResistance': 'Vérification ELU effort tranchant',
    'getMaxSteelStress': "contrainte maximale dans l'armature",
    'SLS_frequentLoadsCrackControl': 'Vérification ELS fissuration, cas de charge fréquents',
    'SLS_quasiPermanentLoadsLoadsCrackControl': 'Vérification ELS fissuration, cas de charge quasi-permanents',
    'ULS_fatigueResistance': 'Vérification ELU fatigue',
    'getAbsSteelStressIncrement': "vérification de l'armature. Différence de contrainte $\delta_{sd}(Q_{fat})$ sous les actions de fatigue",
    'concreteBendingCF':'vérification du béton. Facteur de capacité contraintes de compression',
    'concreteLimitStress':'vérification du béton. Limites contraintes de compression',
    'concreteShearCF':'vérification du béton. Facteur de capacité effort tranchant',
    'shearLimit': 'vérification du béton. Limites effort tranchant',
    
  }

#Predefined colors for sets (progressing from light to dark)

setBasicColors={
    'yellow01':xc.Vector([1,1,0]),
    'yellow02':xc.Vector([1,0.9,0]),
    'orange01':xc.Vector([1,0.8,0.5]),
    'orange02':xc.Vector([1,0.7,0]),
    'orange03':xc.Vector([1,0.6,0]),
    'pink01':xc.Vector([1,0.6,1.0]),
    'pink02':xc.Vector([1,0.2,1]),
    'pink03':xc.Vector([1,0,0.7]),
    'red01':xc.Vector([1,0.1,0.1]),
    'red02':xc.Vector([1,0.2,1]),
    'red03':xc.Vector([1,0,0]),
    'red04':xc.Vector([0.9,0,0]),
    'green01':xc.Vector([0.8,1,0.4]),
    'green02':xc.Vector([0.7,0.9,0.1]),
    'green03':xc.Vector([0.8,0.8,0.1]),
    'green04':xc.Vector([0.6,0.5,0.1]),
    'brown04':xc.Vector([0.5,0.,0.]),
    'brown03':xc.Vector([0.5,0.1,0.]),
    'brown02':xc.Vector([0.5,0.3,0.1]),
    'brown01':xc.Vector([0.6,0.3,0.]),
    'purple04':xc.Vector([0.6,0.2,1.]),
    'purple03':xc.Vector([0.7,0.2,0.7]),
    'purple02':xc.Vector([0.7,0.4,0.7]),
    'purple01':xc.Vector([0.7,0.7,0.8]),
    'blue01':xc.Vector([0.2,1.,1.]),
    'blue02':xc.Vector([0.,0.8,1]),
    'blue03':xc.Vector([0.,0.4,1]),
    'blue04':xc.Vector([0.,0.,0.9])     
  }
