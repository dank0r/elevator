#include <iostream>
#include <vector>
#include <deque>
#include <set>
#include <fstream>
#include <string>
#include <stdio.h>
#include <thread>

using std::cout;
using std::endl;
using std::vector;
using std::deque;
using std::set;
using std::ifstream;
using std::string;
using std::thread;

struct order {
	int id;
	int time;
	int from;
	int to;
};

enum ACTION {EMPTY, WAIT, MOVE, OPEN, IDLE, IN, OUT, CLOSE};
enum DOORS {OPENED, CLOSED};

struct params {
	//number of floors
	int N = 9;
	//number of elevators
	int K = 4;
	//elevator capacity
	int C = 5;
	//time needed for elevator to move one floor up or down
	int T_STAGE = 1;
	//time needed for elevator to open the door
	int T_OPEN = 1;
	//elevator empty state time
	int T_IDLE = 10;
	//time needed for elevator to close the door
	int T_CLOSE = 1;
	//enter time
	int T_IN = 1;
	//exit time
	int T_OUT = 1;
};

class elevator {
	public:
		int id;
		int doors = CLOSED;
		int action = EMPTY;        //available states: empty (default), move, open, wait, idle, close
		int action_start = 0;      //time when action has started
		int direction = 1;         //available states: 1 (up, default), -1 (down), 0 (not moving)
		int floor = 0;
		set<int> dests;            //destinations
		params p;
		
		elevator(int el_id, params &PARAMS) {
			id = el_id;
			p = PARAMS;
		}

		void add_dest(int i) {
			dests.insert(i);
		};

		void remove_dest(int i) {
			dests.erase(i);
		}

		void step(int gtime) {
			switch(action) {
				case EMPTY:
					if(gtime - action_start == p.T_IDLE) {
						action = CLOSE;
						action_start = gtime;
					}
					break;
				case MOVE:
					if(gtime - action_start == p.T_STAGE) {
						floor += direction;
						//we arrived to some floor
						//it's time to check if we have to stop on this one and maybe change action to OPEN
						if(dests.find(floor) != dests.end()) {
							action = OPEN;
							action_start = gtime;
						}
					}
					break;
				case OPEN:
					if(gtime - action_start == p.T_OPEN) {
						action = WAIT;
						action_start = gtime;
						doors = OPENED;
						//at this moment people should be able to walk in or out
					}
					break;
				case CLOSE:
					if(gtime - action_start == p.T_CLOSE) {
						//at this moment we should decide what to do next
						action_start = gtime;
						doors = CLOSED;
					}
					break;
				 
				default:
					break;
			}
		}
};

void dispatcher(vector<elevator> elevators, int id) {
	
}

int main() {
	params PARAMS;
	//global time
	int gtime = 0;

	vector<elevator> elevators;
	for(int i = 0; i < PARAMS.K; i++) {
		elevator el(i, PARAMS);
		elevators.push_back(el);
	}

	deque<order> orders;
	ifstream input("input.txt");
	string line;
	int id = 0;
	while(getline(input, line)) {
		int h, m, s, from, to;
		sscanf(line.c_str(), "%02d:%02d:%02d %d %d", &h, &m, &s, &from, &to);
		int time = h * 3600
		         + m * 60
			 + s;
		order ord {id++, time, from, to};
		orders.push_back(ord);
	}
	
	vector<thread> el_threads;
	for(int i = 0; i < PARAMS.K; i++) {
		el_threads.push_back(thread(dispatcher, elevators, i));
	}

	for(gtime = 0; gtime < 24 * 3600 && !orders.empty(); gtime++) {
		if(orders[0].time == gtime) {
			//add order to some elevator
			orders.pop_front();
		}
		//check if we should add some orders again
	}

	for(auto &t : el_threads) {
		t.join();
	}
}
