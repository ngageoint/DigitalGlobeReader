#include "DGDataReader.h"


DGDataReader::DGDataReader()
{
	this->mMapper = RPCMapper::RPCMapper();
}


DGDataReader::~DGDataReader()
{
}

int DGDataReader::aGetNumBands(){
	int rvalue = std::numeric_limits<int>::quiet_NaN();
	rvalue = this->mNumBands;
	return rvalue;
}

const vector<vector<DGDataReader::GeodeticPoint3D>> * DGDataReader::aGetGrid(){

	vector<vector<DGDataReader::GeodeticPoint3D>> * rvalue = &(this->mGridPoints);
	return rvalue;

}

DGDataReader::GeodeticPoint3D DGDataReader::aGetGridPoint(int xIndex, int yIndex){
	//gets the LLA for the point associated with a 2D grid indexed in x and y
	DGDataReader::GeodeticPoint3D rvalue;
	rvalue.alt = std::numeric_limits<double>::quiet_NaN();
	rvalue.lat = std::numeric_limits<double>::quiet_NaN();
	rvalue.lon = std::numeric_limits<double>::quiet_NaN();

	if (xIndex < this->aGetGrid()->size() && yIndex < this->aGetGrid()[0].size() && xIndex>0 && yIndex>0){
		rvalue = this->mGridPoints[xIndex][yIndex];
	}

	return rvalue;

}

RPCMapper::ImageCoordinates DGDataReader::aGetGridImageCoord(int xIndex, int yIndex){
	//gets the sensor location that the grid point projects to
	RPCMapper::ImageCoordinates rvalue;
	rvalue.XValue = std::numeric_limits<int>::quiet_NaN();
	rvalue.YValue = std::numeric_limits<int>::quiet_NaN();

	if (xIndex<this->mGridImageCoordMappings.size() && yIndex < this->mGridImageCoordMappings[0].size() && xIndex >=0 && yIndex >=0){
		rvalue = this->mGridImageCoordMappings[xIndex][yIndex];
	}

	return rvalue;
}

unsigned short DGDataReader::aGetGridPaint(int xIndex, int yIndex, int bandIndex){
	//gets the pixel values for the sensor location that the LLA of the Grid Point projects to
	unsigned short rvalue = std::numeric_limits<unsigned short>::quiet_NaN();

	if (xIndex < this->mGridDataMappings.size() && yIndex < this->mGridDataMappings[0].size() && 
		xIndex >=0 && yIndex >=0 &&
		bandIndex >= 0 && bandIndex < this->aGetNumBands()){
		rvalue = this->mGridDataMappings[xIndex][yIndex][bandIndex];
	}

	return rvalue;
}

bool DGDataReader::aSetGridImageCoord(int xIndex, int yIndex, RPCMapper::ImageCoordinates ImageCoord){
	//assigns the sensor location that the LLA of the grid projects to to the grid
	bool rvalue = false;
	if (xIndex < this->mGridImageCoordMappings.size() && yIndex < this->mGridImageCoordMappings[0].size() && xIndex >=0 && yIndex >=0){
		this->mGridImageCoordMappings[xIndex][yIndex] = ImageCoord;
		rvalue = true;
	}
	return rvalue;
}
bool DGDataReader::aSetGridPoint(int xIndex, int yIndex, DGDataReader::GeodeticPoint3D value){
	//attaches an LLA to the Grid
	bool rvalue = false;
	if (xIndex < this->aGetGrid()->size() && yIndex < this->aGetGrid()[0].size() && xIndex>0 && yIndex>0){
		this->mGridPoints[xIndex][yIndex] = value;
		rvalue = true;
	}
	return rvalue;
}
double DGDataReader::aGetGSD(){
	return this->mGSD;
}
const unsigned short * DGDataReader::aGetImage(int bandIndex){
	//gets a pointer to the image data for the bandIndex
	unsigned short * rvalue = nullptr;

	if (bandIndex >= 0 && bandIndex < this->mImages.size()){
		rvalue = this->mImages[bandIndex];
	}

	return rvalue;
}

