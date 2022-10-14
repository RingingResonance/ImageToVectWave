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

#include "lodepng/lodepng.h"
#include "lodepng/lodepng.cpp"
#include "pictovectwave.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

using namespace std;

int main(int argc, char *argv[]) {
  //Get command ARGs
  if(GetArgs(argc, argv))return 0;
  //Use lodepng to open the specified PNG file.
  //Using boilerplate code example listed in the header file. Thanks lode!
  //load and decode
  std::vector<unsigned char> image;
  unsigned error = lodepng::decode(image, width, height, filename, LCT_RGB, 8);
  //if there's an error, display it
  if(error) {std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;return 0;}
  //the pixels are now in the vector "image", 3 bytes per pixel, ordered RGBRGB..., use it as texture, draw it, ...
  cout << "Got File. Processing. \n";
  /// Now split the color channels to make it easier to process.
  C_chSplit* O_channels;
  O_channels = new C_chSplit [width*height];
  int eoi = width*height;
  // Fill channels with data
  int ImgIndex = 0;
  cout << "Splitting Channels. \n";
  for(int i=0;i<eoi;i++){
    O_channels[i].chnls[R] = image[ImgIndex];
    ImgIndex++;
    O_channels[i].chnls[G] = image[ImgIndex];
    ImgIndex++;
    O_channels[i].chnls[B] = image[ImgIndex];
    ImgIndex++;
  }
  cout << "Channels Split, doing horizontal pass. \n";
  edgeOut = new unsigned char [(width*height)];
  int tpix = 0;
  int pix = 0;
  /// Now do edge detection for each channel.
  for(int ch=0;ch<3;ch++){
      for(int y=0;y<height;y++){
        pix = O_channels[y*width].chnls[ch];
        for(int x=0;x<width;x++){
            tpix = O_channels[cord(x,y)].chnls[ch];
            if((pix-tpix) > threashold || (tpix-pix) > threashold) edgeOut[cord(x,y)]+=42;
            pix = tpix;
        }
      }
  }
  cout << "Doing Vertical Pass. \n";
  for(int ch=0;ch<3;ch++){
      for(int x=0;x<width;x++){
        pix = O_channels[x*height].chnls[ch];
        for(int y=0;y<height;y++){
            tpix = O_channels[cord(x,y)].chnls[ch];
            if((pix-tpix) > threashold || (tpix-pix) > threashold) edgeOut[cord(x,y)]+=42;
            pix = tpix;
        }
      }
  }
  delete O_channels;
  cout << "Culling low value pixels. \n";
  unsigned int PointCount = 0;
  for(int i=0;i<width*height;i++){
    if(edgeOut[i]>(CullingThreashold*42)-1){
        edgeOut[i]=255;
        PointCount++;
    }
    else edgeOut[i]=0;
  }
///Debug Image Stage 1
  debugToImage(1);
  cout << "Creating Table Of Points. \n";
  C_pTable * O_PTable;
  O_PTable = new C_pTable [PointCount];
  unsigned int PTindex = 0;
  /// Now create table of points to turn into vectors.
  for(unsigned int y=0;y<height;y++){
    for(unsigned int x=0;x<width;x++){
        if(edgeOut[cord(x,y)] == 255){
            O_PTable[PTindex].PTable[CordX] = x;
            O_PTable[PTindex].PTable[CordY] = y;
            PTindex++;
        }
    }
  }
  /// Cull points to filter out strays using 'MaxPointSpace'
  cout << "Culling Points. \n";
  cout << "There are " << PointCount << " points to look at. This may take some time. \n";
  PTindex = 0;
  unsigned int PT2index = 0;
  unsigned int PX = 0;
  unsigned int PY = 0;
  float PcmpX = 0;
  float PcmpY = 0;
  unsigned int Pkeeping = 0;
  int resultcount = 0;
  int compX = 0;
  int compY = 0;
  int reportcount = 0;
  unsigned int cluster = 0;
  PKeepers = new unsigned int [PointCount];
  for(int i=0;i<PointCount;i++){
    //Get a point to compare
    PX = O_PTable[i].PTable[CordX];
    PY = O_PTable[i].PTable[CordY];
    //Now compare it to every point around itself within 'MaxPointSpace' distance.
    for(int y=PY-MaxPointSpace;y<PY+MaxPointSpace;y++){
        if(y>height)break;
        if(y>0){
            for(int x=PX-MaxPointSpace;x<PX+MaxPointSpace;x++){
                if(x>width)break;
                if(x>0 && (x!=PX || y!=PY) && edgeOut[cord(x,y)]==255){
                    PKeepers[i] = 1;        //Mark to keep.
                    cluster++;
                }
            }
        }
    }
    if(cluster < MinPointCluster) PKeepers[i] = 0;  //Mark for deletion if there aren't at least MinPointCluster points nearby.
    if(PKeepers[i] == 1)Pkeeping++;
    cluster=0;
    reportcount = i+1;
  }
  cout << reportcount << " inspected. " << Pkeeping << " keeping. \n";
  /// Delete non-viable points.
  cout << "Deleting non-viable points. \n";
  PTindex = 0;
  O_pRemain = new C_pRemain [Pkeeping];
  for(int i=0;i<PointCount;i++){
    GNX = O_PTable[i].PTable[CordX];
    GNY = O_PTable[i].PTable[CordY];
    if(PKeepers[i]){
        O_pRemain[PTindex].RemainingPoints[CordX] = GNX;
        O_pRemain[PTindex].RemainingPoints[CordY] = GNY;
        PTindex++;
    }
  }
  delete O_PTable;
/// Debug Image Stage 2
debugPointToImage(2,Pkeeping); //Derived from RemainingPoints.
  /// Organize Points in the drawing order based of distance from each other.
  cout << "There are " << Pkeeping << " points remaining to organize into vectors. This may take some time. \n";
  PTindex = 0;
  PT2index = 0;
  PX = 0;
  PY = 0;
  int XM = 0;
  int XP = 0;
  int YM = 0;
  int YP = 0;
  int adj = 0;
  int AX = 0;
  int AY = 0;
  PcmpX = 0;
  PcmpY = 0;
  int noFind = 0;
  double shortestDist = 0;
  int shortestIndex = 0;
  int shortestNumber = 0;
  double distance = 0;
  int rLook = 0;
  int tDist = 0;
  reportcount = 0;
  //Place first point into the current working point.
  PX = O_pRemain[0].RemainingPoints[CordX];
  PY = O_pRemain[0].RemainingPoints[CordY];
  for(int i=0;i<Pkeeping;i++)PKeepers[i]=0; //Clear PKeepers. We are going to use it again for marking points that have already been used.
  for(int i=0;i<Pkeeping;i++){
    if(reportcount>-1){cout << "Processed " << i << " points.\r"; reportcount=0;} reportcount++;
    PT2index = 0;
    shortestDist = 999999;
    noFind = 0;
    for(int f=0;f<Pkeeping;f++){
        PcmpX = O_pRemain[f].RemainingPoints[CordX];
        PcmpY = O_pRemain[f].RemainingPoints[CordY];
        /// Only measure distance for points we haven't measured yet and aren't the same as our working point.
        if(!PKeepers[f] && (PX!=PcmpX || PY!=PcmpY)){
            if(!noFind){
                XM=PX-1; XP=PX+1; YM=PY-1; YP=PY+1; adj=0;
                rLook = reverseLookup(XP, PY, PointCount);
                if(rLook>=0 && !PKeepers[rLook]){
                    shortestNumber = rLook;
                    adj=1; AX=XP; AY=PY;
                    break;
                }
                rLook = reverseLookup(PX, YM, PointCount);
                if(rLook>=0 && !PKeepers[rLook]){
                    shortestNumber = rLook;
                    adj=1; AX=PX; AY=YM;
                    break;
                }
                rLook = reverseLookup(XM, PY, PointCount);
                if(rLook>=0 && !PKeepers[rLook]){
                    shortestNumber = rLook;
                    adj=1; AX=XM; AY=PY;
                    break;
                }
                rLook = reverseLookup(PX, YP, PointCount);
                if(rLook>=0 && !PKeepers[rLook]){
                    shortestNumber = rLook;
                    adj=1; AX=PX; AY=YP;
                    break;
                }
                noFind = 1; //If we made it this far then we didn't find an adjacent point. Skip the next go around.
            }
            distance = sqrt(pow((PX-PcmpX),2)+pow((PY-PcmpY),2));
            if(distance < shortestDist){
                shortestIndex = f;
                shortestNumber = f;
                shortestDist = distance;
            }
        }
    }
    PKeepers[shortestNumber] = 1;
    if(adj){
        O_pRemain[i].PointsOrganized[CordX] = AX;
        PX = AX;
    }
    else {
        O_pRemain[i].PointsOrganized[CordX] = O_pRemain[shortestIndex].RemainingPoints[CordX];
        PX = O_pRemain[shortestIndex].RemainingPoints[CordX];
    }
    if(adj){
        O_pRemain[i].PointsOrganized[CordY] = AY;
        PY = AY;
    }
    else {
        O_pRemain[i].PointsOrganized[CordY] = O_pRemain[shortestIndex].RemainingPoints[CordY];
        PY = O_pRemain[shortestIndex].RemainingPoints[CordY];
    }
    resultcount = i+1;
  }
  delete PKeepers;
  cout << "Processed " << resultcount << " points.\n" << "Normalizing points. \n";
  /// Normalize the points.
  float dimScale = 0;
  if(height>width)dimScale=height;
  else dimScale=width;
  float pointTemp = 0;

  for(int i=0;i<Pkeeping;i++){
        pointTemp = O_pRemain[i].PointsOrganized[CordX];
        O_pRemain[i].PointsNormalized[CordX]=((pointTemp/dimScale)-0.5)*2;

        pointTemp = O_pRemain[i].PointsOrganized[CordY];
        O_pRemain[i].PointsNormalized[CordY]=((pointTemp/dimScale)-0.5)*2;
  }

  /// Save vector file.
  cout << "Saving Vector File. \n";
  ofstream Vfile;
  Vfile.open ("./Vectors.txt");
  PTindex = 0;
  for(int i=0;i<Pkeeping;i++){
        Vfile << i << "## X " << O_pRemain[i].PointsNormalized[CordX];
        Vfile << " : Y " << O_pRemain[i].PointsNormalized[CordY] << " \n";
  }
  Vfile.close();
  /// Adjust points positions to compensate for bad audio DACs
  cout << "Compensating for bad DACs. \n";
  PTindex = 0;
  double sampF = sampleRate;
  float xHighest = 0;
  float xLowest = 0;
  float yHighest = 0;
  float yLowest = 0;
  int XHaddr = 0;
  int XLaddr = 0;
  int YHaddr = 0;
  int YLaddr = 0;
  /// Pre-Center to prepare for compensation.
  // Find +-Peaks and peak averages.
  for(int i=0;i<Pkeeping;i++){
    if(xHighest < O_pRemain[i].PointsNormalized[CordX])xHighest = O_pRemain[i].PointsNormalized[CordX];
    if(xLowest > O_pRemain[i].PointsNormalized[CordX])xLowest = O_pRemain[i].PointsNormalized[CordX];

    if(yHighest < O_pRemain[i].PointsNormalized[CordY])yHighest = O_pRemain[i].PointsNormalized[CordY];
    if(yLowest > O_pRemain[i].PointsNormalized[CordY])yLowest = O_pRemain[i].PointsNormalized[CordY];
    PTindex++;
  }
  //Do centering.
  PTindex=0;
  float Xmove = ((xHighest+xLowest)/2)*-1;
  float Ymove = ((yHighest+yLowest)/2)*-1;
  for(int i=0;i<Pkeeping;i++){
    O_pRemain[i].PointsNormalized[CordX] = O_pRemain[i].PointsNormalized[CordX]+Xmove;
    O_pRemain[i].PointsNormalized[CordY] = O_pRemain[i].PointsNormalized[CordY]+Ymove;
  }
  /// Do integral compensation.
  double rateFactorX = (Pkeeping/sampF)*(pow((xHighest-xLowest),4)*(compFactorX/1000));
  double rateFactorY = (Pkeeping/sampF)*(pow((yHighest-yLowest),4)*(compFactorY/1000));
  PTindex=0;
  xHighest = 0;
  xLowest = 0;
  yHighest = 0;
  yLowest = 0;
  double Xcompensation = 0;
  double Ycompensation = 0;
  float divFactor = 1;
  for(int i=0;i<Pkeeping;i++){
    /// Do X
    Xcompensation+=O_pRemain[i].PointsNormalized[CordX];
    O_pRemain[i].TempNormalized[CordX]=O_pRemain[i].PointsNormalized[CordX]+(Xcompensation*rateFactorX);
    if(xHighest < O_pRemain[i].TempNormalized[CordX]){xHighest = O_pRemain[i].TempNormalized[CordX]; XHaddr = PTindex;}
    if(xLowest > O_pRemain[i].TempNormalized[CordX]){xLowest = O_pRemain[i].TempNormalized[CordX]; XLaddr = PTindex;}
    /// Do Y
    Ycompensation+=O_pRemain[i].PointsNormalized[CordY];
    O_pRemain[i].TempNormalized[CordY]=O_pRemain[i].PointsNormalized[CordY]+(Ycompensation*rateFactorY);
    if(yHighest < O_pRemain[i].TempNormalized[CordY]){yHighest = O_pRemain[i].TempNormalized[CordY]; YHaddr = PTindex;}
    if(yLowest > O_pRemain[i].TempNormalized[CordY]){yLowest = O_pRemain[i].TempNormalized[CordY]; YLaddr = PTindex;}
  }
  for(int i=0;i<Pkeeping;i++){
        O_pRemain[i].PointsNormalized[CordX] = O_pRemain[i].TempNormalized[CordX];
        O_pRemain[i].PointsNormalized[CordY] = O_pRemain[i].TempNormalized[CordY];
  }
  /// Scale and re-center accordingly.
  cout << "Scaling and Centering. \n";
  PTindex = 0;
  Xmove = ((xHighest+xLowest)/2)*-1;
  Ymove = ((yHighest+yLowest)/2)*-1;
  float XscaleFact = 1/((xHighest-xLowest)/2);
  float YscaleFact = 1/((yHighest-yLowest)/2);
  float AbsScale = 0;
  if(XscaleFact > YscaleFact)AbsScale = YscaleFact;
  else AbsScale = XscaleFact;
  for(int i=0;i<Pkeeping;i++){
    O_pRemain[i].PointsNormalized[CordX] = (O_pRemain[i].PointsNormalized[CordX]+Xmove)*AbsScale;
    O_pRemain[i].PointsNormalized[CordY] = (O_pRemain[i].PointsNormalized[CordY]+Ymove)*AbsScale;
  }
  /// Save Adjusted vector file.
  cout << "Saving Adjusted Vector File. \n";
  ofstream V2file;
  V2file.open ("./VectorsCompensated.txt");
  PTindex = 0;
  for(int i=0;i<Pkeeping;i++){
        V2file << "X " << O_pRemain[i].PointsNormalized[CordX];
        V2file << " : Y " << O_pRemain[i].PointsNormalized[CordY] << " \n";
  }
  V2file.close();
  /// Create .wav file from data.
  cout << "Creating WAVE file. \n";
  pointsToWave(Pkeeping, (sampleRate/Pkeeping)*ALength, sampleRate);
  if(traceImages)pointToImage(Pkeeping);
  delete edgeOut;
  return 0;
}

