//
// MD5 message-digest algorithm
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.
//
// C++/object oriented translation and modification:
// Copyright (C) 1995 Mordechai T. Abzug
//
// Further adaptations for SuperTux:
// Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
//
// This translation/modification is provided "as is," without express or
// implied warranty of any kind.
//
// The translators/modifiers do not claim:
// (1) that MD5 will do what you think it does;
// (2) that this translation/ modification is accurate; or
// (3) that this software is "merchantible."
//
// based on:
//
// MD5.H - header file for MD5C.C
// MDDRIVER.C - test driver for MD2, MD4 and MD5
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.
//
// License to copy and use this software is granted provided that it
// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
// Algorithm" in all material mentioning or referencing this software
// or this function.
//
// License is also granted to make and use derivative works provided
// that such works are identified as "derived from the RSA Data
// Security, Inc. MD5 Message-Digest Algorithm" in all material
// mentioning or referencing the derived work.
//
// RSA Data Security, Inc. makes no representations concerning either
// the merchantability of this software or the suitability of this
// software for any particular purpose. It is provided "as is"
// without express or implied warranty of any kind.
//
// These notices must be retained in any copies of any part of this
// documentation and/or software.
//

#ifndef HEADER_SUPERTUX_ADDON_MD5_HPP
#define HEADER_SUPERTUX_ADDON_MD5_HPP

#include <fstream>
#include <stdint.h>

class MD5
{
public:
  MD5();
  MD5(uint8_t* string); /**< digest string, finalize */
  MD5(std::istream& stream); /**< digest stream, finalize */
  MD5(FILE *file); /**< digest file, close, finalize */
  MD5(std::ifstream& stream); /**< digest stream, close, finalize */

  void update(uint8_t* input, unsigned int input_length); /**< MD5 block update operation. Continues an MD5 message-digest operation, processing another message block, and updating the context. */
  void update(std::istream& stream);
  void update(FILE *file);
  void update(std::ifstream& stream);

  uint8_t* raw_digest(); /**< digest as a 16-byte binary array */
  std::string hex_digest(); /**< digest as a 33-byte ascii-hex string */
  friend std::ostream& operator<<(std::ostream&, MD5 context);

private:
  uint32_t state[4];
  uint32_t count[2]; /**< number of _bits_, mod 2^64 */
  uint8_t buffer[64]; /**< input buffer */
  uint8_t digest[16];
  bool finalized;

  void init(); /**< called by all constructors */
  void finalize(); /**< MD5 finalization. Ends an MD5 message-digest operation, writing the the message digest and zeroizing the context. */
  void transform(uint8_t* buffer); /**< MD5 basic transformation. Transforms state based on block. Does the real update work.  Note that length is implied to be 64. */

  static void encode(uint8_t* dest, uint32_t* src, uint32_t length); /**< Encodes input (uint32_t) into output (uint8_t). Assumes len is a multiple of 4. */
  static void decode(uint32_t* dest, uint8_t* src, uint32_t length); /**< Decodes input (uint8_t) into output (uint32_t). Assumes len is a multiple of 4. */
  static void memcpy(uint8_t* dest, uint8_t* src, uint32_t length);
  static void memset(uint8_t* start, uint8_t val, uint32_t length);

  static inline uint32_t rotate_left(uint32_t x, uint32_t n);
  static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z); //*< F, G, H and I are basic MD5 functions. */
  static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z);
  static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z);
  static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z);
  static inline void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac); /**< FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4. Rotation is separate from addition to prevent recomputation. */
  static inline void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
  static inline void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
  static inline void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);

};

#endif

/* EOF */
