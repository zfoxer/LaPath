/*
 * LaPath: Shortest path calculation using Learning Automata
 * Copyright (C) 2014-2021 by Constantine Kyriakopoulos
 * zfox@users.sourceforge.net
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

bool simpleRun()
{
	AdaptiveSystem* la = new LaSystem("topology.json", LaSystem::ITERATIONS);
	auto nodePath = la->path(0, 19);
	for(int node : nodePath)
		std::cout << node << " ";
	std::cout << std::endl;
	delete la;

	return nodePath.size() > 0;
}

int main(int argc, char *argv[])
{
	return simpleRun() ? EXIT_SUCCESS : EXIT_FAILURE;
}