void pointsToWave(unsigned int dataSize,unsigned int passes, unsigned int sampRate){
    unsigned char * AudioOut;
    unsigned char Tbyte = 0;
    unsigned char header[] = {0x52,0x49,0x46,0x46,0x00,0x00,0x00,0x00,
                              0x57,0x41,0x56,0x45,0x66,0x6D,0x74,0x20,
                              0x10,0x00,0x00,0x00,0x01,0x00,0x02,0x00,
                              0x00,0x77,0x01,0x00,0x00,0xDC,0x05,0x00,
                              0x04,0x00,0x10,0x00,0x64,0x61,0x74,0x61,
                              0x00,0x00,0x00,0x00}; //44 bytes total

    /// Calculate data size
    cout << "Generating WAVE header. \n";
    unsigned int totalSize = 44+((passes*dataSize)*4);
    unsigned int chunk2Size = ((passes*dataSize)*2*16)/8;
    unsigned int chunk1Size = chunk2Size + 36;
    unsigned int shifter = 0xFF;
    unsigned int shiftDiv = 0x01;
    cout << "File Size is " << totalSize << " \n";
    //Patch in the chunk sizes.
    for(int i=0;i<4;i++){
        header[4+i] = (chunk1Size&shifter)/shiftDiv;
        shifter*=0x100;
        shiftDiv*=0x100;
    }
    shifter = 0xFF;
    shiftDiv = 0x01;
    for(int i=0;i<4;i++){
        header[40+i] = (chunk2Size&shifter)/shiftDiv;
        shifter*=0x100;
        shiftDiv*=0x100;
    }
    //Patch in the sample rate and it's associated variables.
    shifter = 0xFF;
    shiftDiv = 0x01;
    for(int i=0;i<4;i++){
        header[24+i] = (sampRate&shifter)/shiftDiv;
        shifter*=0x100;
        shiftDiv*=0x100;
    }
    shifter = 0xFF;
    shiftDiv = 0x01;
    for(int i=0;i<4;i++){
        header[28+i] = ((sampRate*4)&shifter)/shiftDiv;
        shifter*=0x100;
        shiftDiv*=0x100;
    }
    //Output the file.
    cout << "Adjusting Volume and consolidating data. \n";
    int flipSplit = 0;
    int audioLeft = 0;
    int audioRight = 0;
    int dataIndex = 0;
    AudioOut = new unsigned char [totalSize];
    for(int i=0;i<totalSize;i++){
        if(i<44)AudioOut[i] = header[i];
        else {
            if(swapXY){
                audioRight = O_pRemain[dataIndex].PointsNormalized[CordX]*((volume/100)*65535);
                audioLeft = O_pRemain[dataIndex].PointsNormalized[CordY]*((volume/100)*65535);
            }
            else{
                audioLeft = O_pRemain[dataIndex].PointsNormalized[CordX]*((volume/100)*65535);
                audioRight = O_pRemain[dataIndex].PointsNormalized[CordY]*((volume/100)*65535);
            }
            if(Xinvert)audioLeft *= -1;
            if(Yinvert)audioRight *= -1;
            switch (flipSplit){
                case 0:
                    AudioOut[i] = 0xFF& audioLeft;
                    flipSplit=1;
                    break;
                case 1:
                    AudioOut[i] = (0xFF00& audioLeft)/0x100;
                    flipSplit=2;
                    break;
                case 2:
                    AudioOut[i] = 0xFF& audioRight;
                    flipSplit=3;
                    break;
                case 3:
                    AudioOut[i] = (0xFF00& audioRight)/0x100;
                    dataIndex++;
                    if(dataIndex>=dataSize)dataIndex=0;
                    flipSplit=0;
                    break;
            }
        }
    }
    /// Save the audio file to disk.
    cout << "Writing File. \n";
    ofstream WVfile;
    WVfile.open ("./image.wav",ios::out | ios::binary);
    for(int i=0;i<totalSize;i++)WVfile << AudioOut[i];
    WVfile.close();
}

