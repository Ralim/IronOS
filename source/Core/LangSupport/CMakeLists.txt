
if (LANG STREQUAL "")
 message(FATAL_ERROR "You must specify a language")
endif()


add_custom_command(
    OUTPUT "translation_${LANG}.cpp"
    COMMAND 

    python3 ${CMAKE_CURRENT_SOURCE_DIR}/../../../Translations/make_translation.py -o translation_${LANG}.cpp  ${LANG}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../../Translations/translation_${LANG}.json
    VERBATIM)
  

set(sources 
translation_${LANG}.cpp
# lang_multi.cpp
lang_single.cpp
)


add_library(languages ${sources})
target_link_libraries(languages PRIVATE drivers brieflz mainSource )

