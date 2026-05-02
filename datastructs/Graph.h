#pragma once
// ============================================================
// FILE: Graph.h
// MODULE: 2 - Custom Data Structures
// OUTPUT-ID: M2-GRP-001
// Expected Output: Shortest path between stops, distance matrix
// OOP CONCEPTS: Templates, adjacency list, nested structs
// WHERE USED: RouteController for city network, Dijkstra for
//             finding nearest vehicle and shortest route
// ============================================================
// HOW GRAPH WORKS:
//   Vertices = Stops (bus stops, metro stations)
//   Edges    = Roads/tracks connecting them
//   Weight   = Distance in km (from GeoCoordinate::distanceTo)
//   Directed = Edge A→B does NOT mean B→A exists
//
// DIJKSTRA ALGORITHM:
//   Find shortest path from source to ALL other vertices.
//   Uses MinHeap as priority queue.
//   Time complexity: O((V + E) log V)
// ============================================================

#include <iostream>
#include <climits>    // INT_MAX (infinity in Dijkstra)
#include "../exceptions/TransitExceptions.h"
#include "DynamicArray.h"
#include "MinHeap.h"

// ============================================================
// GRAPH CLASS
// ============================================================
template <typename T>
class Graph {
private:
    // An edge from one vertex to another with a weight
    struct Edge {
        int    toVertex;   // Index of destination vertex
        double weight;     // Cost/distance to reach destination

        Edge() : toVertex(-1), weight(0.0) {}
        Edge(int to, double w) : toVertex(to), weight(w) {}
    };

    // A vertex stores the data and its adjacency list (outgoing edges)
    struct Vertex {
        T                   data;          // The actual value (e.g. Stop)
        DynamicArray<Edge>  adjacencyList; // List of outgoing edges
        bool                visited;       // Used during Dijkstra

        Vertex() : visited(false) {}
        explicit Vertex(const T& val) : data(val), visited(false) {}
    };

    DynamicArray<Vertex> m_vertices;   // All vertices in graph
    int                  m_edgeCount;  // Total number of edges

public:
    Graph() : m_edgeCount(0) {}

    // -------------------------------------------------------
    // ADD VERTEX: returns the index of the added vertex
    // -------------------------------------------------------
    int addVertex(const T& data) {
        m_vertices.append(Vertex(data));
        return m_vertices.size() - 1;  // Return index
    }

    // -------------------------------------------------------
    // ADD DIRECTED EDGE: from → to with given weight
    // -------------------------------------------------------
    void addEdge(int fromIndex, int toIndex, double weight) {
        if (fromIndex < 0 || fromIndex >= m_vertices.size() ||
            toIndex   < 0 || toIndex   >= m_vertices.size()) {
            throw GraphVertexNotFoundException("Graph::addEdge");
        }
        m_vertices[fromIndex].adjacencyList.append(Edge(toIndex, weight));
        m_edgeCount++;
    }

    // Add undirected edge (both directions)
    void addUndirectedEdge(int fromIndex, int toIndex, double weight) {
        addEdge(fromIndex, toIndex, weight);
        addEdge(toIndex, fromIndex, weight);
    }

    // -------------------------------------------------------
    // FIND VERTEX by data (uses T::operator==)
    // -------------------------------------------------------
    int findVertex(const T& data) const {
        for (int i = 0; i < m_vertices.size(); i++) {
            if (m_vertices[i].data == data) return i;
        }
        return -1;  // Not found
    }

    // Get vertex data by index
    const T& getVertex(int index) const {
        if (index < 0 || index >= m_vertices.size()) {
            throw GraphVertexNotFoundException("Graph::getVertex");
        }
        return m_vertices[index].data;
    }

    int vertexCount() const { return m_vertices.size(); }
    int edgeCount()   const { return m_edgeCount;       }