int reverseLookup(int CX, int CY, int CPointCount){
    unsigned int index = 0;
    int Xcmp;
    int Ycmp;
    if(CX>0 && CX<width && CY>0 && CY<height && edgeOut[cord(CX,CY)]==255){
        for(int h=0;h<CPointCount;h++){
            Xcmp = O_pRemain[h].RemainingPoints[CordX];
            Ycmp = O_pRemain[h].RemainingPoints[CordY];
            if(CX==Xcmp && CY==Ycmp) return h;
        }
    }
    return -1;
}
void debugToImage(int stage){
/// Create Image Based Off Points Data For Debugging And Tuning.
  pixelTMP = new unsigned char [(width*height)];
  for(int i=0;i<(width*height);i++)pixelTMP[i]=edgeOut[i];
  pngmake(stage, width, height, 1);
  delete pixelTMP;
  /// Now output a test image.
}

void debugPointToImage(int stage, int pointRemaining){
/// Create Image Based Off Points Data For Debugging And Tuning.
  unsigned int frameSkip = 0;
  pixelTMP = new unsigned char [(width*height)];
  GNX = 0;
  GNY = 0;
  for(int i=0;i<(width*height);i++)pixelTMP[i]=0;
  for(int i=0;i<pointRemaining;i++){
    GNX = O_pRemain[i].RemainingPoints[CordX];
    GNY = O_pRemain[i].RemainingPoints[CordY];
    if(GNX<width && GNY<height)pixelTMP[cord(GNX,GNY)] = 255;
  }
  pngmake(stage, width, height, 1);
  delete pixelTMP;
}

