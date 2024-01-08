echo "Start making Glide with bind Cam Track"
title Glide with bind Track Maker
timeout 5
python ..\bp_make_track_glide_with_bind.py --places ..\objects_places.csv --markups_dir  "..\JSON" --obzor_track ..\track_obzor.csv --obzor_env ..\env_obzor.csv --offsets ..\objects_offsets.csv --out_track_glide ..\track_glide_with_bind.csv --out_env_glide ..\env_glide_with_bind.csv --out_pairs ..\pairs_glide_with_bind.csv --out_cam_sighting ..\cam_sighting_glide_with_bind.csv --num_points 3 --step_count 10 --radius_of_circle 15 --stop_distance 3 --displacement 0.0 --in_meters True