#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "param.h"
#include "fcntl.h"

#define MAX_TIME 3000 // max time a child will run
#define SLEEP_TIME 25 // Time a process will sleep

int currentTime = 0;

int childCount = 0;

void
childTime(void){
  int pid;

  // Spawn 3 children
  for (int i = 0; i < 3; i++){
    childCount++;
    pid = fork();
    if (pid <= 0) break; // Break if a child hits this
  }

  if (pid == 0){ // Only children should do this
    settickets(childCount * 10); // Set tickets to child number times 10 so they go 10, 20, 30

    while(currentTime < MAX_TIME){
      int b = 0;
      while(b < MAX_TIME) b += 1; // Random garbage operation
      sleep(SLEEP_TIME); // The actual thing it should be doing
      currentTime += SLEEP_TIME;
    }
    printf(1, "Finished\n");
    exit(); // Stop when done
  }
}

// running system call getpinfo()
int main (void){
    childTime(); // Spawn 3 child processes

    // Get initial process data
    struct pstat processes = {0};
    if(getpinfo(&processes) < 0){
      exit();
    }

    // Print a header
    printf(1, "PID(Tickets)\n");

    printf(1, "Time\t");
    for (int i = 0; i < NPROC; i++){
      if (processes.pid[i] == 0) continue;
      printf(1, "%d(%d)\t", processes.pid[i], processes.tickets[i]);
    }
    printf(1, "\n");

    for (int i = 0; i < 30; i++){
      getpinfo(&processes); // Refresh data
      printf(1, "%d,", uptime()); // Print time

      // Print times for each process
      for (int i = 0; i < NPROC; i++){
        if (processes.pid[i] == 0) continue;
        printf(1, "%d,", processes.ticks[i]);
      }
      printf(1, "\n", 1);
      sleep(SLEEP_TIME); // Wait a bit
    }

    exit();
}
