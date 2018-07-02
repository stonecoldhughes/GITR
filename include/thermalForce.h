#ifndef _THERMAL_
#define _THERMAL_

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

#include "Particles.h"
//#include <cmath>
#include "math.h"

struct thermalForce { 
    Particles *particlesPointer;
    const float dt;
    float background_amu;
    int nR_gradT;
    int nZ_gradT;
    float* gradTGridr;
    float* gradTGridz;
    float* gradTiR;
    float* gradTiZ;
    float* gradTiT;
    float* gradTeR;
    float* gradTeZ;
    float* gradTeT;
    int nR_Bfield;
    int nZ_Bfield;
    float * BfieldGridR;
    float * BfieldGridZ;
    float * BfieldR;
    float * BfieldZ;
    float * BfieldT;
    
    float dv_ITGx;
    float dv_ITGy;
    float dv_ITGz;
    float dv_ETGx;
    float dv_ETGy;
    float dv_ETGz;
    float dv_ITG[3];        
    thermalForce(Particles *_particlesPointer, float _dt, float _background_amu,
                 int _nR_gradT, int _nZ_gradT, float* _gradTGridr, float* _gradTGridz,
                 float* _gradTiR, float* _gradTiZ, float* _gradTiT, float* _gradTeR, 
                 float* _gradTeZ,float* _gradTeT,
                 int _nR_Bfield, int _nZ_Bfield,
                 float * _BfieldGridR,
                 float * _BfieldGridZ,
                 float * _BfieldR,
                 float * _BfieldZ,
                 float * _BfieldT)
        
