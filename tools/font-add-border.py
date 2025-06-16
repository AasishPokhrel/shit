#!/usr/bin/env python3
# SuperTux
# Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from PIL import Image
import argparse
import tempfile

# Add a 1 pixel border around every glyph in a font

def fix_font_file(filename, glyph_width, glyph_height):
    print("Processing %s %dx%d" % (filename, glyph_width, glyph_height))
    img = Image.open(filename)
    w, h = img.size
    print("Image size: %dx%d" % (w, h))

    assert w % glyph_width == 0, "image not multiple of glyph width"
    assert h % glyph_height == 0, "image not multiple of glyph height"

    w_g = w // glyph_width
    h_g = h // glyph_height

    print("Glyphs: %ax%a" % (w_g, h_g))

    out = Image.new("RGBA", (w_g * (glyph_width + 2), h_g * (glyph_height + 2)), color=5)

    for y in range(0, h_g):
        for x in range(0, w_g):
            ix = x * glyph_width
            iy = y * glyph_height

            ox = x * (glyph_width + 2) + 1
            oy = y * (glyph_height + 2) + 1

            glyph = img.crop((ix, iy, ix + glyph_width, iy + glyph_height))
            out.paste(glyph, (ox, oy))

    with tempfile.NamedTemporaryFile(suffix=".png", delete=False) as f:
        out.save(f)
        print("File saved as %s" % f.name)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor MAS packer')
    parser.add_argument('FILE', action='store', type=str,
                        help='font image to change')

    parser.add_argument('GLYPH_WIDTH', action='store', type=int,
                        help='glyph width')

    parser.add_argument('GLYPH_HEIGHT', action='store', type=int,
                        help='glyph height')

    args = parser.parse_args()

    fix_font_file(args.FILE, args.GLYPH_WIDTH, args.GLYPH_HEIGHT)

# EOF #
