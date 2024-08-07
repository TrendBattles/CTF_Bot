#include <iostream>
#include <random>
#include <chrono>
#include <utility>
#include <tuple>
#include <queue>
#include <iomanip>
#include "player_1.hpp"
#include "player_2.hpp"


const int MAX_SIZE = 40;

std::vector <std::vector <int>> value(MAX_SIZE, std::vector <int> (MAX_SIZE));
std::vector <std::vector <int>> flag(MAX_SIZE, std::vector <int> (MAX_SIZE));

std::mt19937_64 mt(std::chrono::steady_clock::now().time_since_epoch().count());


int Connected() {
	std::vector <std::vector <int>> visited(MAX_SIZE, std::vector <int> (MAX_SIZE));
	for (int x = 0; x < MAX_SIZE; ++x) {
		for (int y = 0; y < MAX_SIZE; ++y) {
			visited[x][y] = value[x][y] == -1;
		}
	}
		
	int connected = 0;
	
	auto BFS = [&] (int s_x, int s_y) -> void {
		std::queue <std::pair <int, int>> q;
		q.emplace(s_x, s_y);
		visited[s_x][s_y] = true;
		
		while (not q.empty()) {
			int x, y; std::tie(x, y) = q.front(); q.pop();
			
			if (x + 1 < MAX_SIZE and not visited[x + 1][y]) {
				visited[x + 1][y] = true;
				q.emplace(x + 1, y);
			}
			if (y + 1 < MAX_SIZE and not visited[x][y + 1]) {
				visited[x][y + 1] = true;
				q.emplace(x, y + 1);
			}
		}
	};
	for (int x = 0; x < MAX_SIZE; ++x) {
		for (int y = 0; y < MAX_SIZE; ++y) {
			if (visited[x][y]) continue;
			connected += 1;
			
			BFS(x, y);
			
			if (connected > 1) return false;
		}
	}
	
	return connected == 1;
}

int64_t gen_range(int64_t l, int64_t r) {
	return mt() % (r - l + 1) + l;
}
void Setup() {
	//Randomizing the banned cells
	for (int remaining_banned = MAX_SIZE * MAX_SIZE / 5; remaining_banned; --remaining_banned) {
		int x = -1, y = -1, unsuccessful = 0;
		while (unsuccessful <= 30) {
			x = mt() % MAX_SIZE, y = mt() % MAX_SIZE;
			
			unsuccessful += 1;
			
			if (x == MAX_SIZE - 1 and (y == MAX_SIZE - 1 or y == 0)) continue;
			if (value[x][y] == -1) continue;
			
			value[x][y] = -1;
			
			if (Connected()) {
				//std::cerr << x + 1 << " " << y + 1 << '\n';
				break;
			}
			
			value[x][y] = 0;
		}
	}
	
	//Assigning random values to remaining cells
	
	int big_bonus = 20;
    for (int x = 0; x < MAX_SIZE; ++x) {
        for (int y = 0; y < MAX_SIZE; ++y) {
			if (value[x][y] == -1) continue;
			
			//Possibly large values with lower chances to balance the game
			if (mt() % 10 == 0 and big_bonus > 0) {	
                value[x][y] = gen_range(16, 30);
				big_bonus -= 1;
            } else {
                value[x][y] = gen_range(1, 10);
            }
        }
    }
	
}

