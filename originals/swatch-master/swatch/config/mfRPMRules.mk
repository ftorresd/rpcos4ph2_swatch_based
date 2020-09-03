RPMBUILD_DIR=${PackagePath}/rpm/RPMBUILD


BUILD_REQUIRES_TAG = $(if ${PackageBuildRequires} ,BuildRequires: ${PackageBuildRequires} ,\# No BuildRequires tag )
REQUIRES_TAG = $(if ${PackageRequires} ,Requires: ${PackageRequires} ,\# No Requires tag )


.PHONY: rpm _rpmall
rpm: _rpmall
_rpmall: _all _spec_update _rpmbuild

.PHONY: _rpmbuild
_rpmbuild: _spec_update
	mkdir -p ${RPMBUILD_DIR}/{RPMS/{i386,i586,i686,x86_64},SPECS,BUILD,SOURCES,SRPMS}
	rpmbuild --quiet -bb -bl --buildroot=${RPMBUILD_DIR}/BUILD --define  "_topdir ${RPMBUILD_DIR}" rpm/${PackageName}.spec
	find  ${RPMBUILD_DIR} -name "*.rpm" -exec mv {} $(PackagePath)/rpm \;

.PHONY: _spec_update	
_spec_update:
	mkdir -p ${PackagePath}/rpm
	cp ${BUILD_HOME}/config/specTemplate.spec ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__package__#${Package}#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__packagename__#${PackageName}#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__version__#$(PACKAGE_VER_MAJOR).$(PACKAGE_VER_MINOR).$(PACKAGE_VER_PATCH)#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__release__#${PACKAGE_RELEASE}.${CACTUS_OS}#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__prefix__#${CACTUS_ROOT}#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__sources_dir__#${RPMBUILD_DIR}/SOURCES#' ${PackagePath}/rpm/${PackageName}.spec
ifeq ("${HtDocsPath}","")
	sed -i 's#.*_htdocspath.*##' ${PackagePath}/rpm/${PackageName}.spec
else
	sed -i 's#__htdocspath__#${HtDocsPath}#' ${PackagePath}/rpm/${PackageName}.spec
endif
	sed -i 's#__packagedir__#${PackagePath}#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__os__#${CACTUS_OS}#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__platform__#None#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__project__#${Project}#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__author__#${Packager}#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__summary__#None#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__description__#None#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__url__#None#' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's#__includedirs__#$(Includes)#' $(PackagePath)/rpm/$(PackageName).spec
	sed -i 's|^.*__build_requires__.*|${BUILD_REQUIRES_TAG}|' ${PackagePath}/rpm/${PackageName}.spec
	sed -i 's|^.*__requires__.*|${REQUIRES_TAG}|' ${PackagePath}/rpm/${PackageName}.spec

.PHONY: cleanrpm _cleanrpm
cleanrpm: _cleanrpm
_cleanrpm:
	-rm -r rpm

