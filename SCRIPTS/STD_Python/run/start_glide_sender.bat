echo "Starting Glide Sender"
title Glide Sender
timeout 5
python ..\bp_udp_cam_client_move_track_glide.py --idx_start 0 --track track_glide.csv --env env_glide.csv --places objects_places.csv --offsets objects_offsets.csv --timeout 1.0