int DGDataReader::aGetImageRowLength(){
	//gets the number of pixels in a row i.e. the image width
	int rvalue = std::numeric_limits<int>::quiet_NaN();
	rvalue = this->mImageRowLength;
	return rvalue;
}

int DGDataReader::aGetImageRowsTotal(){
	//gets the number of rows in an image i.e. the image height
	int rvalue = std::numeric_limits<int>::quiet_NaN();
	rvalue = this->mImageRowsTotal;
	return rvalue;
}

RPCMapper DGDataReader::aGetMapper(){
	//gets the RPC mapper
	RPCMapper rvalue = this->mMapper;
	return rvalue;
}

const unsigned char * DGDataReader::aGetScaledImage(int bandIndex){
	//gets a scaled version of the image in the bandIndex channel
	unsigned char * rvalue = nullptr;

	if (bandIndex >= 0 && bandIndex < this->mScaledImages.size()){
		rvalue = this->mScaledImages[bandIndex];
	}

	return rvalue;
}

double DGDataReader::aGetScaleFactor(){
	//gets image scale factor
	double rvalue = std::numeric_limits<double>::quiet_NaN();
	rvalue = this->mScaleFactor;
	return rvalue;
}

unsigned char DGDataReader::aGetScaleShift(int bandIndex){
	//gets the number of right shifts needed to get each band under 255 for display
	unsigned char rvalue = std::numeric_limits<unsigned char>::quiet_NaN();
	if (bandIndex >= 0 && bandIndex < this->mScaleShifts.size()){
		rvalue = this->mScaleShifts[bandIndex];
	}
	return rvalue;
}

bool DGDataReader::aSetNumBands(int value){
	//sets number of bands in the image
	bool rvalue = false;

	if (value > 0){
		this->mNumBands = value;
		rvalue = true;
	}

	return rvalue;
}
bool DGDataReader::aSetGSD(double value){
	//sets the GSD of the grid
	bool rvalue = true;
	this->mGSD = value;
	return rvalue;
}

bool DGDataReader::aSetImageRowLength(int pixelsPerRow){
	//sets the width of the iamge
	bool rvalue = true;
	this->mImageRowLength = pixelsPerRow;
	return rvalue;
}

bool DGDataReader::aSetImageRowsTotal(int RowsPerImage){
	//sets the height of the image
	bool rvalue = true;
	this->mImageRowsTotal = RowsPerImage;
	return rvalue;
}

bool DGDataReader::aSetScaledImages(int gridOffsetX, int gridOffsetY, int xSize, int ySize){
	//sets the scaled version of the images as they appear painted on the grid

	this->aSetScaleShifts( gridOffsetX, gridOffsetY, xSize, ySize);
	bool rvalue = false;
	int lNumBands = 8;
	double lScaleFactor = 255.0 / 65535.0;
	double lMaxValue = 0 ;
	unsigned short lOriginalPixelValue;
	unsigned short lScaledPixelValue;

	// right shifts the image data to < 8 bits
	for (int lCurrentBand = 0; lCurrentBand < lNumBands; lCurrentBand++){
		//allocate memory for the images
		lMaxValue = 0;
		this->mScaledImages.push_back(new unsigned char[xSize*ySize]);

			for (int x = 0; x < xSize; x++)
			{
				for (int y = 0; y < ySize; y++){
					lOriginalPixelValue = this->aGetGridPaint(x + gridOffsetX, y + gridOffsetY, lCurrentBand);
					lScaledPixelValue = lOriginalPixelValue >> this->aGetScaleShift(lCurrentBand);
					if (lScaledPixelValue > 255){
						this->mScaledImages[lCurrentBand][x * ySize + y] = 255; // some values may be saturated because we set the bit shifting at 90 percent of max
					}
					else{
						this->mScaledImages[lCurrentBand][x * ySize + y] = (unsigned char)lScaledPixelValue;
					}
				}

			}
			
		
	}


	return rvalue;
}

bool DGDataReader::aSetScaleFactor(double value){
	// this was used for linearly scaling pixel data from 12 bits to 8 bits
	bool rvalue = true;
	this->mScaleFactor = value;
	return rvalue;
}

