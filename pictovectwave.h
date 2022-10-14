/**
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

**/

#ifndef PICTOVECTWAVE_H
#define PICTOVECTWAVE_H

#define R 0
#define G 1
#define B 2
#define CordX 0
#define CordY 1


int threashold = 10;
int CullingThreashold = 1;
/// Max space to look for other points.
int MaxPointSpace = 3;
/// Minimum number of points that must exist within MaxPointSpace area.
int MinPointCluster = 3;
/// Number of frames to skip. -1 is disable skip.
int frameskipVal = 0;
/// Output Audio Length in Seconds
int ALength = 10;
/// Output Audio Sample rate
int sampleRate = 44100;
/// Swap XY
int swapXY = 1;
/// Invert X
int Xinvert = 1;
/// Invert Y
int Yinvert = 0;
/// Create Trace Images
int traceImages = 0;
/// Output Volume Scale 0 - 100
float volume = 25;
/// Bad Audio DAC compensation Factor
double compFactorX = 0.5;
double compFactorY = 3.8;

const char* filename;
int GetArgs(int, char**);
void pngmake(int, int, int, int);
void pointToImage(int);
void debugToImage(int);
void debugPointToImage(int, int);
void debugPointToImage2(int, int);
int reverseLookup(int, int, int);
void pointsToWave(unsigned int, unsigned int, unsigned int);
unsigned int cord(unsigned int, unsigned int);

unsigned char * edgeOut;
unsigned char * pixelTMP;

class C_chSplit{
public:
    unsigned char * chnls;
    /** Memory constructor **/
    C_chSplit(void){
        chnls = new unsigned char [3];
    }
};

class C_pTable{
public:
    unsigned int * PTable;
    /** Memory constructor **/
    C_pTable(void){
        PTable = new unsigned int [2];
    }
};

class C_pRemain{
public:
    unsigned int * RemainingPoints;
    unsigned int * PointsOrganized;
    float * PointsNormalized;
    double * TempNormalized;
    /** Memory constructor **/
    C_pRemain(void){
        RemainingPoints = new unsigned int [2];
        PointsOrganized = new unsigned int [2];
        PointsNormalized = new float [2];
        TempNormalized = new double [2];
    }
};C_pRemain * O_pRemain;

unsigned int * PKeepers;
unsigned int GNX = 0;
unsigned int GNY = 0;
unsigned int width, height;

const char *helpText[]{
"\nThis program is provided without any warranty.\n"
"Copyright: Jarrett Cigainero October 2022 \n"
"\n"
"-h :: This Help Text.\n"
"\nInput Options: \n"
"-i :: Input Filename.\n"
"\nStage 1 Edge Detection Options: \n"
"-t :: Pixel Threshold. The minimum change in value between pixels for edge detection. 0 - 255 :: DEFAULT: 10\n"
"-C :: Pixel Culling Threshold. 0 - 6 :: DEFAULT: 3\n"
"      Used with option -t to help filter stray points.\n"
"      The program makes 6 passes, 3 passes for the RGB channels in\n"
"      horizontal mode, then 3 more passes in vertical mode.\n"
"      Each pass on a successful pixel difference (option -t) it adds 42 to\n"
"      that corresponding pixel in an array. Then this number is used to\n"
"      cull out any low values.\n"
"\nStage 2 Edge Detection Options: \n"
"-m :: Minimum Points In Cluster. Used for culling out stray points. 0 - 255 :: DEFAULT: 3\n"
"-S :: Points Cluster Size. Used for culling out stray points. 0 - 255 :: DEFAULT: 3\n"
"\nAudio Out Options: \n"
"-V :: Wave Output Volume. 0 - 100 :: DEFAULT: 25\n"
"-s :: Wave File Sample Rate. 0 - 256000 :: DEFAULT: 44100\n"
"-L :: Desired Wave File Length in Seconds. 0 - 600 :: DEFAULT: 10\n"
"-U :: Swap X and Y for Wave File Output.\n"
"-X :: Invert X for Wave File Output.\n"
"-Y :: Invert Y for Wave File Output.\n"
"-f :: X bad DAC compensation value. 0 - 10 :: DEFAULT: 0.5\n"
"-F :: Y bad DAC compensation value. 0 - 10 :: DEFAULT: 3.8\n"
"\nDebug Options: \n"
"-T :: Create Draw Order Images. This can take a while!\n"
"-k :: Number of frames to skip when creating Draw Order Images. 0 - 20000 :: DEFAULT: 0\n"
"\n"
};


#endif // PICTOVECTWAVE_H
