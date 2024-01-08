// Fill out your copyright notice in the Description page of Project Settings.


#include "CoordsTransformersLibrary.h"

#include "Engine.h"

#include <string>

// Translate coordinates from Geo to UE
void UCoordsTransformersLibrary::GEO_UE_Transformer
(
	// Input params для ОКИ
	//EPSG:4326
	FString src_crs,
	//EPSG:32638
	FString dst_crs,

	FGEO_DOUBLE_STRUCT lat0, // гео-координата опоры нулевая; 55.030389
	FGEO_DOUBLE_STRUCT lon0, // гео-координата опоры нулевая; 38.819802
	FGEO_DOUBLE_STRUCT lat,  // конечная гео-точка 
	FGEO_DOUBLE_STRUCT lon,  // конечная гео-точка
	FGEO_DOUBLE_STRUCT alt,
	FGEO_DOUBLE_STRUCT z_fix,// поправка на высоту над морем; 184

	// Output params выходные локальные координаты, относительно гео-нуля lat0 и lon0
	float& x,
	float& y,
	float& z
)
{

	if (!src_crs.Len())
	{
		UE_LOG(LogTemp, Warning, TEXT("[GEO UE TRANSFORMER] : SRC CRS STRING Empty!"));
		return;
	}

	if (!dst_crs.Len())
	{
		UE_LOG(LogTemp, Warning, TEXT("[GEO UE TRANSFORMER] : DST CRS STRING Empty!"));
		return;
	}
	// перевод из String UE в Double
	double d_lat0 = lat0.convert_to_double();
	double d_lon0 = lon0.convert_to_double();
	double d_lat = lat.convert_to_double();
	double d_lon = lon.convert_to_double();
	double d_alt = alt.convert_to_double();
	double d_z_fix = z_fix.convert_to_double();
	// перевод из String UE в обычную String
	std::wstring w_src_crs(&src_crs.GetCharArray()[0]);
	std::wstring w_dst_crs(&dst_crs.GetCharArray()[0]);

	std::string s_src_crs(w_src_crs.begin(), w_src_crs.end());
	std::string s_dst_crs(w_dst_crs.begin(), w_dst_crs.end());

	CoordsTransformer geo2utm(s_src_crs.c_str(), s_dst_crs.c_str());

	double d_x0 = 0.0f;
	double d_y0 = 0.0f;

	geo2utm.transform(d_lat0, d_lon0, d_x0, d_y0);

	//UE_LOG(LogTemp, Log, TEXT("[GEO UE TRANSFORMER] : x0 = %f | y0 = %f"), d_x0, d_y0);

	double d_x_r = 0.0f;
	double d_y_r = 0.0f;

	geo2utm.transform(d_lat, d_lon, d_x_r, d_y_r);

	//UE_LOG(LogTemp, Log, TEXT("[GEO UE TRANSFORMER] : xr = %f | yr = %f"), d_x_r, d_y_r);

	x = float(100.0f * (d_x_r - d_x0));
	y = float(-100.0f * (d_y_r - d_y0));
	z = float(100.0f * (d_alt - d_z_fix));

	//UE_LOG(LogTemp, Log, TEXT("[GEO UE TRANSFORMER] : x = %f | y = %f | z = %f"), x, y, z);

}

// Translate coordinates from UE to Geo
void UCoordsTransformersLibrary::UE_GEO_Transformer
(
	// Input params
	FString src_crs,
	FString dst_crs,

	FGEO_DOUBLE_STRUCT lat0,
	FGEO_DOUBLE_STRUCT lon0,
	float x,
	float y,
	float z,
	FGEO_DOUBLE_STRUCT z_fix,
	// Output params
	FGEO_DOUBLE_STRUCT& lat,
	FGEO_DOUBLE_STRUCT& lon,
	FGEO_DOUBLE_STRUCT& alt

)
{
	if (!src_crs.Len())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UE GEO TRANSFORMER] : SRC CRS STRING Empty!"));
		return;
	}

	if (!dst_crs.Len())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UE GEO TRANSFORMER] : DST CRS STRING Empty!"));
		return;
	}

	double d_lat0 = lat0.convert_to_double();
	double d_lon0 = lon0.convert_to_double();
	double d_z_fix = z_fix.convert_to_double();

	std::wstring w_src_crs(&src_crs.GetCharArray()[0]);
	std::wstring w_dst_crs(&dst_crs.GetCharArray()[0]);

	std::string s_src_crs(w_src_crs.begin(), w_src_crs.end());
	std::string s_dst_crs(w_dst_crs.begin(), w_dst_crs.end());

	CoordsTransformer geo2utm(s_src_crs.c_str(), s_dst_crs.c_str());
	CoordsTransformer utm2geo(s_dst_crs.c_str(), s_src_crs.c_str());

	double d_x0 = 0.0f;
	double d_y0 = 0.0f;

	geo2utm.transform(d_lat0, d_lon0, d_x0, d_y0);

	//UE_LOG(LogTemp, Log, TEXT("[UE GEO TRANSFORMER] : x0 = %f | y0 = %f"), d_x0, d_y0);

	double d_x_r = x / 100.0f;
	double d_y_r = -y / 100.0f;

	//UE_LOG(LogTemp, Log, TEXT("[UE GEO TRANSFORMER] : xr = %f | yr = %f"), d_x_r, d_y_r);

	double d_lat = 0.0f;
	double d_lon = 0.0f;
	double d_alt = 0.0f;

	utm2geo.transform((d_x0 + d_x_r), (d_y0 + d_y_r), d_lat, d_lon);
	d_alt = (z / 100.0f) + d_z_fix;

	lat.make_doubleStruct(d_lat);
	lon.make_doubleStruct(d_lon);
	alt.make_doubleStruct(d_alt);

	//UE_LOG(LogTemp, Log, TEXT("[UE GEO TRANSFORMER] : x = %f | y = %f | z = %f"), x, y, z);

}
