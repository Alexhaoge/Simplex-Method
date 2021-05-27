#include<omp.h>
#include<stdio.h>
#include<stdlib.h>
#include<cuda_runtime.h>
#include<float.h>
#define I 2
#define N 5
#define M 5
#define blockx 2
#define blocky 2
#define Thread_num 2
#define J 1
#define K2 0
#define K3 0
#define BK3 0
void generate_matrix(double* matrix,int m,int n)
{
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            matrix[i * n + j] =  ((double)((rand()%10)+1));
        }
    }
    for(int i=0;i<m;i++){
        matrix[i*n] =  (double)((rand()%10)+1);
    }
    for(int i=0;i<n;i++)
    {
        matrix[i]=-matrix[i];
    }
}
void read_matrix(double* matrix,int m,int n)
{
    FILE *fpRead=fopen("data.txt","r");
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            fscanf(fpRead,"%lf",matrix+i*N+j);
        }
    }

}
int Find_min(double* array,int length)
{
    double min=DBL_MAX;
    int min_index=-1;
    for(int i=0;i<length;i++)
    {
    
    if(array[i]<min)
    { 
        min_index=i;
        min=array[i];

    }
    }
    return min_index;
}
__global__ void kernel1(double* theta,double* Columnk,int k,double* SimplexTableauPart,int size)
{
    int idx=blockDim.x*blockIdx.x+threadIdx.x;
    if(idx<N)
    {
    double w=SimplexTableauPart[idx*N+k];
    Columnk[idx]=w;
    theta[idx]=((w>0) ? SimplexTableauPart[idx*N]/w : DBL_MAX);
    }
}
__global__ void kernel1_0(double* theta,double* Columnk,int k,double* SimplexTableauPart,int size)
{
    int idx=blockDim.x*blockIdx.x+threadIdx.x;
    if(idx>0&&idx<N)
    {
    double w=SimplexTableauPart[idx*N+k];
    Columnk[idx]=w;
    theta[idx]=((w>0) ? SimplexTableauPart[idx*N]/w : DBL_MAX);
    }
    else
    {
        double w=SimplexTableauPart[idx*N+k];
        Columnk[idx]=w;
        theta[idx]=DBL_MAX;  
    }
}
__global__ void kernel2(double wp,int r,double *Columnk,double* Liner,double* SimplexTableauPart)
{
    int idx = blockDim.x*blockIdx.x+threadIdx.x;
    if(idx==0) Columnk[r]=-1;
    if(idx<N)
      Liner[idx] = SimplexTableauPart[r*N+idx]/wp;
}
__global__ void Kernel3(int size,double* Columnk,double* Liner,double* SimplexTableauPart)
{
    int idx=blockDim.x*blockIdx.x+threadIdx.x;
    int idy=blockDim.y*blockIdx.y+threadIdx.y;
    if(idy<size&&idx<N)
    {
    double s = SimplexTableauPart[idy*N+idx];
    __shared__ double w[blocky];
    if(threadIdx.x==0)
      w[threadIdx.y] = Columnk[idy];
    __syncthreads();
    SimplexTableauPart[idy*N+idx]=s-w[threadIdx.y]*Liner[idx];
   }
}
__global__ void Kernel3_0(int size,double* Columnk, double* Liner,double* SimplexTableauPart)
{   
    int idx=blockDim.x*blockIdx.x+threadIdx.x;
    int idy=blockDim.y*blockIdx.y+threadIdx.y;
    if(idx!=0||idy!=0)
    {
      if(idy<size&&idx<N)
       {
          double s = SimplexTableauPart[idy*N+idx];
          __shared__ double w[blocky];
          if(threadIdx.x==0||(blockIdx.x==0&&blockIdx.y==0&&threadIdx.x==1))
               w[threadIdx.y] = Columnk[idy];
          __syncthreads();
          SimplexTableauPart[idy*N+idx]=s-w[threadIdx.y]*Liner[idx];
       }
   }
}
__global__ void Kernel4(int size,int k,double wp,double* Columnk,double* SimplexTableauPart)
{
    int idx=blockDim.x*blockIdx.x+threadIdx.x;
    if(idx<size)
    SimplexTableauPart[idx*N+k]=-Columnk[idx]/wp;
}
int main()
{   
    bool label=true;
    int k,r,size,nsize,m0,n0,id;
    double min,wp;
    int* index,*index1,*Min;
    double* Sharedrow,*SimplexTableau,*SimplexTableauPart,*Columnk,*Liner,*LinerCPU,*theta;
    m0=(M+I-1)/I;
    n0=(N+I-1)/I;
    Min=(int*)malloc(sizeof(int)*I);
    index=(int*)malloc(sizeof(int)*(M-1));
    index1=(int*)malloc(sizeof(int)*(N-1));
    Sharedrow=(double*)malloc(sizeof(double)*I*(n0>m0 ? n0 : m0));
    SimplexTableau=(double*)malloc(sizeof(double)*M*N);
    LinerCPU=(double*)malloc(sizeof(double)*N);
    generate_matrix(SimplexTableau,M,N);
    //read_matrix(SimplexTableau,M,N);
    SimplexTableau[0]=DBL_MAX;
    for(int i=0;i<M-1;i++)
    {
        index[i]=i+N;
    }
    for(int i=0;i<N-1;i++)
    {
        index1[i]=i+1;
    }
    printf("start \n ");
    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++){
        if(i==0&&j==0){
            printf(" CCC ");
            continue;
        }
        printf(" %.2f ",SimplexTableau[i*N+j]);
        }
        printf("\n");
    }
    
    #pragma omp parallel num_threads(I) private(theta,SimplexTableauPart,size,nsize,Columnk,Liner) shared(min,index,index1,Sharedrow,k,Min,m0,n0,id,LinerCPU,wp)
    {   
        
        int tid=omp_get_thread_num();
        cudaSetDevice(tid);
        if(tid==(I-1))
          {
              size=M-m0*(I-1);
              nsize=N-n0*(I-1);
          }
        else
          {          
              size=m0;
              nsize=n0;
          }
        cudaMalloc((void**)&Columnk,sizeof(double)*size);
        cudaMalloc((void**)&theta,sizeof(double)*size);
        cudaMalloc((void**)&SimplexTableauPart,sizeof(double)*size*N);
        cudaMalloc((void**)&Liner,sizeof(double)*N);
        cudaMemcpy(SimplexTableauPart,SimplexTableau+N*m0*tid,sizeof(double)*size*N,cudaMemcpyHostToDevice);
        do
        {
        if(tid==0)
            cudaMemcpy(Sharedrow,SimplexTableauPart,sizeof(double)*N,cudaMemcpyDeviceToHost);
        {
        #pragma omp barrier
        }
        Min[tid]=Find_min(Sharedrow+tid*n0,nsize)+tid*n0;
        {
        #pragma omp barrier
        }
        if(tid==0)
        {
         k=Min[0];
         min=Sharedrow[Min[0]];
         for(int i=1;i<I;i++)
           {   
            if(Sharedrow[Min[i]]<min)
            {   
                k=Min[i];
                min=Sharedrow[k];
            }
           }
        printf(" \n k is %d with value %f\n ",k,min);
      }
      {
       #pragma omp barrier
       }
       if(min>=0&&J==1) break;
       if(tid==0)
           kernel1_0<<<(size+Thread_num-1)/Thread_num,Thread_num>>>(theta,Columnk,k,SimplexTableauPart,size);
       else
           kernel1<<<(size+Thread_num-1)/Thread_num,Thread_num>>>(theta,Columnk,k,SimplexTableauPart,size);
       cudaMemcpy(Sharedrow+(tid)*m0,theta,sizeof(double)*size,cudaMemcpyDeviceToHost);
       {
        #pragma omp barrier
       }
       Min[tid]=Find_min(Sharedrow+(tid)*m0,size);
       Min[tid]=((Min[tid]<0)?-1:(Min[tid]+tid*m0));
       {
       #pragma omp barrier
       }
       if(tid==0)
        {
         r=-1;
         double min=DBL_MAX;
         for(int i=0;i<I;i++)  
            if(Min[i]>-1&&Sharedrow[Min[i]]<min)
              {
                 
                 r=Min[i];
                 id=i;
                 min=Sharedrow[r];
              }
         if(r!=-1)
           printf("\n r is %d with value of %f \n",r,min);
         else
           printf("\n r is -1 !!!\n");
        }
       {
       #pragma omp barrier
       }
       
       if(r==-1&&J==1) 
       {   
           label=false;
           break;
       }
       if(tid==id)
       {   
           int tem=index[r-1];
           index[r-1]=index1[k-1];
           index1[k-1]=tem;
           wp=SimplexTableau[r*N+k];
           kernel2<<<(N+Thread_num-1)/Thread_num,Thread_num>>>(wp,r-tid*m0,Columnk,Liner,SimplexTableauPart) ;
           cudaMemcpy(LinerCPU,Liner,sizeof(double)*N,cudaMemcpyDeviceToHost);
           cudaMemset(SimplexTableauPart+(r-tid*m0)*N,0.0,N*sizeof(double));
        }
        {
        #pragma omp barrier
        }
       cudaMemcpy(Liner,LinerCPU,sizeof(double)*N,cudaMemcpyHostToDevice);
       dim3 block_size(blockx,blocky);
       dim3 grid_size((N+blockx-1)/blockx,(size+blocky-1)/blocky);
       if(tid==0)
           Kernel3_0<<<grid_size,block_size>>>(size,Columnk,Liner,SimplexTableauPart);
       else
           Kernel3<<<grid_size,block_size>>>(size,Columnk,Liner,SimplexTableauPart);
       Kernel4<<<(size+Thread_num-1)/Thread_num,Thread_num>>>(size,k,wp,Columnk,SimplexTableauPart);
       cudaDeviceSynchronize();
       
       cudaMemcpy(SimplexTableau+N*m0*tid,SimplexTableauPart,sizeof(double)*size*1,cudaMemcpyDeviceToHost);
       {
            #pragma omp barrier
        }
           
       }while(J==1);
       cudaMemcpy(SimplexTableau+N*m0*tid,SimplexTableauPart,sizeof(double)*size*N,cudaMemcpyDeviceToHost);
       cudaFree(SimplexTableauPart);        
    }
    if(label){
        printf("\n true \n");
        for(int i=0;i<M-1;i++){
            printf("the index i is %d \n",index[i]);
        }
    }
    else
    {
        printf("\n false \n");
    }
    
    
    printf("\n end \n ");
    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++){
        if(i==0&&j==0){
            printf(" CCC ");
            continue;
        }
        printf(" %.2f ",SimplexTableau[i*N+j]);
        }
        printf("\n");
    }
    
    free(SimplexTableau);
    return 0;
}