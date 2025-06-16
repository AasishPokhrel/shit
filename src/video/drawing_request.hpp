//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_VIDEO_DRAWING_REQUEST_HPP
#define HEADER_SUPERTUX_VIDEO_DRAWING_REQUEST_HPP

#include <string>
#include <memory>

#include "math/rectf.hpp"
#include "math/sizef.hpp"
#include "math/vector.hpp"
#include "video/blend.hpp"
#include "video/color.hpp"
#include "video/drawing_transform.hpp"
#include "video/font.hpp"
#include "video/gradient.hpp"

class Surface;

enum class RequestType
{
  TEXTURE, GRADIENT, FILLRECT, INVERSEELLIPSE, GETPIXEL, LINE, TRIANGLE
};

struct DrawingRequest
{
  int layer;
  Flip flip;
  float alpha;
  Blend blend;
  const Rect viewport;

  DrawingRequest() = delete;
  DrawingRequest(const DrawingTransform& transform) :
    layer(),
    flip(transform.flip),
    alpha(transform.alpha),
    blend(),
    viewport(transform.viewport)
  {}
  virtual ~DrawingRequest() {}

  virtual RequestType get_type() const = 0;
};

struct TextureRequest : public DrawingRequest
{
  TextureRequest(const DrawingTransform& transform) :
    DrawingRequest(transform),
    texture(),
    displacement_texture(),
    srcrects(),
    dstrects(),
    angles(),
    color(1.0f, 1.0f, 1.0f)
  {}

  RequestType get_type() const override { return RequestType::TEXTURE; }

  const Texture* texture;
  const Texture* displacement_texture;
  std::vector<Rectf> srcrects;
  std::vector<Rectf> dstrects;
  std::vector<float> angles;
  Color color;

private:
  TextureRequest(const TextureRequest&) = delete;
  TextureRequest& operator=(const TextureRequest&) = delete;
};

struct GradientRequest : public DrawingRequest
{
  GradientRequest(const DrawingTransform& transform)  :
    DrawingRequest(transform),
    pos(0.0f, 0.0f),
    size(0.0f, 0.0f),
    top(),
    bottom(),
    direction(),
    region()
  {}

  RequestType get_type() const override { return RequestType::GRADIENT; }

  Vector pos;
  Vector size;
  Color top;
  Color bottom;
  GradientDirection direction;
  Rectf region;
};

struct FillRectRequest : public DrawingRequest
{
  FillRectRequest(const DrawingTransform& transform) :
    DrawingRequest(transform),
    rect(),
    color(),
    radius()
  {}

  RequestType get_type() const override { return RequestType::FILLRECT; }

  Rectf rect;
  Color color;
  float radius;
};

struct InverseEllipseRequest : public DrawingRequest
{
  InverseEllipseRequest(const DrawingTransform& transform) :
    DrawingRequest(transform),
    pos(0.0f, 0.0f),
    size(0.0f, 0.0f),
    color()
  {}

  RequestType get_type() const override { return RequestType::INVERSEELLIPSE; }

  Vector pos;
  Vector size;
  Color color;
};

struct LineRequest : public DrawingRequest
{
  LineRequest(const DrawingTransform& transform) :
    DrawingRequest(transform),
    pos(0.0f, 0.0f),
    dest_pos(0.0f, 0.0f),
    color()
  {}

  RequestType get_type() const override { return RequestType::LINE; }

  Vector pos;
  Vector dest_pos;
  Color color;
};

struct TriangleRequest : public DrawingRequest
{
  TriangleRequest(const DrawingTransform& transform) :
    DrawingRequest(transform),
    pos1(0.0f, 0.0f),
    pos2(0.0f, 0.0f),
    pos3(0.0f, 0.0f),
    color()
  {}

  RequestType get_type() const override { return RequestType::TRIANGLE; }

  Vector pos1, pos2, pos3;
  Color  color;
};

struct GetPixelRequest : public DrawingRequest
{
  GetPixelRequest(const DrawingTransform& transform) :
    DrawingRequest(transform),
    pos(0.0f, 0.0f),
    color_ptr()
  {}

  RequestType get_type() const override { return RequestType::GETPIXEL; }

  Vector pos;
  std::shared_ptr<Color> color_ptr;

private:
  GetPixelRequest(const GetPixelRequest&) = delete;
  GetPixelRequest& operator=(const GetPixelRequest&) = delete;
};

#endif

/* EOF */
