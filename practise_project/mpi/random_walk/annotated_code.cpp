// Author: Wes Kendall
// Copyright 2011 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header intact.
//
// Example application of random walking using MPI_Send, MPI_Recv, and MPI_Probe.
//
#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <mpi.h>
// 前面学习这个文档的时候分开看每一部分还可以理解，但是合在一起就有点无法理解，感觉代码实现的目的和任务不符
// 后面看了半天，终于把这个例子看懂了，我上面所存在的问题发生的原因就是没有把整体代码的实现联系起来，还是后面看到拓例中的一句话才串联起来
// 这个代码实现的是：
// 将总域“均分”成小域，然后每个域中分布有相同数量的walkers，然后用 initialize_walkers 函数初始化每个 walker 对应有的步数，也就是说，每一个walker的运动都是独立随机的
// 然后在每个进程中，为每个walker调用walk函数，更新 position 实现位置的更改
// 接着，如果 walker 逃离边界，那么会被储存到 outgoingwalkers 中然后被发送到下一个进程中
// 下一个进程接收到新的 incomingwalkers 进行新一轮的上诉操作


using namespace std;

typedef struct {
  int location;
  int num_steps_left_in_walk;
} Walker;

// 划分子域
void decompose_domain(int domain_size,        //总域的大小
                      int world_rank,         //执行程序的进程对应的编号
                      int world_size,         //该通信域中包含的通信子【进程】数量
                      int* subdomain_start,   //子域开始的位置
                      int* subdomain_size     //划分后子域的大小             
                      ){    
  // 判断进程数和总域大小间的关系
  if (world_size > domain_size) {
    // Don't worry about this special case. Assume the domain size
    // is greater than the world size.
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  *subdomain_start = domain_size / world_size * world_rank;     // 分配每个进程所在的子域所对应的起始“索引值”
  *subdomain_size = domain_size / world_size;                   // 计算均值子域大小
  if (world_rank == world_size - 1) {
    // Give remainder to last process
    *subdomain_size += domain_size % world_size;                // 将可能多余的域的大小分给最后一个进程
  }
}

void initialize_walkers(int num_walkers_per_proc,          // 每个进程对应分配的walker数量
                        int max_walk_size,                 // walker最大行走的值【相当于就是子域的大小】
                        int subdomain_start,               // 执行程序对应的进程对应子域的开始“索引值”
                        vector<Walker>* incoming_walkers   // 储存walker类的向量数组 用来储存初始化后的walkers
                        ) {
  Walker walker;

  // 为每个进程对应数量的walker进行初始化随机行走值
  for (int i = 0; i < num_walkers_per_proc; i++) {
    // Initialize walkers at the start of the subdomain
    walker.location = subdomain_start;
    walker.num_steps_left_in_walk =
      (rand() / (float)RAND_MAX) * max_walk_size;
    incoming_walkers->push_back(walker);
  }
}

void walk(Walker* walker,                                  // 待“行走”的结构
          int subdomain_start,                             // walker开始的初始位置【执行程序对应的进程对应子域的开始位置】
          int subdomain_size,                              // 子域大小
          int domain_size,                                 // 总域大小【用来判断是否出界】
          vector<Walker>* outgoing_walkers                 // 储存出界walkers
          ) {
  while (walker->num_steps_left_in_walk > 0) {
    if (walker->location == subdomain_start + subdomain_size) {   // 如果walker到达边界了，那么则进行下面的判断，最终把此walker储存到 outgoingwalkers 中
      // Take care of the case when the walker is at the end
      // of the domain by wrapping it around to the beginning
      if (walker->location == domain_size) {
        walker->location = 0;                                     // 如果达到总域边界，那么初始化此 walker 的 position 为起始位置
      }
      outgoing_walkers->push_back(*walker);
      break;
    } else {                                                      // 前进操作
      walker->num_steps_left_in_walk--;                           
      walker->location++;
    }
  }
}

// 发送超过边界的 walkers 然后清除 outgoingwalkers 储存器
void send_outgoing_walkers(vector<Walker>* outgoing_walkers,
                           int world_rank, 
                           int world_size
                           ) {
  // Send the data as an array of MPI_BYTEs to the next process.
  // The last process sends to process zero.

  // 这一个(world_rank + 1) % world_size操作值得学习，指定消息接受者为紧邻的下一个进程【而最后一个进程发送到第一个进程】
  MPI_Send((void*)outgoing_walkers->data(),
           outgoing_walkers->size() * sizeof(Walker), MPI_BYTE,
           (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
  // Clear the outgoing walkers list
  outgoing_walkers->clear();
}

void receive_incoming_walkers(vector<Walker>* incoming_walkers,
                              int world_rank, 
                              int world_size
                              ) {
  // Probe for new incoming walkers
  MPI_Status status;
  // Receive from the process before you. If you are process zero,
  // receive from the last process
  int incoming_rank =
    (world_rank == 0) ? world_size - 1 : world_rank - 1;
  MPI_Probe(incoming_rank, 0, MPI_COMM_WORLD, &status);
  // Resize your incoming walker buffer based on how much data is
  // being received
  int incoming_walkers_size;
  MPI_Get_count(&status, MPI_BYTE, &incoming_walkers_size);
  incoming_walkers->resize(incoming_walkers_size / sizeof(Walker));
  MPI_Recv((void*)incoming_walkers->data(), incoming_walkers_size,
           MPI_BYTE, incoming_rank, 0, MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);
}

int main(int argc, char** argv) {
  int domain_size;
  int max_walk_size;
  int num_walkers_per_proc;

  if (argc < 4) {
    cerr << "Usage: random_walk domain_size max_walk_size "
         << "num_walkers_per_proc" << endl;
    exit(1);
  }
  // 新的知识：从命令行读取数据
  domain_size = atoi(argv[1]);
  max_walk_size = atoi(argv[2]);
  num_walkers_per_proc = atoi(argv[3]);

  // 基本的初始化和进程信息获取
  MPI_Init(NULL, NULL);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // 随机数据的设置
  srand(time(NULL) * world_rank);
  int subdomain_start, subdomain_size;
  vector<Walker> incoming_walkers, outgoing_walkers;

  // Find your part of the domain
  decompose_domain(domain_size, world_rank, world_size,
                   &subdomain_start, &subdomain_size);
  // Initialize walkers in your subdomain
  initialize_walkers(num_walkers_per_proc, max_walk_size, subdomain_start,
                     &incoming_walkers);

  cout << "Process " << world_rank << " initiated " << num_walkers_per_proc
       << " walkers in subdomain " << subdomain_start << " - "
       << subdomain_start + subdomain_size - 1 << endl;

  // Determine the maximum amount of sends and receives needed to
  // complete all walkers
  int maximum_sends_recvs = max_walk_size / (domain_size / world_size) + 1;
  for (int m = 0; m < maximum_sends_recvs; m++) {
    // Process all incoming walkers
    for (int i = 0; i < incoming_walkers.size(); i++) {
       walk(&incoming_walkers[i], subdomain_start, subdomain_size,
            domain_size, &outgoing_walkers);
    }
    cout << "Process " << world_rank << " sending " << outgoing_walkers.size()
         << " outgoing walkers to process " << (world_rank + 1) % world_size
         << endl;
    if (world_rank % 2 == 0) {
      // Send all outgoing walkers to the next process.
      send_outgoing_walkers(&outgoing_walkers, world_rank,
                            world_size);
      // Receive all the new incoming walkers
      receive_incoming_walkers(&incoming_walkers, world_rank,
                               world_size);
    } else {
      // Receive all the new incoming walkers
      receive_incoming_walkers(&incoming_walkers, world_rank,
                               world_size);
      // Send all outgoing walkers to the next process.
      send_outgoing_walkers(&outgoing_walkers, world_rank,
                            world_size);
    }
    cout << "Process " << world_rank << " received " << incoming_walkers.size()
         << " incoming walkers" << endl;
  }
  cout << "Process " << world_rank << " done" << endl;
  MPI_Finalize();
  return 0;
}