       : particlesPointer(_particlesPointer), dt(_dt), background_amu(_background_amu),
        nR_gradT(_nR_gradT),nZ_gradT(_nZ_gradT),
        gradTGridr(_gradTGridr), gradTGridz(_gradTGridz),
        gradTiR(_gradTiR), gradTiZ(_gradTiZ),gradTiT(_gradTiT), gradTeR(_gradTeR), 
        gradTeZ(_gradTeZ),gradTeT(_gradTeT), 
        nR_Bfield(_nR_Bfield), nZ_Bfield(_nZ_Bfield), 
        BfieldGridR(_BfieldGridR), 
        BfieldGridZ(_BfieldGridZ),
        BfieldR(_BfieldR), BfieldZ(_BfieldZ), 
        BfieldT(_BfieldT)
        ,dv_ITGx{0.0},dv_ITGy{0.0},dv_ITGz{0.0},dv_ETGx{0.0},dv_ETGy{0.0},dv_ETGz{0.0},
        dv_ITG{0.0,0.0,0.0}
        {}

CUDA_CALLABLE_MEMBER    
void operator()(std::size_t indx)  
{ 
  if((particlesPointer->hitWall[indx] == 0.0) && (particlesPointer->charge[indx] > 0.0))
  {
    float MI = 1.6737236e-27;
    float kB = 1.38064852e-23;
    float KelvinPerEv = 11604.522;
    float alpha = 0.0f;
    float beta = 0.0f;
    float mu = 0.0f;
    float gradTe[3] = {0.0f};
    float gradTi[3] = {0.0f};
    //float gradTeDotB = 0.0f;
    //float gradTiDotB = 0.0f;
    float B[3] = {0.0f};
    float B_unit[3] = {0.0f};
    float gradTiPar = 0.0f;
    //float dv_ITG[3] = {0.0f};
    //float dv_ETG[3] = {0.0f};
    float x = particlesPointer->xprevious[indx];
    float y = particlesPointer->yprevious[indx];
    float z = particlesPointer->zprevious[indx];
    float vx = particlesPointer->vx[indx];
    float vy = particlesPointer->vy[indx];
    float vz = particlesPointer->vz[indx];
    float Z = particlesPointer->charge[indx];
    float amu = particlesPointer->amu[indx];
    float propConst = dt/(amu*MI)*kB*KelvinPerEv;
    interp2dVector(&B[0],x,y,z,nR_Bfield,nZ_Bfield,BfieldGridR,
                   BfieldGridZ,BfieldR,BfieldZ,BfieldT);
    vectorNormalize(B,B_unit); 
#if USEFIELDALIGNEDVALUES > 0 
    interpFieldAlignedVector(gradTi,x,y,z,                 
                             nR_gradT,nZ_gradT, 
                             gradTGridr,gradTGridz,gradTiR, 
                             gradTiZ,gradTiT,nR_Bfield,nZ_Bfield, 
                             BfieldGridR,BfieldGridZ,BfieldR, 
                             BfieldZ,BfieldT); 
    interpFieldAlignedVector(gradTe,x,y,z,                 
                             nR_gradT,nZ_gradT, 
                             gradTGridr,gradTGridz,gradTeR, 
                             gradTeZ,gradTeT,nR_Bfield,nZ_Bfield, 
                             BfieldGridR,BfieldGridZ,BfieldR, 
                             BfieldZ,BfieldT);
    B_unit[0] = abs(B_unit[0]);
    B_unit[1] = abs(B_unit[1]);
    B_unit[2] = abs(B_unit[2]);
#else 
    interp2dVector(&gradTi[0],x,y,z,nR_gradT,nZ_gradT,gradTGridr ,gradTGridz ,gradTiR,gradTiZ, gradTiT );
    interp2dVector(&gradTe[0],x,y,z,nR_gradT,nZ_gradT,gradTGridr ,gradTGridz ,gradTeR,gradTeZ, gradTeT );
#endif
	mu = amu/(background_amu + amu);
    alpha = Z*Z*0.71;
	beta =  3.0*(mu + 7.0711*Z*Z*(1.1*powf(mu,(2.5))- 0.35*powf(mu,(1.5))) - 1.0)/(2.6 - 2*mu+ 5.4*mu*mu);
	dv_ETGx = propConst*(alpha*(gradTe[0]*B_unit[0]));
	this->dv_ETGy = propConst*(alpha*(gradTe[1]*B_unit[1]));
	this->dv_ETGz = propConst*(alpha*(gradTe[2]*B_unit[2]));

	dv_ITGx = propConst*(beta*(gradTi[0]*B_unit[0]));
	this->dv_ITGy = propConst*(beta*(gradTi[1]*B_unit[1]));
	this->dv_ITGz = propConst*(beta*(gradTi[2]*B_unit[2]));
    if(this->dv_ITGx != this->dv_ITGx) this->dv_ITGx = 0;
    if(this->dv_ITGy != this->dv_ITGy) this->dv_ITGy = 0;
    if(this->dv_ITGz != this->dv_ITGz) this->dv_ITGz = 0;
    if(this->dv_ETGx != this->dv_ETGx) this->dv_ETGx = 0;
    if(this->dv_ETGy != this->dv_ETGy) this->dv_ETGy = 0;
    if(this->dv_ETGz != this->dv_ETGz) this->dv_ETGz = 0;
#if USE_CUDA 
#else
    if(isnan(dv_ITGx)) this->dv_ITGx = 0;
    if(isnan(dv_ITGy)) this->dv_ITGy = 0;
    if(isnan(dv_ITGz)) this->dv_ITGz = 0;
    if(isnan(dv_ETGx)) this->dv_ETGx = 0;
    if(isnan(dv_ETGy)) this->dv_ETGy = 0;
    if(isnan(dv_ETGz)) this->dv_ETGz = 0;
#endif
    particlesPointer->vx[indx] = particlesPointer->vx[indx] + dv_ETGx + dv_ITGx;
	particlesPointer->vy[indx] = particlesPointer->vy[indx] + dv_ETGy + dv_ITGy;
	particlesPointer->vz[indx] = particlesPointer->vz[indx] + dv_ETGz + dv_ITGz;	
#if USE_CUDA 
#else
    if(isnan(particlesPointer->vx[indx]) || isinf(particlesPointer->vx[indx])) particlesPointer->vx[indx] = vx;
    if(isnan(particlesPointer->vy[indx]) || isinf(particlesPointer->vy[indx])) particlesPointer->vy[indx] = vy;
    if(isnan(particlesPointer->vz[indx]) || isinf(particlesPointer->vz[indx])) particlesPointer->vz[indx] = vz;
#endif        
    //particlesPointer->test[indx] = dv_ETGx;
    //particlesPointer->test0[indx] = dv_ETGy;
    //particlesPointer->test1[indx] = dv_ETGz;
    //particlesPointer->test2[indx] = dv_ITGx;
    //particlesPointer->test3[indx] = dv_ITGy;
    //particlesPointer->test4[indx] = dv_ITGz;
  }
}
};

#endif
