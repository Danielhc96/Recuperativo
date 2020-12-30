#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cstdio>

#define MASTER 0

using namespace std;
/*
*
*   Clase salario
*
*/
class Salario{
    private: 
        int year;
        float salario_peso;
        float salario_dolar;
    public: 
        Salario();
        Salario(int,float,float); 
        void ingreso_salario_dolar(float dolar);
        int get_year();
        float get_salario_peso();
        float get_salario_dolar();
};
Salario::Salario(){
    year=0;
    salario_peso=0;
    salario_dolar=0;
}
Salario::Salario(int _year, float _salario_peso, float _salario_dolar){
    year = _year;
    salario_peso = _salario_peso;
    salario_dolar = _salario_dolar;
}
void Salario::ingreso_salario_dolar(float _dolar){
    salario_dolar = salario_peso/_dolar;
}
int Salario::get_year(){
    return year;
}
float Salario::get_salario_peso(){
    return salario_peso;
}
float Salario::get_salario_dolar(){
    return salario_dolar;
}
/*
*
*   Cabeceras
*
*/
float prom_by_year(char *, int);
int get_year(string);
float get_amount_dollars(string);
float get_salary_by_year(char *, int);
float get_salary_smi(string);
void regresion_lineal(Salario []);
void ver_datos(Salario []);
/*
*
*   Main
*
*/
int main(int argc, char* argv[])
{   

    int rank, size;
    //MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == MASTER){
        int i, year=1995;
        float prom=0;
        Salario salarios[25];
        for(i=0; i<25; i++){
            salarios[i] = Salario(year,get_salary_by_year(argv[1],year),0);
            year++;
        }
        //recibe datos de proceso 1
        i=0;
        for(year=1995; year<2004; year++){
            MPI_Recv(&prom, 1, MPI_FLOAT, 1, year, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            salarios[i].ingreso_salario_dolar(prom);
            i++;
            //cout<<"se recibio: "<<prom<<", año: "<<year<<", desde proceso 1"<<endl;
        }
        //recibe datos de proceso 2
        i=9;
        for(year=2004; year<2012; year++){
            MPI_Recv(&prom, 1, MPI_FLOAT, 2, year, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            salarios[i].ingreso_salario_dolar(prom);
            i++;
            //cout<<"se recibio: "<<prom<<", año: "<<year<<", desde proceso 2"<<endl;
        }
        //recibe datos de proceso 3
        i=17;
        for(year=2012; year<2020; year++){
            MPI_Recv(&prom, 1, MPI_FLOAT, 3, year, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            salarios[i].ingreso_salario_dolar(prom);
            i++;
            //cout<<"se recibio: "<<prom<<", año: "<<year<<", desde proceso 3"<<endl;
        }
        
        regresion_lineal(salarios);
        //ver_datos(salarios);
    }

    if(rank == 1){
        int year;
        float prom;
        //envio de datos a MASTER
        for(year=1995; year<2004; year++){
            prom = prom_by_year(argv[2], year);
            MPI_Send(&prom, 1, MPI_FLOAT, MASTER, year, MPI_COMM_WORLD);
            //cout<<"se envio prom: "<<prom<<", año: "<<year<<", desde proceso "<<rank<<endl;
        }
    }

    if(rank == 2){
        int year;
        float prom;
        //envio de datos a MASTER
        for(year=2004; year<2012; year++){
            prom = prom_by_year(argv[2], year);
            MPI_Send(&prom, 1, MPI_FLOAT, MASTER, year, MPI_COMM_WORLD);
            //cout<<"se envio prom: "<<prom<<", año: "<<year<<", desde proceso "<<rank<<endl;
        }
    }

    if(rank == 3){
        int year;
        float prom;
        //envio de datos a MASTER
        for(year=2012; year<2020; year++){
            prom = prom_by_year(argv[2], year);
            MPI_Send(&prom, 1, MPI_FLOAT, MASTER, year, MPI_COMM_WORLD);
            //cout<<"se envio prom: "<<prom<<", año: "<<year<<", desde proceso "<<rank<<endl;
        }
    }

    MPI_Finalize();
    return 0;
}
/*
*
*   Funciones
*
*/
//función para leer una linea
void ver_datos(Salario salarios[]){
    int i;
    cout<<"\nDatos info"<<endl;
    for(i=0; i<25; i++){
        cout<<"salario ["<<i<<"]    ";
        cout<<"año: "<<salarios[i].get_year()<<"    ";
        cout<<"salario peso: "<<salarios[i].get_salario_peso()<<"    ";
        cout<<"salario dolar; "<<salarios[i].get_salario_dolar()<<endl;
    }
}
//función para obtener promedio de un año
float prom_by_year(char *archivo, int year){
    ifstream file(archivo);
    string line;
    //descarta primera linea
    getline(file,line);
    int i=0;
    float valor=0, prom;
    //cout<<"Datos"<<endl;
    while(!file.eof()){
        getline(file,line);
        //si encontramos el año que buscamos
        if(get_year(line)==year)
        {
            valor=valor+get_amount_dollars(line);
            i++;
        }
    }
    prom=valor/i;
    //cout<<"valor año "<<year<<" es "<<valor<<", n° datos es "<<i<<" y el promedio es "<<prom<<endl;
    return prom;
}
//función para obtener el año de una linea de dollars.csv
int get_year(string line){
    int year;
    string n ="";
    for (int i=1; i<5; i++)n=n+line[i];
    year = atoi(n.c_str());
    return year;
}
//función para obtener el monto de una linea de dollars.csv
float get_amount_dollars(string line){
    float amount;
    string n ="";
    for (int i=14; i<line.length(); i++)n=n+line[i];
    amount= atof(n.c_str());
    return amount;
}
//función para extraer sueldo minimo en peso de un año
float get_salary_by_year(char *archivo, int year){
    ifstream file(archivo);
    string line;
    float valor=0;
    //descarta primera linea
    getline(file,line);
    while(!file.eof()){
        getline(file,line);
        //si encontramos año
        if(get_year(line)==year)valor=get_salary_smi(line);
    }
    //cout<<"valor año "<<year<<" es "<<valor<<endl;
    return valor;
}
//función para extraer sueldo minimo de un string
float get_salary_smi(string line){
    float value;
    string n="";
    for(int i=8; i<line.length()-1; i++)n=n+line[i];
    value = atof(n.c_str());
    return value;
}
//función para obtener regresión lineal
void regresion_lineal(Salario salarios[]){
    int i,n=25;
    float m,b,sumax=0,sumay=0,sumaxy=0,sumax2=0;
    for(i=0; i<n; i++){
        //suma de los productos
        sumaxy += salarios[i].get_year()*salarios[i].get_salario_dolar();
        //suma de los valores de x^2
        sumax2 += salarios[i].get_year()*salarios[i].get_year();
        //suma de los valores de x
        sumax += salarios[i].get_year();
        //suma de los valores de y
        sumay += salarios[i].get_salario_dolar();
    }

    //Calculamos la pendiente (m) y la interseccion (b)
    m = (n*sumaxy - sumax*sumay) / (n*sumax2 - sumax*sumax);
    b = (sumay - m*sumax) / n;
    cout<<"=== Resultado ===\n"<<endl;
    cout<<"y = "<<m<<"x "<<b<<endl;
    cout<<"\n=== Integrantes ===\n"<<endl;
    cout<<"Daniel Hernández Calfuqueo"<<endl;
}


