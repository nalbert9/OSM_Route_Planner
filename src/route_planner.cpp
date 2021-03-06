#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
/* Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
*/

    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2
    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node   = &m_Model.FindClosestNode(end_x, end_y);

}

// TODO 3
float RoutePlanner::CalculateHValue(const RouteModel::Node *node) {
    /* Set h value

       Can use the distance to the end_node for the h value.
       Node objects have a distance method to determine the distance to another node.
    */

    return node->distance( *(this->end_node));

}


// TODO 4
void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    // Expand the current node by adding all unvisited neighbors to the open list.

    // Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
    current_node->RouteModel::Node::FindNeighbors();

    // For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
    for (RouteModel::Node* neighbor : current_node->neighbors){
        neighbor->parent  = current_node;

        //Use CalculateHValue below to implement the h-Value calculation.
        neighbor->h_value = CalculateHValue(neighbor);
        neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);

        this->open_list.push_back(neighbor);
        neighbor->visited = true;
    }

}


bool compareSumHGvalue(const RouteModel::Node *node_1, const RouteModel::Node *node_2) {
    // NextNode method sort comparator 

    float f1 = node_1->g_value + node_1->h_value;
    float f2 = node_2->g_value + node_2->h_value;

    return f1 > f2;

}


// TODO 5
RouteModel::Node *RoutePlanner::NextNode() {
    /*Sort the open_list according to the sum of the h value and g and return the next node.

      Create a pointer to the node in the list with the lowest sum.
      Remove that node from the open_list.
      Return the pointer.
    */

    std::sort(open_list.begin(), open_list.end(), compareSumHGvalue);
    RouteModel::Node* node_lowest_sum = this->open_list.back();
    this->open_list.pop_back();

    return node_lowest_sum;

}


// TODO 6
std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // This method return the final path found from the A* search.

    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    RouteModel::Node *current = current_node;
    while(current != start_node) {
        path_found.push_back(*current);

        // For each node in the chain, add the distance from the node to its parent to the distance variable.
        distance += current->distance(*current->parent);
        current = current->parent;
    }

    path_found.push_back(*current);

    // The returned vector should be in the correct order: the start node should be the first element
    // of the vector, the end node should be the last element.
    std::reverse(path_found.begin(), path_found.end());

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}


// TODO 7
void RoutePlanner::AStarSearch() {
    // A* Search algorithm
    
    RouteModel::Node *current_node = nullptr;
    
    open_list.emplace_back(start_node);
    start_node->visited = true;

    while (!open_list.empty()) {
        // Use the NextNode() method to sort the open_list and return the next node.
        current_node = NextNode();

        // When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
        // Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.
        if (current_node == end_node) m_Model.path = ConstructFinalPath(current_node);
        
        // Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
        AddNeighbors(current_node);
    }

}