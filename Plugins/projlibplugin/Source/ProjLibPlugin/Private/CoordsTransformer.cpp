// Fill out your copyright notice in the Description page of Project Settings.


#include "CoordsTransformer.h"

CoordsTransformer::CoordsTransformer(const char* src_crs, const char* dst_crs)
{
    C = proj_context_create();
    P = proj_create_crs_to_crs(C, src_crs, dst_crs, NULL);

    if (P == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Coords Transformer] : Failed to create transformation object"));
    }

}

CoordsTransformer::~CoordsTransformer()
{
    /* Clean up */
    proj_destroy(P);
    proj_context_destroy(C); /* may be omitted in the single threaded case */
}

void CoordsTransformer::transform(double src_x, double src_y, double& dst_x, double& dst_y)
{
    a = proj_coord(src_x, src_y, 0, 0);
    b = proj_trans(P, PJ_FWD, a);

    dst_x = b.lp.lam;
    dst_y = b.lp.phi;
}

