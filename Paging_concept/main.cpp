# include <iostream>
# include <stdlib.h>
# include <time.h>
# include "CPU.h"
# include "Process.h"
# include <iomanip>
using namespace std;

#define RUNS 5
//static unsigned idCounter = 0;

const unsigned PAGE_REQUESTS = 100;
const unsigned QUANTUM = 100;
extern const unsigned RUN_TIME = 60000; // milliseconds
extern const unsigned JOB_COUNT = 150;
extern const unsigned NUM_PAGES = 100;
extern const std::map<Replacement, std::string> ReplacementString;

void runAvgOfFive();
void runWithTrace(unsigned int a,process_values p[]);

int main() {
    srand(time(NULL));

    runAvgOfFive();
}

/*   Run simulator 5 times, each to complete the one minutes, and compute
 *   the hit/miss ratio of pages referenced by the running jobs for each
 *   run. Then get average of 5 runs. */
void runAvgOfFive() {
    std::cout << "========== RUNNING EACH ALGORITHM 5 TIMES TO GET AVG STATS ==========" << std::endl;
     double hmratio[RUNS];
     unsigned int pstarted[RUNS];
     process_values p[150]; unsigned int pr =0;
     unsigned np, idCounter;
     for(int i=0; i<RUNS;i++){hmratio[i]=0.0;pstarted[i]=0;}
     for(int i=0; i<RUNS; i++)
     {
	idCounter = 0;
	for(int j=0; j<JOB_COUNT; j++)
	{
	    p[j].process_id = idCounter++;
	    p[j].arrivaltime = rand()%RUN_TIME;
	    p[j].serviceduration = (rand() % 5 + 1) * 1000;
	    np = rand()%4;
	    if(np==0)p[j].numpages =5;
	    if(np==1)p[j].numpages =11;
	    if(np==2)p[j].numpages =17;
	    if(np==3)p[j].numpages =31;
	}
	std::cout <<"****************************************************************** RUN - " << i << " ************************************************************" << endl;
	std::cout << left << setw(50) << "Algorithm" << left << setw(50) << "HitRatio" << left << setw(50) << "SwappedIn-Processes" << std::endl;
	std::cout <<"----------------------------------------------------------------------------------------------------------------------------------------" << endl;
	for(int algorithmInt = FIFO; algorithmInt != RANDOM +1; ++algorithmInt)
	{
	    double hitMissRatio =0.0; unsigned int processesStarted =0;
	    Replacement algorithm = static_cast<Replacement>(algorithmInt);
            std::string algorithmString = ReplacementString.find(algorithm)->second;
	    CPU cpu = CPU(algorithm, p);
//	    if(i==0) {
//		    cout << "Algorithm" << algorithmString << endl;
//		    cpu.print_queued_jobs();
//	    }
	    while (!cpu.isProcessesComplete() && cpu.getClockTime() <= RUN_TIME) {
		    Process *p = cpu.getNextProcess();
		    cpu.runProcess(QUANTUM, p);
            }
	    hitMissRatio = cpu.getRatio();
	    processesStarted = cpu.getProcessesStarted();
	    hmratio[algorithmInt] = hmratio[algorithmInt] + cpu.getRatio();
	    pstarted[algorithmInt] = pstarted[algorithmInt] + cpu.getProcessesStarted();
	    if(algorithmString == "FIFO") {double si = processesStarted; algorithmString.replace(algorithmString.begin(),algorithmString.end(),"LFU");}
	    else if(algorithmString == "LFU") algorithmString.replace(algorithmString.begin(),algorithmString.end(),"FIFO");
	    std::cout << left << setw(50) << algorithmString << left << setw(50) << hitMissRatio << left << setw(50) << processesStarted << std::endl;
	    if(i==4) {
		    runWithTrace(algorithmInt,p);
	    }
	}
     }
     std::cout << "********************************************************************************************************************************" << std::endl;
     std::cout << "============================================================= Average of 5 runs ================================================" << std::endl;
     std::cout << left << setw(50) << "Algorithm" << left << setw(50) << "HitRatio" << left << setw(50) << "SwappedIn-Processes" << std::endl;
     std::cout<<  "--------------------------------------------------------------------------------------------------------------------------------" << endl;
     unsigned val =0;
     for(int k=0;k<5;k++)
     {
         Replacement algorithm = static_cast<Replacement>(k);
         std::string algorithmString = ReplacementString.find(algorithm)->second;
	 if(algorithmString == "FIFO") {val = pstarted[2]/RUNS; algorithmString.replace(algorithmString.begin(),algorithmString.end(),"LFU");}
	 else if(algorithmString == "LFU") {val = pstarted[0]/RUNS; algorithmString.replace(algorithmString.begin(),algorithmString.end(),"FIFO");}
	 else val = pstarted[k]/RUNS;
	 std::cout << left << setw(50) << algorithmString << left << setw(50) << hmratio[k]/RUNS << left << setw(50) << val << std::endl;
     }
}

/*  Run the simulator for 100 page references, and for each reference, print
 *  all the print statements listed in the project guide. */
void runWithTrace(unsigned algorithmInt, process_values p[]) {
    std::cout << "========== 100 PAGE REQUESTS PRINTING CPU/MEMORY STATES FOR EACH ALGORITHM ==========" << std::endl;
         Replacement algorithm = static_cast<Replacement>(algorithmInt);
         std::string algorithmString = ReplacementString.find(algorithm)->second;
         std::cout << std::endl << "=============================================== Running with " << algorithmString;
         std::cout << " replacement. =======================================================" << std::endl;

         unsigned i = 0;
         CPU cpu = CPU(algorithm, p, true);
         while (!cpu.isProcessesComplete() && i < PAGE_REQUESTS) {
             Process *p = cpu.getNextProcess();
             cpu.runProcess(QUANTUM, p);
             i++;
         }
         /*std::cout << std::endl;
         std::cout << "Number of swapped in processes: ";
         std::cout << cpu.getProcessesStarted() << std::endl;
         std::cout << "Hit/Miss Ratio: " << cpu.getRatio() << std::endl;*/
}
