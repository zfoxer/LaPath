/*
 * LaPath: Shortest path calculation using Learning Automata
 * Copyright (C) 2014-2020 by Constantine Kyriakopoulos
 * @version 1.0.2
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

/**
 * Default constructor.
 */
LA::LA()
{
	std::random_device rd;
	gen = std::mt19937_64(rd());
}

/**
 * Constructor that uses an initializer list for LA's items.
 */
LA::LA(std::initializer_list<int> neighs)
{
	for(auto neigh : neighs)
	{
		probs[neigh] = 1.0 / neighs.size();
		lastTimes[neigh] = 0;
		sizes[neigh] = 1;
	}
	std::random_device rd;
	gen = std::mt19937_64(rd());
}

/**
 * Empty destructor.
 */
LA::~LA() { }

/**
 * Updates all probabilities. Increases the one of the given item and decreases all others.
 * Summary before and after the increase is equal to 1. The sum of the decrease per item is
 * the amount of the increase for the input item.
 *
 * @param node The item whose probability will be increased
 * @param time The timestamp of the increase
 * @param feedback Environment's response to the item
 * @throws std::out_of_range The item is unknown to this LA
 */
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
	// The value of 'l' determines the convergence speed to the actual
	// demand probabilities and 'a' makes low priority neighbours not reach zero value
	for(auto& nodeProb : probs)
		if(nodeProb.first != node)
		{
			sumPj += (probs[nodeProb.first] - a);
			probs[nodeProb.first] = (probs[nodeProb.first] 
					- l * feedback * (probs[nodeProb.first] - a));
		}
	
	// The amount that was subtracted from the other items, will be added to this one.
	probs[node] = (probs[node] + l * feedback * sumPj);
	lastTimes[node] = time;
}

/**
 * Updates the time the given item was last selected.
 *
 * @param item The item whose "last selected time" will be updated
 * @param time The timestamp
 * @throws std::out_of_range The item is unknown to this LA
 */
void LA::timeChange(int item, double time) noexcept(false)
{
	if(probs.find(item) == probs.end())
		throw std::invalid_argument("LA::timeChange(..): Non-existent item");
	
	lastTimes[item] = time;
}

/**
 * Fetches all local items.
 * 
 * @return std::list<int> All locally monitored items
 */
std::list<int> LA::items()
{
	std::list<int> retItems;
	for(auto& pair : probs)
		retItems.push_back(pair.first);
	
	return retItems;
}

/**
 * Inserts a new item to this LA. Probabilities are updated to be equal.
 * 
 * @param node A new item to be inserted
 * @param size Its size
 */
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

/**
 * Chooses the next item considering its cost value.
 *
 * @param time The current time value
 * @return int The chosen item
 */
int LA::nextItem(double time)
{
	double maxCost = std::numeric_limits<double>::min();
	int chosenNeigh = NO_NEXT_ITEM;
	for(const auto& pair : probs)
	{
		double nodeCost = std::pow(time - lastTimes[pair.first], 2) 
				* pair.second / sizes[pair.first];
		// Keeping track of the item with the maximum cost
		if(nodeCost > maxCost)
			maxCost = nodeCost;
	}
	
	std::vector<int> chosenNeighs;
	for(const auto& pair : probs)
		if(maxCost == std::pow(time - lastTimes[pair.first], 2) * pair.second / sizes[pair.first])
			chosenNeighs.push_back(pair.first);
	
	// Collect all items with cost equal to maximum
	if(chosenNeighs.size())
	{
		// Choose one by using a uniform distribution
		std::uniform_int_distribution<> distro(0, chosenNeighs.size() - 1);
		chosenNeigh = chosenNeighs[distro(gen)];
	}
	
	return chosenNeigh;
}

/**
 * Prints all current probability values to the specified output stream.
 *
 * @param out The STL output stream probabilities will be written to
 */
void LA::dumbProbs(std::ostream& out)
{
	for(const auto& pair : probs)
		out << "node: " << pair.first << ", prob: " << pair.second << std::endl;
}

/**
 * Constructor for the LaSystem. 
 *
 * @param filename The JSON filename containing the physical topology
 * @param iterations The number of iterations, LAs will use to converge
 */
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
	
	this->iterations = (iterations > 0) ? iterations : ITERATIONS;
}

/**
 * Empty destructor.
 */
LaSystem::~LaSystem() { }

/**
 * Inserts a new edge to the LA System. Constructs the virtual topology internally.
 *
 * @param edge The edge to be inserted
 */
void LaSystem::insertEdge(Edge edge)
{
	localEdges.insert(edge);
	LA la;
	la.insertItem(edge.edgeEnd, sizeFromLength(edge.weight));
	
	// Every node is mapped to a LA. LAs contain its neighbours.
	if(las.find(edge.edgeStart) == las.end())
		las[edge.edgeStart] = la;
	else
		las[edge.edgeStart].insertItem(edge.edgeEnd, sizeFromLength(edge.weight));
	
	LA la2;
	if(las.find(edge.edgeEnd) == las.end())
		las[edge.edgeEnd] = la2;
}

/**
 * Creates a size value from an edge's length. This value will be considered from the LA
 * for choosing the next item.
 *
 * @param length The weight of an edge
 */