void pointToImage(int frameNo){
/// Create Image Based Off Points Data For Debugging And Tuning.
 cout << "Saving Output Images. \n";
  unsigned int PTindex = 0;
  int frameSkip = 0;
  pixelTMP = new unsigned char [(width*height)];
  GNX = 0;
  GNY = 0;
  for(int i=0;i<(width*height);i++)pixelTMP[i]=0;
  for(int i=0;i<frameNo;i++){
    GNX = O_pRemain[i].PointsOrganized[CordX];
    GNY = O_pRemain[i].PointsOrganized[CordY];
    if(GNX<width && GNY<height)pixelTMP[cord(GNX,GNY)] = 255;
    if(frameSkip > frameskipVal){pngmake(i, width, height, 0);frameSkip=0;cout << "Writing Frame " << i << "\r";}frameSkip++;
  }
  delete pixelTMP;
}

/* png image make */
/* Converts the three individual RGB channels into an RGB array for the lodepng library*/
void pngmake(int frameNum, int xRes, int yRes, int type){
    unsigned int res = xRes*yRes;
    unsigned int i = 0;
    unsigned char * rgbimage;
    //create a new array that's the appropriate size for our resolution. No alpha channel.
    rgbimage = new unsigned char [(res*3)];
    //Copy the RGB data over to an array suitable for the PNG conversion library.
    for(int d=0;d<res;d++){
        rgbimage[i] = pixelTMP[d];
        i++;
        rgbimage[i] = pixelTMP[d];
        i++;
        rgbimage[i] = pixelTMP[d];
        i++;
    }
    //lodepng_encode24_file(("./output/frame" + std::to_string(frameNum) + ".png"), rgbimage, xRes, yRes);
    stringstream fileNumber;
    switch(type){
        case 0: fileNumber << "./output/frame" << frameNum << ".png";
        break;
        case 1: fileNumber << "./debugOut/stage" << frameNum << ".png";
        break;
    }
    lodepng_encode24_file(fileNumber.str().c_str(), rgbimage, xRes, yRes);
    //cleanup
    delete rgbimage;
}

