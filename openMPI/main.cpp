//
//  main.cpp
//  openMPI
//
//  Created by Paweł Szudrowicz on 07.06.2017.
//  Copyright © 2017 Paweł Szudrowicz. All rights reserved.
//

#include "mpi.h"
#include <stdio.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
//    // Get the name of the processor
//    char processor_name[MPI_MAX_PROCESSOR_NAME];
//    int name_len;
//    MPI_Get_processor_name(processor_name, &name_len);
    
    int dataToSend = 0;
    if(world_rank == 0) {
        MPI_Send(&dataToSend, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        
        MPI_Recv(&dataToSend, 1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Received %d,from %d, I'm %d \n", dataToSend, 3, world_rank);
    }
    else {
        
        int receiveFrom = world_rank-1;
        
        MPI_Recv(&dataToSend, 1, MPI_INT, receiveFrom, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Received %d,from %d, I'm %d \n", dataToSend, receiveFrom, world_rank);
        dataToSend += 1;
        MPI_Send(&dataToSend, 1, MPI_INT, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);
    }
    
    
    // Finalize the MPI environment.
    MPI_Finalize();
}
