// Fill out your copyright notice in the Description page of Project Settings.
#include "CamAndLight.h"

FCamAndLight& FCamAndLight::operator=(const CamAndLight& v_data)
{
    index = v_data.index;

    xc = v_data.xc;
    yc = v_data.yc;
    zc = v_data.zc;

    rx = v_data.rx;
    ry = v_data.ry;
    rz = v_data.rz;

    lux = v_data.lux;
    l_rx = v_data.l_rx;
    l_ry = v_data.l_ry;
    l_rz = v_data.l_rz;
    
    return *this;
}
