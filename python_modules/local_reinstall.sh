#!/bin/sh
#Deletes all the modules previously installed and then installs the new modules.
#This is to solve the problem of zombie modules: Python files that has been
#deleted or replaced but which remains installed in the machine.

echo "Uninstalling modules."
sudo bash ./uninstall
echo "Reinstalling modules."
sh ./local_install.sh

