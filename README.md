# Flow-Field-Pathfinder

## What is it?

This flow-field pathfinder directs a collection of agents toward a common goal. It can be useful when pathfinding for a large number of agents since its performance is independent of the number of agents.

## How can it be used?

You can change the default map size and agent movement speed in the settings.txt file. They are the first and second numbers respectively.

To create an obstacle you press the left mouse button while hovering over a tile. To remove an obstacle do the same but with the right mouse button.

To change the position of the green goal tile you can press space while hovering over a tile.

You can create an agent by pressing E while hovering over a spot. Agent movement can be toggled using the backspace key.

Further information can be shown by pressing 2 - display the Dijktra map, 3 - display the unnormalisesd direction vectors in column notation, 4 - display the normalised direction vectors in column notation, and 5 - show the direction vectors as arrows. You can get rid of this information by pressing 1.

## How does it work?

Before path planning, the map is discretized into a grid of tiles, some of which are marked as obstacles which the agents cannot traverse over.

The first stage of the flow-field pathfinder involves generating a Dijkstra map via a wavefront algorithm. The Dijkstra map stores a distance value for every tile representing its distance to the goal tile, factoring in obstacles along the way.

The second stage, for which the pathfinder is named, involves generating a flow-field / vector-field from the Dijkstra map: a direction vector is generated for every tile where the x component is the distance value of the tile on the left subtract the value of the right on the tile and the y component is the one above subtract the one below.

These direction vectors are then normalised.

For each agent, it checks the direction vector of every tile it occupies, adds them altogether to get the average direction, normalises the result, and then calculates a velocity vector based on the speed of the agents.
