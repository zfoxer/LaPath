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

#include "adaptivesystem.h"

/**
 * Empty constructor.
 */
AdaptiveSystem::AdaptiveSystem() { }

/**
 * Empty destructor.
 */
AdaptiveSystem::~AdaptiveSystem() { }

/**
 * Edge constructor.
 */
AdaptiveSystem::Edge::Edge()
{
	edgeStart = edgeEnd = weight = id = 0;
}

/**
 * Comparison of current instance with the rhs, based on IDs.
 *
 * @param rhs The right hand side object
 * @return bool The indication of current id being less than rhs'
 */
bool AdaptiveSystem::Edge::operator<(const Edge& rhs) const
{
	return id < rhs.id;
}

/**
 * Comparison of current instance with the rhs, based on IDs.
 *
 * @param rhs The right hand side object
 * @return bool The indication of current id being greater than rhs'
 */
bool AdaptiveSystem::Edge::operator>(const Edge& rhs) const
{
	return id > rhs.id;
}

/**
 * Comparison of current instance with the rhs for equality, based on IDs.
 *
 * @param rhs The right hand side object
 * @return bool The indication of equality
 */
bool AdaptiveSystem::Edge::operator==(const Edge& rhs) const
{
	return id == rhs.id;
}

/**
 * Initialises the internal topology representation.
 *
 * @param filename The JSON filename containing the physical topology
 */
void AdaptiveSystem::initTopo(const std::string& filename)
{
	ptree pt;
	boost::property_tree::read_json(filename, pt);
	ptree::const_iterator end = pt.end();
	for(ptree::const_iterator it = pt.begin(); it != end; ++it)
	{
		if(!std::strcmp(it->first.c_str(), "number_of_nodes"))
			continue;

		ptree::const_iterator end2 = it->second.end();
		for(ptree::const_iterator it2 = it->second.begin(); it2 != end2; ++it2)
		{
			ptree::const_iterator end3 = it2->second.end();
			int src = 0; int dest = 0; int length = 0;
			for(ptree::const_iterator it3 = it2->second.begin(); it3 != end3; ++it3)
			{
				if(!std::strcmp(it3->first.c_str(), "nodes"))
				{
					int index = 0;
					ptree::const_iterator end31 = it3->second.end();
					for(ptree::const_iterator it31 = it3->second.begin(); it31 != end31; ++it31, ++index)
					{
						if(index == 0)
							src = it31->second.get_value<int>();
						if(index == 1)
							dest = it31->second.get_value<int>();
					}
				}
				if(!std::strcmp(it3->first.c_str(), "length"))
					length = it3->second.get_value<int>();
			}

			insertEdge(src, dest, static_cast<double>(length));
		}
	}
}

/**
 * Inserts an edge.
 * 
 * @param src Source node
 * @param dest Destination node
 * @param weight Weight for the edge
 */
void AdaptiveSystem::insertEdge(int src, int dest, double weight) noexcept(false)
{
	AdaptiveSystem::Edge edge;
	edge.edgeStart = src;
	edge.edgeEnd = dest;
	edge.weight = weight;
	edge.id = ++edgeIdCnt;
	edges.push_back(edge);
}

/**
 * Used for producing edge IDs.
 */
int AdaptiveSystem::edgeIdCnt = 0;
