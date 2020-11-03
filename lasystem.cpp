/*
 * LaPath: Shortest path calculation using Learning Automata
 * Copyright (C) 2014-2020 by Constantine Kyriakopoulos
 * @version 0.9
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

#include "lasystem.h"

LA::LA() { }

LA::LA(std::initializer_list<int> neighs)
{
	for(auto neigh : neighs)
	{
		probs[neigh] = 1.0 / neighs.size();
		lastTimes[neigh] = 0;
		sizes[neigh] = 1;
	}
}

LA::~LA() { }

void LA::updateProbs(int node, double time, double feedback) noexcept(false)
{
	if(probs.find(node) == probs.end())
		throw std::invalid_argument("LA::updateProbs(..): Non-existent node");
	
	if(feedback < 0)
		feedback = 0;
	if(feedback > 1)
		feedback = 1;
	
	double a = 0.0001;
	double l = 0.15;
	double sumPj = 0;
	for(auto& nodeProb : probs)
		if(nodeProb.first != node)
		{
			sumPj += (probs[nodeProb.first] - a);
			probs[nodeProb.first] = (probs[nodeProb.first] 
					- l * feedback * (probs[nodeProb.first] - a));
		}
	
	probs[node] = (probs[node] + l * feedback * sumPj);
	lastTimes[node] = time;
}

void LA::timeChange(int item, double time) noexcept(false)
{
	if(probs.find(item) == probs.end())
		throw std::invalid_argument("LA::timeChange(..): Non-existent item");
	
	lastTimes[item] = time;
}

std::list<int> LA::items()
{
	std::list<int> retItems;
	for(auto& pair : probs)
		retItems.push_back(pair.first);
	
	return retItems;
}

void LA::insertItem(int node, int size)
{
	if(probs.find(node) != probs.end())
		return;
	
	lastTimes[node] = 0;
	probs[node] = 0;
	sizes[node] = size;
	int items = probs.size();
	for(auto& pair : probs)
		pair.second = 1.0 / items;
}

int LA::nextItem(double time)
{
	double maxCost = std::numeric_limits<double>::min();
	int chosenNeigh = NO_NEXT_ITEM;
	for(const auto& pair : probs)
	{
		double nodeCost = std::pow(time - lastTimes[pair.first], 2) 
				* pair.second / sizes[pair.first];
		if(nodeCost > maxCost)
			maxCost = nodeCost;
	}
	
	std::vector<int> chosenNeighs;
	for(const auto& pair : probs)
		if(maxCost == std::pow(time - lastTimes[pair.first], 2) * pair.second / sizes[pair.first])
			chosenNeighs.push_back(pair.first);
	if(chosenNeighs.size())
	{
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<> distro(0, chosenNeighs.size() - 1);
		chosenNeigh = chosenNeighs[distro(gen)];
	}
	
	return chosenNeigh;
}

void LA::dumbProbs(std::ostream& out)
{
	for(const auto& pair : probs)
		out << "node: " << pair.first << ", prob: " << pair.second << std::endl;
}

LaSystem::LaSystem(const std::string& filename, int iterations) 
{
	maxLength = 0;
	try
	{
		initTopo(filename);
		for(auto& edge : edges)
		{
			insertEdge(edge);
			if(edge.weight > maxLength)
				maxLength = edge.weight;
		}
		
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
    
    if(iterations > 0)
    {
        this->iterations = iterations;
    }
    else
    {
        this->iterations = ITERATIONS;
    }
}

LaSystem::~LaSystem() { }

void LaSystem::insertEdge(Edge edge)
{
	localEdges.insert(edge);
	LA la;
	la.insertItem(edge.edgeEnd, sizeFromLength(edge.weight));
	
	if(las.find(edge.edgeStart) == las.end())
		las[edge.edgeStart] = la;
	else
		las[edge.edgeStart].insertItem(edge.edgeEnd, sizeFromLength(edge.weight));
	
	LA la2;
	if(las.find(edge.edgeEnd) == las.end())
		las[edge.edgeEnd] = la2;
}

int LaSystem::sizeFromLength(double length)
{
	std::array<int, 4> availSizes{1, 2, 3, 4};
	if(length >= maxLength) 
		return availSizes[3];
	if(length <= 0) 
		return availSizes[0];
	
	return availSizes[static_cast<int>(std::floor(availSizes.size() * length / maxLength))];
}

std::vector<int> LaSystem::path(int src, int dest)
{
	std::list<int> bestPath;
	double evaluation = std::numeric_limits<double>::max();
	double time = TIME_SLOT;
	
	for(int i = 1; i <= ITERATIONS; ++i)
	{
		std::list<int> path;
		traverse(src, dest, path, time);
		if(path.front() != src || path.back() != dest)
		{
			applyTimeChange(path, time);
			time += TIME_SLOT;
			continue;
		}
			
		double length = std::numeric_limits<double>::max();
		try
		{ 
			length = pathLength(path);
		}
		catch(std::exception& exc) 
		{
			applyTimeChange(path, time);
			time += TIME_SLOT;
			continue;
		}
		
		if(length < evaluation)
		{
			evaluation = length;
			bestPath.clear();
			bestPath = path;
		}	
		
		applyFeedback(path, time, calcFeedback(path));
		time += TIME_SLOT;
	}
	
	std::vector<int> retPath;
	for(int node : bestPath)
		retPath.push_back(node);
	
	return retPath;
}

std::list<int> LaSystem::fewerHops(const std::vector<std::list<int> >& paths)
{
	std::list<int> chosenPath;
	unsigned int nodes = std::numeric_limits<unsigned int>::max();
	std::for_each(paths.begin(), paths.end(),
			[&chosenPath, &nodes](std::list<int> path)
			{
				if(path.size() < nodes)
				{
					nodes = path.size();
					chosenPath = path;
				}
			});
	
	return chosenPath;
}
	
double LaSystem::pathLength(const std::list<int>& path) const noexcept(false)
{
	if(path.size() <= 1 || static_cast<int>(path.size()) > las.size())
		throw std::invalid_argument("LaSystem::pathLength(..): No suitable path");

	std::vector<int> vecPath;
	for(int node : path)
		vecPath.push_back(node);
	
	double weightSum = 0;
	for(int i = 0; i < (int)vecPath.size() - 1; ++i)
	{
		std::for_each(localEdges.cbegin(), localEdges.cend(),
				[&vecPath, i, &weightSum](Edge edge)
				{ 
					if(edge.edgeStart == vecPath[i] && edge.edgeEnd == vecPath[i + 1])
						weightSum += edge.weight;
				});
	}
	
	return weightSum;
}

void LaSystem::applyFeedback(std::list<int>& path, double time, double feedback)
{
	std::vector<int> vecPath;
	for(int node : path)
		vecPath.push_back(node);
		
	for(unsigned int i = 0; i < vecPath.size() - 1; ++i)
		try
		{
			getLA(vecPath[i])->updateProbs(vecPath[i + 1], time, feedback);
		}
		catch(std::exception& exc)
		{
			std::cerr << exc.what() << std::endl;
		}
}

void LaSystem::applyTimeChange(std::list<int>& path, double time)
{
	std::vector<int> vecPath;
	for(int node : path)
		vecPath.push_back(node);
		
	for(unsigned int i = 0; i < vecPath.size() - 1; ++i)
		try
		{
			getLA(vecPath[i])->timeChange(vecPath[i + 1], time);
		}
		catch(std::exception& exc)
		{
			std::cerr << exc.what() << std::endl;
		}
}

double LaSystem::calcFeedback(std::list<int>& path)
{
	double feedback = static_cast<double>(las.size()) / path.size();
	
	if(feedback < 0)
		feedback = 0;
	if(feedback > 1)
		feedback = 1;

	return feedback;
}

void LaSystem::traverse(int node, int dest, std::list<int>& path, double currentTime)
{
	path.push_back(node);
	if(node == dest || detectCycle(path))
		return;
	
	int nextNode;
	if((nextNode = getLA(node)->nextItem(currentTime)) == LA::NO_NEXT_ITEM)
		return;
	
	traverse(nextNode, dest, path, currentTime);
}

LA* LaSystem::getLA(int item)
{
	return &las[item];
}

bool LaSystem::detectCycle(const std::list<int>& items)
{
	std::set<int> uniqueItems;
	for(int item : items)
		uniqueItems.insert(item);
	
	return items.size() != uniqueItems.size();
}

void LaSystem::clear()
{
	localEdges.clear();
	edges.clear();
	las.clear();
}

const double LaSystem::TIME_SLOT = 0.001;
