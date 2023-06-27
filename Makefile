#!/usr/bin/env make
# Root Makefile for IronOS - Soldering Iron Open Source Firmware Project


### global adjustable variables

# command for docker-compose tool
ifndef DOCKER_COMPOSE
DOCKER_COMPOSE:=docker-compose
endif

# command for python-based mkdocs tool
ifndef MKDOCS
MKDOCS:=mkdocs
endif


### global static variables

# docker-related files
DOCKER_YML=$(CURDIR)/IronOS.yml
DOCKER_FILE=$(CURDIR)/scripts/IronOS.Dockerfile

# docker dependencies
DOCKER_DEPS=$(DOCKER_YML) $(DOCKER_FILE)

# compose docker-compose command
DOCKER_CMD=$(DOCKER_COMPOSE)  -f $(DOCKER_YML)  run  --rm  builder

# MkDocs config
MKDOCS_YML=$(CURDIR)/scripts/IronOS-mkdocs.yml


### helper targets

# former start_dev.sh
docker-shell: $(DOCKER_DEPS)
	@$(DOCKER_CMD)

# former build.sh
docker-build: $(DOCKER_DEPS)
	@$(DOCKER_CMD)  /bin/bash  /build/ci/buildAll.sh

# delete container
docker-clean:
	-@docker  rmi  ironos-builder:latest

# generate docs in site/ directory (DIR for -d is relative to mkdocs.yml file location, hence use default name/location site by setting up ../site)
docs: $(MKDOCS_YML)  Documentation/*  Documentation/Flashing/*  Documentation/images/*
	@$(MKDOCS)  build  -f $(MKDOCS_YML)  -d ../site

# pass-through target for Makefile inside source/ dir
%:
	make  -C source/  $@

# global clean-up target
clean-full: docker-clean
	make  -C source/  clean-all
	rm  -Rf  site
	rm  -Rf  scripts/ci/artefacts

.PHONY:  docker-shell  docker-build  docker-clean  docs  clean-full
