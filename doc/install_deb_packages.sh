# Must run as root so that we can shutdown backuppc and mount drives
if [ $(whoami) != "root" ]; then
	echo "You must run this script as root."
	echo "Use 'sudo sh install_deb_packages.sh' and enter the password when prompted."
	exit 1
fi

echo "Some packages are in the "contrib" and "non-free" areas of the Debian distribution so these areas should be included in the sources.list file before running this script."

# Verificar que el usuario desea continuar
read -p "Continue (y/n)?" REPLY
if [ $REPLY != "y" ]; then
	echo "Exiting..."
	exit 1
fi

#Fixed some bugs detected by Bernd Hahnebach; thanks! (1/10/2018 LCPT):

apt-get install cmake cimg-dev g++ gfortran gnuplot libarpack2-dev libarpack++2-dev libcgal-dev libdb-dev libf2c2-dev libglib2.0-dev libgmp3-dev libgtk2.0-dev libgtkgl2.0-dev libgtkglextmm-x11-1.2-dev libgtkmm-2.4-dev libgts-bin libgts-dev liblapack-dev libmpfr-dev libmysql++-dev libparmetis-dev libplot-dev libsqlite3-dev libsuperlu-dev libsuitesparse-dev libvtk6-dev libx11-dev petsc-dev tcl-dev python-vtk6 python-scipy python-sympy python-matplotlib python-pandas python-sklearn python-pip


#mayavi installation. Some 'mayavi' packages seems
#to require VTK 6 so we use pip. If you're a Debian user
#you can help us with this sending us your comments.
# (to reconsider because we already use VTK 6 LCPT 24/09/2018)

sudo -H pip install mayavi
