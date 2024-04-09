//
// Copyright 2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// loadimage.h: Defines image loading functions

#ifndef ETC2_TRANSCODER_H
#define ETC2_TRANSCODER_H

#include <stddef.h>
#include <stdint.h>
#include <memory>

namespace etc2transcoder
{

void LoadETC1RGB8ToBC1(size_t width,
                       size_t height,
                       size_t depth,
                       const uint8_t *input,
                       size_t inputRowPitch,
                       size_t inputDepthPitch,
                       uint8_t *output,
                       size_t outputRowPitch,
                       size_t outputDepthPitch);

void LoadETC2RGB8ToBC1(size_t width,
                       size_t height,
                       size_t depth,
                       const uint8_t *input,
                       size_t inputRowPitch,
                       size_t inputDepthPitch,
                       uint8_t *output,
                       size_t outputRowPitch,
                       size_t outputDepthPitch);

void LoadETC2SRGB8ToBC1(size_t width,
                        size_t height,
                        size_t depth,
                        const uint8_t *input,
                        size_t inputRowPitch,
                        size_t inputDepthPitch,
                        uint8_t *output,
                        size_t outputRowPitch,
                        size_t outputDepthPitch);

void LoadETC2RGB8A1ToBC1(size_t width,
                         size_t height,
                         size_t depth,
                         const uint8_t *input,
                         size_t inputRowPitch,
                         size_t inputDepthPitch,
                         uint8_t *output,
                         size_t outputRowPitch,
                         size_t outputDepthPitch);

void LoadETC2SRGB8A1ToBC1(size_t width,
                          size_t height,
                          size_t depth,
                          const uint8_t *input,
                          size_t inputRowPitch,
                          size_t inputDepthPitch,
                          uint8_t *output,
                          size_t outputRowPitch,
                          size_t outputDepthPitch);

void LoadETC2RGBA8ToRGBA8(size_t width,
                          size_t height,
                          size_t depth,
                          const uint8_t *input,
                          size_t inputRowPitch,
                          size_t inputDepthPitch,
                          uint8_t *output,
                          size_t outputRowPitch,
                          size_t outputDepthPitch);

void LoadETC2RGBA8ToBC3(size_t width,
                        size_t height,
                        size_t depth,
                        const uint8_t *input,
                        size_t inputRowPitch,
                        size_t inputDepthPitch,
                        uint8_t *output,
                        size_t outputRowPitch,
                        size_t outputDepthPitch);

void LoadETC2SRGBA8ToSRGBA8(size_t width,
                            size_t height,
                            size_t depth,
                            const uint8_t *input,
                            size_t inputRowPitch,
                            size_t inputDepthPitch,
                            uint8_t *output,
                            size_t outputRowPitch,
                            size_t outputDepthPitch);

void LoadETC2SRGBA8ToBC3(size_t width,
                         size_t height,
                         size_t depth,
                         const uint8_t *input,
                         size_t inputRowPitch,
                         size_t inputDepthPitch,
                         uint8_t *output,
                         size_t outputRowPitch,
                         size_t outputDepthPitch);

void LoadEACR11ToBC4(size_t width,
                     size_t height,
                     size_t depth,
                     const uint8_t *input,
                     size_t inputRowPitch,
                     size_t inputDepthPitch,
                     uint8_t *output,
                     size_t outputRowPitch,
                     size_t outputDepthPitch);

void LoadEACR11SToBC4(size_t width,
                      size_t height,
                      size_t depth,
                      const uint8_t *input,
                      size_t inputRowPitch,
                      size_t inputDepthPitch,
                      uint8_t *output,
                      size_t outputRowPitch,
                      size_t outputDepthPitch);

void LoadEACRG11ToBC5(size_t width,
                      size_t height,
                      size_t depth,
                      const uint8_t *input,
                      size_t inputRowPitch,
                      size_t inputDepthPitch,
                      uint8_t *output,
                      size_t outputRowPitch,
                      size_t outputDepthPitch);

void LoadEACRG11SToBC5(size_t width,
                       size_t height,
                       size_t depth,
                       const uint8_t *input,
                       size_t inputRowPitch,
                       size_t inputDepthPitch,
                       uint8_t *output,
                       size_t outputRowPitch,
                       size_t outputDepthPitch);

}  // namespace angle

#endif  // IMAGEUTIL_LOADIMAGE_H_
