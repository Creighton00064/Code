#include <tchar.h>
#include <urlmon.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std::chrono_literals;

#pragma comment (lib,"urlmon.lib")

struct Player
{
	friend bool operator==(const std::string& nick, const Player& obj)
	{
		return nick == obj.nick;
	}
	std::string nick;
	int score = 0;
	int played_games = 0;
	int wins = 0;
	int loses = 0;
	int draws = 0;
	int elo = 0;
	Player(std::string nick, int score, int played_games, int wins, int loses, int draws, int elo)
		: nick(nick), score(score), played_games(played_games), wins(wins), loses(loses), draws(draws), elo(elo)
	{}
	bool operator<(const Player& other) const
	{
		if (score != other.score)
			return score < other.score;
		if (played_games != other.played_games)
			return played_games > other.played_games;
		if (wins != other.wins)
			return wins < other.wins;
		if (elo != elo)
			return elo < other.elo;
		return nick < other.nick;
	}
	bool operator==(const Player& other) const
	{
		return nick == other.nick;
	}
	bool operator==(const std::string& nick) const
	{
		return this->nick == nick;
	}
};

int main()
{
	std::cout << "Enter id of the tournament to add to the excel file: ";
	std::wstring link_games(L"https://lichess.org/api/tournament/");
	std::wstring link_results(L"https://lichess.org/api/tournament/");
	wchar_t id[100];
	std::wcin >> id;
	link_games += id;
	link_results += id;
	link_games += L"/games";
	link_results += L"/results";
	std::wstring path_games(id);
	path_games += L".pgn";
	HRESULT games = URLDownloadToFile(NULL, link_games.c_str(), path_games.c_str(), 0, NULL);
	if (games != S_OK)
	{
		std::cerr << "Error downloading games from tournament" << std::endl;
		std::this_thread::sleep_for(3s);
		exit(2);
	}
	HRESULT results = URLDownloadToFile(NULL, link_results.c_str(), id, 0, NULL);
	if (results != S_OK)
	{
		std::cerr << "Error downloading results from tournament" << std::endl;
		std::this_thread::sleep_for(3s);
		exit(2);
	}
	std::cout << "Games downloaded successfully!" << std::endl;
	std::cout << "Results downloaded successfully!" << std::endl;
	std::wstring _csv_file(id);
	_csv_file += L".csv";
	std::wstring _pgn_file(id);
	std::wstring _file(id);
	_pgn_file += L".pgn";
	std::ifstream pgn_file(_pgn_file);
	std::ifstream file(_file);
	if (!pgn_file)
	{
		std::cerr << "Error opening games file\n";
		exit(3);
	}
	if (!file)
	{
		std::cerr << "Error opening results file\n";
		exit(4);
	}
	std::ofstream csv_file(_csv_file);
	if (!csv_file)
	{
		std::cerr << "Error creating csv file\n";
		std::this_thread::sleep_for(3s);
		exit(5);
	}
	std::vector<Player> players;
	std::string _event;
	std::cin.clear();
	std::cin.ignore();
	std::getline(pgn_file, _event);
	pgn_file.close();
	pgn_file.open(_pgn_file);
	std::string event = _event.substr(8, _event.length() - 10);
	csv_file << event << std::endl;
	csv_file << "Poøadí;Nickname;Body;Odehrané partie;Výhry;Prohry;Remízy;Elo" << std::endl;
	std::cin.clear();
	std::cin.ignore();
	////TYPES////
	// 1 = event
	// 2 = site
	// 3 = date
	// 4 = white name
	// 5 = black name
	// 6 = result
	// 7 = utc date
	// 8 = utc time
	// 9 = white elo
	//10 = black elo
	//11 = white rating difference
	//12 = black rating difference
	//13 = variant
	//14 = time control
	//15 = eco
	//16 = termination
	//17 = blank line #1
	//18 = game
	//19 = blank line #2
	//20 = blank line #3
	while (!file.eof())
	{
		std::string line;
		std::cin.clear();
		std::cin.ignore();
		std::getline(file, line);
		std::string nick;
		int score = 0;
		int elo = 0;
		size_t index_nick = line.find("\"username\":\"");
		index_nick += 12;
		if (index_nick == std::string::npos)
		{
			std::cerr << "Username not found" << std::endl;
			std::this_thread::sleep_for(3s);
			exit(6);
		}
		for (; index_nick < line.length(); ++index_nick)
		{
			if (line.at(index_nick) == '\"')
				break;
			nick.push_back(line.at(index_nick));
		}
		size_t index_elo = line.find("\"rating\":");
		index_elo += 9;
		std::stringstream ss_elo("");
		for (; index_elo < line.length(); ++index_elo)
		{
			if (line.at(index_elo) == ',')
				break;
			ss_elo << line.at(index_elo);
		}
		ss_elo >> elo;
		size_t index_score = line.find("\"score\":");
		index_score += 8;
		std::stringstream ss_score("");
		for (; index_score < line.length(); ++index_score)
		{
			if (line.at(index_score) == ',')
				break;
			ss_score << line.at(index_score);
		}
		ss_score >> score;
		if (nick != "")
			players.emplace_back(nick, score, 0, 0, 0, 0, elo);
	}
	while (!pgn_file.eof())
	{
		std::string nothing;
		std::string w_nick_line;
		std::string b_nick_line;
		std::string result_line;
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, w_nick_line);
		std::getline(pgn_file, b_nick_line);
		std::getline(pgn_file, result_line);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		if (w_nick_line == "" || b_nick_line == "" || result_line == "")
		{
			std::clog << "Empty string in reading pgn file. Breaking loop.\n";
			break;
		}
		if (nothing.find("Title") != std::string::npos)
		{
			std::getline(pgn_file, nothing);
			if (nothing.find("Title") != std::string::npos)
			{
				std::getline(pgn_file, nothing);
			}
		}
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::getline(pgn_file, nothing);
		std::string w_nick = w_nick_line.substr(8, w_nick_line.length() - 10);
		std::string b_nick = b_nick_line.substr(8, b_nick_line.length() - 10);
		double result{};
		if (result_line.length() != 14)
		{
			result = 0.5;
		}
		else
		{
			if (result_line.at(9) == '1')
			{
				result = 1.0;
			}
			else if (result_line.at(9) == '0')
			{
				result = 0.0;
			}
			else
			{
				std::cerr << "Error, invalid result\n";
				std::this_thread::sleep_for(3s);
				exit(5);
			}
		}
		auto it_w = std::find(players.begin(), players.end(), w_nick);
		if (it_w == players.end())
		{
			std::cerr << "Player " << w_nick << " not found\n";
			std::this_thread::sleep_for(3s);
			exit(6);
		}
		auto it_b = std::find(players.begin(), players.end(), b_nick);
		if (it_b == players.end())
		{
			std::cerr << "Player " << b_nick << " not found\n";
			std::this_thread::sleep_for(3s);
			exit(7);
		}
		++(it_w->played_games);
		++(it_b->played_games);
		if (result == 1)
		{
			++(it_w->wins);
			++(it_b->loses);
		}
		else if (result == 0.5)
		{
			++(it_w->draws);
			++(it_b->draws);
		}
		else if (result == 0)
		{
			++(it_b->wins);
			++(it_w->loses);
		}
	}
	size_t i = 1;
	std::sort(players.rbegin(), players.rend());
	for (const Player& player : players)
	{
		csv_file << i++ << ";" << player.nick << ";" << player.score << ";" << player.played_games << ";" << player.wins << ";"
			<< player.loses << ";" << player.draws << ";" << player.elo << std::endl;
	}
	pgn_file.close();
	file.close();
	csv_file.close();
	std::cin.get();
	std::cin.get();
	return 0;
}