# Flow-Field-Pathfinder

## What is it?

This flow-field pathfinder directs a collection of agents toward a common goal. It can be useful when pathfinding for a large number of agents since its performance is independent of the number of agents.

## How does it work?

Before path planning, the map is discretized into a grid of tiles, some of which are marked as obstacles which the agents cannot traverse over.

The first stage of the flow-field pathfinder involves generating a Dijkstra map via a wavefront algorithm. The Dijkstra map stores a distance value for every tile representing its distance to the goal tile, factoring in obstacles along the way.

The second stage, for which the pathfinder is named, involves generating a flow-field / vector-field from the Dijkstra map: a direction vector is generated for every tile where the x component is the distance value of the tile on the left subtract the value of the right on the tile and the y component is the one above subtract the one below.

These direction vectors are then normalised.

For each agent, it checks the direction vector of every tile it occupies, adds them altogether to get the average direction, normalises the result, and then calculates a velocity vector based on the speed of the agents.
