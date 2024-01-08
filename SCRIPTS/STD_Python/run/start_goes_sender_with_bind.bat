echo "Starting Goes Sender with bind"
title Goes Sender with bind
timeout 5
python ..\bp_udp_cam_client_move_track_goes.py --idx_start 0 --mode track --track ..\track_goes_with_bind.csv --env ..\env_goes_with_bind.csv --places ..\objects_places.csv --offsets ..\objects_offsets.csv --timeout 0.1