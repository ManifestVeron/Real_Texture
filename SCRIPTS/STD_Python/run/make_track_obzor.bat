echo "Start making Obzor Cam Track"
title Obzor Track Maker
timeout 5
python ..\bp_make_track_obzor.py --x_range [-14950.0;18970.0] --y_range [19420.0;-20880.0] --uav_height 78620.0 --num_points 100 --num_weathers 1 --lux_range [30.0;50.0] --lrx 0.0 --lry_range [0.0;-15.0] --lrz_range [0.0;360.0] --out_track track_obzor.csv --out_env env_obzor.csv
