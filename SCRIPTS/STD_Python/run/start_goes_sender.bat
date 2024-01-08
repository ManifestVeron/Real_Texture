echo "Starting Goes Sender"
title Goes Sender
timeout 5
python ..\bp_udp_cam_client_move_track_obzor.py --idx_start 0 --mode track --track ..\track_goes.csv --env ..\env_goes.csv --places ..\objects_places.csv --offsets ..\objects_offsets.csv --timeout 0.1