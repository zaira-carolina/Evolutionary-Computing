// algoritmo de pareto en dos dimensiones
#include <bits/stdc++.h>
#include <algorithm>
using namespace std; 

// Estructura 2d
struct Point {
    double x;
    double y;
};

// Verificar si un punto es dominado por otro punto en la lista
bool is_dominated(const Point& p1, const Point& p2) {
    return (p1.x <= p2.x && p1.y <= p2.y && (p1.x < p2.x || p1.y < p2.y));
}

bool is_Dominated(Point p1, Point p2){
    return (p1.x <= p2.x && p1.y <= p2.y && (p1.x < p2.x || p1.y < p2.y));
}

// FUERZA_BRUTA
vector<Point> frontera_pareto_nn(const vector<Point> &points){
    vector<Point> pareto_points; 
    // checar todos contra todos, aqui es n*n
    for (const auto&p : points){
        bool dominado = false; 
        // hay que ir todos contra todos 
        for (const auto &q: points){
            // si son distintos, ver si estan dominados por al menos un punto
            if (&p != &q && is_dominated(p, q)){
                dominado = true;
                break; 
            }
        }
        // si no esta dominado, lo anadimos a los puntos de la frontera 
        if (!dominado) pareto_points.push_back(p); 
    }
    return pareto_points; 
}


/*********************  ALGORITMO SCAN 2D *************************/

bool coordX(const Point& p1, const Point& p2){
    return (p1.x < p2.x); 
}

vector<Point> frontera_pareto_scan2D(vector<Point> points){
    // primero ordenar de acuerdo al primer objetivo
    sort(points.begin(), points.end(), coordX);
    
    vector<Point> pareto_points; 

    Point curr = points[0]; 
    for (int i=1; i<points.size(); i++){
        if (curr.y >= points[i].y){
            pareto_points.push_back(curr);
        }
        curr = points[i]; 
    }

    pareto_points.push_back(points.back()); 
    
    return pareto_points; 
}



int main(){
    vector<Point> points = {{5, 7}, {3, 9}, {8, 4}, {4, 6}, {9,2}, {6, 5}}; 
    
    
    cout << "primera forma: " << endl;
    vector<Point> result = frontera_pareto_nn(points); 
    for (int i=0; i<result.size(); i++){
        cout << result[i].x << " " << result[i].y << " - "; 
    }

    cout << "\n segunda forma: " << endl; 
    vector<Point> r = frontera_pareto_scan2D(points); 
    for (int i=0; i<r.size(); i++){
        cout << r[i].x << " " << r[i].y << " - "; 
    }





    return 0; 
}