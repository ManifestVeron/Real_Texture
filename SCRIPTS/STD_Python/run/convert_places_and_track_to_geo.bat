@echo off
echo "Convert track file to geo coordinates..."
timeout 2
rem Operation 1
python ..\convert_track_ue_geo.py --track ..\track_cam_light_goes_with_bind_Z5.csv --coords_idx_start 1 --src_crs 4326 --dst_crs 32637 --lat0 55.0 --lon0 37.0 --z_fix 195.0
rem Operation 2
echo "Convert places file to geo coordinates..."
timeout 2
python ..\convert_places_ue_geo.py --places ..\objects_places_Z5.csv --src_crs 4326 --dst_crs 32637 --lat0 55.0 --lon0 37.0 --z_fix 195.0