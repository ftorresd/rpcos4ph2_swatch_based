RPMBUILD_DIR=${PackagePath}/rpm/RPMBUILD
# 
ifndef PythonModules
	$(error Python module names missing "PythonModules")
endif

.PHONY: rpm _rpmall
rpm: _rpmall
_rpmall: _all _setup_update _rpmbuild

# Copy the package skeleton
# Insure the existence of the module directory
# Copy the libraries into python module
.PHONY: _rpmbuild
_rpmbuild: _setup_update
	# Change directory into pkg and copy everything into rpm/pkg
	cd pkg && \
	find . -name "*" -exec install -D \{\} ${RPMBUILD_DIR}/\{\} \;
	# Add a manifest file
	echo "include */*.so" > ${RPMBUILD_DIR}/MANIFEST.in
	# Change into rpm/pkg to finally run the customized setup.py
	cd ${RPMBUILD_DIR} && python ${PackageName}.py bdist_rpm \
	--release ${PACKAGE_RELEASE}.${CACTUS_OS}.python${PYTHON_VERSION} \
	--binary-only --force-arch=`uname -m`
	# Harvest the crop
	find rpm -name "*.rpm" -exec cp -a \{\} rpm/ \;


.PHONY: _setup_update	
_setup_update:
	${MakeDir} ${RPMBUILD_DIR}
	cp ${BUILD_HOME}/config/setupTemplate.py ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__python_packages__#${PythonModules}#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__package__#${Package}#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__packagename__#${PackageName}#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__version__#$(PACKAGE_VER_MAJOR).$(PACKAGE_VER_MINOR).$(PACKAGE_VER_PATCH)#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__release__#${PACKAGE_RELEASE}.${CACTUS_OS}#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__prefix__#${CACTUS_ROOT}#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__packagedir__#${PackagePath}#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__os__#${CACTUS_OS}#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__platform__#None#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__project__#${Project}#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__author__#${Packager}#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__summary__#None#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__description__#None#' ${RPMBUILD_DIR}/${PackageName}.py
	sed -i 's#__url__#None#' ${RPMBUILD_DIR}/${PackageName}.py


.PHONY: cleanrpm _cleanrpm
cleanrpm: _cleanrpm
_cleanrpm:
	-rm -r rpm
