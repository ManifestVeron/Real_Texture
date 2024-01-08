#### 1. track_cam_light.csv

Трек обзорного датчика

В нем есть следующие поля

xc yc zc rx ry rz lux l_rx l_ry l_rz

Должны быть следующие поля

pose_idx xc yc zc rx ry rz lux l_rx l_ry l_rz

pose_idx - индекс текущей позиции. Должен быть уникальным

#### 2. track_cam_light_goes.csv

Трек гиростабилизирующегося датчика

В нем есть следующие поля

xc yc zc rx ry rz lux l_rx l_ry l_rz

Должны быть следующие поля

pose_idx xc yc zc rx ry rz lux l_rx l_ry l_rz

pose_idx - индекс текущей позиции. Может повторяться, поскольку ГОЭС облетает всех, кого увидел обзорный.