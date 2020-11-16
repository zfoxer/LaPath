/*
 * LaPath: Shortest path calculation using Learning Automata
 * Copyright (C) 2014-2020 by Constantine Kyriakopoulos
 * @version 1.0.1
 * 
 * @section LICENSE
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef LASYSTEM_H
#define LASYSTEM_H

#include "adaptivesystem.h"
#include <initializer_list>
#include <random>
#include <exception>
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#include <list>

class LA
{
public:
	static const int DEFAULT_ITEM_SIZE = 1;
	static const int NO_NEXT_ITEM = -1;
	LA();
	LA(std::initializer_list<int>);						
	~LA();
	void insertItem(int, int = DEFAULT_ITEM_SIZE);		
	int nextItem(double);
	void updateProbs(int, double, double) noexcept(false);
	void timeChange(int, double) noexcept(false);
	void dumbProbs(std::ostream&);
	std::list<int> items();

private:
	std::map<int, double> probs;		
	std::map<int, double> lastTimes;	
	std::map<int, int> sizes;
    std::mt19937_64 gen;
};

class LaSystem : public AdaptiveSystem
{
public:
	struct edgeHash
	{
		size_t operator()(const AdaptiveSystem::Edge& edge) const
		{
			return std::hash<long int>()(edge.id);
		}
	};
	static const int ITERATIONS = 3000;
	static const double TIME_SLOT;
	LaSystem(const std::string&, int = 0);
	virtual ~LaSystem();
	virtual std::vector<int> path(int, int);
	virtual void clear();
	
private:
	void insertEdge(AdaptiveSystem::Edge);
	void traverse(int, int, std::list<int>&, double);
	bool detectCycle(const std::list<int>&);
	LA* getLA(int);
	double pathLength(const std::list<int>&) const noexcept(false);
	void applyFeedback(std::list<int>&, double, double = 0.5);
	void applyTimeChange(std::list<int>&, double);
	double calcFeedback(std::list<int>&);
	int sizeFromLength(double);
	double maxLength;
	std::unordered_set<AdaptiveSystem::Edge, edgeHash> localEdges;  
	std::unordered_map<int, LA> las;
	int iterations;
};

#endif // LASYSTEM_H