unsigned int cord(unsigned int x, unsigned int y){
    return (y*width)+x;
}

int GetArgs(int argc, char **argv){
float argNumber = 0;
  int gotFile = 1;
  for(int i=1;i<argc;i++){
    if(argv[i][0]=='-'){
        if(argv[i][1]!='i' && argv[i][1]!='h' && argv[i][1]!='T' &&
           argv[i][1]!='U' && argv[i][1]!='X' && argv[i][1]!='Y'){
            std::stringstream wasd(&argv[i][3]);
            wasd >> argNumber;
        }
        switch (argv[i][1]){
            case 't':
                if(argNumber>=0&&argNumber<=255)threashold=argNumber;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 255\n"; return 1;}
                break;
            case 'C':
                if(argNumber>=0&&argNumber<=6)CullingThreashold=argNumber;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 6\n"; return 1;}
                break;
            case 'S':
                if(argNumber>0&&argNumber<=255)MaxPointSpace=argNumber;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 255\n"; return 1;}
                break;
            case 'm':
                if(argNumber>=0&&argNumber<256)MinPointCluster=argNumber;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 255\n"; return 1;}
                break;
            case 'k':
                if(argNumber>=-1&&argNumber<=20000)frameskipVal=argNumber-1;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 20000\n"; return 1;}
                break;
            case 'L':
                if(argNumber>=0&&argNumber<=600)ALength=argNumber;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 600\n"; return 1;}
                break;
            case 's':
                if(argNumber>=0&&argNumber<=256000)sampleRate=argNumber;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 256000\n"; return 1;}
                break;
            case 'U':
                swapXY=0;
                break;
            case 'X':
                Xinvert=0;
                break;
            case 'Y':
                Yinvert=1;
                break;
            case 'T':
                traceImages=1;
                break;
            case 'V':
                if(argNumber>=0&&argNumber<=100)volume=argNumber;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 100\n"; return 1;}
                break;
            case 'f':
                if(argNumber>=0&&argNumber<=10)compFactorX=argNumber;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 10\n"; return 1;}
                break;
            case 'F':
                if(argNumber>=0&&argNumber<=10)compFactorY=argNumber;
                else {cout << "\nArgument Value Out Of Range For: " << &argv[i][0] << ", limit 0 - 10\n"; return 1;}
                break;
            case 'i':
                filename=&argv[i][3];
                gotFile = 0;
                break;
            case 'h':
                cout << helpText[0];
                return 1;
                break;
        }
    }
  }
  if(gotFile)cout << "Give filename using '-i' option, or -h for help text.\n";
  return gotFile;
}
