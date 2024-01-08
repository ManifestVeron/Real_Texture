#pragma once
#include <utility>
#include <stdint.h>

#ifdef  COMPILE_DLL
#define  DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif
extern "C"
{
	namespace google {
		namespace protobuf {
			class Message;
		}
	}
	class GeoPosition;
	class Orientation;
	class Vector3d;
	class Satellites;
	class PseudoCoordinate;
	class Navigation;
	class AngleDimension;
	class FrameDescription;
	//enum CameraTypes;
	//enum LensTypes;
}
namespace VF {
	enum TCameraTypes : int { UnknownCamera = 0, CANON550D = 1, EVS = 2, TAU640 = 3, CANON650D = 4 };
	enum TLensTypes : int {	UnknownLens = 0,	TheiaSL940 = 1,	EF24F28 = 2,	EF50F18 = 3 };
}
 //----interface data structures
class DLL_DECL TGeoPosition
{
	double latitude; // Degrees //required  // =1
	double longitude; // Degrees//required
	double altitude; // Meters  //required
public: std::pair<bool, int32_t> epsg; // EPSG number 
public: TGeoPosition(double la = 0., double lo = 0., double al = 0.) :latitude(la), longitude(lo), altitude(al) {
	epsg.first = false;
}
public: TGeoPosition(double la, double lo, double al, int32_t _epsg) :latitude(la), longitude(lo), altitude(al), epsg(true, _epsg) {}
public: TGeoPosition(GeoPosition const& gpos);

public:double GetLatitude() { return latitude; }
public:double GetLongitude() { return longitude; }
public:double GetAltitude() { return altitude; }
public:void SetEPSG(int32_t val) { epsg.first = true;	epsg.second = val; }
};
class DLL_DECL TOrientation
{
	float roll; // Degrees     //required     // =1
	float pitch; // Degrees    //required
	float yaw; // Degrees       //required
public: TOrientation(float r = 0., float p = 0., float y = 0.) :roll(r), pitch(p), yaw(y) {}
public: TOrientation(Orientation const& val);
public:float GetRoll() { return roll; }
public:double GetPitch() { return pitch; }
public:double GetYaw() { return yaw; }
};
class DLL_DECL TVector3d
{
	double X; //            //required   // =1
	double Y; // Meters     //required
	double Z; // Meters     //required
public:std::pair<bool, int32_t> epsg; // EPSG number 
public: TVector3d(Vector3d const& v);

public: TVector3d(double x = 0., double y = 0., double z = 0.) :X(x), Y(y), Z(z){	epsg.first = false;}
public: TVector3d(double x, double y, double z, int32_t _epsg) :X(x), Y(y), Z(z), epsg(true, _epsg){}
public:double GetX() { return X; }
public:double GetY() { return Y; }
public:double GetZ() { return Z; }
public:void SetEPSG(int32_t val) { epsg.first = true;	epsg.second = val; }

};
class DLL_DECL TSatellites
{
public:std::vector< char> 	 satellites;// = 1; //optional  	// Array of satellites numbers used in calculation
public:std::vector< char> 	 potentiallySatellites;// = 2; //optional // Array of potentitaly satellites
public:TSatellites() {};
public:TSatellites(Satellites const& s);
};
class DLL_DECL TPseudoCoordinate
{
public:std::vector< int32_t> pseudoDistance;// = 1 optional
public:std::vector< int32_t> pseudoVelocity;// optional
public:std::vector< int32_t> debugInfo;     //optional
public:TPseudoCoordinate() {}
public:TPseudoCoordinate(PseudoCoordinate const& pc);
};
class DLL_DECL TNavigation
{
	int64_t timestamp;                 // Timestamp, .Net ticks                  //required
public:
	std::pair<bool, float> gpsCourse;                   // GPS track course, degrees      // = 1
	std::pair<bool, float> velocity;                    // Velocity, meters per second
	std::pair<bool, std::string> name;                  // User friendly navigation chip name
	std::pair<bool, TGeoPosition> geoPosition;          // Geodetic position
	std::pair<bool, TOrientation> orientation;          // Orientation of object
	std::pair<bool, TVector3d> XYZPosition;             // Geocentric position
	std::pair<bool, TVector3d> XYZSpeed;                // Geocentric velocity
	std::pair<bool, int32_t> tow;                       // Time of week
	std::pair<bool, int32_t> debug;                     // Mystical debug value
	std::pair<bool, float> quality;                     // Accuracy 0..100, %
	std::pair<bool, TSatellites> satellites;            // Satellites
	std::pair<bool, TPseudoCoordinate> pseudoCoordinate; // Pseudo coordinate
	std::pair<bool, double> aerialAltitude;             // Aerial altitude
	std::pair<bool, int32_t> strobeMarker;              // Strobe marker 
														//public: TNavigation() :orientation{ false, TOrientation(0.,0.,0.) }{}
public: TNavigation(int64_t _t);
public: TNavigation(Navigation& _proto);
public: Navigation EncodeProtoMessage();

public:void SetGpsCourse(float val) { gpsCourse.first = true;	gpsCourse.second = val; }
public:void SetVelocity(float val) { velocity.first = true;	velocity.second = val; }
public:void SetName(std::string& val) { name.first = true;	name.second = val; }
public:void SetGeoPosition(TGeoPosition val) { geoPosition.first = true;	geoPosition.second = val; }
public:void SetOrientation(TOrientation val) { orientation.first = true;	orientation.second = val; }
public:void SetXYZPosition(TVector3d val) { XYZPosition.first = true;	XYZPosition.second = val; }
public:void SetXYZSpeed(TVector3d val) { XYZSpeed.first = true;	XYZSpeed.second = val; }
public:void SetTow(int32_t val) { tow.first = true;	tow.second = val; }
public:void SetDebug(int32_t val) { debug.first = true;	debug.second = val; }
public:void SetQuality(float val) { quality.first = true;	quality.second = val; }
public:void SetSatellites(TSatellites val) { satellites.first = true;	satellites.second = val; }
public:void SetPseudoCoordinate(TPseudoCoordinate val) { pseudoCoordinate.first = true;	pseudoCoordinate.second = val; }
public:void SetAerialAltitude(double val) { aerialAltitude.first = true;	aerialAltitude.second = val; }
public:void SetStrobeMarker(int32_t val) { strobeMarker.first = true;	strobeMarker.second = val; }
public: int64_t GetTimeStamp() { return timestamp; }


};
class DLL_DECL TAngleDimension {
	float width;// required // = 1;
	float height;// required
public:	TAngleDimension(float w = 0., float h = 0.) : width(w), height(h) {}
public: float GetWidth() { return width; }
public: float GetHeight() { return height; }

};
class DLL_DECL TFrameDescription
{
	std::vector< TNavigation> geoData;// = { TNavigation() }; // Attached navigation segment      //required // = 1
public:
	std::pair<bool, TAngleDimension> angleDimension;    // Frame angle dimensions                               
	std::pair<bool, VF::TCameraTypes> cameraType;           // Camera type                                          // [default = UnknownCamera];
	std::pair<bool, VF::TLensTypes> lensType;               // Lens type                                            //[default = UnknownLens]; 
	std::pair<bool, float> zoom;                        // Current zoom value                                   
	std::pair<bool, std::string> description;           // User-friendly description of camera                  
	std::pair<bool, std::string> lensName;              // User friendly lens name                              
	std::pair<bool, int64_t> navBegin;                  // Navigation segment begin                             
	std::pair<bool, int64_t> navEnd;                    // Navigation segment end                               
	std::pair<bool, uint32_t> strobeMask;               // Strobe mask of frame device   

public: TFrameDescription();
public: google::protobuf::Message* EncodeProto();

public: TFrameDescription(char* _buffer, size_t sz);//decode protobuf message

public: size_t GetNavigationCount() { return geoData.size(); }

public: TNavigation* GetNavigationItem(int32_t pos)
{
	if (pos < geoData.size())
		return &geoData[pos];
	else
		return nullptr;
}
public: void AddNavigationItem(TNavigation n) {
	geoData.push_back(n);
}
public:void SetAngleDimension(float _width, float _height) {
	angleDimension.first = true;
	angleDimension.second = TAngleDimension(_width, _height);
}
public:void SetNavigation(int64_t n_begin, int64_t n_end) {
	navBegin.first = true;	navBegin.second = n_begin;	//
	navBegin.first = true;	navEnd.second = n_end;
}

public:void SetCameraTypes(VF::TCameraTypes  val);// { cameraType.first = true;	cameraType.second = val; }
public:void SetLensTypes(VF::TLensTypes val);// { lensType.first = true;	lensType.second = val; }

public:void SetZoom(float val) { zoom.first = true;	zoom.second = val; }
public:void SetDescription(std::string& val) { description.first = true;	description.second = val; }
public:void SetLensName(std::string& val) { lensName.first = true;	lensName.second = val; }
public:void SetStrobeMask(uint32_t val) { strobeMask.first = true;	strobeMask.second = val; }
};

