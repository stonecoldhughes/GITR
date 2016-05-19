#ifndef _GEOM_
#define _GEOM_

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER_DEVICE __device__
#else
#define CUDA_CALLABLE_MEMBER_DEVICE
#endif

#include "Particle.h"
#include "Boundary.h"

template <typename T>
CUDA_CALLABLE_MEMBER_DEVICE
int sgn(T val) {
        return (T(0) < val) - (val < T(0));
}

struct geometry_check { 

    const int nLines;
#ifdef __CUDACC__
    const Boundary * boundaryVector;
#else    
    const std::vector<Boundary> & boundaryVector;
#endif

#ifdef __CUDACC__
    geometry_check(int _nLines,Boundary *  _boundaryVector) : nLines(_nLines), boundaryVector(_boundaryVector) {}
#else    
    geometry_check(int _nLines,std::vector<Boundary> & _boundaryVector) : nLines(_nLines), boundaryVector(_boundaryVector) {} 
#endif
CUDA_CALLABLE_MEMBER_DEVICE    
void operator()(Particle &p) const { 

	    if(p.hitWall == 0.0)
        {  
            double particle_slope = (p.z - p.zprevious)/(p.x - p.xprevious);
            double particle_intercept = -particle_slope*p.x + p.z;
            double intersectionx[4] = {0.0, 0.0, 0.0, 0.0};
            //intersectionx = new double[nPoints];
            double intersectiony[4] = {0.0, 0.0, 0.0, 0.0};
            //intersectiony = new double[nPoints];
            double distances[4] = {0.0, 0.0, 0.0, 0.0};
            //distances = new double[nPoints];
            double tol_small = 1e-12;       
            double tol = 1e12;
	        int nIntersections = 0;
            double signPoint;
            double signPoint0;
            double signLine1;
            double signLine2;
            double minDist = 1e12;
            int minDistInd = 0;
                //std::cout << "particle slope " << particle_slope << " " << particle_intercept << std::endl;
                            //std::cout << "r " << boundaryVector[0].x1 << " " << boundaryVector[1].x1 << " " << boundaryVector[0].slope_dzdx << std::endl;
                          //  std::cout << "r0 " << p.xprevious << " " << p.yprevious << " " << p.zprevious<< std::endl;
            for (int i=0; i<nLines; i++)
                {   //std::cout << "vert geom " << i << "  " << fabs(boundaryVector[i].slope_dzdx) << " " << tol << std::endl;
                    if (fabs(boundaryVector[i].slope_dzdx) >= tol*0.75)
                    {
                        signPoint = sgn(p.x - boundaryVector[i].x1);
                        signPoint0 = sgn(p.xprevious - boundaryVector[i].x1);
                        //std::cout << "signpoint1 " << signPoint << " " << signPoint0 << std::endl;
                    }
                    else
                    {
                        signPoint = sgn(p.z - p.x*boundaryVector[i].slope_dzdx - boundaryVector[i].intercept_z);
                        signPoint0 = sgn(p.zprevious - p.xprevious*boundaryVector[i].slope_dzdx - boundaryVector[i].intercept_z);
                        //std::cout << "signpoint2 " << signPoint << " " << signPoint0 << std::endl;
                    }

                    if (signPoint != signPoint0)
                    {
                        signLine1 = sgn(boundaryVector[i].z1 - boundaryVector[i].x1*particle_slope - particle_intercept);
                        signLine2 = sgn(boundaryVector[i].z2 - boundaryVector[i].x2*particle_slope - particle_intercept);
                        //std::cout << "signLines " << signLine1 << " " << signLine2 << std::endl;
                        //std::cout << "bound vec points " << boundaryVector[i].z1 << " " << boundaryVector[i].x1 << 
                        //   " " << boundaryVector[i].z2 << " " << boundaryVector[i].x2 << std::endl; 
                        if (signLine1 != signLine2)
                        {
                            nIntersections++;
                            //std::cout << "nintersections " << nIntersections << std::endl;
                           // std::cout << fabs(p.x - p.xprevious) << tol_small << std::endl;        
                            if (fabs(p.x - p.xprevious) < tol_small)
                            {
                              //  std::cout << "vertical line" << std::cout;
                                intersectionx[nIntersections-1] = p.xprevious;
                                intersectiony[nIntersections-1] = intersectionx[nIntersections-1]*boundaryVector[i].slope_dzdx +
                                                                    boundaryVector[i].intercept_z;
                            }
                            else
                            {
                              // std::cout << "not vertical line" << std::endl;
                            //std::cout << 0.0*7.0 << " " << i << " " << nParam << " " << lines[i*nParam+4] << "  " <<tol << std::endl;
                                if (boundaryVector[i].slope_dzdx >= tol*0.75)
                                {
                                    intersectionx[nIntersections-1] = boundaryVector[i].x1;
                                }
                                else
                                {
                                    intersectionx[nIntersections-1] = (boundaryVector[i].intercept_z - particle_intercept)/
                                                (particle_slope - boundaryVector[i].slope_dzdx);
                                    //std::cout << "in this else "<< intersectionx[nIntersections -1] << std::endl;
                                }
                                intersectiony[nIntersections-1] = intersectionx[nIntersections-1]*particle_slope
                                                                               + particle_intercept;
                            }
                        }
                    }
                }
            
	        if (nIntersections == 0)
            {
                p.xprevious = p.x;
                p.yprevious = p.y;
                p.zprevious = p.z;

                //std::cout << "r " << p.x << " " << p.y << " " << p.z << std::endl;
                //std::cout << "r0 " << p.xprevious << " " << p.yprevious << " " << p.zprevious<< std::endl;
            }
            else if (nIntersections ==1)
            {
                p.hitWall = 1.0;
                p.y = p.yprevious + (intersectionx[0] - p.xprevious)/(p.x - p.xprevious)*(p.y - p.yprevious);//go back here and calculate p.y
                p.x = intersectionx[0];
                p.z = intersectiony[0];
                //std::cout << "nInt = 1 position " << intersectionx[0] << " " << intersectiony[0]  << std::endl;
            }
            else
            {
                //std::cout << "nInts greater than 1 " << nIntersections << std::endl;
                for (int i=0; i<nIntersections; i++)
                {
                    distances[i] = (p.xprevious - intersectionx[i])*(p.xprevious - intersectionx[i]) + 
                        (p.zprevious - intersectiony[i])*(p.zprevious - intersectiony[i]);
                    if (distances[i] < minDist)
                    {
                        minDist = distances[i];
                        minDistInd = i;
                    }
                }

                p.hitWall = 1.0;
                p.y = p.yprevious + (intersectionx[minDistInd] - p.xprevious)/(p.x - p.xprevious)*(p.y - p.yprevious);
                p.x = intersectionx[minDistInd];
                p.z = intersectiony[minDistInd];
            }
   
            if (boundaryVector[nLines].periodic)
            {
                /*if (p.y < boundaryVector[nLines].y1)
                {
                    p.y = boundaryVector[nLines].y2  - (boundaryVector[nLines].y1 - p.y);
                }
                else if (p.y > boundaryVector[nLines].y2)
                {
                    p.y = boundaryVector[nLines].y1  + (p.y - boundaryVector[nLines].y2);
                }*/
            }
            else
            {
                if (p.y < boundaryVector[nLines].y1)
                {
                    p.hitWall = 1.0;
                }
                else if (p.y > boundaryVector[nLines].y2)
                {
                    p.hitWall = 1.0;
                }
            }
            }
    	}
};

#endif
