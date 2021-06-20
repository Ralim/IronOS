# Development

Building this software can be performed two ways, using the STMCubeIDE or using command line tools.

## STM Cube IDE

The easiest way to start working using the STM Cube IDE is to create a new project for the STM32F103RCTx.
Then once this is created remove the auto-generated source code.
Next drag the contents of the `source` folder into the project and choose to link to files.
You will need to update the build settings for include paths & point to the new `.ld` linker file

## Developing with command line tools & building a release

In the `source` folder there is a makefile that can be used to build the repository using command line tools.
when running the `make` command, specify which model of the device & the language you would like to use.

`make -j8 lang=EN model=TS80`

To build a release instead, run the build.sh script. This will update translations and also build every language for both TS100 and TS80 models.

## Updating languages

To update the language translation files & associated font map, execute the `make_translation.py` code from the translations directory.

## Building Pinecil

I highly recommend using the command line tools and using docker to run the compiler.
It's a bit more fussy on setup than the STM tooling and this is by far the easiest way.
If you _need_ an IDE I have used [Nuclei's IDE](https://nucleisys.com/download.php)
And follow same idea as the STM Cube IDE notes above.