//-------------------------------------------------------------------------------------------
struct DLL_DECL TPicInfo
{
	double x00; // Координата x левого верхнего угла //required // = 1
	double y00; // Координата y левого верхнего угла //required
	double x01; // Координата x правого верхнего угла//required
	double y01; // Координата y правого верхнего угла//required
	double x10; // Координата x левого нижнего угла  //required
	double y10; // Координата y левого нижнего угла  //required
	double x11; // Координата x правого нижнего угла //required
	double y11; // Координата y правого нижнего угла //required
	int64_t time; // Время получения снимка          //required
	int32_t width; // ширина изображения в пикселях  //required
	int32_t height; // высота изображения в пикселях //required
					//
					//private:	
					//Picinfo* pic_info = { nullptr };
					//TPicInfo(const TPicInfo&) {};
					//TPicInfo& operator=(TPicInfo&) {};
					//public: TPicInfo(   double _x00, double _x01, double _x10, double _x11
					//             , double _y00, double _y01, double _y10, double _y11
					//           ,  int64_t _time, int32_t _width, int32_t _height)
					//{}
					//std::cout << "size after serilizing is " << proto_info->ByteSize() 
					//<< ' ' << proto_info->ByteSizeLong() << std::endl;
					//public:	~TPicInfo()
					//{delete pic_info;}
	google::protobuf::Message* EncodeProto();
	TPicInfo(char* _buffer, size_t sz);
	TPicInfo() {}
};
struct DLL_DECL TUAVData
{
	int board; 	            // Board number
	unsigned int number; 	// Packet number
	unsigned int flags; 	// Flags related to header specifics: checksum absence etc
	long long timestamp; 	// Time in arbitary format
	double latitude; 	    // Plane latitude (degrees)
	double longitude;     	// Plane longitude (degrees)
	float altitude;         // Plane altitude (meters)
	float roll;             // Plane roll (degrees)
	float pitch;            // Plane pitch (degrees)
	float yaw;              // Plane orientation yaw (degrees)
	float courseYaw;        // Plane course yaw (degrees)
	float speed;            // Plane speed (km/h)
};
enum  PacketType { VIDEO = 0xAABBCCDD, RADAR = 0xAABBCCEE , JSON = 0xAACCDDFF};
struct DLL_DECL FrameData: TPicInfo, TFrameDescription
{
	// TFrameDescription proto;
	TUAVData uav;
	PacketType type ;