#define CAPTURED_BY_1 -100
#define CAPTURED_BY_2 -200
void Print_The_Board() {
	for (int x = 0; x < MAX_SIZE; ++x) {
		for (int y = 0; y < MAX_SIZE; ++y) {
			std::cout << std::setw(4) << std::setfill(' ');
			
			
			if (value[x][y] == CAPTURED_BY_1) {
				std::cout << "x";
			} else if (value[x][y] == CAPTURED_BY_2) {
				std::cout << "o";
			} else if (value[x][y] == -1) {
				std::cout << "#";
			} else {
				std::cout << value[x][y];
			}
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}
int Put_Flag(std::pair <int, int> cell) {
	int x, y; std::tie(x, y) = cell;
	if (x < 1 or x > MAX_SIZE or y < 1 or y > MAX_SIZE) {
		std::cerr << "Invalid cell: (" << x << "," << y << ")\n";
		return -1;
	}
	x -= 1; y -= 1;
		
	if (flag[x][y]) {
		std::cerr << "Cell (" << x + 1 << "," << y + 1 << ") has been flagged already\n";
		return -1;
	}
	
	flag[x][y] = true;
	return 2;
}

int Next_Cell(std::pair <int, int> dest, std::pair <int, int> start) {
	if (dest.first < 1 or dest.first > MAX_SIZE or dest.second < 1 or dest.second > MAX_SIZE) {
		std::cerr << "Invalid cell: (" << dest.first << "," << dest.second << ")\n";
		return -1;
	}
		
	if (abs(start.first - dest.first) + abs(start.second - dest.second) != 1) {
		std::cerr << "(" << dest.first << "," << dest.second << ") is not adjacent (" << start.first << "," << start.second << ")\n";
		return -1;
	}
	int x, y; std::tie(x, y) = dest;
	x -= 1; y -= 1;
	if (value[x][y] == -1) {
		std::cerr << "(" << x + 1 << "," << y + 1 << ") is not allowed to enter\n";
		return -1;
	}
	return 1;
}

int main() {
    Setup();
	/* value = {
		{7,   1,  8,  8,  4,  1,  6,   1,  2,  -1},
	   {3,   5,   7,   3,   9,   8,   3,   -1,   9,   -1},
	   {7,   -1,   4,   -1,   3,  27,   2,   8,   5,   -1},
	   {6,   -1,   6,   1,   7,   -1,   -1,   -1,   9,  26},
	   {8,  10,   -1,   4,   3,  10,   8,   8,   6,   -1},
	   {9,   -1,   -1,   6,   5,   -1,   3,   -1,   5,   -1},
	   {2,   3,  -1,   -1,   -1,   -1,   6,   7,  27,   -1},
	   {25,  10,   7,  24,   3,   -1,   2,   -1,   3,   -1},
	   {1,  -1,   -1,   -1,   9,   3,   1,   5,   2,   -1},
	   {10,   7,   4,   1,   2,   4,   7,   -1,   7,   8}
	}; */
	/*
	{7,   1,  8,  8,  4,  1,  6,   1,  2,  -1},
   {3,   5,   7,   3,   9,   8,   3,   -1,   9,   -1},
   {7,   -1,   4,   -1,   3,  27,   2,   8,   5,   -1},
   {6,   -1,   6,   1,   7,   -1,   -1,   -1,   9,  26},
   {8,  10,   -1,   4,   3,  10,   8,   8,   6,   -1,
   {9,   -1,   -1,   6,   5,   -1,   3,   -1,   5,   -1}
   {2,   3,  -1,   -1,   -1,   -1,   6,   7,  27,   -1},
   {25,  10,   7,  24,   3,   -1,   2,   -1,   3,   -1},
   {1,  -1,   -1,   -1,   9,   3,   1,   5,   2,   -1},
   {10,   7,   4,   1,   2,   4,   7,   -1,   7,   8},
*/
/*
	5   2   1  10   1   3   8  10   4   7
   2   4   2   2   #   #   3   #   7   3
   3   #  23   3   8   #  23   8   8   #
   7   #   #   9   2   #   4   #  10   4
   4   3   #  25   8   #   9   #   7   #
   8   #   #   6   4   #   6   6   6   3
   9  27   #   #   5   7   #   7   #   #
   5   4   1  10   #   5   8   3   #   #
   1   4   #   8   3   5   8   2   6  23
   9   5   7   #   #   #  10   #   6  10
*/
	int gameplay_time = 500;
    Player_1::Input(value, MAX_SIZE, 1, gameplay_time);
	Player_2::Input(value, MAX_SIZE, MAX_SIZE, gameplay_time);
	
	std::pair <int, int> p_1 = Player_1::Current_Cell(), p_2 = Player_2::Current_Cell();	
	for (int iter = 0; iter < gameplay_time; ++iter) {
		int state = -1; std::pair <int, int> cell_pos;
		
		Print_The_Board();
		
		if (iter % 2 == 0) {
			while (state == -1) {
				cell_pos = Player_1::Turn();
				
				if (cell_pos == p_1) {
					state = Put_Flag(cell_pos);
				} else {
					state = Next_Cell(cell_pos, p_1);
				}
				
				if (state == -1) {
					std::cerr << "Game suspended: Player_1 made an invalid move!\n\n";
					//return 0;
				}
				
				Player_1::Set_Position(cell_pos);
				if (state == 2) {
					Player_1::Add_Point(value[cell_pos.first - 1][cell_pos.second - 1]);
					value[cell_pos.first - 1][cell_pos.second - 1] = CAPTURED_BY_1;
				}
			}
		} else {
			while (state == -1) {
				cell_pos = Player_2::Turn();
				
				if (cell_pos == p_2) {
					state = Put_Flag(cell_pos);
				} else {
					state = Next_Cell(cell_pos, p_2);
				}
				
				if (state == -1) {
					std::cerr << "Game suspended: Player_2 made an invalid move!\n\n";
				}
				
				Player_2::Set_Position(cell_pos);
				
				if (state == 2) {
					Player_2::Add_Point(value[cell_pos.first - 1][cell_pos.second - 1]);
					value[cell_pos.first - 1][cell_pos.second - 1] = CAPTURED_BY_2;
				}
			}
		}
		
		if (state == 2) {
			std::cerr << "Flagging at (" << cell_pos.first << "," << cell_pos.second << ")\n\n";
			Player_1::Mark_Flag(cell_pos);
			Player_2::Mark_Flag(cell_pos);
		}
		
		p_1 = Player_1::Current_Cell(); p_2 = Player_2::Current_Cell();
		std::cerr << "1: (" << p_1.first << "," << p_1.second << ") " << Player_1::Get_Point() << "\n";
		
		std::cerr << "2: (" << p_2.first << "," << p_2.second << ") " << Player_2::Get_Point() << "\n\n";
		
		Player_1::Set_Opponent_Position(p_2);
		Player_2::Set_Opponent_Position(p_1);
		Player_1::Set_Opponent_Point(Player_2::Get_Point());
		Player_2::Set_Opponent_Point(Player_1::Get_Point());
	}
	
	Print_The_Board();
    return 0;
}