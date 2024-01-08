echo "Start making Glide Cam Track"
title Glide Track maker
timeout 2
python ..\bp_make_track_glide.py --input objects_places.csv --out_track track_glide.csv --out_env env_glide.csv --out_cam_sighting cam_sighting_glide.csv --num_points 18 --num_weathers 1 --step_count 185 --uav_height 400 --radius_of_circle 3000 --stop_distance 300 --displacement 0.0 --in_meters True --lux_range [2.0;18.0] --lrx -149.0 --lry_range [-30.0;-150.0] --lrz_range [0.0;360.0]