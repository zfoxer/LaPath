/*
 * LaPath: Shortest path calculation using Learning Automata
 * Copyright (C) 2014-2020 by Constantine Kyriakopoulos
 * @version 1.0
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

int simpleRun()
{
	AdaptiveSystem* la = new LaSystem("topology.json", LaSystem::ITERATIONS);
	auto nodePath = la->path(0, 19);
	for(int n : nodePath)
		std::cout << n << " ";
	std::cout << std::endl;
	delete la;

	return nodePath.size() ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
	return simpleRun();
}