bool DGDataReader::aSetScaleShifts(int gridOffsetX, int gridOffsetY, int xSize, int ySize){
	//determines a shift for eachband that saturates < 10% of each band
	bool rvalue = false;
	int lNumBands = 8;
	int lBitShiftHistogram[8];
	int lCumulativeBitShiftHistogram[8];
	unsigned short lDummy;
	int lShiftCounter = 0;

	for (int lCurrentBand = 0; lCurrentBand < lNumBands; lCurrentBand++){
		//reset Histogram datasets
		for (int i = 0; i < 8; i++)
		{
			lBitShiftHistogram[i] = 0;
			lCumulativeBitShiftHistogram[i] = 0;
		}
		//scan the image
			for (int x = 0; x < xSize; x++)
			{
				for (int y = 0; y < ySize; y++){
					lShiftCounter = 0;
					lDummy = this->aGetGridPaint(x + gridOffsetX, y + gridOffsetY, lCurrentBand);
					
					//count how many times you have to shift the value to make it fit in a byte
					while (lDummy > 255){
						lDummy >>= 1;
						lShiftCounter++;
					}
					lBitShiftHistogram[lShiftCounter]++;

				}

			}
	//determine which shift we are going to use
			int lTotalValues = 0;
			lTotalValues += lBitShiftHistogram[0];
			lCumulativeBitShiftHistogram[0] += lBitShiftHistogram[0];
			for (int i = 1; i < lNumBands; i++)
			{
				lTotalValues += lBitShiftHistogram[i];
				lCumulativeBitShiftHistogram[i] += lBitShiftHistogram[i] + lCumulativeBitShiftHistogram[i-1];
			}
			for (int i = 0; i <lNumBands; i++)
			{
				if (((double)lCumulativeBitShiftHistogram[i]) / ((double)lTotalValues) > .9){
					this->mScaleShifts.push_back(i);
					i = lNumBands;
				}
				
			}
	}

	rvalue = true;
	return rvalue;
}
bool DGDataReader::aInitializeDefaultGrid(double alt){
	//sets up the grid
	bool rvalue = false;

	//make sure that the bounding box is set and that the GSD value is set
	if (!isnan(this->mMapper.aGetRPCBoundingBoxElement(0)) && !isnan(this->aGetGSD())){

	//##################################
	//Code modified From NGA run webpage to calculate meters per degree of latitude / Longitude
	//msi.nga.mil/MSISiteContent/StaticFiles/Calculators/degree.html
	double lLat =  3.14159265358979 / 180.0 *this->mMapper.aGetLLAOffsetsElement(0); // offset for lat in middle of bounding box converted to radians
	double lm1 = 111132.92;		// latitude calculation term 1
	double lm2 = -559.82;		// latitude calculation term 2
	double lm3 = 1.175;			// latitude calculation term 3
	double lm4 = -0.0023;		// latitude calculation term 4
	double lp1 = 111412.84;		// longitude calculation term 1
	double lp2 = -93.5;			// longitude calculation term 2
	double lp3 = 0.118;			// longitude calculation term 3

	// Calculate the length of a degree of latitude and longitude in meters
	double lMetersPerLatDeg = lm1 + (lm2 * cos(2 * lLat)) + (lm3 * cos(4 * lLat)) +
		(lm4 * cos(6 * lLat));
	double lMetersPerLonDeg = (lp1 * cos(lLat)) + (lp2 * cos(3 * lLat)) +
		(lp3 * cos(5 * lLat));
	//End of NGA webpage code
	//###################################

	//get the number of degrees in a single GSD of Latitude and Longitude
	double lDegPerGSDLat = (this->aGetGSD() / lMetersPerLatDeg);
	double lDegPerGSDLon = (this->aGetGSD() / lMetersPerLonDeg);

	//Determine how many GSDs fit into the RPC bounding box
	int lGridSizeLat = (this->mMapper.aGetRPCBoundingBoxElement(3) - this->mMapper.aGetRPCBoundingBoxElement(0)) / lDegPerGSDLat;
	int lGridSizeLon = (this->mMapper.aGetRPCBoundingBoxElement(4) - this->mMapper.aGetRPCBoundingBoxElement(1)) / lDegPerGSDLon;
	
	//create the grid and load it with points
	for (int i = 0; i < lGridSizeLat;i++){
		this->mGridPoints.push_back(vector<GeodeticPoint3D>());

		for (int j = 0; j < lGridSizeLon; j++){
			this->mGridPoints[i].push_back(GeodeticPoint3D());
			
			this->mGridPoints[i][j].lat = this->mMapper.aGetRPCBoundingBoxElement(0) + lDegPerGSDLat * ((double)i);
			this->mGridPoints[i][j].lon = this->mMapper.aGetRPCBoundingBoxElement(1) + lDegPerGSDLon * ((double)j);
			this->mGridPoints[i][j].alt = alt;

		}
	}
	

	//the grid has been created so change the rvalue
	rvalue = true;
	}

	return rvalue;
}
bool DGDataReader::aIntializeMapper(){
	//sets up the RPC mapper 
	bool rvalue = false;

	this->mMapper.cGenerateBoundingBox(); //generate the bounding box

	for (int i = 0; i < this->mMapper.sLLASize * 2; i++){
		if (std::isnan(this->mMapper.aGetRPCBoundingBoxElement(i))) i = this->mMapper.sLLASize * 2; //a NaN exist so exit loop somethings wrong
		if (!std::isnan(this->mMapper.aGetRPCBoundingBoxElement(i)) && i == this->mMapper.sLLASize * 2 - 1) rvalue = true; // No NaNs where present so set rvalue to true;
	}
	return rvalue;
}
bool DGDataReader::aLoadImages(char* filename){
	//load all the tiff images straigt into memory
	bool rvalue = false;
	
	//open tiff for reading the tiff
	TIFF* lTif = TIFFOpen(filename, "r");

	if (lTif) { // tiff file was found

		uint32 lImageLength;
		tdata_t lDataBuffer;
		uint32 lRow;
		short lBandConfig;
		int lBytesPerLine;
		uint32 lImageWidth, lImageHeight;


		lBytesPerLine = TIFFScanlineSize(lTif);
		lDataBuffer = _TIFFmalloc(lBytesPerLine);				// this buffer gives 2 bytes per sample for a length width*2

		TIFFGetField(lTif, TIFFTAG_IMAGEWIDTH, &lImageWidth);	//Image Width
		TIFFGetField(lTif, TIFFTAG_IMAGELENGTH, &lImageHeight);	//Image Height
		TIFFGetField(lTif, TIFFTAG_IMAGELENGTH, &lImageLength); //rows of pixels in the image
		TIFFGetField(lTif, TIFFTAG_PLANARCONFIG, &lBandConfig); //get the way pixels are packed clumped RGBRGBRGB or plannar RRRGGGBBB
		
		this->aSetImageRowLength(lImageWidth);
		this->aSetImageRowsTotal(lImageHeight);

		if (lBandConfig == PLANARCONFIG_CONTIG) {
			for (lRow = 0; lRow < lImageLength; lRow++)
				TIFFReadScanline(lTif, lDataBuffer, lRow);
		}
		else if (lBandConfig == PLANARCONFIG_SEPARATE) { // each plane holds a band 
			
			uint16 lCurrentBand, lNumBands;
			TIFFGetField(lTif, TIFFTAG_SAMPLESPERPIXEL, &lNumBands);
			
			this->aSetNumBands((int)lNumBands); //save the number of bands to the class instance

			for (lCurrentBand = 0; lCurrentBand < lNumBands; lCurrentBand++){
				//allocate memory for the images

				this->mImages.push_back(new unsigned short[lImageWidth*lImageHeight]);

				for (lRow = 0; lRow < lImageLength; lRow++){
					TIFFReadScanline(lTif, lDataBuffer, lRow, lCurrentBand); //go through each component plane (s) and for each row copy the row data to the buffer
					std::memcpy(&(this->mImages[lCurrentBand][lRow*lImageWidth]), lDataBuffer, lBytesPerLine);
				}
			}
		}
		_TIFFfree(lDataBuffer);
		TIFFClose(lTif);
		rvalue = true;
	}

	return rvalue;
}

