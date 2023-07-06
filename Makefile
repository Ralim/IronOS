#!/usr/bin/env make
INFO:=top-level Makefile for IronOS - Soldering Iron Open Source Firmware Project.


### global adjustable variables

# command for "docker compose" from DOCKER env. var.
ifdef DOCKER
DOCKER_BIN:=$(DOCKER)
else
DOCKER_BIN:=
endif

# detect availability of docker
ifndef DOCKER_BIN
DOCKER_COMPOSE:=$(shell command -v docker-compose 2>/dev/null)
DOCKER_TOOL:=$(shell command -v docker 2>/dev/null)
ifdef DOCKER_COMPOSE
DOCKER_BIN:=$(DOCKER_COMPOSE)
else ifdef DOCKER_TOOL
DOCKER_BIN:=$(DOCKER_TOOL) compose
endif # DOCKER_* checks
endif # DOCKER_BIN

# command for python-based mkdocs tool
ifndef MKDOCS
MKDOCS:=mkdocs
endif


### global static variables

# docker-related files
DOCKER_YML=$(CURDIR)/Env.yml
DOCKER_FILE=$(CURDIR)/scripts/IronOS.Dockerfile

# docker dependencies
DOCKER_DEPS=$(DOCKER_YML) $(DOCKER_FILE)

# compose docker-compose command
DOCKER_CMD=$(DOCKER_BIN)  -f $(DOCKER_YML)  run  --rm  builder

# MkDocs config
MKDOCS_YML=$(CURDIR)/scripts/IronOS-mkdocs.yml


### targets

# default target to show help
help:
	@echo
	@echo "Welcome!\nThis is $(INFO)"
	@echo "To read more about supported commands (aka \"targets\"), type \"make list\"."
	@echo "But if you're impatient then just type \"make docker-build\" - it will:"
	@echo "\t * download, configure & start docker container"
	@echo "\t * compile builds of IronOS firmware for all supported models inside that container"
	@echo "\t * export generated binaries to \"scripts/ci/artefacts/\" local directory"
	@echo "Patches are welcome. Happy Hacking!"
	@echo

# target to list supported targets with additional info
list:
	@echo ""
	@echo "Supported top-level targets:"
	@echo "\t * help         - shows short basic help"
	@echo "\t * list         - this output"
	@echo "\t * docker-shell - start docker container with shell inside to work on IronOS with all tools needed"
	@echo "\t * docker-build - compile builds of IronOS for supported models inside docker container and place them to \"scripts/ci/artefacts/\""
	@echo "\t * docker-clean - delete created docker container (but not pre-downloaded data for it)"
	@echo "\t * docs         - generate \"site\"/ directory with documentation in a form of static html files using ReadTheDocs framework and $(MKDOCS_YML) local config file"
	@echo "\t * docs-deploy  - generate & deploy docs online to gh-pages branch of current github repo"
	@echo "\t * clean-build  - delete generated files & dirs produced during builds EXCEPT generated docker container image"
	@echo "\t * clean-full   - delete generated files & dirs produced during builds INCLUDING generated docker container image"
	@echo ""
	@echo "NOTES on supported pass-trough targets:"
	@echo "\t * main Makefile is located in source/ directory and used to build the firmware itself;"
	@echo "\t * this top-level Makefile supports to call targets from source/Makefile;"
	@echo "\t * if you set up development environment right on your host, then to build firmware locally, you can just type right from here:"
	@echo
	@echo "\t> make firmware-LANG_ID model=MODEL_ID"
	@echo
	@echo "Full list of current supported IDs:"
	@echo "\t * LANG_ID: BE BG CS DA DE EL EN ES FI FR HR HU IT JA_JP LT NB NL_BE NL PL PT RO RU SK SL SR_CYRL SR_LATN SV TR UK VI YUE_HK ZH_CN ZH_TW"
	@echo "\t * MODEL_ID: TS100 TS101 TS80 TS80P MHP30 Pinecil Pinecilv2 S60"
	@echo
	@echo "For example, to make a local build of IronOS firmware for TS100 with English language, just type:"
	@echo "\n\t> make firmware-EN model=TS100"
	@echo

# bash one-liner to generate langs for "make list":
# echo "`ls Translations/ | grep -e "^translation_.*.json$" | sed -e 's,^translation_,,g; s,\.json$,,g; ' | tr '\n' ' '`"

# detect availability of docker
docker-check:
ifeq ($(DOCKER_BIN),)
	@echo "ERROR: Can't find docker-compose nor docker tool. Please, install docker and try again"
	@exit 1
else
	@true
endif

# former start_dev.sh
docker-shell: docker-check  $(DOCKER_DEPS)
	$(DOCKER_CMD)

# former build.sh
docker-build: docker-check  $(DOCKER_DEPS)
	$(DOCKER_CMD)  /bin/bash  /build/ci/buildAll.sh

# delete container
docker-clean: docker-check
	-docker  rmi  ironos-builder:latest

# generate docs in site/ directory (DIR for -d is relative to mkdocs.yml file location, hence use default name/location site by setting up ../site)
docs: $(MKDOCS_YML)  Documentation/*  Documentation/Flashing/*  Documentation/images/*
	$(MKDOCS)  build  -f $(MKDOCS_YML)  -d ../site

# deploy docs to gh-pages branch of current repo automagically using ReadTheDocs framework
docs-deploy: $(MKDOCS_YML)  Documentation/*  Documentation/Flashing/*  Documentation/images/*
	$(MKDOCS)  gh-deploy  -f $(MKDOCS_YML)  -d ../site

# pass-through target for Makefile inside source/ dir
%:
	make  -C source/  $@

# global clean-up target for produced/generated files inside tree
clean-build:
	make  -C source/  clean-all
	rm  -Rf  site
	rm  -Rf  scripts/ci/artefacts

# global clean-up target
clean-full: clean-build  docker-clean

.PHONY:  help  list  docker-check  docker-shell  docker-build  docker-clean  docs  docs-deploy  clean-build  clean-full