	FrameData(TFrameDescription frame, TUAVData u, PacketType t):uav(u),type(t)
	{
		static_cast<TFrameDescription&>(*this) = frame;
	}
	FrameData(TPicInfo info, TUAVData u, PacketType t) :uav(u), type(t)
	{
		static_cast<TPicInfo&>(*this) = info;
	}
	FrameData() {}
};
namespace cv{	class Mat;}
typedef std::pair<cv::Mat, FrameData> TProtoData;
//----recv interface
DLL_DECL int InitReceive(unsigned short port);
DLL_DECL void CloseReceive();
DLL_DECL TProtoData ReceiveData(void);
//----send interface
DLL_DECL int InitSend(const char* _ip, unsigned short s_port, size_t _frame_sz, size_t _ratio);
DLL_DECL int CloseSend();
DLL_DECL void SendData(const unsigned char* img_array, size_t img_array_size, FrameData _proto_frame,int _frame_number);
//multicast interface
DLL_DECL int InitMulticastUdpReceive(const char* _interface_ip, const char* _multi_ip, unsigned short _multi_port);
DLL_DECL void CloseMulticastUdpReceiver();
DLL_DECL TUAVData GetUavMulticast();

//-----------------------------------
DLL_DECL int SendMarks(const char* json, uint16_t frame_number);

