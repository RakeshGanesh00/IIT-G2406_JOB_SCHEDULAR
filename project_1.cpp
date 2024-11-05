#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <queue>
#include <tuple>
using namespace std;

// Job structure to hold job details
class Job {
public:
    int jobId;
    int arrivalDay;
    int arrivalHour;
    int memReq;
    int cpuReq;
    int exeTime;
};

// Worker node structure
class WorkerNode {
public:
    int cores = 24;
    int ram = 64;
    int availableCores = 24;
    int availableRam = 64;
    // Store jobs and remaining execution time
    vector<tuple<Job, int>> runningJobs; 

    // Function to allocate job resources
    bool allocate(Job &job) {
        if (availableCores >= job.cpuReq && availableRam >= job.memReq) {
            availableCores = availableCores -job.cpuReq;
            availableRam = availableRam -job.memReq;
            runningJobs.push_back(make_tuple(job, job.exeTime)); // Add to running jobs
            return true;
        }
        return false;
    }

    // Function to free resources after job completion
    void freeResources(Job &job) {
        availableCores = availableCores + job.cpuReq;
        availableRam = availableRam + job.memReq;
    }

    // Function to update running jobs' execution time and free resources for completed jobs
    void updateRunningJobs() {
        for (auto it = runningJobs.begin(); it != runningJobs.end();) {
            Job &job = get<0>(*it);
            int &remainingTime = get<1>(*it);
            remainingTime--; 

            if (remainingTime <= 0) {
                freeResources(job);
                it = runningJobs.erase(it);
            } else {
                ++it;
            }
        }
    }
};


Job parseJob(string line) {
    Job job;
    sscanf(line.c_str(),
           "JobId: %d Arrival Day: %d Time Hour: %d MemReq: %d CPUReg: %d ExeTime: %d",
           &job.jobId, &job.arrivalDay, &job.arrivalHour, &job.memReq, &job.cpuReq, &job.exeTime);
    return job;
}

int main() {
    vector<WorkerNode> workers(128);

    ifstream jobFile("jobs.txt");
    if (!jobFile.is_open()) {
        cerr << "Error: Could not open the jobs file!" << endl;
        return 1;
    }

    // add csv file
    ofstream csvFile("overall_report.csv");
    csvFile << "JobId, ArrivalDay, ArrivalHour, MemReq, CPUReq, ExeTime, Status "<<endl;;

    string line;
    queue<Job> unallocatedJobs; // to hold unallocated jobs
    int currentDay = 0, currentHour = 0; 

    // Reading the job file line by line
    while (getline(jobFile, line)) {
        if (line.empty()) continue; // Skip empty lines

        // Parse the job from the line
        Job job = parseJob(line);

        // Try to allocate job to a worker node
        bool allocated = false;
        for (auto &worker : workers) {
            if (worker.allocate(job)) {
                allocated = true;
                csvFile << job.jobId << ", " << job.arrivalDay << ", " << job.arrivalHour << ", "
                        << job.memReq << ", " << job.cpuReq << ", " << job.exeTime << ", Allocated "<<endl;;
                cout << "Job " << job.jobId << " allocated successfully. "<<endl;;
                break;
            }
        }

        if (!allocated) {
            // Add to unallocated jobs queue
            unallocatedJobs.push(job);
            csvFile << job.jobId << ", " << job.arrivalDay << ", " << job.arrivalHour << ", "
                    << job.memReq << ", " << job.cpuReq << ", " << job.exeTime << ", Reinserted "<<endl;;
            cout << "Job " << job.jobId << " reinserted into queue. "<<endl;;
        }

        // Update the simulation time (for simplicity, advance hour by hour)
        currentHour++;
        if (currentHour == 24) {
            currentHour = 0;
            currentDay++;
        }

        // Update all workers' running jobs and free resources for completed jobs
        for (auto &worker : workers) {
            worker.updateRunningJobs();
        }

        // Retry allocation for unallocated jobs
        int unallocatedCount = unallocatedJobs.size();
        for (int i = 0; i < unallocatedCount; i++) {
            Job unallocatedJob = unallocatedJobs.front();
            unallocatedJobs.pop();

            bool reallocated = false;
            for (auto &worker : workers) {
                if (worker.allocate(unallocatedJob)) {
                    reallocated = true;
                    csvFile << unallocatedJob.jobId << ", " << unallocatedJob.arrivalDay << ", "
                            << unallocatedJob.arrivalHour << ", " << unallocatedJob.memReq << ", "
                            << unallocatedJob.cpuReq << ", " << unallocatedJob.exeTime << ", Allocated After Retry "<<endl;;
                    cout << "Job " << unallocatedJob.jobId << " allocated after retry. "<<endl;;
                    break;
                }
            }

            if (!reallocated) {
                // If still not allocated, reinsert it into the queue
                unallocatedJobs.push(unallocatedJob);
            }
        }
    }

    jobFile.close();
    csvFile.close();

    return 0;
} 