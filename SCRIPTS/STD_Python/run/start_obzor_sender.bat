echo "Starting Obzor Sender"
title Obzor Sender
timeout 5
python ..\bp_udp_cam_client_move_track_obzor.py --idx_start 0 --mode track --track track_obzor.csv --env env_obzor.csv --places objects_places.csv --offsets objects_offsets.csv --timeout 1.0