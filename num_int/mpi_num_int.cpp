#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

using namespace std::chrono;

float get_function_value(int f,float x, int intensity) {
    switch(f) {
      case 1:
          return f1(x, intensity);
          break;
      case 2:
          return f2(x, intensity);
          break;
      case 3:
          return f3(x, intensity);
          break;
      case 4:
          return f4(x, intensity);
          break;
  }
  
  std::cout << "Error: f is not valid.\n";
  return 0.0f;
}

  
int main (int argc, char* argv[]) {
  
  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }
  
  MPI_Init (&argc, &argv);
  int rank, total_processes;
  MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  int fuctionID    = atoi(argv[1]);
  float lowerBound = atof(argv[2]); // This is a
  float upperBound = atof(argv[3]); // This is b
  int n            = atoi(argv[4]);
  int intensity    = atoi(argv[5]);
  
  auto startTime = system_clock::now();
  
  float result = 0;
  float start = (upperBound - lowerBound) / static_cast<float>(n);
  float temp = 0.0f;
  
  float results[total_processes] = {0};
  //std::vector<float> results(total_processes);
  
  
  // Find out what part it does
  int part_size = n / total_processes;
  int start_index = part_size * rank;
  int end_index = start_index + part_size - 1;

  if(rank + 1 == total_processes) { // last part special case
      end_index = n;
  }

  for(int i = start_index; i < end_index; i++) {
      float x_value = lowerBound + (i + 0.5f) * start;
      temp += get_function_value(fuctionID, x_value, intensity);
  }

  std::cout << rank << " ," << temp << std::endl;
  
  // Submit work
  MPI_Gather(&temp, 1, MPI_FLOAT,
            &(results[0]), total_processes, MPI_FLOAT,
             0, MPI_COMM_WORLD);
  
  if (rank == 0) {
    // sum up all of the work
    for (int i = 0; i < total_processes; i++) {
      result += results[i];
      std::cout << results[i] << std::endl;
    }
    
    result = start * result;
    
    auto stopTime = system_clock::now();
    std::cout << result << std::endl;
    
    std::chrono::duration<double> diff = stopTime - startTime;
    
    std::cerr << diff.count();
}
  
  
  

  MPI_Finalize();
  return 0;
}
