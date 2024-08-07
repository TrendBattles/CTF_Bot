#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <cmath>

//Works good on 10x10 but sucks at larger boards since the branch is too large
namespace Player_1 {
	struct Distance_Map {
		std::vector <std::vector <int>> dist;
		int N;
		Distance_Map() {}
		
		Distance_Map(int N): dist(N, std::vector <int> (N, -1)), N(N) {}
		
		void BFS(int source_x, int source_y);
	};
	
    int N = 0, time_left = 0, given_time = 0;
    std::vector <std::vector <int>> board, flag;
	
	int cell_x = -1, cell_y = -1, point = 0;
	int opponent_x = -1, opponent_y = -1, opponent_point = 0;
	
	std::vector <std::vector <int>> player_steps, opponent_steps;
	
	
	int del_x[4] = {0, 0, -1, 1};
	int del_y[4] = {-1, 1, 0, 0};
	std::vector <std::vector <Distance_Map>> Cell_Map;
	
	int Open_Cell(int x, int y) {
		return x >= 0 and x < N and y >= 0 and y < N and board[x][y] != -1;
	}
	
    void Input(std::vector <std::vector <int>> value, int start_x, int start_y, int T) {
        board = value;
        N = (int) board.size();
		
        flag.assign(N, std::vector <int> (N));
		player_steps.assign(N, std::vector <int> (N));
		opponent_steps.assign(N, std::vector <int> (N));
		
		cell_x = start_x; cell_y = start_y;
		time_left = given_time = T;
		
		opponent_x = start_x;
		opponent_y = N + 1 - start_y;
		
		
		Cell_Map.assign(N, std::vector <Distance_Map> (N));
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				if (not Open_Cell(i, j)) continue;
					
				Cell_Map[i][j] = Distance_Map(N);
				Cell_Map[i][j].BFS(i, j);
			}
		}
    }


	const int inf = 0x3f3f3f3f, max_depth = 10;
	
	int Find_Distance(int x, int y, int s, int t) {
		return Cell_Map[x][y].dist[s][t];
	}
	
	std::vector <std::vector <int>> analysis_flag, analysis_player_steps, analysis_opponent_steps;
	int Estimation(int x, int y, int r_x, int r_y);
	
	
	int Search(int depth, int player_x, int player_y, int enemy_x, int enemy_y, int stored_delta, int alpha, int beta);
	
	int optimal_x = -1, optimal_y = -1;
	std::pair <int, int> Turn() {
		/*
		int x, y; std::cin >> x >> y;
		return std::make_pair(x, y);
		*/
		
		analysis_flag = flag;
		analysis_player_steps = player_steps;
		analysis_opponent_steps = opponent_steps;
		
		optimal_x = -1; optimal_y = -1;
		
		int EV = Search(std::min(max_depth, time_left), cell_x - 1, cell_y - 1, opponent_x - 1, opponent_y - 1, 0, -inf, inf);
	
		return std::make_pair(optimal_x + 1, optimal_y + 1);
	}
	
	std::pair <int, int> Current_Cell() {
		return std::make_pair(cell_x, cell_y);
	}
	
	void Mark_Flag(std::pair <int, int> cell) {
		int x, y; std::tie(x, y) = cell;
		if (x == -1) return;
		
		x -= 1; y -= 1;
		flag[x][y] = true;
	}
	
	void Set_Position(std::pair <int, int> new_cell) {
		cell_x = new_cell.first; cell_y = new_cell.second;
		player_steps[cell_x - 1][cell_y - 1] += 1;
	}
	
	void Set_Opponent_Position(std::pair <int, int> new_cell) {
		opponent_x = new_cell.first; opponent_y = new_cell.second;
		opponent_steps[opponent_x - 1][opponent_y - 1] += 1;
		time_left -= 1;
	}
	
	void Add_Point(int p) {
		point += p;
	}
	int Get_Point() {
		return point;
	}
	void Set_Opponent_Point(int p) {
		opponent_point = p;
	}
}

void Player_1::Distance_Map::BFS(int source_x, int source_y) {
	std::queue <std::pair <int, int>> q;
	dist[source_x][source_y] = 0;
		
	q.emplace(source_x, source_y);
	while (not q.empty()) {
		int x, y; std::tie(x, y) = q.front(); q.pop();
			
		for (int direction = 0; direction < 4; ++direction) {
			int new_x = x + del_x[direction], new_y = y + del_y[direction];
				
			if (not Open_Cell(new_x, new_y) or dist[new_x][new_y] != -1) continue;
				
			dist[new_x][new_y] = dist[x][y] + 1;
			q.emplace(new_x, new_y);
		}
	}
}