bool DGDataReader::aLoadXMLDoc(char* filename){
	//this method loads the RPC data from the XL document that is provided by DG with each image
	bool rvalue = false;
	tinyxml2::XMLDocument							lDataXML;

	lDataXML.LoadFile(filename);


	std::stringstream lBuffer;
	std::string lInput;
	double lDummy;

	//grab the mean GSD for the Image
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("IMD")->FirstChildElement("IMAGE")->FirstChildElement("MEANPRODUCTGSD")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->aSetGSD(lDummy);
	lBuffer.str("");
	lBuffer.clear();


	//grab the Sample offset
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("SAMPOFFSET")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetPixelOffsetsElement(0, lDummy);
	lBuffer.str("");
	lBuffer.clear();

	//grab the Line offset
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("LINEOFFSET")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetPixelOffsetsElement(1, lDummy);
	lBuffer.str("");
	lBuffer.clear();


	//grab the lat offset
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("LATOFFSET")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetLLAOffsetsElement(0, lDummy);
	lBuffer.str("");
	lBuffer.clear();

	//grab the long offset
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("LONGOFFSET")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetLLAOffsetsElement(1, lDummy);
	lBuffer.str("");
	lBuffer.clear();

	//grab the alt offset
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("HEIGHTOFFSET")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetLLAOffsetsElement(2, lDummy);
	lBuffer.str("");
	lBuffer.clear();

	//grab the lineScale 
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("LINESCALE")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetPixelScaleFactorsElement(1, lDummy);
	lBuffer.str("");
	lBuffer.clear();

	//grab the SampleScale 
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("SAMPSCALE")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetPixelScaleFactorsElement(0, lDummy);
	lBuffer.str("");
	lBuffer.clear();

	//grab the LatScale 
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("LATSCALE")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetLLAScaleFactorsElement(0, lDummy);
	lBuffer.str("");
	lBuffer.clear();

	//grab the LonScale 
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("LONGSCALE")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetLLAScaleFactorsElement(1, lDummy);
	lBuffer.str("");
	lBuffer.clear();

	//grab the AltScale 
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("HEIGHTSCALE")->GetText();
	lBuffer << lInput;
	lBuffer >> lDummy;
	this->mMapper.aSetLLAScaleFactorsElement(2, lDummy);
	lBuffer.str("");
	lBuffer.clear();



	//grab the Line Numerator Coefficients
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("LINENUMCOEFList")->FirstChildElement("LINENUMCOEF")->GetText();

	std::istringstream lBuffer2(lInput);
	std::vector<std::string> lCoefficients;

	std::copy(std::istream_iterator<std::string>(lBuffer2),
		std::istream_iterator<std::string>(),
		std::back_inserter(lCoefficients));

	for (int i = 0; i < lCoefficients.size(); i++){

		lBuffer << lCoefficients[i];
		lBuffer >> lDummy;
		this->mMapper.aSetYNumCoefficientsElement(i, lDummy);
		lBuffer.str("");
		lBuffer.clear();

	}

	//grab the Line Numerator Coefficients
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("LINEDENCOEFList")->FirstChildElement("LINEDENCOEF")->GetText();

	lBuffer2 = std::istringstream(lInput);
	lCoefficients.clear();

	std::copy(std::istream_iterator<std::string>(lBuffer2),
		std::istream_iterator<std::string>(),
		std::back_inserter(lCoefficients));

	for (int i = 0; i < lCoefficients.size(); i++){

		lBuffer << lCoefficients[i];
		lBuffer >> lDummy;
		this->mMapper.aSetYDenCoefficientsElement(i, lDummy);
		lBuffer.str("");
		lBuffer.clear();

	}

	//grab the Sample Numerator Coefficients
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("SAMPNUMCOEFList")->FirstChildElement("SAMPNUMCOEF")->GetText();

	lBuffer2 = std::istringstream(lInput);
	lCoefficients.clear();

	std::copy(std::istream_iterator<std::string>(lBuffer2),
		std::istream_iterator<std::string>(),
		std::back_inserter(lCoefficients));

	for (int i = 0; i < lCoefficients.size(); i++){

		lBuffer << lCoefficients[i];
		lBuffer >> lDummy;
		this->mMapper.aSetXNumCoefficientsElement(i, lDummy);
		lBuffer.str("");
		lBuffer.clear();

	}

	//grab the Sample Denomenator Coefficients
	lInput = lDataXML.FirstChildElement("isd")->FirstChildElement("RPB")->FirstChildElement("IMAGE")->FirstChildElement("SAMPDENCOEFList")->FirstChildElement("SAMPDENCOEF")->GetText();

	lBuffer2 = std::istringstream(lInput);
	lCoefficients.clear();

	std::copy(std::istream_iterator<std::string>(lBuffer2),
		std::istream_iterator<std::string>(),
		std::back_inserter(lCoefficients));

	for (int i = 0; i < lCoefficients.size(); i++){

		lBuffer << lCoefficients[i];
		lBuffer >> lDummy;
		this->mMapper.aSetXDenCoefficientsElement(i, lDummy);
		lBuffer.str("");
		lBuffer.clear();

	}

	return rvalue;
}