int LaSystem::sizeFromLength(double length)
{
	std::array<int, 8> availSizes{1, 2, 3, 4, 5, 6, 7, 8};
	if(length >= maxLength) 
		return availSizes[7];
	if(length <= 0) 
		return availSizes[0];
	
	return availSizes[static_cast<int>(std::floor(availSizes.size() 
			* length / (maxLength + 1)))];
}

/**
 * Finds the best path from a source node to destination, using the LA system.
 *
 * @param src Starting node
 * @param dest Ending node
 * @return std::vector<int> The converged path
 */
std::vector<int> LaSystem::path(int src, int dest)
{
	std::list<int> bestPath;
	double evaluation = std::numeric_limits<double>::max();
	double time = TIME_SLOT;
	
	// All these attempts will be made
	for(int i = 1; i <= iterations; ++i)
	{
		std::list<int> path;
		traverse(src, dest, path, time);
		if(path.front() != src || path.back() != dest)
		{
			// Failed to find adequate path
			// Path nodes will have their 'chosen' timestamps updated
			applyTimeChange(path, time);
			time += TIME_SLOT;
			continue;
		}
			
		double length = std::numeric_limits<double>::max();
		try
		{ 
			// A valid path is found, so it will be evaluated
			length = pathLength(path);
		}
		catch(std::exception& exc) 
		{
			// Evaluation failed, cancel this attempt
			applyTimeChange(path, time);
			time += TIME_SLOT;
			continue;
		}
		
		// Lower evaluation values are better, so keep the lowest
		if(length < evaluation)
		{
			evaluation = length;
			bestPath.clear();
			bestPath = path;
		}	
		
		// Update path's nodes with apropriate feedback
		applyFeedback(path, time, calcFeedback(path));
		time += TIME_SLOT;
	}

	std::vector<int> retPath;
	for(int node : bestPath)
		retPath.push_back(node);
	
	return retPath;
}

/**
 * Path length calculation method.
 *
 * @param path The path to be evaluated
 * @return double The given path's length
 * @throws std::invalid_argument Invalid path value
 */
double LaSystem::pathLength(const std::list<int>& path) const noexcept(false)
{
	if(path.size() <= 1 || static_cast<int>(path.size()) > las.size())
		throw std::invalid_argument("LaSystem::pathLength(..): No suitable path");

	std::vector<int> vecPath;
	for(int node : path)
		vecPath.push_back(node);
	
	double weightSum = 0;
	// For every path segment
	for(int i = 0; i < (int)vecPath.size() - 1; ++i)
	{
		// Find the edges that start with current trace node using the next lambda function
		std::for_each(localEdges.cbegin(), localEdges.cend(),
				[&vecPath, i, &weightSum](Edge edge)
				{ 
					if(edge.edgeStart == vecPath[i] && edge.edgeEnd == vecPath[i + 1])
						weightSum += edge.weight;
				});
	}
	
	return weightSum;
}

/**
 * Applies a feedback value to path's nodes.
 *
 * @param path The path containing the nodes
 * @param time The current update time
 * @param feedback	The feedback value (range 0-1)
 */
void LaSystem::applyFeedback(std::list<int>& path, double time, double feedback)
{
	std::vector<int> vecPath;
	for(int node : path)
		vecPath.push_back(node);
		
	// Get the right LA for path's nodes and update the probability for the right neighbour
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

/**
 * Applies a time change value to path's nodes.
 *
 * @param path The path containing the nodes
 * @param time The current update time
 */
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

/**
 * Calculates the feedback for a given path.
 *
 * @param path The path containing the nodes
 * @return double The feedback value [0,1]
 */
double LaSystem::calcFeedback(std::list<int>& path)
{
	return 1 - path.size() / static_cast<double>(las.size());
}

/**
 * Brute force method to find a path between two nodes. All valid result are returned.
 *
 * @param node The current node
 * @param dest The destination to be reached
 * @param path The current path nodes
 * @param currentTime The current time slot
 */
void LaSystem::traverse(int node, int dest, std::list<int>& path, double currentTime)
{
	path.push_back(node);
	if(node == dest || detectCycle(path))
		return;
	
	int nextNode;
	if((nextNode = getLA(node)->nextItem(currentTime)) == LA::NO_NEXT_ITEM)
		return;
	
	// Recurse to the next node
	traverse(nextNode, dest, path, currentTime);
}

/**
 * Returns the LA that is mapped to a node.
 *
 * @param item The item that is mapped to an LA containing its neighbours
 * @return LA* The LA pointer
 */
LA* LaSystem::getLA(int item)
{
	return &las[item];
}

/**
 * Detects if a cycle is formed inside the sequence of nodes.
 *
 * @param items The sequence of nodes
 * @return bool The indication of a cyclic sequence
 */
bool LaSystem::detectCycle(const std::list<int>& items)
{
	std::set<int> uniqueItems;
	for(int item : items)
		uniqueItems.insert(item);
	
	return items.size() != uniqueItems.size();
}

/**
 * Clears instance's state
 */
void LaSystem::clear()
{
	localEdges.clear();
	edges.clear();
	las.clear();
}

/**
 * Virtual slotted time for LA
 */
const double LaSystem::TIME_SLOT = 0.001;
