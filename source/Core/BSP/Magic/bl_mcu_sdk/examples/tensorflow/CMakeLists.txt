set(mains main.c)
set(TARGET_REQUIRED_LIBS tflite)
set(TARGET_REQUIRED_SRCS main_functions.cc output_handler.cc constants.cc model.cc)
generate_bin()



