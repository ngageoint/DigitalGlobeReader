#pragma once

//standard files
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <map>

//custom files
#include <RPCMapper.h>
#include <tinyxml2.h>	// https://codeload.github.com/leethomason/tinyxml2/zip/master
#include <tiffio.h>		// http://bigtiff.org/libtiff.lib-4.1.zip

using namespace tinyxml2;
using namespace std;


class DGDataReader
{

public:
	struct GeodeticPoint3D{
		double lat;
		double lon;
		double alt;
	};

private:
	//note that these members can get very large and blow through the 4GB limit of a 32 bit program hence the x64 compilation
	RPCMapper										mMapper;					// Used to map the imagery from a 3D volume to the images
	int												mNumBands;					// Number of bands in the DG image
	vector<vector<GeodeticPoint3D>>					mGridPoints;				// Grid that holds the Lat, Long, and Alt of various points, typically a plane, in 3D space
	vector<vector<RPCMapper::ImageCoordinates>>		mGridImageCoordMappings;	// Holds the image coordinates that the grid point maps to via RPCs
	vector<vector<std::map<int,unsigned short>>>	mGridDataMappings;			// Grid that holds the Lat, Long, and Alt of various points, typically a plane, in 3D space
	vector<unsigned short *>						mImages;					// the Digital Globe images that we will be mapping with
	double											mGSD;						// the provided MEAN GSD for the images
	vector <unsigned char *>						mScaledImages;				// sub images that have been bit shifted to fit within the 8 bit limit
	double											mScaleFactor;				// scale factor for scaling images down to  8 bit currently superceded by bit shifting
	vector <unsigned char>							mScaleShifts;				// holds the number of shifts required for each band to get 90% (because I had to choose something that didn't allow outliers to dominate the scaling) of the band data down to 8 bits
	int												mImageRowLength;			// length of an image's pixel row. this is used to index into the images
	int												mImageRowsTotal;			// number of rows in Image

public:
	DGDataReader();
	~DGDataReader();

	//################
	//Accessor Methods
	//################
	
	//get methods
	int										aGetNumBands();
	const vector<vector<GeodeticPoint3D>> * aGetGrid();
	GeodeticPoint3D							aGetGridPoint(int xIndex, int yIndex);
	RPCMapper::ImageCoordinates				aGetGridImageCoord(int xIndex, int yIndex);
	unsigned short							aGetGridPaint(int xIndex, int yIndex, int bandIndex);
	double									aGetGSD();
	const unsigned short *					aGetImage(int bandIndex);
	int										aGetImageRowLength();
	int										aGetImageRowsTotal();
	RPCMapper								aGetMapper();
	const unsigned char *					aGetScaledImage(int bandIndex);
	double									aGetScaleFactor();
	unsigned char							aGetScaleShift(int bandIndex);

	//set methods
	bool			aSetNumBands(int numBands);
	bool			aSetGridPoint(int xIndex, int yIndex, GeodeticPoint3D Point);
	bool			aSetGridImageCoord(int xIndex, int yIndex, RPCMapper::ImageCoordinates ImageCoord);
	bool			aSetGSD(double value);
	bool			aSetImageRowLength(int pixelsPerRow);
	bool			aSetImageRowsTotal(int RowsPerImage);
	bool			aSetScaledImages(int gridOffsetX, int gridOffsetY, int xSize, int ySize);
	bool			aSetScaleFactor(double value);
	bool			aSetScaleShifts(int gridOffsetX, int gridOffsetY, int xSize, int ySize);
	

	//initialization methods
	bool			aIntializeMapper();						//build the RPC Bounding Box
	bool			aInitializeDefaultGrid(double alt);		//Initialize a horizontal grid at the altitude passed
	bool			aLoadImages(char* filename);			//load all the bands from the tiff into Images;
	bool			aLoadXMLDoc(char* filename);			//read XML file and fille in the member variables for the RPC mapper
	
	//#################
	//Calculate Methods
	//#################

	bool			cMapData();								//Map the 3D points to the 2D image plane
	bool			cGenerateGridData();					//Assign Image point intensity values to the grid points
	GeodeticPoint3D cDeltas(double lat);					//calculates the deg / meter for latitude and longitude at a specific latitude
};

