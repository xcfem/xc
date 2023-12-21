# -*- coding: utf-8 -*-
''' Verify ProjectDirTree class.

'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
from postprocess.config import default_config
from misc_utils import log_messages as lmsg
from pathlib import Path

# Get a default configuration.
cfg= default_config.get_temporary_env_config() # Store results in temporary files.
cfg.projectDirTree.createTree() # Create project directory tree.

testFolderName= 'piles'
newFolder= cfg.projectDirTree.newResultsPath(testFolderName) # Create custom results path.
testOK= (newFolder is not None) # Check result.

foundFolder= cfg.projectDirTree.findResultsPath(testFolderName)
testOK= (testOK and (newFolder==foundFolder)) # Check result.

# Create a new file to test findResultsFile() method
testFileName= 'piles_axial_force.json'
newFileName= newFolder+'/'+testFileName
Path(newFileName).touch() # created.
# Search the file.
foundFile= cfg.projectDirTree.findResultsFile(testFileName)

testOK= (testOK and (newFileName==foundFile))

# print(newFolder)
# print(foundFile, newFileName)

fname= os.path.basename(__file__)
if(testOK):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

cfg.cleandirs() # clean after yourself.
