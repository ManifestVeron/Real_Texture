echo "Start making Goes Cam Track"
title Goes Track Maker
timeout 5
python ..\bp_make_track_goes.py --input ..\objects_places.csv --out_track ..\track_goes.csv --out_env ..\env_goes.csv --out_cam_sighting ..\cam_sighting_goes.csv --num_points 241 --num_weathers 6 --uav_height 500.0 --radius_of_circle 100 --displacement 0.0 --in_meters True --lux_range [2.0;18.0] --lrx -149.0 --lry_range [-30.0;-150.0] --lrz_range [0.0;360.0]