int Player_1::Estimation(int x, int y, int r_x, int r_y) {
	if (time_left <= max_depth) return 0;
	
	double weight = 0;
	
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			//std::cerr << analysis_flag[i][j] << ' ';
			if (analysis_flag[i][j] or not Open_Cell(i, j)) continue;
			
			int my_distance = Find_Distance(x, y, i, j);
			int opponent_distance = Find_Distance(r_x, r_y, i, j);
				
			if (my_distance == 0) {
				weight += board[i][j] * N; continue;
			}
			if (opponent_distance == 0) {
				weight -= board[i][j] * N; continue;
			}	
			
			//Higher chance of losing those points
			if (opponent_distance > my_distance) {
				weight += (double) board[i][j] * (N / 10) * (opponent_distance - my_distance) / my_distance;
			} else {
				weight += (double) board[i][j] * (N / 10) * (opponent_distance - my_distance) / opponent_distance;
			}
		}
			//std::cerr << '\n';
	}

	return (int) weight;
}
	
int Player_1::Search(int depth, int player_x, int player_y, int enemy_x, int enemy_y, int stored_delta, int alpha, int beta) {
	if (depth == 0) {
		//std::cerr << "YES " << player_x << ' ' << player_y << ' ' << enemy_x << ' ' << enemy_y << ' ' << Estimation(player_x, player_y, enemy_x, enemy_y) << '\n';
		return Estimation(player_x, player_y, enemy_x, enemy_y) + stored_delta;
	}
	
	int& steps_count = std::min(time_left, max_depth) % 2 == depth % 2 ? analysis_player_steps[player_x][player_y] : analysis_opponent_steps[player_x][player_y];
	steps_count += 1;
	
	//const int debugging = depth == std::min(time_left, max_depth);
	
	int origin_distance = std::min(time_left, max_depth) % 2 == depth % 2 ? Find_Distance(cell_x - 1, cell_y - 1, player_x, player_y) : Find_Distance(opponent_x - 1, opponent_y - 1, player_x, player_y);
	
	//Taking the flag
	if (analysis_flag[player_x][player_y] == false) {
		analysis_flag[player_x][player_y] = true;
		
		
		//Little tweak:
		//Flagging some cells on the path
		//Repetition -> Need to be flagged
		//Out of time -> Need to be flagged as soon as possible
		
		int coeff = board[player_x][player_y] * N * 2 / std::max(1, origin_distance) + steps_count * steps_count * steps_count + N * (N / 5) * (given_time - std::min(time_left, max_depth) + depth);
		int delta_EV = -Search(depth - 1, enemy_x, enemy_y, player_x, player_y, -(stored_delta + coeff), -beta, -alpha);
		
		analysis_flag[player_x][player_y] = false;
			
		//Alpha-Beta pruning: Move is too good, the opponent will avoid this move
		if (delta_EV >= beta) {
			steps_count -= 1;
			
			return beta;
		}
		//Don't forget to receive some points on the path
		
		/* if (debugging) {
			std::cerr << "DEBUG " << delta_EV << ' ' << player_x << ' ' << player_y << '\n';
		} */
		if (alpha < delta_EV) {
			alpha = delta_EV;
			
			if (depth == std::min(time_left, max_depth)) {
				optimal_x = player_x;
				optimal_y = player_y;
			}
		}
	}
		
	//Moving to adjacent cells

	for (int T = 0; T < 4; ++T) {
		int new_x = player_x + del_x[T], new_y = player_y + del_y[T];
			
		if (not Open_Cell(new_x, new_y)) continue;
			
		//Moving too much is not worth
		int coeff = -steps_count * steps_count * steps_count - N * (N / 5) * (given_time - std::min(time_left, max_depth) + depth);
		int delta_EV = -Search(depth - 1, enemy_x, enemy_y, new_x, new_y, -(stored_delta + coeff), -beta, -alpha);
	
		/* if (debugging) {
			std::cerr << "DEBUG " << delta_EV << ' ' << new_x << ' ' << new_y << '\n';
		} */
		
		//Alpha-Beta pruning
		if (delta_EV >= beta) {
			steps_count -= 1;
			
			return beta;
		}
		
		if (alpha >= delta_EV) continue;
		
		alpha = delta_EV;
		
		if (depth == std::min(time_left, max_depth)) {
			optimal_x = new_x;
			optimal_y = new_y;
		}
	}
	
	steps_count -= 1;
	
	return alpha;
}