# -*- coding: utf-8 -*-
''' File names to store calculation results.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2024,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

# Simple load cases.
simpleLoadCaseReportFile= 'report_resSimplLC'
simpleLoadCaseResultsDirectory= 'resSimplLC/'

# Normal stresses.
normalStressesVerificationResultsFile= 'verifRsl_normStrsULS'
normalStressesVerificationReportFile= 'report_normStrsULS'
normalStressesDirectory= 'normStrsULS/'

# Buckling.
bucklingVerificationResultsFile= 'verifRsl_bucklingULS'
bucklingVerificationReportFile= 'report_bucklingULS'
bucklingDirectory= 'bucklingULS/'

# Shear.
shearVerificationResultsFile= 'verifRsl_shearULS'
shearVerificationReportFile= 'report_shearULS'
shearDirectory= 'shearULS/'

# Torsion.
torsionVerificationResultsFile= 'verifRsl_torsionULS'
torsionVerificationReportFile= 'report_torsionULS'
torsionDirectory= 'torsionULS/'

# Von Mises stresses.
vonMisesStressesVerificationResultsFile= 'verifRsl_VonMisesStressULS'
vonMisesStressesVerificationReportFile= 'report_VonMisesStressULS'
vonMisesStressesDirectory= 'VonMisesStressULS/'

# Crack control.
crackControlVerificationResultsFile= 'verifRsl_crackingSLS'
crackControlVerificationReportFile= 'report_crackingSLS'
crackControlDirectory= 'crackingSLS'

# Crack control. Rare loads.
crackControlRareVerificationResultsFile= crackControlVerificationResultsFile+'_rare'
crackControlRareVerificationReportFile= crackControlVerificationReportFile+'_rare'
crackControlRareDirectory= crackControlDirectory+'_rare/'

# Crack control. Frequent loads.
crackControlFreqVerificationResultsFile= crackControlVerificationResultsFile+'_freq'
crackControlFreqVerificationReportFile= crackControlVerificationReportFile+'_freq'
crackControlFreqDirectory= crackControlDirectory+'_freq/'

# Crack control. Quasi-permanent loads.
crackControlQpermVerificationResultsFile= crackControlVerificationResultsFile+'_qperm'
crackControlQpermVerificationReportFile= crackControlVerificationReportFile+'_qperm'
crackControlQpermDirectory= crackControlDirectory+'_qperm/'

# Fatigue.
fatigueVerificationResultsFile= 'verifRsl_fatigueULS'
fatigueVerificationReportFile= 'report_fatigueULS'
fatigueDirectory= 'fatigueULS/'