bool  DGDataReader::cMapData(){
	//set a LLA to each grid point
	bool rvalue = false;

	for (int i = 0; i < this->mGridPoints.size(); i++){
		this->mGridImageCoordMappings.push_back(vector<RPCMapper::ImageCoordinates>());

		for (int j = 0; j < this->mGridPoints[0].size(); j++){
			this->mGridImageCoordMappings[i].push_back(RPCMapper::ImageCoordinates());
			this->aSetGridImageCoord(i,j, this->mMapper.MapLLA(this->mGridPoints[i][j].lat, this->mGridPoints[i][j].lon, this->mGridPoints[i][j].alt));
		}
	}

	rvalue = true;
	return rvalue;
}
bool DGDataReader::cGenerateGridData(){
	bool rvalue = false;
	RPCMapper::ImageCoordinates lCoord;
	
	for (int i = 0; i < this->mGridPoints.size(); i++){
		this->mGridDataMappings.push_back( vector<map<int,unsigned short>>());

		for (int j = 0; j < this->mGridPoints[0].size(); j++){
			this->mGridDataMappings[i].push_back(map<int, unsigned short>());
			lCoord = this->aGetGridImageCoord(i,j);

			for (int k = 0; k < this->aGetNumBands(); k++){
			
				this->mGridDataMappings[i][j][k] = this->aGetImage(k)[lCoord.YValue*this->aGetImageRowLength() + lCoord.XValue];
			
			}
			
		}
	}
	rvalue = true;
	return rvalue;
}

