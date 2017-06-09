//
//  main.cpp
//  openMPI
//
//  Created by Paweł Szudrowicz on 07.06.2017.
//  Copyright © 2017 Paweł Szudrowicz. All rights reserved.
//

#include "mpi.h"
#include <iostream>
#include "stdlib.h"
#include <stdio.h>
#include <string.h>
using namespace std;

enum ProcesType {
    Agent,
    University,
    Auditor
};



int main(int argc, char** argv) {
    
    ProcesType procesType;
    int numberOfAgents = 1;
    int numberOfUniv = 2;
    int numberOfAudit = 1;
    int S = 100;
    int A = 50;
    
    setbuf(stdout, NULL);
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    
    if(numberOfAgents + numberOfAudit + numberOfUniv != world_size) {
        printf("Number of processes not correct!\n");
        MPI_Abort(MPI_COMM_WORLD, 0);
    }
    
    if(world_rank < numberOfAgents) {
        procesType = ProcesType::Agent;
    } else if (world_rank < numberOfAgents + numberOfUniv) {
        procesType = ProcesType::University;
    } else {
        procesType = ProcesType::Auditor;
    }
    
    
    if(procesType == Agent) {
        
        //while(true) {
            srand((unsigned int)time(NULL));
            int numberOfStudents = rand() % S + 1;
            for(int i = 0; i<numberOfUniv ; i++) {
                MPI_Send(&numberOfStudents, 1, MPI_INT, numberOfAgents + i, 0, MPI_COMM_WORLD);
                printf("Agent %d: Sent numberOfStudent = %d to University %d\n", world_rank, numberOfStudents, numberOfAgents + i);
            }
            
            char message[numberOfUniv][8];
            for(int k = 0; k<numberOfUniv ; k++) {
                MPI_Recv(message[k], 8, MPI_CHAR, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("Agent %d: Received message = %s from University\n", world_rank, message[k]);
            }
            
            
        //}
        
    }
    else if(procesType == University) {
        
        char message[8];
        bool punished = false;
        int currentStudentsInUniversity = 0;
        int totalStudents=0;
        //while(true) {
            int receivedStudents;
            for(int i=0; i<numberOfAgents; i++) {
                MPI_Recv(&receivedStudents, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("University %d: Received students = %d from Agent %d\n", world_rank, receivedStudents,i);
                totalStudents+=receivedStudents;
            }
            
            
            
            //pierwszy uniwersytet
            if(world_rank==numberOfAgents) {
                if(punished && (totalStudents - A) >= 0) {
                    currentStudentsInUniversity+=A;
                    totalStudents-=A;
                }
                
                MPI_Send(&totalStudents, 1, MPI_INT, world_rank+1, 4, MPI_COMM_WORLD);
                printf("University a %d [PUNISHED]: Send availableStudent = %d to University %d\n", world_rank, totalStudents, world_rank+1);
                
                MPI_Recv(&totalStudents, 1, MPI_INT, numberOfAgents+numberOfUniv-1, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("University d %d [PUNISHED]: Received availableStudent = %d from University %d\n", world_rank, totalStudents, numberOfAgents+numberOfUniv-1);
            } else {
                MPI_Recv(&totalStudents, 1, MPI_INT, world_rank - 1, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("University b %d [PUNISHED]: Received availableStudent = %d from University %d\n", world_rank, totalStudents, world_rank-1);
                if(punished && (totalStudents - A) >= 0) {
                    currentStudentsInUniversity+=A;
                    totalStudents-=A;
                }
                
                
                MPI_Send(&totalStudents, 1, MPI_INT, ((world_rank+2)%numberOfUniv) + numberOfAgents, 5, MPI_COMM_WORLD);
                printf("University c %d [PUNISHED]: Send availableStudent = %d to University %d\n", world_rank, totalStudents, ((world_rank+2)%numberOfUniv + numberOfAgents));
            }
            
            
            
            //druga tura dla niekaranych
            
            //pierwszy uniwersytet
            if(world_rank==numberOfAgents) {
                if(punished==false && (totalStudents - A) >= 0) {
                    currentStudentsInUniversity+=A;
                    totalStudents-=A;
                }
                MPI_Send(&totalStudents, 1, MPI_INT, world_rank+1, 6, MPI_COMM_WORLD);
                printf("University %d [NOT PUNISHED]: Send availableStudent = %d to University %d\n", world_rank, totalStudents, world_rank+1);
                MPI_Recv(&totalStudents, 1, MPI_INT, numberOfAgents+numberOfUniv-1, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("University %d [NOT PUNISHED]: Received availableStudent = %d from University %d\n", world_rank, totalStudents, numberOfAgents+numberOfUniv-1);
            } else {
                MPI_Recv(&totalStudents, 1, MPI_INT, world_rank - 1, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("University %d [NOT PUNISHED]: Received availableStudent = %d from University %d\n", world_rank, totalStudents, world_rank - 1);
                if(punished==false && (totalStudents - A) >= 0) {
                    currentStudentsInUniversity+=A;
                    totalStudents-=A;
                }
                
                
                MPI_Send(&totalStudents, 1, MPI_INT, ((world_rank+2)%numberOfUniv) + numberOfAgents, 7, MPI_COMM_WORLD);
                printf("University %d [NOT PUNISHED]: Send availableStudent = %d to University %d\n", world_rank, totalStudents, ((world_rank+2)%numberOfUniv + numberOfAgents));
            }
            
            sprintf(message, "%d:%d:", world_rank, A-totalStudents);
            for(int i = 0; i<numberOfAgents ; i++) {
                MPI_Send(message, 8, MPI_CHAR, i, 1, MPI_COMM_WORLD);
                printf("University %d: Send message = %s to Agent %d\n", world_rank, message, i);
            }
            for(int i = 0; i<numberOfAudit ; i++) {
                MPI_Send(message, 8, MPI_CHAR, numberOfAgents + numberOfUniv + i, 2, MPI_COMM_WORLD);
                printf("University %d: Send message = %s to Auditor %d\n", world_rank, message, numberOfAgents + numberOfUniv + i);
            }
            
        //}
        
    } else if(procesType == Auditor) {
        
       // while(true) {
            char message[numberOfUniv][8];
            for(int k = 0; k<numberOfUniv ; k++) {
                MPI_Recv(message[k], 8, MPI_CHAR, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("Auditor %d: Received message = %s from University\n", world_rank, message[k]);
            }
            int punish[numberOfUniv] ;
            for(int i =0 ; i< numberOfUniv ; i++) {
                punish[i] = 0;
            }
            
            for(int z = 0; z<numberOfUniv/2 ; z++) {
                int univID = rand() % numberOfUniv;
                std::string str = string(message[univID]);

                std::string delimiter = ":";
                size_t pos = 0;
                std::string token[2];
                int num = 0;
                while ((pos = str.find(delimiter)) != std::string::npos) {
                    
                    token[num] = str.substr(0, pos);
                    str.erase(0, pos + delimiter.length());
                    num++;
                }
                
                
                int idProc = atoi(token[0].c_str());
                int studentToFill = atoi(token[1].c_str());
                
                if(studentToFill == 0) {
                    //? array index -1
                    punish[numberOfAgents + numberOfUniv - idProc] = 1;
                } else {
                    punish[numberOfAgents + numberOfUniv - idProc] = 0;
                }
            }
            
            for(int i = 0 ;i< numberOfUniv; i++) {
                MPI_Send(&punish[i], 1, MPI_INT, numberOfAgents + i, 3, MPI_COMM_WORLD);
                printf("Auditor %d: Send punish = %d to University %d\n", world_rank, punish[i], numberOfAgents + i);
            }
       // }
        
    }
    
    
    // Finalize the MPI environment.
    MPI_Finalize();
}
