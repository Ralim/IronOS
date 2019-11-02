# Development

Building this software can be performed two ways, using the STMCubeIDE or using command line tools.

## STM Cube IDE

The repository is setup to try and make this painless, when starting the IDE you can import the project by pointing the internal search to the workspace folder of this repository. If you start with a fresh installation, close the welcome screen tab, and then in the left sidebar, there is an option to "import". Select General -> Import existing projects -> Set the top location option to this repositories workspace -> Should find the TS100 project.

## Developing with command line tools & building a release

In the `workspace/TS100` folder there is a makefile that can be used to build the repository using command line tools.
when running the `make` command, specify which model of the device & the language you would like to use.

`make -j8 lang=EN model=TS80`

To build a release instead, run the build.sh script. This will update translations and also build every language for both TS100 and TS80 models.

## Updating languages

To update the language translation files & associated font map, execute the `make_translation.py` code from the translations directory.
