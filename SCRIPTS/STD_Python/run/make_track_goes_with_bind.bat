echo "Start making GOES with bind Cam Track"
title GOES with bind Track Maker
timeout 5
python ..\bp_make_track_goes_with_bind.py --places ..\objects_places.csv --markups_dir  "..\JSON" --obzor_track ..\track_obzor.csv --obzor_env ..\env_obzor.csv --offsets ..\objects_offsets.csv --out_track_goes ..\track_goes_with_bind.csv --out_env_goes ..\env_goes_with_bind.csv --out_pairs ..\pairs_goes_with_bind.csv --out_cam_sighting ..\cam_sighting_goes_with_bind.csv --num_points 3 --radius_of_circle 50 --displacement 5.5 --in_meters True 
