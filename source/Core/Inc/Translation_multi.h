#ifndef TRANSLATION_MULTI_H_
#define TRANSLATION_MULTI_H_

#include "Translation.h"
#include <stdbool.h>
// The compressed translation data will be decompressed to this buffer. These
// data may include:
//  - TranslationData (translation index table and translation strings)
//  - Font table(s)
// The translation index table consists of uint16_t (half words) which has a
// 2-byte alignment. Therefore, the declaration of this buffer must include
// the alignment specifier `alignas(TranslationData)` to satisfy its alignment.
// TranslationData must always be decompressed to the start of this buffer.
extern uint8_t        translation_data_out_buffer[];
extern const uint16_t translation_data_out_buffer_size;

struct LanguageMeta {
  uint16_t       uniqueID;
  const uint8_t *translation_data;
  uint16_t       translation_size : 15;
  bool           translation_is_compressed : 1;
};

extern const LanguageMeta LanguageMetas[];
extern const uint8_t      LanguageCount;

#endif /* TRANSLATION_MULTI_H_ */
