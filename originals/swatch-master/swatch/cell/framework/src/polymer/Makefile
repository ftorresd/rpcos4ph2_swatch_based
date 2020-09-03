
.PHONY: default clean _cleanall all _all build _buildall rpm _rpmall

default: build


clean: _cleanall
_cleanall:
	rm -rf node_modules


all: _all
build: _all
_buildall: _all

_all:
	npm install
	grunt


rpm: _rpmall


