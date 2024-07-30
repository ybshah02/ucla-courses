#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process {
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  bool completed;
  u32 wait_time;
  u32 response_time;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end) {
  u32 current = 0;
  bool started = false;
  while (*data != data_end) {
    char c = **data;

    if (c < 0x30 || c > 0x39) {
      if (started) {
	return current;
      }
    }
    else {
      if (!started) {
	current = (c - 0x30);
	started = true;
      }
      else {
	current *= 10;
	current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data) {
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++])) {
    if (c < 0x30 || c > 0x39) {
      exit(EINVAL);
    }
    if (!started) {
      current = (c - 0x30);
      started = true;
    }
    else {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1) {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED) {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;
  

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL) {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i) {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }
  
  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

  u32 t = 0;
  bool repeat = false;
  u32 temp = quantum_length;

  bool cont = true;
  // continue until cont set false
  while (cont) {
    
    // cont set to false once every process has ran
    u32 count = 0;
    for (u32 i = 0; i < size; i++) {
        if (data[i].burst_time != 0){
            cont = true;
        } else {
            count++;
        }
        // all processes have been ran
        if (count == size-1){
            cont = false;
            break;
        }
    }

    count = 0;
    while (count < size) {
      // new process arrives to the ready queue
      if (t == data[count].arrival_time) {
          if (!repeat) {
            // add process to end of queue because LIFO
            TAILQ_INSERT_TAIL(&list, &data[count], pointers);
            data[count].completed = false;
          }
      }
      count+=1;
    }

    struct process *current;

    //process first in queue
    current = TAILQ_FIRST(&list);
    if (current != NULL) {
        // process done executing
        if (current->burst_time <= 0) {
            repeat = true;
            // reset temp qauntum length
            temp = quantum_length;
            // remove process from round robin
            TAILQ_REMOVE(&list, current, pointers);
            current->completed = true;
            continue;
        }
        // process hasn't been executed
        else {
            if (temp <= 0) {
                repeat = true;
                //reset temp quantum time
                temp = quantum_length;
                // pop the queue
                TAILQ_REMOVE(&list, current, pointers);
                // add process to end of queue
                TAILQ_INSERT_TAIL(&list, current, pointers);
                current->completed = true;
                continue;
            } else {
                // execute
                repeat = false;
                current->burst_time-=1;
                temp-=1;
                current->completed = true;
            }
        }

        struct process *itr;
        // deal with remaining processes in queue
        TAILQ_FOREACH(itr, &list, pointers){
            if (current != itr && !itr->completed) {
                itr->wait_time+=1;
                itr->response_time+=1;
            } else if(current != itr) {
                itr->wait_time+=1;
            }
        }
    }
    t+=1;
  }

  // add up the timings
  for (u32 i=0; i<size; i++) {
    total_response_time += data[i].response_time;
    total_waiting_time += data[i].wait_time;
  }

  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float) total_waiting_time / (float) size);
  printf("Average response time: %.2f\n", (float) total_response_time / (float) size);

  free(data);
  return 0;
}