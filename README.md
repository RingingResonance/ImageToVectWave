
    Copyright (c) Jarrett Cigainero 2022

    This file is part of Picture To Vector Wave.

    Picture To Vector Wave is free software: you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation,
    either version 3 of the License, or (at your option) any later version.

    Picture To Vector Wave is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with Picture To Vector Wave.
    If not, see <https://www.gnu.org/licenses/>.

    How to use this program.

    -h :: This Help Text.

    Input Options:
    -i :: Input Filename.

    Stage 1 Edge Detection Options:
    -t :: Pixel Threshold. The minimum change in value between pixels for edge detection. 0 - 255 :: DEFAULT: 10
    -C :: Pixel Culling Threshold. 0 - 6 :: DEFAULT: 3
          Used with option -t to help filter stray points.
          The program makes 6 passes, 3 passes for the RGB channels in
         horizontal mode, then 3 more passes in vertical mode.
         Each pass on a successful pixel difference (option -t) it adds 42 to
         that corresponding pixel in an array. Then this number is used to
         cull out any low values.

    Stage 2 Edge Detection Options:
    -m :: Minimum Points In Cluster. Used for culling out stray points. 0 - 255 :: DEFAULT: 3
    -S :: Points Cluster Size. Used for culling out stray points. 0 - 255 :: DEFAULT: 3

    Audio Out Options:
    -V :: Wave Output Volume. 0 - 100 :: DEFAULT: 25
    -s :: Wave File Sample Rate. 0 - 256000 :: DEFAULT: 44100
    -L :: Desired Wave File Length in Seconds. 0 - 600 :: DEFAULT: 10
    -U :: Swap X and Y for Wave File Output.
    -X :: Invert X for Wave File Output.
    -Y :: Invert Y for Wave File Output.
    -f :: LEFT CHANNEL bad DAC compensation value. 0 - 10 :: DEFAULT: 0.5
    -F :: RIGHT CHANNEL bad DAC compensation value. 0 - 10 :: DEFAULT: 3.8

    Debug Options:
    -T :: Create Draw Order Images. This can take a while!
    -k :: Number of frames to skip when creating Draw Order Images. 0 - 20000 :: DEFAULT: 0
