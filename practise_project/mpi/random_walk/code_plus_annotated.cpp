// Author: Wes Kendall
// Copyright 2011 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header intact.
//
// Example application of random walking using MPI_Send, MPI_Recv, and MPI_Probe.
//

// 这个plus版本拟实现的目标是：
// 图像化输出起始及终末walker状态
// 拟实现方式是：
// 遍历每一个进程中的walker数组，将它们的位置用数组记录下来
// 然后顺序打印每一个进程对应的数据
    // 不好做，进程的执行是无序的
    // 怎么才能按照进程编号顺序获取和输出数据呢？
// 尝试一：
// 先建立一个全局的数组，然后每个进程按照线程编号对应计算索引并使用
// 比如：
// 在全局上：
// int total_space = num_walkers_per_proc*world_size;
// int *datas_tem = (int *)malloc(sizeof(int)*total_space);
// *dates_tem = 0;
// 然后统计数据：
// datas_tem[world_rank*abdomain_size*walker.location] ++;
// 统计完后：
// for(int i = 0 ; i < total_space ; i ++) {
//      printf("%d ",datas_tem[i]);
// }
// *datas_tem = 0;

#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <mpi.h>

using namespace std;

typedef struct {
  int location;
  int num_steps_left_in_walk;
} Walker;