    // -------------------------------------------------------
    // DIJKSTRA: Find shortest paths from sourceIndex to all others
    // Returns: array of shortest distances from source to each vertex
    //          dist[i] = shortest distance from source to vertex i
    //          -1.0 means "not reachable"
    // -------------------------------------------------------
    DynamicArray<double> dijkstra(int sourceIndex) const {
        int n = m_vertices.size();

        // Initialize all distances to "infinity"
        DynamicArray<double> dist;
        for (int i = 0; i < n; i++) dist.append(1e18);  // "Infinity"
        dist[sourceIndex] = 0.0;  // Distance to self = 0

        // Priority queue entry: {distance, vertexIndex}
        // We use a simple struct for Dijkstra priority queue
        struct DijkEntry {
            double distance;
            int    vertexIdx;
            DijkEntry() : distance(0.0), vertexIdx(0) {}
            DijkEntry(double d, int v) : distance(d), vertexIdx(v) {}
            bool operator<(const DijkEntry& o) const { return distance < o.distance; }
            bool operator==(const DijkEntry& o) const { return vertexIdx == o.vertexIdx; }
        };

        MinHeap<DijkEntry> pq;
        pq.insert(DijkEntry(0.0, sourceIndex));

        while (!pq.isEmpty()) {
            DijkEntry current = pq.extractMin();  // Get vertex with smallest distance
            int u = current.vertexIdx;

            // Skip if we already found a better path
            if (current.distance > dist[u]) continue;

            // Explore all neighbours of u
            const DynamicArray<Edge>& edges = m_vertices[u].adjacencyList;
            for (int i = 0; i < edges.size(); i++) {
                int    v         = edges[i].toVertex;
                double newDist   = dist[u] + edges[i].weight;

                // Relaxation: if we found a shorter path to v
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    pq.insert(DijkEntry(newDist, v));  // Add updated distance to queue
                }
            }
        }

        // Replace "infinity" with -1.0 for unreachable vertices
        for (int i = 0; i < n; i++) {
            if (dist[i] >= 1e17) dist[i] = -1.0;
        }
        return dist;
    }

    // -------------------------------------------------------
    // SHORTEST PATH: returns sequence of vertex indices
    // from source to destination
    // -------------------------------------------------------
    DynamicArray<int> shortestPath(int sourceIndex, int destIndex) const {
        int n = m_vertices.size();
        DynamicArray<double> dist;
        DynamicArray<int>    prev;   // previous vertex in optimal path

        for (int i = 0; i < n; i++) {
            dist.append(1e18);
            prev.append(-1);      // -1 = no predecessor
        }
        dist[sourceIndex] = 0.0;

        struct DijkEntry {
            double distance; int vertexIdx;
            DijkEntry() : distance(0), vertexIdx(0) {}
            DijkEntry(double d, int v) : distance(d), vertexIdx(v) {}
            bool operator<(const DijkEntry& o) const { return distance < o.distance; }
            bool operator==(const DijkEntry& o) const { return vertexIdx == o.vertexIdx; }
        };

        MinHeap<DijkEntry> pq;
        pq.insert(DijkEntry(0.0, sourceIndex));

        while (!pq.isEmpty()) {
            DijkEntry cur = pq.extractMin();
            int u = cur.vertexIdx;
            if (u == destIndex) break;   // Reached destination!
            if (cur.distance > dist[u]) continue;

            const DynamicArray<Edge>& edges = m_vertices[u].adjacencyList;
            for (int i = 0; i < edges.size(); i++) {
                int    v       = edges[i].toVertex;
                double newDist = dist[u] + edges[i].weight;
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    prev[v] = u;         // Record: came from u to reach v
                    pq.insert(DijkEntry(newDist, v));
                }
            }
        }

        // Reconstruct path by following prev[] backwards
        DynamicArray<int> path;
        int cur = destIndex;
        while (cur != -1) {
            path.insert(0, cur);    // Insert at front to reverse
            cur = prev[cur];
        }
        if (path.size() == 0 || path[0] != sourceIndex) {
            // No path found
            return DynamicArray<int>();
        }
        return path;
    }

    // Print all vertices and their connections
    void printGraph() const {
        std::cout << "Graph (" << m_vertices.size() << " vertices, "
                  << m_edgeCount << " edges):\n";
        for (int i = 0; i < m_vertices.size(); i++) {
            std::cout << "  [" << i << "] " << m_vertices[i].data << " → ";
            const DynamicArray<Edge>& edges = m_vertices[i].adjacencyList;
            for (int j = 0; j < edges.size(); j++) {
                std::cout << "[" << edges[j].toVertex << "] ("
                          << edges[j].weight << "km)";
                if (j < edges.size()-1) std::cout << ", ";
            }
            std::cout << "\n";
        }
    }
};
