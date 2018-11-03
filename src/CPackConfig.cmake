#To be tested 11/10/2018 LCPT
SET(CPACK_GENERATOR "DEB")
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "BibXC")

SET(CPACK_DEBIAN_PACKAGE_NAME "BibXC")
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
INCLUDE(${DIR_FUENTES_XC}/CMake/debian_package_params.txt)
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++ (>=4.3), libgcc1 (>=1:4.3), libc6 (>=2.7), libvtk5 (>=5.0), libcgal2 (>=3.3), libplot2c2 (>=2.5), libmpfr1ldbl  (>=2.3), libgmp3c2 (>=4.2), libboost-regex1.34.1 (>=1.34), libmysql++3 (>=3.0), libsqlite3-0 (>=3.5), libxt6  (>=1.0), libsm6  (>=1.0), libxext6  (>=1.0), libice6  (>=1.0), libxft2  (>=2.1), xc_basic_utils (>=0.9), xc_utils (>=0.9)")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Luis C. Pérez Tato") #required
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "XC function library.")
SET(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
 
INCLUDE(CPack)

