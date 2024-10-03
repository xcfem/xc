# Source: https://github.com/Changaco/version.py

import xc
import json
import os
import subprocess

jsonFileName= "last_xc_version.json"

def get_git_revision_short_hash() -> str:
    return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip()

if os.path.isfile(jsonFileName):
    with open(jsonFileName, "r") as fr:
        last_xc_version_dict= json.load(fr)
else:
    last_xc_version_dict= {'last_xc_git_version':str(r"v0.12-7509-g26741dd58-d"), 'beta':0, 'last_git_short_hash':0}
    with open(jsonFileName, "w") as fw:
        json.dump(last_xc_version_dict, fw)

def get_xc_version():
    xc_git_version= xc.getXCVersion()
    last_xc_git_version= last_xc_version_dict['last_xc_git_version']
    beta= int(last_xc_version_dict['beta'])
    if(xc_git_version != last_xc_git_version):
        beta+= 1
    xc_git_short_hash= get_git_revision_short_hash()
    last_xc_git_short_hash= last_xc_version_dict['last_git_short_hash']
    if(xc_git_short_hash != last_xc_git_short_hash):
        beta+= 1
    components= xc_git_version.split('-')
    major_minor= components[0].split('.')
    major= major_minor[0][1:]
    minor= major_minor[1]
    retval= major+'.'+minor+'.'+str(beta)
    return retval

if __name__ == '__main__':
    print(get_xc_version())
