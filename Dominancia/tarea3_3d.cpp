/* 
Resolver la frontera de pareto en tres dimensiones, utilizando un arbol de segmentos. 
En el ejemplo se ven cuatro, porque el ultimo valor es el indice del vector. 
*/

#include <bits/stdc++.h>
using namespace std;

const int MAX_N = 1e4;
const int inf = 1e9;
vector<int> tree (MAX_N, -inf); // inicializarlo con un numero alto, porque buscamos el minimo
vector<bool> isDominated(MAX_N, false); 

struct Point{
    double x; 
    double y;
    double z;
    int indice; 
};


void update(int index, int curr, int start, int end, int pos, int valor){    
    if (start == end){
        tree[pos] = max(tree[pos], valor);
        return; 
    }else{
        int mitad = (start + end)/2;
        if (pos <= mitad){
            update(index, 2*curr, start, mitad, pos, valor);
        }
        else{
            update(index, 2*curr+1, mitad+1, end, pos, valor);
        }
    }
}

// Ordenamos por todos los objetivos y mapeamos
bool ordenar(const Point &p1, const Point &p2){
    if (p1.x != p2.x){
        return p1.x < p2.x;
    } else if (p1.y != p2.y){
        return p1.y < p2.y;
    }else{
        return p1.z < p2.z;
    }
}

// comparacion de coordenadas 
bool coordX(const Point &p1, const Point &p2){
    return p1.x < p2.x;
}

bool coordY(const Point &p1, const Point &p2){
    return p1.y < p2.y;
}

bool coordZ(const Point &p1, const Point &p2){
    return p1.z < p2.z;
}


vector<Point> compression(vector<Point> points){
    int N = points.size(); 
    vector<Point> newPoints = points;
    map<double, int> mapa; 

    // EJE X
    int curr = -1;
    sort(points.begin(), points.end(), coordX); 
    for (int i=0; i<N; i++){
        if (!mapa.count(points[i].x)) curr++; 
        mapa[points[i].x] = curr;
    }
    for (int i=0; i<N; i++) newPoints[i].x = mapa[newPoints[i].x];

    // EJE Y 
    sort(points.begin(), points.end(), coordY);
    mapa.clear();
    curr = -1; 
    for (int i=0; i<N; i++){
        if (!mapa.count(points[i].y)) curr++;
        mapa[points[i].y] = curr;
    }
    for (int i=0; i<N; i++) newPoints[i].y = mapa[newPoints[i].y]; 

    // EJE Z 
    sort(points.begin(), points.end(), coordZ); 
    mapa.clear();
    curr = -1; 
    for (int i=0; i<N; i++){
        if (!mapa.count(points[i].z)) curr++;
        mapa[points[i].z] = curr;
    }
    for (int i=0; i<N; i++) newPoints[i].z = mapa[newPoints[i].z]; 
    mapa.clear(); 

    return newPoints;
}

bool is_pareto(const Point& p1, const Point& p2) {
    return (p1.x <= p2.x && p1.y <= p2.y && p1.z <= p2.z) &&  (p1.x < p2.x || p1.y < p2.y || p1.z < p2.z);
}

vector<Point> frontera_pareto_nn(const vector<Point> &points){
    vector<Point> pareto_points; 
    // checar todos contra todos, aqui es n*n
    for (const auto&p : points){
        bool dominado = false; 
        // hay que ir todos contra todos 
        for (const auto &q: points){
            // si son distintos, ver si estan dominados por al menos un punto
            if (&p != &q && is_pareto(p, q)){
                dominado = true;
                break; 
            }
        }
        // si no esta dominado, lo anadimos a los puntos de la frontera 
        if (!dominado) pareto_points.push_back(p); 
    }
    return pareto_points; 
}

int main(){

    vector<Point> points =  {
                            {3, 5, 2, 0}, 
                            {3, 5, 4, 1}, 
                            {4, 3, 1, 2}, 
                            {1, 2, 4, 3}, 
                            {5, 1, 5, 4}
                            };


    vector<Point> points_nn = frontera_pareto_nn(points);
    for (int i=0; i<points_nn.size(); i++) cout << points_nn[i].x << " " << points_nn[i].y << " " << points_nn[i].z << endl; 

    cout << endl; 
    int N = points.size(); 
    /* Ordenar por todos los objetivos y hacemos un mapeo de forma que si son
       N vectores, los valores sean numeros enteros en el rango [0:N-1] */
    
    vector<Point> newPoints = points; //compression(points);
    // Ordenamos por el primer objetivo, por el segundo y por el tercero. 
    sort(newPoints.begin(), newPoints.end(), ordenar); 

    // Insertar en el arbol de segmentos 
    for (int i=0; i<N; i++){
        update(newPoints[i].indice, 0, 0, N-1, newPoints[i].y, newPoints[i].z); 
    }

    // ya se puede ver directamente si esta dominado
    vector<Point> pareto; 
    for (int i=0; i<N; i++){
        int u = tree[newPoints[i].y];
        bool dominado = false; 
        if (newPoints[i].z < u){
            dominado = true;
        }
        if (!dominado) pareto.push_back(newPoints[i]);
    }

    cout << "otro metodo" << endl; 
    for (int i=0; i<pareto.size(); i++){
        cout << pareto[i].x << " " << pareto[i].y << " " << pareto[i].z << endl; 
    }

    



    
    return 0; 
}