// 划分子域
void decompose_domain(int domain_size, int world_rank, int world_size, int* subdomain_start, int* subdomain_size) {
  if (world_size > domain_size) {
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  *subdomain_start = domain_size / world_size * world_rank;
  *subdomain_size = domain_size / world_size;
  if (world_rank == world_size - 1) {
    *subdomain_size += domain_size % world_size;
  }
}

void initialize_walkers(int num_walkers_per_proc, int max_walk_size, int subdomain_start, vector<Walker>* incoming_walkers, int* datas_tem, int world_rank, int subdomain_size) {
  Walker walker;
  for (int i = 0; i < num_walkers_per_proc; i++) {
    walker.location = subdomain_start;
    walker.num_steps_left_in_walk = (rand() / (float)RAND_MAX) * max_walk_size;
    incoming_walkers->push_back(walker);
    datas_tem[walker.location]++;
  }
}

// void walk(Walker* walker, 
//           int subdomain_start, 
//           int subdomain_size, 
//           int domain_size, 
//           vector<Walker>* outgoing_walkers,
//           int *datas_tem_end
//           ) {
  
//   if(walker->num_steps_left_in_walk == 0) {
//     datas_tem_end[subdomain_start]++;
//     return;
//   }         

//   while (walker->num_steps_left_in_walk > 0) {
//     if (walker->location == subdomain_start + subdomain_size) {
//       if (walker->location == domain_size) {
//         walker->location = 0;
//         // if(walker->num_steps_left_in_walk == 0) {
//         //   datas_tem_end[walker->location]++;  // 捕捉漏在边界的随机点
//         // }
//       }
//       outgoing_walkers->push_back(*walker);
//       break;
//     } else {
//       walker->num_steps_left_in_walk--;
//       walker->location++;
//     }

//     if(walker->num_steps_left_in_walk == 0) {
//       datas_tem_end[walker->location]++;  // 但是恰好落在边界的点统计不到
//     }

//     // // 新增代码：捕捉刚好停在子域边界的walker
//     // if (walker->num_steps_left_in_walk == 0 && walker->location == subdomain_start + subdomain_size) {
//     //   datas_tem_end[walker->location]++;
//     // }

//     // // 新增代码：捕捉刚好停在子域边界的walker
//     // if (walker->num_steps_left_in_walk == 0 && walker->location == subdomain_start) {
//     //   datas_tem_end[walker->location]++;
//     // }
//   }
// }

void walk(Walker* walker, 
          int subdomain_start, 
          int subdomain_size, 
          int domain_size, 
          vector<Walker>* outgoing_walkers,
          int *datas_tem_end
          ) {
    while (walker->num_steps_left_in_walk > 0) {
        walker->num_steps_left_in_walk--;
        walker->location++;
        if (walker->location == domain_size) {
            walker->location = 0; // 环绕到全域的起始点
        }
    }

    // Walker 停止后，检查是否仍在当前进程的子域范围
    if (walker->location >= subdomain_start && walker->location < subdomain_start + subdomain_size) {
        // int local_index = walker->location - subdomain_start;
          int local_index = walker->location ;
        datas_tem_end[local_index]++;
    } else {
        // 若不在范围内，将其发送到目标进程
        outgoing_walkers->push_back(*walker);
    }
}


void send_outgoing_walkers(vector<Walker>* outgoing_walkers, int world_rank, int world_size) {
  MPI_Send((void*)outgoing_walkers->data(), outgoing_walkers->size() * sizeof(Walker), MPI_BYTE, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
  outgoing_walkers->clear();
}

void receive_incoming_walkers(vector<Walker>* incoming_walkers, int world_rank, int world_size) {
  MPI_Status status;
  int incoming_rank = (world_rank == 0) ? world_size - 1 : world_rank - 1;
  MPI_Probe(incoming_rank, 0, MPI_COMM_WORLD, &status);
  int incoming_walkers_size;
  MPI_Get_count(&status, MPI_BYTE, &incoming_walkers_size);
  incoming_walkers->resize(incoming_walkers_size / sizeof(Walker));
  MPI_Recv((void*)incoming_walkers->data(), incoming_walkers_size, MPI_BYTE, incoming_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

int main(int argc, char** argv) {
  int domain_size, max_walk_size, num_walkers_per_proc;

  if (argc < 4) {
    cerr << "Usage: random_walk domain_size max_walk_size num_walkers_per_proc" << endl;
    exit(1);
  }

  domain_size = atoi(argv[1]);
  max_walk_size = atoi(argv[2]);
  num_walkers_per_proc = atoi(argv[3]);

  MPI_Init(NULL, NULL);
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  
  if(world_rank == 1) {
    cout << "domain_size is : "<< domain_size<<endl;
    cout <<"max_walk_size is : "<<max_walk_size<<endl;
    cout <<"num_walkers_per_proc is : "<<num_walkers_per_proc<<endl; 
  }

  int total_space = domain_size; // Total space is the domain size
  int* datas_tem = (int*)malloc(sizeof(int) * total_space); // Allocate space for all walkers
  for (int i = 0; i < total_space; i++) {
    datas_tem[i] = 0;
  }

  int* datas_tem_end = (int*)malloc(sizeof(int) * total_space); // Allocate space for all walkers
  for (int i = 0; i < total_space; i++) {
    datas_tem_end[i] = 0;
  }

  srand(time(NULL) * world_rank);
  int subdomain_start, subdomain_size;
  vector<Walker> incoming_walkers, outgoing_walkers;

  decompose_domain(domain_size, world_rank, world_size, &subdomain_start, &subdomain_size);
  initialize_walkers(num_walkers_per_proc, max_walk_size, subdomain_start, &incoming_walkers, datas_tem, world_rank, subdomain_size);

  // Synchronize all processes before gathering the walker data
  MPI_Barrier(MPI_COMM_WORLD);

  // Use MPI_Gather to gather all walkers' positions from all processes to process 0
  int* gathered_data = NULL;
  if (world_rank == 0) {
    gathered_data = (int*)malloc(sizeof(int) * total_space * world_size); // Allocate for all processes
  }

  // Gather the walker counts from all processes
  MPI_Gather(datas_tem, total_space, MPI_INT, gathered_data, total_space, MPI_INT, 0, MPI_COMM_WORLD);

  // Print the gathered data in process 0
  if (world_rank == 0) {
    cout << "Walker states by process:" << endl;
    for (int i = 0; i < world_size; i++) {
      // cout << "Process " << i << ": ";
      // MPI_Gather应该是把所有的数据全部串联在一个指定的数组里面去了，所以不能简单的只是修改一个循环终止条件达到输出一条位置信息的数据目的
      
      // 原来的代码：
      //   for (int j = 0; j < total_space; j++) {
      //     cout << gathered_data[i * total_space + j] << " ";
      //   }
      // 对应输出：
      // Process 0: 2 0 0 0 0 0 0 0 0 0 0 0 
      // Process 1: 0 0 0 2 0 0 0 0 0 0 0 0 
      // Process 2: 0 0 0 0 0 0 2 0 0 0 0 0 
      // Process 3: 0 0 0 0 0 0 0 0 0 2 0 0 
      // 没有达到我想要的直接数据一行反应整个walker状态的数据

      // 修改后的代码
      // 对应的输出：
      // Walker states by process:
      // 2 0 0 2 0 0 2 0 0 2 0 0 
      for (int j = 0; j < subdomain_size; j++) {
        cout << gathered_data[i * total_space + j + subdomain_size*i] << " ";
      }
    }
    cout << endl << "-------------------------------------------------" ;
    free(gathered_data);
  }

  // Perform random walk
  int maximum_sends_recvs = max_walk_size / (domain_size / world_size) + 1;
  for (int m = 0; m < maximum_sends_recvs; m++) {
    for (int i = 0; i < incoming_walkers.size(); i++) {
      walk(&incoming_walkers[i], subdomain_start, subdomain_size, domain_size, &outgoing_walkers,datas_tem_end);
    }
    if (world_rank % 2 == 0) {
      send_outgoing_walkers(&outgoing_walkers, world_rank, world_size);
      receive_incoming_walkers(&incoming_walkers, world_rank, world_size);
    } else {
      receive_incoming_walkers(&incoming_walkers, world_rank, world_size);
      send_outgoing_walkers(&outgoing_walkers, world_rank, world_size);
    }
  }


// Synchronize all processes before gathering the walker data
  MPI_Barrier(MPI_COMM_WORLD);

  // Use MPI_Gather to gather all walkers' positions from all processes to process 0
  int* gathered_data_end = NULL;
  if (world_rank == 0) {
    gathered_data_end = (int*)malloc(sizeof(int) * total_space * world_size); // Allocate for all processes
  }

  // Gather the walker counts from all processes
  MPI_Gather(datas_tem_end, total_space, MPI_INT, gathered_data_end, total_space, MPI_INT, 0, MPI_COMM_WORLD);

  // Print the gathered data in process 0
  int cnt_judge_base = 0;
  if (world_rank == 0) {
    cout << endl ;
    cout << "The walkers positon after random motion:" << endl;

      // cout << "Process " << i << ": ";
      // MPI_Gather应该是把所有的数据全部串联在一个指定的数组里面去了，所以不能简单的只是修改一个循环终止条件达到输出一条位置信息的数据目的
      
      // 原来的代码：
      
      for (int i = 0; i < world_size; i++) {
          cout << "rank " << i << " records datas: ";
          for (int j = 0; j < total_space; j++) {
            cout << gathered_data_end[i * total_space + j] << " ";
            cnt_judge_base += gathered_data_end[i * total_space + j];
          }
          cout << endl; 
      }
      // 经过调试发现，这里出现的问题是：
      // 每个进程对应统计的位置出现了问题
    cout << "The total_walkers_num_base = " << cnt_judge_base ;
    cout << endl << "-------------------------------------------------"<<endl ;
  }


  int cnt_judge = 0;
  if (world_rank == 0) {
    cout << "Position distribution after statistics:" << endl;
    

      // cout << "Process " << i << ": ";
      // MPI_Gather应该是把所有的数据全部串联在一个指定的数组里面去了，所以不能简单的只是修改一个循环终止条件达到输出一条位置信息的数据目的
      
      // 原来的代码：
    for (int i = 0; i < world_size - 1 ; i++) {
        for (int j = 0; j < domain_size / world_size ; j++) {
          cout << gathered_data_end[i * (domain_size + subdomain_size) + j] << " ";
          cnt_judge += gathered_data_end[i * (domain_size + subdomain_size) + j];
        }
    }

    for(int j = 0 ; j < domain_size / world_size + domain_size % world_size ; j ++) {
        cout << gathered_data_end[(world_size - 1) * (domain_size + subdomain_size) + j] << " ";
        cnt_judge += gathered_data_end[(world_size - 1) * (domain_size + subdomain_size) + j];
    }
        // cout << endl; 
      // 经过调试发现，这里出现的问题是：
      // 每个进程对应统计的位置出现了问题

      // 打印最后一个特殊进程的数据：
      // for (int j = 0; j < domain_size / world_size + domain_size % world_size; j++) {
      //     cout << gathered_data_end[world_size - 1 * domain_size + j] << " ";
      //     cnt_judge += gathered_data_end[world_size - 1 * total_space + j];
      //   }

      cout << endl<< "The total_walkers_num_recorded = " << cnt_judge << endl;
      if(cnt_judge == cnt_judge_base) {
          cout << "cnt_judge is equal to cnt_judge_base !"<<"  "<<"Your disposal is right !"<< endl;
      } else cout << "cnt_judge is not equal to cnt_judge_base !"<<"  "<<"Your disposal is wrong !"<< endl;

      free(gathered_data_end);
  }


  cout << "Process " << world_rank << " done" << endl;
  MPI_Finalize();
  return 0;
}
