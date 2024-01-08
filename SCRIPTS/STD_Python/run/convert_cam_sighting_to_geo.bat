@echo off
echo "Convert Camera sighting file to geo coordinates..."
timeout 2
python ..\convert_cam_sighting_ue_geo.py --track ..\cam_sighting_goes_Z5.csv --src_crs 4326 --dst_crs 32637 --lat0 55.0 --lon0 37.0 --z_fix 195.0