DGDataReader::GeodeticPoint3D DGDataReader::cDeltas(double lat){
	//gets lat and lon deltas for movement along the grid's axes
	DGDataReader::GeodeticPoint3D rvalue;

	//##################################
	//Code modified From NGA run webpage to calculate meters per degree of latitude / Longitude
	//msi.nga.mil/MSISiteContent/StaticFiles/Calculators/degree.html
	double lLat = 3.14159265358979 / 180.0 *lat; 
	double lm1 = 111132.92;		// latitude calculation term 1
	double lm2 = -559.82;		// latitude calculation term 2
	double lm3 = 1.175;			// latitude calculation term 3
	double lm4 = -0.0023;		// latitude calculation term 4
	double lp1 = 111412.84;		// longitude calculation term 1
	double lp2 = -93.5;			// longitude calculation term 2
	double lp3 = 0.118;			// longitude calculation term 3

	// Calculate the length of a degree of latitude and longitude in meters
	double lMetersPerLatDeg = lm1 + (lm2 * cos(2 * lLat)) + (lm3 * cos(4 * lLat)) +
		(lm4 * cos(6 * lLat));
	double lMetersPerLonDeg = (lp1 * cos(lLat)) + (lp2 * cos(3 * lLat)) +
		(lp3 * cos(5 * lLat));
	//End of NGA code
	//###################################

	//get the number of degrees in a single GSD of Latitude and Longitude
	rvalue.lat = (this->aGetGSD() / lMetersPerLatDeg);
	rvalue.lon = (this->aGetGSD() / lMetersPerLonDeg);
	
	return rvalue;
}
